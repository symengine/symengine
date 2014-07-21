#include <chrono>

#include "basic.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "symbol.h"
#include "rational.h"
#include "complex.h"
#include "add.h"

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

void test_printing()
{
    RCP<const Basic> r, r1, r2;
    RCP<const Integer> i = integer(-1);
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");

    r = div(integer(12), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "3/49*196^(1/2)");

    r = mul(integer(12), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "12*196^(1/2)");

    r = mul(integer(23), mul(pow(integer(5), div(integer(1), integer(2))),
        pow(integer(7), div(integer(1), integer(2)))));
    assert(r->__str__() == "23*5^(1/2)*7^(1/2)");

    r = mul(integer(2), pow(symbol("x"), integer(2)));
    assert(r->__str__() == "2x^2");

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
    assert(r->__str__() == "-196^(1/2)");
    r = pow(integer(-6), div(integer(1), integer(2)));
    assert(r->__str__() == "(-6)^1/2");

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
    assert(r1->__str__() == "x^(1/2 + 5/7*I)");
    assert(r2->__str__() == "x^(1/2 - 5/7*I)");


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
    assert(r1->__str__() == "x^(5/7*I)");
    assert(r2->__str__() == "x^(-5/7*I)");



    c1 = Complex::from_two_nums(*rn2, *rn1);
    c2 = Complex::from_two_nums(*rn3, *rn1);
    r1 = mul(c1, x);
    r2 = mul(c2, x);
    assert(c1->__str__() == "5/7");
    assert(c2->__str__() == "-5/7");
    assert(r1->__str__() == "5/7x");
    assert(r2->__str__() == "-5/7x");
    r1 = pow(x, c1);
    r2 = pow(x, c2);
    assert(r1->__str__() == "x^5/7");
    assert(r2->__str__() == "x^(-5/7)");

    rn1 = Rational::from_two_ints(integer(1), integer(1));
    c1 = Complex::from_two_nums(*rn2, *rn1);
    assert(c1->__str__() == "5/7 + I");
    rn1 = Rational::from_two_ints(integer(-1), integer(1));
    c1 = Complex::from_two_nums(*rn2, *rn1);
    assert(c1->__str__() == "5/7 - I");


}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_printing();

    return 0;
}
