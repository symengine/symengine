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

void test_eval_double()
{
    RCP<const Basic> r1, r2, r3;

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

    // Symbol must raise an exception
    CSYMPY_CHECK_THROW(eval_double(*symbol("x")), std::runtime_error)

    // TODO: this is not implemented yet, so we check that it raises an
    // exception for now
    CSYMPY_CHECK_THROW(eval_double(*cot(r1)), std::runtime_error)
    CSYMPY_CHECK_THROW(eval_double(*asin(r1)), std::runtime_error)
    CSYMPY_CHECK_THROW(eval_double(*acos(r1)), std::runtime_error)
    CSYMPY_CHECK_THROW(eval_double(*atan(r1)), std::runtime_error)
    // ... we don't test the rest of functions that are not implemented.
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_eval_double();

    return 0;
}
