# Design

Summary of our design decisions and some pointers to literature.

## SymEngine is used from different languages

The C++ SymEngine library doesn't care about SymPy objects at all. We are just trying to implement things in some maintainable way, currently we settled on using `Basic`, `Mul`, `Pow`, ... hierarchy and implement most functionality using the visitor pattern or single dispatch, so `Basic` doesn't need many methods. We are keeping an option to perhaps do things differently if they turn out to be faster. Either way though, this shouldn't matter at all for how it is actually used from Python, Ruby or Julia. 

Let's talk about just Python: the wrappers are in the [symengine.py project](https://github.com/symengine/symengine.py). They are implemented using Cython, and they are free to introduce any kind of classes (including SymPy's `Expr` or Sage's `Expression` if needed), and the point of the wrappers is to make sure that things work out of the box from SymPy and Sage. The only job of the C++ SymEngine library is to ensure that the library's C++ API is implemented in such a way so that the wrappers can be written to do what they need. For example, we could easily introduce SymPy's `Expr` into the wrappers, by simply introducing the `Expr` class and make all the other classes subclass from it instead of from `Basic`.

That was the reason we split the wrappers, so now in the (pure) C++ `symengine/symengine` repository, we only have to worry about speed, correctness, maintainability and a usable API, and we can concentrate on these things without worrying or even testing any kind of wrappers. In the wrappers (`symengine/symengine.py`, or `.jl`, `.rb`), we simply just use the C++ (or C) API and the only thing we care is so that the (Python) wrapper can be used from sympy/Sage (and we test that in the test suite), and that it doesn't introduce unnecessary overhead in terms of speed. Ruby or Julia wrappers then care about interoperability with other libraries in those languages.

* https://github.com/symengine/symengine.py
* https://github.com/symengine/symengine.rb
* https://github.com/symengine/SymEngine.jl

## Reference Counted Pointers

### Teuchos `RCP`

