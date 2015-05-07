# Design

Summary of our design decisions and some pointers to literature.

## Reference Counted Pointers

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
`src/csympy_rcp.h`). Our implementation is faster, but it only implements a
subset of all the functinoality and it requires all our objects to have a
`refcount_` variable. Otherwise the usage of our RCP is identical to
`Teuchos::RCP`, and Travis-CI tests both implementations of RCP to make sure
the code works with both.


## Operator Overloading

Ideally, we would like to be able to do:

    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));
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
