from csympy import Symbol, symbols
from csympy.utilities import raises


def test_symbol():
    x = Symbol("x")
    assert str(x) == "x"
    assert str(x) != "y"
    assert repr(x) == str(x)


def test_symbols():
    x = Symbol('x')
    y = Symbol('y')
    z = Symbol('z')

    assert symbols('x') == x
    assert symbols('x ') == x
    assert symbols(' x ') == x
    assert symbols('x,') == (x,)
    assert symbols('x, ') == (x,)
    assert symbols('x ,') == (x,)

    assert symbols('x , y') == (x, y)

    assert symbols('x,y,z') == (x, y, z)
    assert symbols('x y z') == (x, y, z)

    assert symbols('x,y,z,') == (x, y, z)
    assert symbols('x y z ') == (x, y, z)

    xyz = Symbol('xyz')
    abc = Symbol('abc')

    assert symbols('xyz') == xyz
    assert symbols('xyz,') == (xyz,)
    assert symbols('xyz,abc') == (xyz, abc)

    assert symbols(('xyz',)) == (xyz,)
    assert symbols(('xyz,',)) == ((xyz,),)
    assert symbols(('x,y,z,',)) == ((x, y, z),)
    assert symbols(('xyz', 'abc')) == (xyz, abc)
    assert symbols(('xyz,abc',)) == ((xyz, abc),)
    assert symbols(('xyz,abc', 'x,y,z')) == ((xyz, abc), (x, y, z))

    assert symbols(('x', 'y', 'z')) == (x, y, z)
    assert symbols(['x', 'y', 'z']) == [x, y, z]
    assert symbols(set(['x', 'y', 'z'])) == set([x, y, z])

    raises(ValueError, lambda: symbols(''))
    raises(ValueError, lambda: symbols(','))
    raises(ValueError, lambda: symbols('x,,y,,z'))
    raises(ValueError, lambda: symbols(('x', '', 'y', '', 'z')))

    x0 = Symbol('x0')
    x1 = Symbol('x1')
    x2 = Symbol('x2')

    y0 = Symbol('y0')
    y1 = Symbol('y1')

    assert symbols('x0:0') == ()
    assert symbols('x0:1') == (x0,)
    assert symbols('x0:2') == (x0, x1)
    assert symbols('x0:3') == (x0, x1, x2)

    assert symbols('x:0') == ()
    assert symbols('x:1') == (x0,)
    assert symbols('x:2') == (x0, x1)
    assert symbols('x:3') == (x0, x1, x2)

    assert symbols('x1:1') == ()
    assert symbols('x1:2') == (x1,)
    assert symbols('x1:3') == (x1, x2)

    assert symbols('x1:3,x,y,z') == (x1, x2, x, y, z)

    assert symbols('x:3,y:2') == (x0, x1, x2, y0, y1)
    assert symbols(('x:3', 'y:2')) == ((x0, x1, x2), (y0, y1))

    a = Symbol('a')
    b = Symbol('b')
    c = Symbol('c')
    d = Symbol('d')

    assert symbols('x:z') == (x, y, z)
    assert symbols('a:d,x:z') == (a, b, c, d, x, y, z)
    assert symbols(('a:d', 'x:z')) == ((a, b, c, d), (x, y, z))

    aa = Symbol('aa')
    ab = Symbol('ab')
    ac = Symbol('ac')
    ad = Symbol('ad')

    assert symbols('aa:d') == (aa, ab, ac, ad)
    assert symbols('aa:d,x:z') == (aa, ab, ac, ad, x, y, z)
    assert symbols(('aa:d','x:z')) == ((aa, ab, ac, ad), (x, y, z))

    def sym(s):
        return str(symbols(s))
    assert sym('a0:4') == '(a0, a1, a2, a3)'
    assert sym('a2:4,b1:3') == '(a2, a3, b1, b2)'
    assert sym('a1(2:4)') == '(a12, a13)'
    assert sym(('a0:2.0:2')) == '(a0.0, a0.1, a1.0, a1.1)'
    assert sym(('aa:cz')) == '(aaz, abz, acz)'
    assert sym('aa:c0:2') == '(aa0, aa1, ab0, ab1, ac0, ac1)'
    assert sym('aa:ba:b') == '(aaa, aab, aba, abb)'
    assert sym('a:3b') == '(a0b, a1b, a2b)'
    assert sym('a-1:3b') == '(a-1b, a-2b)'
    assert sym('a:2\,:2' + chr(0)) == '(a0,0%s, a0,1%s, a1,0%s, a1,1%s)' % (
        (chr(0),)*4)
    assert sym('x(:a:3)') == '(x(a0), x(a1), x(a2))'
    assert sym('x(:c):1') == '(xa0, xb0, xc0)'
    assert sym('x((:a)):3') == '(x(a)0, x(a)1, x(a)2)'
    assert sym('x(:a:3') == '(x(a0, x(a1, x(a2)'
    assert sym(':2') == '(0, 1)'
    assert sym(':b') == '(a, b)'
    assert sym(':b:2') == '(a0, a1, b0, b1)'
    assert sym(':2:2') == '(00, 01, 10, 11)'
    assert sym(':b:b') == '(aa, ab, ba, bb)'

    raises(ValueError, lambda: symbols(':'))
    raises(ValueError, lambda: symbols('a:'))
    raises(ValueError, lambda: symbols('::'))
    raises(ValueError, lambda: symbols('a::'))
    raises(ValueError, lambda: symbols(':a:'))
    raises(ValueError, lambda: symbols('::a'))
