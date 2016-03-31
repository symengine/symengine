#include "catch.hpp"
#include <cmath>
#include <iostream>

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/visitor.h>
#include <symengine/eval_double.h>
#include <symengine/eval_mpfr.h>
#include <symengine/eval_mpc.h>

using SymEngine::Basic;
using SymEngine::constant;
using SymEngine::Complex;
using SymEngine::complex_double;
using SymEngine::real_double;
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
using SymEngine::erf;
using SymEngine::log;
using SymEngine::pi;
using SymEngine::E;
using SymEngine::EulerGamma;
using SymEngine::loggamma;
using SymEngine::vec_basic;
using SymEngine::rational_class;
using SymEngine::max;
using SymEngine::min;
using SymEngine::min;

TEST_CASE("eval_double: eval_double", "[eval_double]")
{
    RCP<const Basic> r1, r2, r3, r4, r5;
    r1 = sin(integer(1));
    r2 = sin(div(integer(1), integer(2)));
    r3 = div(one, integer(5));
    r4 = integer(5);

#ifdef HAVE_SYMENGINE_MPFR
    SymEngine::mpfr_class a(100);
    SymEngine::eval_mpfr(a.get_mpfr_t(), *r1, MPFR_RNDN);
    r5 = SymEngine::real_mpfr(std::move(a));
#else
    r5 = SymEngine::real_double(SymEngine::eval_double(*r1));
#endif

    std::vector<std::pair<RCP<const Basic>, double>> vec = {
        {r1, 0.841470984808},
        {r2, 0.479425538604},
        {add(r1, r2), 1.320896523412},
        {mul(r1, r2), 0.403422680111},
        {pow(r1, r2), 0.920580670898},
        {tan(pow(r1, r2)), 1.314847038576},
        {erf(E), 0.9998790689599},
        {add(sin(r3), add(cos(r4), add(tan(r3), add(sec(integer(6)),
                                                    add(csc(r4), cot(r4)))))),
         0.387875350057},
        {add(asin(r3),
             add(acos(r3), add(atan(r3), add(asec(integer(6)),
                                             add(acsc(r4), acot(r4)))))),
         3.570293614860},
        {add(add(sinh(one), add(cosh(one), add(tanh(one), coth(one)))),
             csch(r3)),
         9.759732838729},
        {add(add(add(asinh(r4), add(acosh(r4), add(atanh(r3), acoth(r4)))),
                 csch(r4)),
             acsch(r3)),
         7.336249966045},
        {add(add(sinh(one), add(cosh(one), add(tanh(one), coth(one)))),
             sech(r3)),
         5.773239267559},
        {sub(add(add(asinh(r4), add(acosh(r4), add(atanh(r3), acoth(r4)))),
                 sech(r4)),
             acsch(r4)),
         4.825120290814},
        {SymEngine::abs(log(div(pi, mul(E, integer(2))))), 0.548417294710},
        {SymEngine::atan2(r1, neg(r2)), 2.08867384922582},
        {mul(pi, mul(E, EulerGamma)), 4.92926836742289},
        {pow(mul(EulerGamma, r4), integer(8)), 4813.54354505117582},
        {mul(EulerGamma, integer(10)), 5.7721566490153286},
        {max({r2, r1}), 0.841470984808},
        {min({add(r1, r4), r2}), 0.479425538604},
        {gamma(div(integer(4), integer(3))), 0.892979511569249211},
        {loggamma(div(integer(7), integer(2))), 1.200973602347074224},
        {loggamma(pi), 0.82769459232343710152},
        {add(asech(div(one, integer(2))), real_double(0.1)), 1.41695789692482},
        {r5, 0.841470984807897},
    };

    for (unsigned i = 0; i < vec.size(); i++) {
        double val = eval_double(*vec[i].first);
        std::cout.precision(12);
        std::cout << vec[i].first->__str__() << " ~ " << val << std::endl;
        REQUIRE(::fabs(val - vec[i].second) < 1e-12);
    }

    for (unsigned i = 0; i < vec.size(); i++) {
        double val = eval_double_single_dispatch(*vec[i].first);
        REQUIRE(::fabs(val - vec[i].second) < 1e-12);
    }

    // Symbol must raise an exception
    CHECK_THROWS_AS(eval_double(*symbol("x")), std::runtime_error);
    CHECK_THROWS_AS(eval_double_single_dispatch(*symbol("x")),
                    std::runtime_error);

    // TODO: this is not implemented yet, so we check that it raises an
    // exception for now
    CHECK_THROWS_AS(eval_double(*levi_civita({r1})), std::runtime_error);
    CHECK_THROWS_AS(eval_double_single_dispatch(*levi_civita({r1})),
                    std::runtime_error);

    CHECK_THROWS_AS(eval_double(*zeta(r1, r2)), std::runtime_error);
    CHECK_THROWS_AS(eval_double_single_dispatch(*zeta(r1, r2)),
                    std::runtime_error);

    CHECK_THROWS_AS(eval_double(*constant("dummy")), std::runtime_error);
    CHECK_THROWS_AS(eval_double_single_dispatch(*constant("dummy")),
                    std::runtime_error);
    // ... we don't test the rest of functions that are not implemented.
}