Memory management is handled by RCP (reference counted pointers) from
`Trilinos` (module `Teuchos`). We have copied the relevant files into
`src/teuchos`, so no external dependency is needed.  Brief code snippets of the
most frequent operations are given in our [C++ Style
Guide](style_guide.md), this is useful to consult if you are unsure about
the syntax.  In order to understand how it works under the hood, read
[Teuchos::RCP Beginner's
Guide](http://trilinos.sandia.gov/RefCountPtrBeginnersGuideSAND.pdf) (pdf).
Finally, more thorough exposition is given in [Teuchos C++ Memory Management
Classes, Idioms, and Related Topics --- The Complete
Reference](http://www.cs.sandia.gov/~rabartl/TeuchosMemoryManagementSAND.pdf)
(pdf).

`Teuchos`' RCP implements reference counting of objects, exactly like Python
works. When an object runs out of scope, its reference count is decreased. When
it is copied, its reference count is increased. When reference count goes to
zero, it is deallocated. This all happens automatically, so as long as our C++
Style Guide is followed, things just work.

When `CMAKE_BUILD_TYPE=Debug` is set in our `CMake` (the default), then
`Teuchos` is compiled with debugging support, which means that as long as you
follow our C++ Style Guide, the C++ code should never segfault (since you never
access raw pointers that could segfault and Teuchos raises nice exception with
full stacktrace if there is any problem, very similar to Python). Use this mode
when developing.

When `CMAKE_BUILD_TYPE=Release`, then `Teuchos` is compiled without debugging
support, which means that all pointer operations become either as fast as raw
pointers, or very close. As such, there is pretty much zero overhead. However,
in this mode, the program can segfault if you access memory incorrectly. This
segfault however would be prevented if `CMAKE_BUILD_TYPE=Debug`, so always use
the Debug build to test your code, only when all tests pass you can enable
Release mode.

The Trilinos RCP pointers as described above are only used when
`WITH_SYMENGINE_RCP=OFF` is set in `CMake`. The default option is
`WITH_SYMENGINE_RCP=ON`, which uses our own implementation of `RCP` (see
`src/symengine_rcp.h`). Our implementation is faster, but it only implements a
subset of all the functinoality and it requires all our objects to have a
`refcount_` variable. Otherwise the usage of our RCP is identical to
`Teuchos::RCP`, and Travis-CI tests both implementations of RCP to make sure
the code works with both.

### `Ptr` and `RCP`

The `RCP` has an overhead with every assignment (refcount increase/decrease). You can get access to the inner pointer as `Ptr` (just call `.ptr()`), which has the same performance as raw pointer in Release mode, yet it is 100% safe in Debug mode (i.e. if the original `RCP` gets out of scope, the object deallocated, then the `Ptr` becomes dangling, but you get a nice exception in Debug mode if you try to access it ---- in Release mode it will segfault or have undefined behavior, just like raw pointers). The idea is that for non-owning access --- i.e. typically you just want to read some term in Add, we should be passing around `Ptr`, not `RCP` (which has the extra refcount increase/decrease, which is a waste since we do not plan to own it). This we can implement already in `SymEngine`.

### `UniquePtr`

`std::unique_ptr` has performance just like raw pointers + manual new/delete, so there is no reason to use manual new/delete and raw pointers, one should use `std::unique_ptr`. One issue with `std::unique_ptr` is that if you get access to the raw pointer using `.get()`, then it will segfault if it becomes dangling (i.e. there are no Debug time checks implemented in the standard library for this, because it returns a raw pointer, not a `Ptr`). This can however be fixed by writing a new class `UniquePtr` that returns `Ptr` instead of raw pointer if you want to pass it around, and is 100% Debug time checked, so it can't segfault in Debug mode. I am implementing it in https://github.com/certik/trilinos/pull/1, it's a bit of work, since it needs to work with custom deallocators and be a drop-in replacement for `std::unique_ptr`. But it will get done. The beauty of this new `UniquePtr` is that together with `Ptr`, there is no need to every use manual new/delete and raw pointers. `UniquePtr` has the same performance in Release mode, yet it is 100% safe in Debug mode. It's great.

However, the issue is that even manual new/delete (or the equivalent `UniquePtr`) is slow, so we want to avoid it, or only do it as little as possible. I am still thinking if we could perhaps use `UniquePtr` instead of `RCP`. It would mean that for example the `Add` container would deallocate its contents (i.e. instead of having a hashtable full of RCP, it would have a hashtable full of `UniquePtr`), and if you access a given term, you either get just a `Ptr` (thus very fast, but could become dangling if `Add` goes out of scope --- which would be Debug time checked, so no segfault, but in Release it would segfault, and for example if one does it at runtime in Python in Release mode, it would segfault, so one would have to make sure this is not exposed to the user), or you need to make a copy. For example, if you create some symbols and then use them in expressions, then currently we just pass around `RCP`, i.e. reference counted pointers to the original symbol. With the new approach, we would need to make a copy. Since we do not want to copy the `std::string` from inside Symbol, we want store the symbols in some kind of table, and only pass a simple reference to the table (and also we need to deallocate things from the table if they are not used anymore). In other words, we just reinvented `RCP` again. So for Symbols, it seems it wouldn't have much benefit. It might have some benefit for classes like `Add`. If they internally use `UniquePtr`, we can do an optimization in Release mode and store the contents directly in the hashtable (i.e. no pointers at all), and still pass around the `Ptr` to other code (i.e in Debug mode it would use `UniquePtr`, thus we would make sure that things are not dangling, and in Release mode we just pass around `Ptr`, with the performance of a raw pointer, to the internal array), that way we avoid new/delete. Also with this one can do custom allocator, i.e. allocate a chunk of memory for the hashtable, and just do placement new. I played with this, and surprisingly, the performance wasn't much different to `UniquePtr` (for smaller objects it was a bit faster, but for larger objects --- remember they are stored by value now --- it was even slower). Also the creation of `RCP` vs `UniquePtr` was almost the same fast a well. The reason is that a simple refcount initialization is negligible in terms of time compared to the `new` call. What is slow is if you pass around `RCP` instead of `Ptr`, because raw pointer (which is what `Ptr` is in Release mode) is much faster than a refcount increase/decrease.  We should still investigate if we can get rid of new/delete using the approaches from this paragraph.

### Conclusion:

* We should pass around `Ptr` instead of `RCP` whenever possible, and we can do this right away. 
* Use `UniquePtr` (after it is implemented) whenever possible instead of `RCP` --- though most places in `SymEngine` seem to require `RCP`. But we should keep this in mind, there might still be one or two places where `UniquePtr` is the way to go.
* Never use raw new/delete and never use raw pointers (use `UniquePtr` + `Ptr`, and if it is not sufficient, use the slower `RCP` + `Ptr`).
* Never pass pointers to some internal data --- pass `Ptr` and have it Debug time checked by using `UniquePtr` in Debug mode, and use the data directly in Release mode

As an example of the last point, e.g. to give access to an internal `std::map` (as a pointer, so that the map is not copied), here is how to do it:
```c++
class A {
private:
#ifdef DEBUG_MODE
    UniquePtr<std::map<int, int> > m;
#else
    std::map<int, int> m;
#endif
public:
    Ptr<std::map<int, int>> get_access() {
#ifdef DEBUG_MODE
        return m.ptr();
#else
        return ptrFromRef(m);
#endif
    }
};
```
That way, in debug mode, you can catch dangling references but in optimized mode it is optimally fast.

## Object creation and `is_canonical()`

Classes like `Add`, `Mul`, `Pow` are initialized through their constructor using their internal representation. `Add`, `Mul` have a `coeff` and `dict`, while `Pow` has `base` and `exp`. There are restrictions on what `coeff` and `dict` can be (for example `coeff` cannot be zero in `Mul`, and if `Mul` is used inside `Add`, then `Mul`'s coeff must be one, etc.). All these restrictions are checked when `SYMENGINE_ASSERT` is enabled inside the constructors using the `is_canonical()` method. That way, you don't have to worry about creating `Add`/`Mul`/`Pow` with wrong arguments, as it will be caught by the tests. In the Release mode no checks are done, so you can construct `Add`/`Mul`/`Pow` very quickly. The idea is that depending on the algorithm, you sometimes know that things are already canonical, so you simply pass it directly to `Add`/`Mul`/`Pow` and you avoid expensive type checking and canonicalization. At the same time, you need to make sure that tests are still running with `SYMENGINE_ASSERT` enabled, so that `Add`/`Mul`/`Pow` are never in an inconsistent state.

The philosophy of symengine is that you impose as many restrictions in `is_canonical()` for each class as you can (and only check that in Debug mode), so that inside the class you can assume all those things and call faster algorithms (e.g. in `Rational` you know it's not an integer, so you don't need to worry about that special case, at the same time if you have an integer, you are forced to use the `Integer` class, thus automatically using faster algorithms for just integers). Then the idea is to use the information about the algorithm to construct arguments of the symengine classes in canonical form and then call the constructor without any checks.

For cases where you can't or don't want to bother constructing in canonical form, we provide high level functions like `add`, `mul`, `pow`, `rational`, where you just provide arguments that are not necessarily in canonical form, and these functions will check and simplify. E.g. `add(x, x)` will check and simplify to `Mul(2, x)`, e.g. you never have the instance `Add(x, x)`. In the same spirit, `rational(2, 1)` will check and convert to `Integer(2)`, e.g. you never have `Rational(2, 1)`.

Summary: always try to construct objects directly using their constructors and all the knowledge that you have for the given algorithm, that way things will be very fast. If you want slower but simpler code, you can use the `add()`, `mul()`, `pow()`, `rational()` functions that perform general and possibly slow canonicalization first.

## Operator Overloading

Ideally, we would like to be able to do:

    RCP<Basic> x  = make_rcp<Symbol>("x");
    RCP<Basic> y  = make_rcp<Symbol>("y");
    RCP<Basic> r = (x + y) + (y + x);
    std::cout << r << std::endl;

But the problem is, that the `+`, `-` and `<<` operations must be defined on the `RCP` objects.
However, just as you should not redefine what `double + double` is, you should not try to redefine operator overloading for an existing type (`RCP`). We can override operators for `Basic` objects, like so:

    ((*x) + (*y)) + ((*y) + (*x))

But here the problem is that the `+` operator only gets access to `Basic`, but it needs access to `RCP<Basic>`
for memory management. In order to allow for operator overloaded types that use dynamic memory allocation, we will need to create our own "handle" classes. It is hard to write a handle class in C++ that are const-correct and clean and simple to use for most C++ developers. It can be done, but it is very hard, especially since
we care about performance. In our opinion, we are better off writing such a layer in Python.
An example of handle classes is [2] --- it is non-const correct, but should give ok performance.


Solution: using non-member non-friend functions is much more clear and much cleaner:

    add(add(x, y), add(y, x))

The function signature of `add` is:

    inline RCP<Basic> add(const RCP<Basic> &a, const RCP<Basic> &b);

For more complicated expressions, instead of `add`, we might also consider
using the naming scheme proposed in [1]. Another advantage of this approach is
that compiler errors are much easier to understand, since it is either finding
our function or it does not, while when overloading operators of our templated
classes (and RCP), any mistake typically results in pages of compiler errors in
gcc.

The Python wrappers then just call this `add` function and provide natural mathematical syntax `(x + y) + (y + x)` at the Python level.


[1] http://trilinos.sandia.gov/packages/docs/dev/packages/thyra/doc/html/LinearAlgebraFunctionConvention.pdf

[2] http://www.math.ttu.edu/~kelong/Playa/html/

