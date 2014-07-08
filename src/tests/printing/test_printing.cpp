#include <chrono>

#include "basic.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "symbol.h"
#include "add.h"
#include "matrix.h"

using CSymPy::RCP;
using CSymPy::Basic;
using CSymPy::div;
using CSymPy::pow;
using CSymPy::mul;
using CSymPy::integer;
using CSymPy::print_stack_on_segfault;
using CSymPy::symbol;
using CSymPy::add;
using CSymPy::Symbol;
using CSymPy::Integer;
using CSymPy::DenseMatrix;

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
