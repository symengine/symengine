# Design

Summary of our design decisions and some pointers to literature.

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

For more complicated expressions, instead of `add`, we might also consider using the naming scheme proposed in [1].

The Python wrappers then just call this `add` function and provide natural mathematical syntax `(x + y) + (y + x)` at the Python level.


[1] http://trilinos.sandia.gov/packages/docs/dev/packages/thyra/doc/html/LinearAlgebraFunctionConvention.pdf

[2] http://www.math.ttu.edu/~kelong/Playa/html/
