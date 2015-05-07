# C++ Style Guide

Please follow these guidelines when submitting patches.

## Whitespace

Use 4 spaces. Format ``if`` as follows:

    if (d.find(t) == d.end()) {
        d[t] = coef;
    } else {
        d[t] = d[t] + coef;
    }

## Pointers

Never use raw C++ pointers. Always use the smart pointers provided by `csympy_rcp.h`
(depending on WITH_CSYMPY_RCP, those are either Teuchos::RCP, or our own,
faster implementation).

### Declaration

In the `.cpp` files you can declare:

    using SymEngine::RCP;
    using SymEngine::Ptr;
    using SymEngine::outArg;
    using SymEngine::rcp;
    using SymEngine::rcp_dynamic_cast;
    
and then just use `RCP` or `Ptr`.

In the `.h` header files use the full name like `SymEngine::RCP` or `SymEngine::Ptr`.

### Initialization

Initialize as follows:

    RCP<Basic> x  = rcp(new Symbol("x"));

This is the only place where you use the naked ``new``. In fact, you should always be
using the factory functions if they are available, e.g. in this case `symbol()`
as follows:

    RCP<Basic> x  = symbol("x");

This does the same thing, but it's easier to use.

### Freeing

The `RCP` pointer is released automatically. You never call the naked `delete`.

### Passing To/From Functions

Use C++ references for objects that you are **not** passing around. If the object
is *not* modified, use `const A &a`:

    void gcd(const Integer &a, const Integer &b)
    {
        mpz_class g;
        mpz_gcd(g.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());
        ...
    }

If it *is* modified, use `A &a` (see the first argument):

    void Add::dict_add_term(umap_basic_num &d, const RCP<Integer> &coef,
            const RCP<Basic> &t)
    {
        if (d.find(t) == d.end()) {
            d[t] = coef;
        } else {
            d[t] = d[t] + coef;
        }
    }

If the objects **are** passed around, you have to use `RCP`. You also need to
use `RCP` whenever you call some function that uses `RCP`.

Declare functions with two input arguments (and one return value) as follows:

    RCP<const Basic> multiply(const RCP<const Basic> &a,
            const RCP<const Basic> &b)
    {
        ...
        return rcp(new Integer(1));
    }

Functions with one input and two output arguments are declared:

    void as_coef_term(const RCP<const Basic> &self,
        const Ptr<RCP<const Integer>> &coef,
        const Ptr<RCP<const Basic>> &term)
    {
        ...
        *coef = rcp(new Integer(1));
        *term = self
        ...
    }

and used as follows:

    RCP<Integer> coef;
    RCP<Basic> t;
    as_coef_term(b, outArg(coef), outArg(t));

`SymEngine` objects are always immutable, so you always declare them as `const`.
And `RCP` is only used with `SymEngine`'s objects, so you always use `const
RCP<const Integer> &i`. But if the `Integer` was somehow mutable (it's not in
`SymEngine`), you would use `const RCP<Integer> &i`.

For returning objects from functions, simply declare the return type as `RCP<const Basic>` as shown above.

### Casting

You can use dynamic cast as follows:

    RCP<Basic> tmp;
    RCP<Integer> coef;
    coef = rcp_dynamic_cast<Integer>(tmp);

## Namespaces

Never use "implicit imports": ``using namespace std;``.

In cpp files, either use the full name of the symbol (e.g. ``SymEngine::RCP``),
or use "explicit import" as follows: ``using SymEngine::RCP;``.

In header files, always use the full name (never import symbols there).
