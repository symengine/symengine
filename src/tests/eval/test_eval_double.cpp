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
using SymEngine::Complex;
using SymEngine::Rational;
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

    std::vector<std::pair<RCP<const Basic>, double>> vec = {
        { r1, 0.841470984808 },
        { r2, 0.479425538604 },
        { add(r1, r2), 1.320896523412 },
        { mul(r1, r2), 0.403422680111 },
        { pow(r1, r2), 0.920580670898 },
        { tan(pow(r1, r2)), 1.314847038576 },
        { add(sin(r3), add(cos(r4), add(tan(r3), add(sec(integer(6)), add(csc(r4), cot(r4)))))), 0.387875350057 },
        { add(asin(r3), add(acos(r3), add(atan(r3), add(asec(integer(6)), add(acsc(r4), acot(r4)))))), 3.570293614860 },
        { add(sinh(one), add(cosh(one), add(tanh(one), coth(one)))), 4.792911269914 },
        { add(asinh(r4), add(acosh(r4), add(atanh(r3), acoth(r4)))), 5.010335118942 },
        { SymEngine::abs(log(div(pi, mul(E, integer(2))))), 0.548417294710 },
        { SymEngine::atan2(r1, neg(r2)), 2.08867384922582 }
    };

    for (unsigned i = 0; i < vec.size(); i++) {
        double val = eval_double(*vec[i].first);
        std::cout.precision(12);
        std::cout << vec[i].first->__str__() << " ~ " << val << std::endl;
        assert(::fabs(val - vec[i].second) < 1e-12);
    }

    for (unsigned i = 0; i < vec.size(); i++) {
        double val = eval_double_single_dispatch(*vec[i].first);
        assert(::fabs(val - vec[i].second) < 1e-12);
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

void test_eval_complex_double()
{
    RCP<const Basic> r1, r2, r3, r4;
    r1 = sin(pow(integer(-5), div(integer(1), integer(2))));
    r2 = asin(Complex::from_two_nums(*integer(1), *integer(2)));
    r3 = Complex::from_two_nums(*Rational::from_mpq(mpq_class(3, 5)), *Rational::from_mpq(mpq_class(4, 5)));
    r4 = Complex::from_two_nums(*Rational::from_mpq(mpq_class(5, 13)), *Rational::from_mpq(mpq_class(12, 13)));

    std::vector<std::pair<RCP<const Basic>, std::complex<double>>> vec = {
        { r1, std::complex<double>(0.0, 4.624795545470) },
        { r2, std::complex<double>(0.427078586392476, 1.52857091948100)},
        { sub(add(pow(r1, r2), div(r1, r2)), r1), std::complex<double>(2.63366236495646, -3.81810521824194) },
        { add(sin(r3), add(neg(cos(r4)), add(tan(r3), add(neg(cot(r4)), add(sec(r3), neg(csc(r4))))))),
            std::complex<double>(-0.235180108309931, 4.27879468879223) },
        { add(asin(r3), add(neg(acos(r4)), add(atan(r3), add(neg(acot(r4)), add(asec(r3), neg(acsc(r4))))))),
                    std::complex<double>(0.0, 4.67018141674983) },
        { add(sinh(r3), add(neg(cosh(r4)), add(tanh(r3), neg(coth(r4))))),
                std::complex<double>(0.106911448328291, 1.70522247007143) },
        { add(asinh(r3), add(neg(acosh(r4)), add(atanh(r3), neg(acoth(r4))))),
                std::complex<double>(0.00319030587888230, 0.965754104505096) },
        { log(div(pi, mul(E, r1))),
                std::complex<double>(-1.38670227775307, -1.57079632679490) },
        { abs(r1), std::complex<double>(4.62479554547038, 0.0) }
    };

    for (unsigned i = 0; i < vec.size(); i++) {
        std::complex<double> val = eval_complex_double(*vec[i].first);
        std::cout.precision(12);
        std::cout << vec[i].first->__str__() << " ~ " << val << std::endl;
        assert(std::abs(val.imag() - vec[i].second.imag()) < 1e-12);
        assert(std::abs(val.real() - vec[i].second.real()) < 1e-12);
    }
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_eval_double();

    test_eval_complex_double();

    return 0;
}
