===============
C++ Style Guide
===============

Please follow these guidelines when submitting patches.

Whitespace
==========

Use 4 spaces. Format ``if`` as follows::

    if (d.find(t) == d.end()) {
        d[t] = coef;
    } else {
        d[t] = d[t] + coef;
    }

Pointers
========

Never use raw C++ pointers. Always use the pointers provided by Teuchos.
In cpp you can declare::

    using Teuchos::RCP;
    using Teuchos::Ptr;
    using Teuchos::outArg;
    using Teuchos::rcp;
    using Teuchos::rcp_dynamic_cast;

In header files use the full name.

Initialize as follows (this is the only place where you use ``new``)::

    RCP<Basic> x  = rcp(new Symbol("x"));

Declare functions with two input arguments (and one return value) as follows::

    RCP<Basic> multiply(const RCP<Basic> &a, const RCP<Basic> &b)

Functions with one input and two output arguments are declared::

    void as_coef_term(const RCP<Basic> &self, const Ptr<RCP<Integer>> &coef,
            const Ptr<RCP<Basic>> &term)
    {
        ...
        *coef = rcp(new Integer(1));
        *term = self
        ...
    }

and used as follows::

    RCP<Integer> coef;
    RCP<Basic> t;
    as_coef_term(b, outArg(coef), outArg(t));

You can use dynamic cast as follows::

        RCP<Basic> tmp;
        RCP<Integer> coef;
        coef = rcp_dynamic_cast<Integer>(tmp);


References
==========

You can use C++ references as follows (see the first argument)::

    void Add::dict_add_term(Dict_int &d, const RCP<Integer> &coef,
            const RCP<Basic> &t)
    {
        if (d.find(t) == d.end()) {
            d[t] = coef;
        } else {
            d[t] = d[t] + coef;
        }
    }



Namespaces
==========

Never use "implicit imports": ``using namespace std;``.

In cpp files, either use the full name of the symbol (e.g. ``Teuchos::RCP``),
or use "explicit import" as follows: ``using Teuchos::RCP;``.

In header files, always use the full name (never import symbols there).
