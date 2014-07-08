#include <chrono>

#include "basic.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "symbol.h"
#include "rational.h"
#include "complex.h"

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

void test_printing()
{
    RCP<const Basic> r;

    r = div(integer(12), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "12*196^(-1/2)");

    r = mul(integer(12), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "12*196^(1/2)");

    r = mul(integer(23), mul(pow(integer(5), div(integer(1), integer(2))),
        pow(integer(7), div(integer(1), integer(2)))));
    assert(r->__str__() == "23*5^(1/2)*7^(1/2)");

    r = mul(integer(2), pow(symbol("x"), integer(2)));
    assert(r->__str__() == "2x^2");

    RCP<const Number> r1, r2, r3, c1, c2;
    r1 = Rational::from_two_ints(integer(2), integer(4));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(-5), integer(7));

    c1 = Complex::from_two_rats(static_cast<const Rational&>(*r1), static_cast<const Rational&>(*r2));
    c2 = Complex::from_two_rats(static_cast<const Rational&>(*r1), static_cast<const Rational&>(*r3));
    assert(c1->__str__() == "1/2 + i5/7");
    assert(c2->__str__() == "1/2 - i5/7");

    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    assert(c1->__str__() == "1/2 + i5/7");
    assert(c2->__str__() == "1/2 - i5/7");

}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_printing();

    return 0;
}