TEST_CASE("eval_complex_double: eval_double", "[eval_double]")
{
    RCP<const Basic> r1, r2, r3, r4, r5;
    r1 = sin(pow(integer(-5), div(integer(1), integer(2))));
    r2 = asin(Complex::from_two_nums(*integer(1), *integer(2)));
    r3 = Complex::from_two_nums(*Rational::from_mpq(rational_class(3, 5)),
                                *Rational::from_mpq(rational_class(4, 5)));
    r4 = Complex::from_two_nums(*Rational::from_mpq(rational_class(5, 13)),
                                *Rational::from_mpq(rational_class(12, 13)));

#ifdef HAVE_SYMENGINE_MPC
    SymEngine::mpc_class a(100);
    SymEngine::eval_mpc(a.get_mpc_t(), *r1, MPFR_RNDN);
    r5 = SymEngine::complex_mpc(std::move(a));
#else
    r5 = SymEngine::complex_double(SymEngine::eval_complex_double(*r1));
#endif

    std::vector<std::pair<RCP<const Basic>, std::complex<double>>> vec = {
        {r1, std::complex<double>(0.0, 4.624795545470)},
        {r5, std::complex<double>(0.0, 4.624795545470)},
        {r2, std::complex<double>(0.427078586392476, 1.52857091948100)},
        {sub(add(pow(r1, r2), div(r1, r2)), r1),
         std::complex<double>(2.63366236495646, -3.81810521824194)},
        {add(sin(r3),
             add(neg(cos(r4)),
                 add(tan(r3), add(neg(cot(r4)), add(sec(r3), neg(csc(r4))))))),
         std::complex<double>(-0.235180108309931, 4.27879468879223)},
        {add(asin(r3), add(neg(acos(r4)),
                           add(atan(r3), add(neg(acot(r4)),
                                             add(asec(r3), neg(acsc(r4))))))),
         std::complex<double>(0.0, 4.67018141674983)},
        {add(add(add(sinh(r3),
                     add(neg(cosh(r4)), add(tanh(r3), neg(coth(r4))))),
                 csch(r3)),
             sech(r4)),
         std::complex<double>(1.8376773309548138, 0.1756408412537389)},
        {add(add(asinh(r3),
                 add(neg(acosh(r4)), add(atanh(r3), neg(acoth(r4))))),
             acsch(r3)),
         std::complex<double>(0.71589877741418859, 0.28103490150281308)},
        {log(div(pi, mul(E, r1))),
         std::complex<double>(-1.38670227775307, -1.57079632679490)},
        {add(abs(r1), complex_double(std::complex<double>(0.1, 0.1))),
         std::complex<double>(4.72479554547038, 0.1)}};

    for (unsigned i = 0; i < vec.size(); i++) {
        std::complex<double> val = eval_complex_double(*vec[i].first);
        std::cout.precision(12);
        std::cout << vec[i].first->__str__() << " ~ " << val << std::endl;
        REQUIRE(std::abs(val.imag() - vec[i].second.imag()) < 1e-12);
        REQUIRE(std::abs(val.real() - vec[i].second.real()) < 1e-12);
    }
}
