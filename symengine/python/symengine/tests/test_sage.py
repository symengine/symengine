from symengine import (Integer, symbols, sin, cos, pi, E, I,
    function_symbol, DenseMatrix, sympify)


def test_sage_conversions():
    try:
        import sage.all as sage
    except ImportError:
        return

    x, y = sage.SR.var('x y')
    x1, y1 = symbols('x, y')

    # Symbol
    assert x1._sage_() == x
    assert x1 == sympify(x)

    # Integer
    assert Integer(12)._sage_() == sage.Integer(12)
    assert Integer(12) == sympify(sage.Integer(12))

    # Rational
    assert (Integer(1) / 2)._sage_() == sage.Integer(1) / 2
    assert Integer(1) / 2 == sympify(sage.Integer(1) / 2)

    # Operators
    assert x1 + y == x1 + y1
    assert x1 * y == x1 * y1
    assert x1 ** y == x1 ** y1
    assert x1 - y == x1 - y1
    assert x1 / y == x1 / y1

    assert x + y1 == x + y
    assert x * y1 == x * y
    # Doesn't work in Sage 6.1.1ubuntu2
    # assert x ** y1 == x ** y
    assert x - y1 == x - y
    assert x / y1 == x / y

    # Conversions
    assert (x1 + y1)._sage_() == x + y
    assert (x1 * y1)._sage_() == x * y
    assert (x1 ** y1)._sage_() == x ** y
    assert (x1 - y1)._sage_() == x - y
    assert (x1 / y1)._sage_() == x / y

    assert x1 + y1 == sympify(x + y)
    assert x1 * y1 == sympify(x * y)
    assert x1 ** y1 == sympify(x ** y)
    assert x1 - y1 == sympify(x - y)
    assert x1 / y1 == sympify(x / y)

    # Functions
    assert sin(x1) == sin(x)
    assert sin(x1)._sage_() == sage.sin(x)
    assert sin(x1) == sympify(sage.sin(x))

    assert cos(x1) == cos(x)
    assert cos(x1)._sage_() == sage.cos(x)
    assert cos(x1) == sympify(sage.cos(x))

    assert function_symbol('f', x1, y1)._sage_() == sage.function('f', x, y)
    assert function_symbol('f', 2 * x1, x1 + y1).diff(x1)._sage_() == sage.function('f', 2 * x, x + y).diff(x)

    # For the following test, sage needs to be modified
    # assert sage.sin(x) == sage.sin(x1)

    # Constants
    assert pi._sage_() == sage.pi
    assert E._sage_() == sage.e
    assert I._sage_() == sage.I

    assert pi == sympify(sage.pi)
    assert E == sympify(sage.e)

    # SympyConverter does not support converting the following
    # assert I == sympify(sage.I)

    # Matrix
    assert DenseMatrix(1, 2, [x1, y1])._sage_() == sage.matrix([[x, y]])

    # SympyConverter does not support converting the following
    # assert DenseMatrix(1, 2, [x1, y1]) == sympify(sage.matrix([[x, y]]))

# This string contains Sage doctests, that execute all the functions above.
# When you add a new function, add it here as well.
"""
TESTS::
    sage: test_sage_conversions()
"""
