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

using SymEngine::Basic;
using SymEngine::symbol;
using SymEngine::integer;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::pow;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;
using SymEngine::levi_civita;
using SymEngine::zeta;
using SymEngine::sin;
using SymEngine::cos;
using SymEngine::tan;
using SymEngine::sec;
using SymEngine::csc;
using SymEngine::cot;
using SymEngine::asin;
using SymEngine::acos;
using SymEngine::atan;
using SymEngine::asec;
using SymEngine::acsc;
using SymEngine::acot;
using SymEngine::sinh;
using SymEngine::cosh;
using SymEngine::tanh;
using SymEngine::coth;
using SymEngine::asinh;
using SymEngine::acosh;
using SymEngine::atanh;
using SymEngine::acoth;
using SymEngine::log;
using SymEngine::pi;
using SymEngine::E;
using SymEngine::vec_basic;

void test_eval_double()
{
    RCP<const Basic> r1, r2, r3, r4;
    r1 = sin(integer(1));
    r2 = sin(div(integer(1), integer(2)));
    r3 = div(one, integer(5));
    r4 = integer(5);

    vec_basic vec;
    std::vector<double> result;

    vec.push_back(r1);
    result.push_back(0.841470984808);

    vec.push_back(r2);
    result.push_back(0.479425538604);

    vec.push_back(add(r1, r2));
    result.push_back(1.320896523412);

    vec.push_back(mul(r1, r2));
    result.push_back(0.403422680111);

    vec.push_back(pow(r1, r2));
    result.push_back(0.920580670898);

    vec.push_back(tan(pow(r1, r2)));
    result.push_back(1.314847038576);

    vec.push_back(add(asin(r3), add(acos(r3), add(atan(r3), add(asec(r4), add(acsc(r4), acot(r4)))))));
    result.push_back(3.536383773289);

    vec.push_back(add(sinh(one), add(cosh(one), add(tanh(one), coth(one)))));
    result.push_back(4.792911269914);

    vec.push_back(add(asinh(r4), add(acosh(r4), add(atanh(r3), acoth(r4)))));
    result.push_back(5.010335118942);

    vec.push_back(SymEngine::abs(log(div(pi, mul(E, integer(2))))));
    result.push_back(0.548417294710);

    for (unsigned i = 0; i < vec.size(); i++) {
        double val = eval_double(*vec[i]);
        std::cout.precision(12);
        std::cout << vec[i]->__str__() << " ~ " << val << std::endl;
        assert(::fabs(val - result[i]) < 1e-12);
    }

    for (unsigned i = 0; i < vec.size(); i++) {
        double val = eval_double_single_dispatch(*vec[i]);
        assert(::fabs(val - result[i]) < 1e-12);
    }

    // Symbol must raise an exception
    SYMENGINE_CHECK_THROW(eval_double(*symbol("x")), std::runtime_error)
    SYMENGINE_CHECK_THROW(eval_double_single_dispatch(*symbol("x")), std::runtime_error)

    // TODO: this is not implemented yet, so we check that it raises an
    // exception for now
    SYMENGINE_CHECK_THROW(eval_double(*levi_civita({r1})), std::runtime_error)
    SYMENGINE_CHECK_THROW(eval_double_single_dispatch(*levi_civita({r1})), std::runtime_error)

    SYMENGINE_CHECK_THROW(eval_double(*zeta(r1, r2)), std::runtime_error)
    SYMENGINE_CHECK_THROW(eval_double_single_dispatch(*zeta(r1, r2)), std::runtime_error)
    // ... we don't test the rest of functions that are not implemented.
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_eval_double();

    return 0;
}
