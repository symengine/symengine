from csympy.lib.csympy_wrapper import have_sage, Integer, Symbol, sin, cos, pi, E, I, \
    function_symbol, DenseMatrix

if have_sage:
    import sage.all as sage
    def test_sage():
        x, y = sage.var('x y')
        x1, y1 = Symbol('x'), Symbol('y')

        assert x1._sage_() == x
        assert Integer(12)._sage_() == sage.Integer(12)
        assert (Integer(1) / Integer(2))._sage_() == sage.Integer(1) / sage.Integer(2)

        assert (x1 + y1)._sage_() == x + y
        assert (x1 * y1)._sage_() == x * y
        assert (x1 ** y1)._sage_() == x ** y
        assert (x1 - y1)._sage_() == x - y

        assert sin(x1)._sage_() == sage.sin(x)
        assert cos(x1)._sage_() == sage.cos(x)

        assert function_symbol('f', x1, y1)._sage_() == sage.function('f', x, y)
        assert function_symbol('f', 2 * x1, x1 + y1).diff(x1)._sage_() == sage.function('f', 2 * x, x + y).diff(x)

        assert pi._sage_() == sage.pi
        assert E._sage_() == sage.e
        assert I._sage_() == sage.I

        assert DenseMatrix(1, 2, [x1, y1])._sage_() == sage.matrix([[x, y]])

# This string contains Sage doctests, that execute all the functions above.
# When you add a new function, please add it here as well.
"""
TESTS::
    sage: test_sage()
"""