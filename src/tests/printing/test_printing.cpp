#include <chrono>

#include "basic.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "symbol.h"
#include "rational.h"
#include "complex.h"
#include "add.h"
#include "matrix.h"
#include "printer.h"

using CSymPy::RCP;
using CSymPy::Basic;
using CSymPy::div;
using CSymPy::pow;
using CSymPy::mul;
using CSymPy::integer;
using CSymPy::print_stack_on_segfault;
using CSymPy::symbol;
using CSymPy::Complex;
using CSymPy::Rational;
using CSymPy::Number;
using CSymPy::add;
using CSymPy::Symbol;
using CSymPy::Integer;
using CSymPy::DenseMatrix;
using CSymPy::rcp;
using CSymPy::Subs;
using CSymPy::Derivative;
using CSymPy::function_symbol;
using CSymPy::I;

void test_printing()
{
    RCP<const Basic> r, r1, r2;
    RCP<const Integer> i = integer(-1);
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const Symbol> z  = symbol("z");

    r = div(integer(12), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "(3/49)*196**(1/2)");

    r = mul(integer(12), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "12*196**(1/2)");

    r = mul(integer(23), mul(pow(integer(5), div(integer(1), integer(2))),
        pow(integer(7), div(integer(1), integer(2)))));
    assert(r->__str__() == "23*5**(1/2)*7**(1/2)");

    r = mul(integer(2), pow(symbol("x"), integer(2)));
    assert(r->__str__() == "2*x**2");

    r = mul(integer(23), mul(pow(div(integer(5), integer(2)), div(integer(1), integer(2))),
        pow(div(integer(7), integer(3)), div(integer(1), integer(2)))));
    assert(r->__str__() == "23*(7/3)**(1/2)*(5/2)**(1/2)");
	
    r = pow(div(symbol("x"), integer(2)), div(integer(1), integer(2)));
    assert(r->__str__() == "((1/2)*x)**(1/2)");

    r = pow(div(integer(3), integer(2)),div(integer(1), integer(2)));
    assert(r->__str__() == "(3/2)**(1/2)");	
	
    r1 = mul(integer(12), pow(integer(196), div(integer(-1), integer(2))));
    r2 = mul(integer(294), pow(integer(196), div(integer(-1), integer(2))));
    r = add(integer(-51), mul(r1, r2));
    assert(r->__str__() == "-33");

    r1 = mul(x, i);
    r2 = mul(r1, y);
    assert(r1->__str__() == "-x");
    assert(r1->__str__() != "-1x");
    assert(r2->__str__() == "-x*y");
    assert(r2->__str__() != "-1x*y");

    r = mul(integer(-1), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "-196**(1/2)");
    r = pow(integer(-6), div(integer(1), integer(2)));
    assert(r->__str__() == "(-6)**(1/2)");

    RCP<const Number> rn1, rn2, rn3, c1, c2;
    rn1 = Rational::from_two_ints(integer(2), integer(4));
    rn2 = Rational::from_two_ints(integer(5), integer(7));
    rn3 = Rational::from_two_ints(integer(-5), integer(7));

    c1 = Complex::from_two_rats(static_cast<const Rational&>(*rn1), static_cast<const Rational&>(*rn2));
    c2 = Complex::from_two_rats(static_cast<const Rational&>(*rn1), static_cast<const Rational&>(*rn3));
    r1 = mul(c1, x);
    r2 = mul(c2, x);
    assert(c1->__str__() == "1/2 + 5/7*I");
    assert(c2->__str__() == "1/2 - 5/7*I");
    assert(r1->__str__() == "(1/2 + 5/7*I)*x");
    assert(r2->__str__() == "(1/2 - 5/7*I)*x");
    r1 = pow(x, c1);
    r2 = pow(x, c2);
    assert(r1->__str__() == "x**(1/2 + 5/7*I)");
    assert(r2->__str__() == "x**(1/2 - 5/7*I)");


    c1 = Complex::from_two_nums(*rn1, *rn2);
    c2 = Complex::from_two_nums(*rn1, *rn3);
    assert(c1->__str__() == "1/2 + 5/7*I");
    assert(c2->__str__() == "1/2 - 5/7*I");

    rn1 = Rational::from_two_ints(integer(0), integer(4));
    c1 = Complex::from_two_nums(*rn1, *rn2);
    c2 = Complex::from_two_nums(*rn1, *rn3);
    r1 = mul(c1, x);
    r2 = mul(c2, x);
    assert(c1->__str__() == "5/7*I");
    assert(c2->__str__() == "-5/7*I");
    assert(r1->__str__() == "5/7*I*x");
    assert(r2->__str__() == "-5/7*I*x");
    r1 = pow(x, c1);
    r2 = pow(x, c2);
    assert(r1->__str__() == "x**(5/7*I)");
    assert(r2->__str__() == "x**(-5/7*I)");



    c1 = Complex::from_two_nums(*rn2, *rn1);
    c2 = Complex::from_two_nums(*rn3, *rn1);
    r1 = mul(c1, x);
    r2 = mul(c2, x);
    assert(c1->__str__() == "5/7");
    assert(c2->__str__() == "-5/7");
    assert(r1->__str__() == "(5/7)*x");
    assert(r2->__str__() == "(-5/7)*x");
    r1 = pow(x, c1);
    r2 = pow(x, c2);
    assert(r1->__str__() == "x**(5/7)");
    assert(r2->__str__() == "x**(-5/7)");

    rn1 = Rational::from_two_ints(integer(1), integer(1));
    c1 = Complex::from_two_nums(*rn2, *rn1);
    assert(c1->__str__() == "5/7 + I");
    rn1 = Rational::from_two_ints(integer(-1), integer(1));
    c1 = Complex::from_two_nums(*rn2, *rn1);
    assert(c1->__str__() == "5/7 - I");

    r1 = mul(c1, x);
    assert(r1->__str__() == "(5/7 - I)*x" );

    r1 = mul(integer(2), x);
    assert(r1->__str__() == "2*x" );

    r1 = mul(mul(integer(2), pow(symbol("x"), div(integer(2), integer(3)))), y);
    assert(r1->__str__() == "2*x**(2/3)*y" );

    r1 = mul(x, y);
    assert(r1->__str__() == "x*y" );

    r = div(x, add(x, y));
    r1 = div(x, pow(add(x, y), div(integer(2), integer(3))));
    r2 = div(x, pow(add(x, y), div(integer(-2), integer(3))));
    assert(r->__str__() == "x/(x + y)");
    assert(r1->__str__() == "x/(x + y)**(2/3)");
    assert(r2->__str__() == "x*(x + y)**(2/3)");

    r = div(integer(1), mul(x, add(x, y)));
    r1 = div(mul(y, integer(-1)), mul(x, add(x, y)));
    r2 = mul(pow(y, x), pow(x, y));
    assert(r->__str__() == "1/(x*(x + y))");
    assert(r1->__str__() == "-y/(x*(x + y))");
    assert(r2->__str__() == "x**y*y**x");

    r = pow(y, pow(x, integer(2)));
    r1 = pow(integer(3), mul(integer(2), x));
    r2 = pow(integer(3), mul(integer(-1), x));
    assert(r->__str__() == "y**(x**2)");
    assert(r1->__str__() == "3**(2*x)" );
    assert(r2->__str__() == "3**(-x)" );

    r1 = pow(mul(integer(2), x), y);
    r2 = pow(mul(x, y), z);
    assert(r1->__str__() == "(2*x)**y");
    assert(r2->__str__() == "(x*y)**z");

    r1 = pow(pow(integer(2), x), y);
    r2 = pow(pow(x, y), z);
    assert(r1->__str__() == "(2**x)**y");
    assert(r2->__str__() == "(x**y)**z");

    r = pow(I, x);
    r1 = sub(sub(integer(2), x), y);
    assert(r->__str__() == "I**x");
    assert(r1->__str__() == "2 - x - y");

    RCP<const Basic> f = function_symbol("f", x);
    RCP<const Basic> g = function_symbol("g", x);
    r = f->diff(x);
    r1 = rcp(new Derivative(f, {x}));
    r2 = rcp(new Derivative(g, {x}));

    assert(r->__str__() == "Derivative(f(x), x)");
    assert(r1->__str__() == "Derivative(f(x), x)");
    assert(r2->__str__() == "Derivative(g(x), x)");

    r1 = f->diff(x)->diff(x);
    assert(r1->__str__() == "Derivative(f(x), x, x)");

    f = function_symbol("f", {x, y});
    r = f->diff(x)->diff(y);
    r1 = rcp(new Derivative(f, {x, y}));
    r2 = rcp(new Derivative(f, {y, x}));
    assert(r->__str__() == "Derivative(f(x, y), x, y)");
    assert(r1->__str__() == "Derivative(f(x, y), x, y)");
    assert(r2->__str__() == "Derivative(f(x, y), y, x)");

    r1 = rcp(new Subs(rcp(new Derivative(function_symbol("f", {y, x}), {x})), {{x, add(x, y)}}));
    assert(r1->__str__() == "Subs(Derivative(f(y, x), x), (x), (x + y))");
}

void test_matrix()
{
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0),
        integer(1)});
    assert(A.__str__() == "[1, 0]\n[0, 1]\n");
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_printing();

    test_matrix();

    return 0;
}
