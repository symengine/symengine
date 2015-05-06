#include <cmath>
#include <iostream>

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "visitor.h"
#include "eval_double.h"

using CSymPy::Basic;
using CSymPy::symbol;
using CSymPy::integer;
using CSymPy::one;
using CSymPy::zero;
using CSymPy::pow;
using CSymPy::RCP;
using CSymPy::print_stack_on_segfault;
using CSymPy::levi_civita;
using CSymPy::zeta;
using CSymPy::sin;
using CSymPy::cos;
using CSymPy::tan;
using CSymPy::sec;
using CSymPy::csc;
using CSymPy::cot;
using CSymPy::asin;
using CSymPy::acos;
using CSymPy::atan;
using CSymPy::asec;
using CSymPy::acsc;
using CSymPy::acot;
using CSymPy::sinh;
using CSymPy::cosh;
using CSymPy::tanh;
using CSymPy::coth;
using CSymPy::asinh;
using CSymPy::acosh;
using CSymPy::atanh;
using CSymPy::acoth;
using CSymPy::log;
using CSymPy::pi;
using CSymPy::E;

void test_eval_double()
{
    RCP<const Basic> r1, r2, r3, r4, r5;

    r1 = sin(integer(1));
    assert(::fabs(eval_double(*r1) - 0.841470984808) < 1e-12);
    assert(::fabs(eval_double(*r1) - 0.85) > 1e-12);

    r2 = sin(div(integer(1), integer(2)));
    assert(::fabs(eval_double(*r2) - 0.479425538604) < 1e-12);
    assert(::fabs(eval_double(*r2) - 0.48) > 1e-12);

    r3 = add(r1, r2);
    assert(::fabs(eval_double(*r3) - 1.320896523412) < 1e-12);

    r3 = mul(r1, r2);
    assert(::fabs(eval_double(*r3) - 0.403422680111) < 1e-12);

    r3 = pow(r1, r2);
    assert(::fabs(eval_double(*r3) - 0.920580670898) < 1e-12);

    r3 = tan(pow(r1, r2));
    assert(::fabs(eval_double(*r3) - 1.314847038576) < 1e-12);

    r4 = div(one, integer(5));
    r5 = integer(5);

    r3 = add(asin(r4), add(acos(r4), add(atan(r4), add(asec(r5), add(acsc(r5), acot(r5))))));
    assert(::fabs(eval_double(*r3) - 3.536383773289) < 1e-12);

    r3 = add(sinh(one), add(cosh(one), add(tanh(one), coth(one))));
    assert(::fabs(eval_double(*r3) - 4.792911269914) < 1e-12);

    r3 = add(asinh(r5), add(acosh(r5), add(atanh(r4), acoth(r5))));
    assert(::fabs(eval_double(*r3) - 5.010335118942) < 1e-12);

    r3 = CSymPy::abs(log(div(pi, mul(E, integer(2)))));
    assert(::fabs(eval_double(*r3) - 0.548417294710) < 1e-12);

    // Symbol must raise an exception
    CSYMPY_CHECK_THROW(eval_double(*symbol("x")), std::runtime_error)

    // TODO: this is not implemented yet, so we check that it raises an
    // exception for now
    CSYMPY_CHECK_THROW(eval_double(*levi_civita({r1})), std::runtime_error)
    CSYMPY_CHECK_THROW(eval_double(*zeta(r1, r2)), std::runtime_error)
    // ... we don't test the rest of functions that are not implemented.
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_eval_double();

    return 0;
}
