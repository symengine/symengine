#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>
#include <symengine/real_mpfr.h>
#include <symengine/complex_mpc.h>
#include <symengine/eval_double.h>

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Pow;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::umap_basic_num;
using SymEngine::map_vec_int;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::multinomial_coefficients;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::sin;
using SymEngine::cos;
using SymEngine::tan;
using SymEngine::cot;
using SymEngine::csc;
using SymEngine::sec;
using SymEngine::asin;
using SymEngine::acos;
using SymEngine::asec;
using SymEngine::acsc;
using SymEngine::atan;
using SymEngine::acot;
using SymEngine::atan2;
using SymEngine::lambertw;
using SymEngine::log;
using SymEngine::exp;
using SymEngine::function_symbol;
using SymEngine::Derivative;
using SymEngine::pi;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::sqrt;
using SymEngine::sinh;
using SymEngine::cosh;
using SymEngine::tanh;
using SymEngine::coth;
using SymEngine::asinh;
using SymEngine::acosh;
using SymEngine::atanh;
using SymEngine::acoth;
using SymEngine::asech;
using SymEngine::kronecker_delta;
using SymEngine::levi_civita;
using SymEngine::zeta;
using SymEngine::dirichlet_eta;
using SymEngine::gamma;
using SymEngine::lowergamma;
using SymEngine::uppergamma;
using SymEngine::abs;
using SymEngine::Subs;
using SymEngine::FunctionWrapper;
using SymEngine::vec_basic;
using SymEngine::real_double;
using SymEngine::complex_double;
using SymEngine::RealDouble;
using SymEngine::ComplexDouble;
using SymEngine::Number;
using SymEngine::eval_double;
using SymEngine::is_a;
using SymEngine::neg;

#ifdef HAVE_SYMENGINE_MPFR
using SymEngine::real_mpfr;
using SymEngine::RealMPFR;
using SymEngine::mpfr_class;
#endif

#ifdef HAVE_SYMENGINE_MPC
using SymEngine::complex_mpc;
using SymEngine::ComplexMPC;
using SymEngine::mpc_class;
#endif

TEST_CASE("Sin: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");

    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = sin(x);
    r2 = sin(x);
    std::cout << *r1 << std::endl;

    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *zero));

    r1 = sin(zero);
    r2 = zero;
    std::cout << *r1 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = sin(x)->diff(x);
    r2 = cos(x);
    REQUIRE(eq(*r1, *r2));

    r1 = mul(i2, x)->diff(x);
    r2 = i2;
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = sin(mul(i2, x))->diff(x);
    r2 = mul(i2, cos(mul(i2, x)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = mul(x, sin(x))->diff(x);
    r2 = add(sin(x), mul(x, cos(x)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = mul(x, sin(x))->diff(x)->diff(x);
    r2 = add(mul(i2, cos(x)), neg(mul(x, sin(x))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = mul(sin(x), cos(x))->diff(x);
    r2 = sub(pow(cos(x), i2), pow(sin(x), i2));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    // sin(-y) = -sin(y)
    r1 = sin(mul(im1, y));
    r2 = mul(im1, sin(y));
    REQUIRE(eq(*r1, *r2));

    // sin(pi - y) = sin(y)
    r1 = sin(sub(pi, y));
    r2 = sin(y);
    REQUIRE(eq(*r1, *r2));

    // sin(pi + y) = -sin(y)
    r1 = sin(add(pi, y));
    r2 = mul(im1, sin(y));
    REQUIRE(eq(*r1, *r2));

    // sin(2*pi - y) = -sin(y)
    r1 = sin(sub(mul(i2, pi), y));
    r2 = mul(im1, sin(y));
    REQUIRE(eq(*r1, *r2));

    // sin(12*pi + y) = sin(y)
    r1 = sin(add(mul(i12, pi), y));
    r2 = sin(y);
    REQUIRE(eq(*r1, *r2));

    // sin(3*pi - y) = sin(y)
    r1 = sin(sub(mul(i3, pi), y));
    r2 = sin(y);
    REQUIRE(eq(*r1, *r2));

    // sin(pi/2 + y) = cos(y)
    r1 = sin(add(div(pi, i2), y));
    r2 = cos(y);
    REQUIRE(eq(*r1, *r2));

    // sin(pi/2 - y) = cos(y)
    r1 = sin(sub(div(pi, i2), y));
    r2 = cos(y);
    REQUIRE(eq(*r1, *r2));

    // sin(-pi/2 - y) = -cos(y)
    r1 = sin(sub(neg(div(pi, i2)), y));
    r2 = neg(cos(y));
    REQUIRE(eq(*r1, *r2));

    // sin(12*pi + y + pi/2) = cos(y)
    r1 = sin(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = cos(y);
    REQUIRE(eq(*r1, *r2));

    // sin(12*pi - y + pi/2) = cos(y)
    r1 = sin(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = cos(y);
    REQUIRE(eq(*r1, *r2));

    r1 = sin(real_double(1.0));
    r2 = sin(sub(div(pi, i2), real_double(2.0)));
    REQUIRE(is_a<RealDouble>(*r1));
    REQUIRE(is_a<RealDouble>(*r2));
    REQUIRE(std::abs(static_cast<const RealDouble &>(*r1).i - 0.841470984807897) < 1e-12);
    REQUIRE(std::abs(static_cast<const RealDouble &>(*r2).i + 0.416146836547142) < 1e-12);
}

TEST_CASE("Cos: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = cos(x);
    r2 = cos(x);
    std::cout << *r1 << std::endl;

    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *zero));

    r1 = cos(zero);
    r2 = one;
    REQUIRE(eq(*r1, *r2));

    r1 = cos(x)->diff(x);
    r2 = mul(im1, sin(x));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    // cos(-pi) = -1
    r1 = cos(neg(pi));
    r2 = im1;
    REQUIRE(eq(*r1, *r2));

    // cos(-y) = cos(y)
    r1 = cos(mul(im1, y));
    r2 = cos(y);
    REQUIRE(eq(*r1, *r2));

    // cos(pi - y) = -cos(y)
    r1 = cos(sub(pi, y));
    r2 = mul(im1, cos(y));
    REQUIRE(eq(*r1, *r2));

    // cos(pi + y) = -cos(y)
    r1 = cos(add(pi, y));
    r2 = mul(im1, cos(y));
    REQUIRE(eq(*r1, *r2));

    // cos(2*pi - y) = cos(y)
    r1 = cos(sub(mul(i2, pi), y));
    r2 = cos(y);
    REQUIRE(eq(*r1, *r2));

    // cos(12*pi + y) = cos(y)
    r1 = cos(add(mul(i12, pi), y));
    r2 = cos(y);
    REQUIRE(eq(*r1, *r2));

    // cos(3*pi - y) = -cos(y)
    r1 = cos(sub(mul(i3, pi), y));
    r2 = mul(im1, cos(y));
    REQUIRE(eq(*r1, *r2));

    // cos(pi/2 + y) = -sin(y)
    r1 = cos(add(div(pi, i2), y));
    r2 = mul(im1, sin(y));
    REQUIRE(eq(*r1, *r2));

    // cos(pi/2 - y) = sin(y)
    r1 = cos(sub(div(pi, i2), y));
    r2 = sin(y);
    REQUIRE(eq(*r1, *r2));

    // cos(12*pi + y + pi/2) = -sin(y)
    r1 = cos(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = mul(im1, sin(y));
    REQUIRE(eq(*r1, *r2));

    // cos(12*pi - y + pi/2) = sin(y)
    r1 = cos(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = sin(y);
    REQUIRE(eq(*r1, *r2));

    r1 = cos(real_double(1.0));
    r2 = cos(sub(div(pi, i2), real_double(2.0)));
    REQUIRE(is_a<RealDouble>(*r1));
    REQUIRE(is_a<RealDouble>(*r2));
    REQUIRE(std::abs(static_cast<const RealDouble &>(*r1).i - 0.540302305868140) < 1e-12);
    REQUIRE(std::abs(static_cast<const RealDouble &>(*r2).i - 0.909297426825682) < 1e-12);
}

TEST_CASE("Tan: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);


    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = tan(x);
    r2 = tan(x);
    std::cout << *r1 << std::endl;

    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *zero));

    r1 = tan(zero);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = tan(x)->diff(x);
    r2 = add(pow(tan(x), i2), i1);
    REQUIRE(eq(*r1, *r2));

    r1 = tan(mul(i2, x))->diff(x);
    r2 = mul(i2, add(pow(tan(mul(i2, x)), i2), i1));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = mul(x, tan(x))->diff(x);
    r2 = add(tan(x), mul(x, add(pow(tan(x), i2), i1)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    // tan(-y) = -tan(y)
    r1 = tan(mul(im1, y));
    r2 = mul(im1, tan(y));
    REQUIRE(eq(*r1, *r2));

    // tan(pi - y) = -tan(y)
    r1 = tan(sub(pi, y));
    r2 = mul(im1, tan(y));
    REQUIRE(eq(*r1, *r2));

    // tan(pi + y) = -tan(y)
    r1 = tan(add(pi, y));
    r2 = tan(y);
    REQUIRE(eq(*r1, *r2));

    // tan(2*pi - y) = -tan(y)
    r1 = tan(sub(mul(i2, pi), y));
    r2 = mul(im1, tan(y));
    REQUIRE(eq(*r1, *r2));

    // tan(12*pi + y) = tan(y)
    r1 = tan(add(mul(i12, pi), y));
    r2 = tan(y);
    REQUIRE(eq(*r1, *r2));

    // tan(3*pi - y) = -tan(y)
    r1 = tan(sub(mul(i3, pi), y));
    r2 = mul(im1, tan(y));
    REQUIRE(eq(*r1, *r2));

    // tan(pi/2 + y) = -cot(y)
    r1 = tan(add(div(pi, i2), y));
    r2 = mul(im1, cot(y));
    REQUIRE(eq(*r1, *r2));

    // tan(pi/2 - y) = cot(y)
    r1 = tan(sub(div(pi, i2), y));
    r2 = cot(y);
    REQUIRE(eq(*r1, *r2));

    // tan(12*pi + y + pi/2) = -cot(y)
    r1 = tan(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = mul(im1, cot(y));
    REQUIRE(eq(*r1, *r2));

    // tan(12*pi - y + pi/2) = cot(y)
    r1 = tan(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = cot(y);
    REQUIRE(eq(*r1, *r2));

}

TEST_CASE("Cot: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = cot(x);
    r2 = cot(x);
    std::cout << *r1 << std::endl;

    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *zero));

    r1 = cot(x)->diff(x);
    r2 = mul(im1, add(pow(cot(x), i2), i1));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = cot(mul(i2, x))->diff(x);
    r2 = mul(integer(-2), add(pow(cot(mul(i2, x)), i2), i1));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = mul(x, cot(x))->diff(x);
    r2 = add(cot(x), mul(x, mul(add(pow(cot(x), i2), i1), im1)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    // cot(-y) = -cot(y)
    r1 = cot(mul(im1, y));
    r2 = mul(im1, cot(y));
    REQUIRE(eq(*r1, *r2));

    // cot(pi - y) = -cot(y)
    r1 = cot(sub(pi, y));
    r2 = mul(im1, cot(y));
    REQUIRE(eq(*r1, *r2));

    // cot(pi + y) = -cot(y)
    r1 = cot(add(pi, y));
    r2 = cot(y);
    REQUIRE(eq(*r1, *r2));

    // cot(2*pi - y) = -cot(y)
    r1 = cot(sub(mul(i2, pi), y));
    r2 = mul(im1, cot(y));
    REQUIRE(eq(*r1, *r2));

    // cot(12*pi + y) = cot(y)
    r1 = cot(add(mul(i12, pi), y));
    r2 = cot(y);
    REQUIRE(eq(*r1, *r2));

    // cot(3*pi - y) = -cot(y)
    r1 = cot(sub(mul(i3, pi), y));
    r2 = mul(im1, cot(y));
    REQUIRE(eq(*r1, *r2));

    // cot(pi/2 + y) = -tan(y)
    r1 = cot(add(div(pi, i2), y));
    r2 = mul(im1, tan(y));
    REQUIRE(eq(*r1, *r2));

    // cot(pi/2 - y) = cot(y)
    r1 = cot(sub(div(pi, i2), y));
    r2 = tan(y);
    REQUIRE(eq(*r1, *r2));

    // cot(12*pi + y + pi/2) = -tan(y)
    r1 = cot(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = mul(im1, tan(y));
    REQUIRE(eq(*r1, *r2));

    // cot(12*pi - y + pi/2) = tan(y)
    r1 = cot(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = tan(y);
    REQUIRE(eq(*r1, *r2));


}

TEST_CASE("Csc: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = csc(x);
    r2 = csc(x);
    std::cout << *r1 << std::endl;

    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *zero));

    r1 = csc(x)->diff(x);
    r2 = mul(im1, mul(cot(x), csc(x)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = csc(mul(i2, x))->diff(x);
    r2 = mul(integer(-2), mul(cot(mul(i2, x)), csc(mul(i2, x))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = mul(x, csc(x))->diff(x);
    r2 = add(csc(x), mul(x, mul(im1, mul(cot(x), csc(x)))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    // csc(-y) = -csc(y)
    r1 = csc(mul(im1, y));
    r2 = mul(im1, csc(y));
    REQUIRE(eq(*r1, *r2));

    // csc(pi - y) = csc(y)
    r1 = csc(sub(pi, y));
    r2 = csc(y);
    REQUIRE(eq(*r1, *r2));

    // csc(pi + y) = -csc(y)
    r1 = csc(add(pi, y));
    r2 = mul(im1, csc(y));
    REQUIRE(eq(*r1, *r2));

    // csc(2*pi - y) = -csc(y)
    r1 = csc(sub(mul(i2, pi), y));
    r2 = mul(im1, csc(y));
    REQUIRE(eq(*r1, *r2));

    // csc(12*pi + y) = csc(y)
    r1 = csc(add(mul(i12, pi), y));
    r2 = csc(y);
    REQUIRE(eq(*r1, *r2));

    // csc(3*pi - y) = csc(y)
    r1 = csc(sub(mul(i3, pi), y));
    r2 = csc(y);
    REQUIRE(eq(*r1, *r2));

    // csc(pi/2 + y) = sec(y)
    r1 = csc(add(div(pi, i2), y));
    r2 = sec(y);
    REQUIRE(eq(*r1, *r2));

    // csc(pi/2 - y) = sec(y)
    r1 = csc(sub(div(pi, i2), y));
    r2 = sec(y);
    REQUIRE(eq(*r1, *r2));

    // csc(12*pi + y + pi/2) = sec(y)
    r1 = csc(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = sec(y);
    REQUIRE(eq(*r1, *r2));

    // csc(12*pi - y + pi/2) = sec(y)
    r1 = csc(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = sec(y);
    REQUIRE(eq(*r1, *r2));

}

TEST_CASE("Sec: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = sec(x);
    r2 = sec(x);
    std::cout << *r1 << std::endl;

    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *zero));

    r1 = sec(x)->diff(x);
    r2 = mul(tan(x), sec(x));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = sec(mul(i2, x))->diff(x);
    r2 = mul(i2, mul(tan(mul(i2, x)), sec(mul(i2, x))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = mul(x, sec(x))->diff(x);
    r2 = add(sec(x), mul(x, mul(tan(x), sec(x))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));

    // sec(-y) = sec(y)
    r1 = sec(mul(im1, y));
    r2 = sec(y);
    REQUIRE(eq(*r1, *r2));

    // sec(pi - y) = -sec(y)
    r1 = sec(sub(pi, y));
    r2 = mul(im1, sec(y));
    REQUIRE(eq(*r1, *r2));

    // sec(pi + y) = -sec(y)
    r1 = sec(add(pi, y));
    r2 = mul(im1, sec(y));
    REQUIRE(eq(*r1, *r2));

    // sec(2*pi - y) = sec(y)
    r1 = sec(sub(mul(i2, pi), y));
    r2 = sec(y);
    REQUIRE(eq(*r1, *r2));

    // sec(12*pi + y) = sec(y)
    r1 = sec(add(mul(i12, pi), y));
    r2 = sec(y);
    REQUIRE(eq(*r1, *r2));

    // sec(3*pi - y) = -sec(y)
    r1 = sec(sub(mul(i3, pi), y));
    r2 = mul(im1, sec(y));
    REQUIRE(eq(*r1, *r2));

    // sec(pi/2 + y) = -csc(y)
    r1 = sec(add(div(pi, i2), y));
    r2 = mul(im1, csc(y));
    REQUIRE(eq(*r1, *r2));

    // sec(pi/2 - y) = csc(y)
    r1 = sec(sub(div(pi, i2), y));
    r2 = csc(y);
    REQUIRE(eq(*r1, *r2));

    // sec(12*pi + y + pi/2) = -csc(y)
    r1 = sec(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = mul(im1, csc(y));
    REQUIRE(eq(*r1, *r2));

    // sec(12*pi - y + pi/2) = csc(y)
    r1 = sec(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = csc(y);
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("function_symbol: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = function_symbol("f", x);
    r2 = function_symbol("f", x);
    std::cout << *r1 << std::endl;

    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *zero));

    r1 = function_symbol("f", x);
    r2 = function_symbol("g", x);
    REQUIRE(neq(*r1, *r2));

    r1 = function_symbol("f", x);
    r2 = function_symbol("f", y);
    REQUIRE(neq(*r1, *r2));

    r1 = function_symbol("f", {x, y});
    r2 = function_symbol("f", {x, y});
    REQUIRE(eq(*r1, *r2));

    r1 = function_symbol("f", {x, y});
    r2 = function_symbol("f", {y, x});
    REQUIRE(neq(*r1, *r2));

    r1 = function_symbol("f", {x, y});
    r2 = function_symbol("f", x);
    REQUIRE(neq(*r1, *r2));

    r1 = function_symbol("f", zero);
    r2 = one;
    REQUIRE(neq(*r1, *r2));

    r1 = function_symbol("f", x)->diff(y);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = function_symbol("f", {x, y})->diff(z);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = mul(i2, pow(function_symbol("f", add(add(x, y), z)), i2));
    r2 = mul(i2, pow(function_symbol("f", add(add(y, z), x)), i2));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Derivative: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> _x = symbol("_x");
    RCP<const Symbol> __x = symbol("__x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> f = function_symbol("f", x);
    RCP<const Basic> g = function_symbol("g", x);

    RCP<const Basic> r1, r2, r3;

    r1 = f->diff(x);
    r2 = Derivative::create(f, {x});
    r3 = Derivative::create(g, {x});
    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *r3));
    REQUIRE(neq(*r2, *r3));
    REQUIRE(vec_basic_eq(r1->get_args(), {f, x}));

    r1 = f->diff(x)->diff(x);
    r2 = Derivative::create(f, {x, x});
    REQUIRE(eq(*r1, *r2));
    REQUIRE(vec_basic_eq(r1->get_args(), {f, x, x}));

    f = function_symbol("f", {x, y});
    r1 = f->diff(x)->diff(y);
    r2 = Derivative::create(f, {x, y});
    r3 = Derivative::create(f, {y, x});
    REQUIRE(eq(*r1, *r2));
    REQUIRE(neq(*r1, *r3));

    f = function_symbol("f", pow(x, integer(2)));
    r1 = f->diff(x);
    std::cout << *f << " " << *r1 << std::endl;
    r2 = Derivative::create(function_symbol("f", _x), {_x});
    r2 = Subs::create(r2, {{_x, pow(x, integer(2))}});
    REQUIRE(eq(*r1, *mul(mul(integer(2), x), r2)));

    f = function_symbol("f", {x, x});
    r1 = f->diff(x);
    std::cout << *f << " " << *r1 << std::endl;
    r2 = Derivative::create(function_symbol("f", {_x, x}), {_x});
    r2 = Subs::create(r2, {{_x, x}});
    r3 = Derivative::create(function_symbol("f", {x, _x}), {_x});
    r3 = Subs::create(r3, {{_x, x}});
    REQUIRE(eq(*r1, *add(r2, r3)));

    f = function_symbol("f", {y, add(x, y)});
    r1 = f->diff(x);
    std::cout << *f << " " << *r1 << std::endl;
    r2 = Derivative::create(function_symbol("f", {y, _x}), {_x});
    r2 = Subs::create(r2, {{_x, add(y, x)}});
    REQUIRE(eq(*r1, *r2));

    r1 = function_symbol("f", add(_x, x))->diff(_x);
    std::cout << *f << " " << *r1 << std::endl;
    r2 = Subs::create(Derivative::create(function_symbol("f", __x), {__x}), {{__x, add(_x, x)}});
    REQUIRE(eq(*r1, *r2));

    // Test is_canonical()
    f = function_symbol("f", x);
    RCP<const Derivative> r4 = Derivative::create(f, {x});
    REQUIRE(r4->is_canonical(function_symbol("f", {y, x}), {x}));
    REQUIRE(not r4->is_canonical(function_symbol("f", y), {x}));
    REQUIRE(r4->is_canonical(function_symbol("f", x), {x, y, x, x}));
    REQUIRE(not (r4->is_canonical(function_symbol("f", x), {pow(x, integer(2))})));

    // Test get_args()
    r1 = Derivative::create(function_symbol("f", {x, pow(y, integer(2))}), {x, x, y});
    REQUIRE(vec_basic_eq(r1->get_args(), {function_symbol("f", {x, pow(y, integer(2))}), x, x, y}));

    // Test Derivative::subs
    r1 = Derivative::create(function_symbol("f", {x, add(y, y)}), {x});
    r2 = r1->subs({{x, y}});
    r3 = Subs::create(Derivative::create(function_symbol("f", {x, add(y, y)}), {x}), {{x, y}});
    REQUIRE(eq(*r2, *r3));

    r2 = r1->subs({{x, z}});
    r3 = Derivative::create(function_symbol("f", {z, add(y, y)}), {z});
    REQUIRE(eq(*r2, *r3));

    r2 = r1->subs({{y, z}});
    r3 = Derivative::create(function_symbol("f", {x, add(z, z)}), {x});
    REQUIRE(eq(*r2, *r3));
}

TEST_CASE("Subs: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> _x = symbol("_x");
    RCP<const Basic> r1, r2, r3, r4;

    // Test Subs::subs
    r1 = Subs::create(Derivative::create(function_symbol("f", {y, x}), {x}), {{x, add(x, y)}});
    r2 = Subs::create(Derivative::create(function_symbol("f", {y, x}), {x}), {{x, z}, {y, z}});
    r3 = Subs::create(Derivative::create(function_symbol("f", {y, x}), {x}), {{y, z}, {x, z}});
    REQUIRE(eq(*r2, *r3));

    r2 = r1->subs({{y, z}});
    r3 = Subs::create(Derivative::create(function_symbol("f", {z, x}), {x}), {{x, add(x, z)}});
    REQUIRE(eq(*r2, *r3));

    r2 = r1->subs({{x, z}});
    r3 = Subs::create(Derivative::create(function_symbol("f", {y, x}), {x}), {{x, add(z, y)}});
    REQUIRE(eq(*r2, *r3));

    r2 = r1->subs({{r1, r3}});
    REQUIRE(eq(*r2, *r3));

    // Test Subs::diff
    r1 = function_symbol("f", {add(y, y), add(x, y)})->diff(x);

    r2 = r1->diff(_x);
    r3 = zero;
    REQUIRE(eq(*r2, *r3));

    r2 = r1->diff(x);
    r3 = Subs::create(Derivative::create(function_symbol("f", {add(y, y), _x}), {_x, _x}),
                        {{_x, add(x, y)}});
    REQUIRE(eq(*r2, *r3));

    r2 = r1->diff(y);
    r3 = Subs::create(Derivative::create(function_symbol("f", {add(y, y), _x}), {_x, _x}),
                        {{_x, add(x, y)}});
    r4 = Subs::create(Derivative::create(function_symbol("f", {add(y, y), _x}), {_x, y}),
                        {{_x, add(x, y)}});
    r3 = add(r3, r4);
    REQUIRE(eq(*r2, *r3));
}

TEST_CASE("Get pi shift: functions", "[functions]")
{
    RCP<const Basic> r;
    RCP<const Basic> r1;
    RCP<const Integer> n;
    bool b;

    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);
    RCP<const Basic> i8 = integer(8);

    RCP<const Basic> sq3 = sqrt(i3);
    RCP<const Basic> sq2 = sqrt(i2);

    RCP<const Symbol> x = symbol("x");

    // arg = k + n*pi
    r = add(i3, mul(i2, pi));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == true);
    REQUIRE(eq(*n, *integer(24)));
    REQUIRE(eq(*r1, *i3));

    // arg = n*pi/12
    r = mul(pi, div(one, integer(12)));
    get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(eq(*n, *one));
    REQUIRE(eq(*r1, *zero));

    // arg = n*pi/12
    r = mul(pi, div(i2, integer(3)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE((eq(*n, *i8) and (b == true) and eq(*r1, *zero)));

    // arg neq n*pi/12 , n not an integer
    r = mul(pi, div(i2, integer(5)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == false);

    // arg neq theta + n*pi/12 (no pi symbol, pi as pow)
    r = mul(pow(pi, i2), div(i2, integer(3)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == false);

    // arg neq theta + n*pi/12 (no pi symbol, pi as mul form)
    r = mul(mul(pi, x), div(i2, integer(3)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == false);

    // arg = theta + n*pi/12 (theta is just another symbol)
    r = add(mul(i2, x), mul(pi, div(i2, integer(3))));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == true);
    REQUIRE(eq(*n, *i8));
    REQUIRE(eq(*r1, *mul(i2, x)));

    // arg = theta + n*pi/12 (theta is constant plus a symbol)
    r = add(i2, add(x, mul(pi, div(i2, integer(3)))));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == true);
    REQUIRE(eq(*n, *i8));
    REQUIRE(eq(*r1, *add(i2, x)));

    // arg = theta + n*pi/12 (theta is an expression)
    r = add(i2, add(mul(x, i2), mul(pi, div(i2, integer(3)))));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == true);
    REQUIRE(eq(*n, *i8));
    REQUIRE(eq(*r1, *add(i2, mul(x, i2))));

    // arg neq n*pi/12 (n is not integer)
    r = mul(pi, div(i2, integer(5)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == false);

    // arg neq n*pi/12 (pi is not in form of symbol)
    r = mul(pow(pi, i2), div(i2, integer(3)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == false);

    // arg = pi (it is neither of form add nor mul, just a symbol)
    b = get_pi_shift(pi, outArg(n), outArg(r1));
    REQUIRE(((b == true) and eq(*n, *i12) and eq(*r1, *zero)));

    // arg = theta + n*pi/12 (theta is an expression of >1 symbols)
    r = add(add(mul(i2, x), mul(i2, symbol("y"))), mul(pi, div(i2, integer(3))));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    REQUIRE(b == true);
    REQUIRE(eq(*n, *i8));
    REQUIRE(eq(*r1, *add(mul(i2, x), mul(i2, symbol("y")))));
}

TEST_CASE("Sin table: functions", "[functions]")
{
    RCP<const Basic> r1;
    RCP<const Basic> r2;

    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);
    RCP<const Basic> im1 = integer(-1);

    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const Basic> sq3 = sqrt(i3);
    RCP<const Basic> sq2 = sqrt(i2);

    // sin(2pi + pi/6) = 1/2
    r1 = sin(add(mul(pi, i2), mul(div(pi, i12), i2)));
    r2 = div(one, i2);
    REQUIRE(eq(*r1, *r2));

    // sin(n*pi + pi/6) = 1/2
    r1 = sin(add(mul(pi, integer(10)), mul(div(pi, i12), i2)));
    r2 = div(one, i2);
    REQUIRE(eq(*r1, *r2));

    // sin(n*pi) = 0
    r1 = sin(mul(pi, i12));
    REQUIRE(eq(*r1, *zero));

    // sin(2pi + pi/2) = 1
    r1 = sin(add(mul(pi, i2), div(pi, i2)));
    REQUIRE(eq(*r1, *one));

    // sin(pi/3) = sqrt(3)/2
    r1 = sin(div(pi, integer(3)));
    r2 = div(sq3, i2);
    REQUIRE(eq(*r1, *r2));

    // sin(pi/4) = 1/sqrt(2)
    r1 = sin(div(pi, integer(4)));
    r2 = div(sq2, i2);
    REQUIRE(eq(*r1, *r2));

    // sin(pi/12) = (sqrt(3) - 1)/(2*sqrt(2))
    r1 = sin(div(pi, i12));
    r2 = div(sub(sq3, one), mul(i2, sq2));
    REQUIRE(eq(*r1, *r2));

    // sin(5*pi/12) = (sqrt(3) + 1)/(2*sqrt(2))
    r1 = sin(mul(div(pi, i12), integer(5)));
    r2 = div(add(sq3, one), mul(i2, sq2));
    REQUIRE(eq(*r1, *r2));

}

TEST_CASE("Could extract minus: functions", "[functions]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");

    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> r;
    bool b;

    r = add(mul(im1, x), mul(im1, mul(i2, y)));
    b = could_extract_minus(r);
    REQUIRE(b == true);

    r = add(mul(im1, x), mul(i2, y));
    b = could_extract_minus(r);
    REQUIRE(b == false);

    r = mul(mul(x, integer(-10)), y);
    b = could_extract_minus(r);
    REQUIRE(b == true);

    r = mul(mul(x, i2), y);
    b = could_extract_minus(r);
    REQUIRE(b == false);

    r = add(mul(im1, x), mul(im1, div(mul(i2, y), integer(3))));
    b = could_extract_minus(r);
    REQUIRE(b == true);

    r = mul(div(x, i2), y);
    b = could_extract_minus(r);
    REQUIRE(b == false);

}

TEST_CASE("Asin: functions", "[functions]")
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = asin(im1);
    r2 = mul(im1, div(pi, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = asin(div(im1, i2));
    r2 = div(pi, mul(im2, i3));
    REQUIRE(eq(*r1, *r2));

    r1 = asin(div(sqrt(i2), i2));
    r2 = div(pi, mul(i2, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = asin(div(add(sqrt(i3), i1), mul(i2, sqrt(i2))));
    r2 = div(pi, mul(i3, pow(i2, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = asin(div(sqrt(sub(i5, sqrt(i5))), integer(8)));
    r2 = div(pi, i5);
    REQUIRE(eq(*r1, *r2));

    r1 = asin(mul(div(sub(sqrt(i5), i1), integer(4)), im1));
    r2 = div(pi, mul(im2, i5));
    REQUIRE(eq(*r1, *r2));

    r1 = asin(real_double(0.5));
    REQUIRE(is_a<RealDouble>(*r1));
    REQUIRE(std::abs(static_cast<const RealDouble &>(*r1).i - 0.523598775598299) < 1e-12);

    r1 = asin(complex_double(std::complex<double>(1, 1)));
    r2 = asin(real_double(2.0));
    REQUIRE(is_a<ComplexDouble>(*r1));
    REQUIRE(is_a<ComplexDouble>(*r2));
    REQUIRE(std::abs(std::abs(static_cast<const ComplexDouble &>(*r1).i) - 1.2530681300031) < 1e-10);
    REQUIRE(std::abs(std::abs(static_cast<const ComplexDouble &>(*r2).i) - 2.0498241882037) < 1e-10);
}

TEST_CASE("Acos: functions", "[functions]")
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acos(im1);
    r2 = pi;
    REQUIRE(eq(*r1, *r2));

    r1 = acos(div(im1, i2));
    r2 = mul(i2, div(pi,  i3));
    REQUIRE(eq(*r1, *r2));

    r1 = acos(div(sqrt(i2), i2));
    r2 = div(pi, mul(i2, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = acos(div(add(sqrt(i3), i1), mul(i2, sqrt(i2))));
    r2 = mul(i5, div(pi, mul(i3, pow(i2, i2))));
    REQUIRE(eq(*r1, *r2));

    r1 = acos(div(sqrt(sub(i5, sqrt(i5))), integer(8)));
    r2 = mul(i3, div(pi, mul(i2, i5)));
    REQUIRE(eq(*r1, *r2));

    r1 = acos(mul(div(sub(sqrt(i5), i1), integer(4)), im1));
    r2 = mul(i3, div(pi, i5));
    REQUIRE(eq(*r1, *r2));

    r1 = acos(div(sub(sqrt(i5), i1), integer(4)));
    r2 = mul(i2, div(pi, i5));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Asec: functions", "[functions]")
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = asec(im1);
    r2 = pi;
    REQUIRE(eq(*r1, *r2));

    r1 = asec(div(i2, im1));
    r2 = mul(i2, div(pi,  i3));
    REQUIRE(eq(*r1, *r2));

    r1 = asec(sqrt(i2));
    r2 = div(pi, mul(i2, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = asec(div(mul(i2, sqrt(i2)), add(sqrt(i3), i1)));
    r2 = mul(i5, div(pi, mul(i3, pow(i2, i2))));
    std::cout<<*r1<<std::endl;
    std::cout<<*r2<<std::endl;
    REQUIRE(eq(*r1, *r2));

    r1 = asec(div(integer(8), sqrt(sub(i5, sqrt(i5)))));
    r2 = mul(i3, div(pi, mul(i2, i5)));
    REQUIRE(eq(*r1, *r2));

    r1 = asec(mul(div(integer(4), sub(sqrt(i5), i1)), im1));
    r2 = mul(i3, div(pi, i5));
    REQUIRE(eq(*r1, *r2));

    r1 = asec(div(integer(4), sub(sqrt(i5), i1)));
    r2 = mul(i2, div(pi, i5));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Acsc: functions", "[functions]")
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acsc(im1);
    r2 = mul(im1, div(pi, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = acsc(im2);
    r2 = div(pi, mul(im2, i3));
    REQUIRE(eq(*r1, *r2));

    r1 = acsc(sqrt(i2));
    r2 = div(pi, mul(i2, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = acsc(div(mul(i2, sqrt(i2)), add(sqrt(i3), i1)));
    r2 = div(pi, mul(i3, pow(i2, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = acsc(div(integer(8), sqrt(sub(i5, sqrt(i5)))));
    r2 = div(pi, i5);
    REQUIRE(eq(*r1, *r2));

    r1 = acsc(mul(div(integer(4), sub(sqrt(i5), i1)), im1));
    r2 = div(pi, mul(im2, i5));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("atan: functions", "[functions]")
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = atan(i1);
    r2 = div(pi, integer(4));
    REQUIRE(eq(*r1, *r2));

    r1 = atan(im1);
    r2 = div(pi, integer(-4));
    REQUIRE(eq(*r1, *r2));

    r1 = atan(div(one, sqrt(i3)));
    r2 = div(pi, integer(6));
    REQUIRE(eq(*r1, *r2));

    r1 = atan(mul(im1, add(one, sqrt(i2))));
    r2 = div(mul(pi, i3), integer(-8));
    REQUIRE(eq(*r1, *r2));

    r1 = atan(sub(sqrt(i2), one));
    r2 = div(pi, integer(8));
    REQUIRE(eq(*r1, *r2));

    r1 = atan(sub(i2, sqrt(i3)));
    r2 = div(pi, integer(12));
    REQUIRE(eq(*r1, *r2));

    r1 = atan(mul(im1, sqrt(add(i5, mul(i2, sqrt(i5))))));
    r2 = div(mul(pi, im2), i5);
    REQUIRE(eq(*r1, *r2));

}

TEST_CASE("Acot: functions", "[functions]")
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acot(i1);
    r2 = div(pi, integer(4));
    REQUIRE(eq(*r1, *r2));

    r1 = acot(im1);
    r2 = mul(i3, div(pi, integer(4)));
    REQUIRE(eq(*r1, *r2));

    r1 = acot(div(one, sqrt(i3)));
    r2 = div(pi, i3);
    REQUIRE(eq(*r1, *r2));

    r1 = acot(mul(im1, add(one, sqrt(i2))));
    r2 = div(mul(pi, integer(7)), integer(8));
    REQUIRE(eq(*r1, *r2));

    r1 = acot(sub(sqrt(i2), one));
    r2 = mul(i3, div(pi, integer(8)));
    REQUIRE(eq(*r1, *r2));

    r1 = acot(sub(i2, sqrt(i3)));
    r2 = mul(i5, div(pi, integer(12)));
    REQUIRE(eq(*r1, *r2));

    r1 = acot(mul(im1, sqrt(add(i5, mul(i2, sqrt(i5))))));
    r2 = div(mul(pi, integer(9)), mul(i5, i2));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Atan2: functions", "[functions]")
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = atan2(i1, i1);
    r2 = div(pi, integer(4));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(im1, i1);
    r2 = div(pi, integer(-4));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(i1, im1);
    r2 = div(mul(i3, pi), integer(4));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(im1, im1);
    r2 = div(mul(i3, pi), integer(-4));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(one, sqrt(i3));
    r2 = div(pi, integer(6));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(add(one, sqrt(i2)), im1);
    r2 = div(mul(pi, i3), integer(-8));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(sub(sqrt(i2), one), i1);
    r2 = div(pi, integer(8));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(sub(i2, sqrt(i3)), i1);
    r2 = div(pi, integer(12));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(sqrt(add(i5, mul(i2, sqrt(i5)))), im1);
    r2 = div(mul(pi, im2), i5);
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(y, x)->diff(x);
    r2 = div(mul(im1, y), add(pow(x, i2), pow(y, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(y, x)->diff(y);
    r2 = div(x, add(pow(x, i2), pow(y, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = atan2(y, x);
    REQUIRE(vec_basic_eq(r1->get_args(), {y, x}));
}

TEST_CASE("Lambertw: functions", "[functions]")
{
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> im2 = integer(-2);
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = lambertw(zero);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = lambertw(exp(im1));
    r2 = im1;
    REQUIRE(eq(*r1, *r2));

    r1 = lambertw(div(log(i2), im2));
    r2 = log(div(one, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = lambertw(x)->diff(y);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = lambertw(mul(i2, x))->diff(x);
    r2 = div(lambertw(mul(i2, x)), mul(x, add(lambertw(mul(i2, x)), one)));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Sinh: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = sinh(zero);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = sinh(im1);
    r2 = mul(im1, sinh(one));
    REQUIRE(eq(*r1, *r2));

    r1 = sinh(x)->expand_as_exp();
    r2 = div(add(exp(x), mul(im1, exp(mul(im1, x)))), i2);
    REQUIRE(eq(*r1, *r2));
    // tests cosh(-x) = cosh(x) and sinh(x)->diff(x) = cosh(x)
    r1 = sinh(mul(im1, x))->diff(x);
    r2 = mul(im1, cosh(x));
    REQUIRE(eq(*r1, *r2));

}

TEST_CASE("Cosh: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = cosh(zero);
    r2 = one;
    REQUIRE(eq(*r1, *r2));

    r1 = cosh(im1);
    r2 = cosh(one);
    REQUIRE(eq(*r1, *r2));

    r1 = cosh(x)->expand_as_exp();
    r2 = div(add(exp(x), exp(mul(im1, x))), i2);
    REQUIRE(eq(*r1, *r2));
    // tests sinh(-x) = -sinh(x) and cosh(x)->diff(x) = sinh(x)
    r1 = cosh(mul(im1, x))->diff(x);
    r2 = mul(im1, sinh(mul(im1, x)));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Tanh: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = tanh(zero);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = tanh(im1);
    r2 = mul(im1, tanh(one));
    REQUIRE(eq(*r1, *r2));

    r1 = tanh(x)->expand_as_exp();
    r2 = div(sub(exp(x), exp(mul(im1, x))), add(exp(x), exp(mul(im1, x))));
    REQUIRE(eq(*r1, *r2));

    r1 = tanh(mul(im1, x))->diff(x);
    r2 = add(pow(tanh(x), i2), im1);
    std::cout<<*r1<<std::endl;
    std::cout<<*r2<<std::endl;
    // REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Coth: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = coth(im1);
    r2 = mul(im1, coth(one));
    REQUIRE(eq(*r1, *r2));

    r1 = coth(x)->expand_as_exp();
    r2 = div(add(exp(x), exp(mul(im1, x))), sub(exp(x), exp(mul(im1, x))));
    REQUIRE(eq(*r1, *r2));

    r1 = coth(mul(im1, x))->diff(x);
    r2 = pow(sinh(x), im2);
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Asinh: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Basic> i4 = integer(4);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = asinh(zero);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = asinh(one);
    r2 = log(add(sqrt(i2), one));
    REQUIRE(eq(*r1, *r2));

    r1 = asinh(im1);
    r2 = log(add(sqrt(i2), im1));
    REQUIRE(eq(*r1, *r2));

    r1 = asinh(mul(im1, x))->diff(x);
    r2 = div(im1, sqrt(add(pow(x, i2), one)));
    REQUIRE(eq(*r1, *r2));

    r1 = asinh(mul(i2, y))->diff(y);
    r2 = div(i2, sqrt(add(mul(i4, pow(y, i2)), one)));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Acosh: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Basic> i4 = integer(4);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acosh(one);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = acosh(mul(im1, x))->diff(x);
    r2 = div(im1, sqrt(add(pow(x, i2), im1)));
    REQUIRE(eq(*r1, *r2));

    r1 = acosh(mul(i2, y))->diff(y);
    r2 = div(i2, sqrt(add(mul(i4, pow(y, i2)), im1)));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Atanh: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Basic> i4 = integer(4);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = atanh(zero);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = atanh(mul(im1, x))->diff(x);
    r2 = div(im1, sub(one, pow(x, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = atanh(mul(i2, y))->diff(y);
    r2 = div(i2, sub(one, mul(i4, pow(y, i2))));
    REQUIRE(eq(*r1, *r2));

    r1 = atanh(mul(im1, x));
    r2 = mul(im1, atanh(x));
    REQUIRE(eq(*r1, *r2));

    r1 = atanh(real_double(0.5));
    REQUIRE(is_a<RealDouble>(*r1));
    REQUIRE(std::abs(static_cast<const RealDouble &>(*r1).i - 0.549306144334055) < 1e-12);

    r1 = atanh(complex_double(std::complex<double>(1, 1)));
    r2 = atanh(real_double(2.0));
    REQUIRE(is_a<ComplexDouble>(*r1));
    REQUIRE(is_a<ComplexDouble>(*r2));
    REQUIRE(std::abs(std::abs(static_cast<const ComplexDouble &>(*r1).i) - 1.09390752881482) < 1e-12);
    REQUIRE(std::abs(std::abs(static_cast<const ComplexDouble &>(*r2).i) - 1.66407281705924) < 1e-12);
}

TEST_CASE("Acoth: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acoth(mul(im1, x))->diff(x);
    r2 = div(im1, sub(one, pow(x, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = acoth(mul(im1, x));
    r2 = mul(im1, acoth(x));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Asech: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = asech(one);
    r2 = zero;
    REQUIRE(eq(*r1, *r2));

    r1 = asech(x)->diff(x);
    r2 = div(im1, mul(sqrt(sub(one, pow(x, i2))), x));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Kronecker Delta: functions", "[functions]")
{
    RCP<const Symbol> i = symbol("i");
    RCP<const Symbol> j = symbol("j");

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = kronecker_delta(i, i);
    r2 = one;
    REQUIRE(eq(*r1, *r2));

    r1 = kronecker_delta(i, add(i, one));
    r2 = zero;
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Zeta: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = zeta(zero, x);
    r2 = sub(div(one, i2), x);
    REQUIRE(eq(*r1, *r2));

    r1 = zeta(zero, im1);
    r2 = div(one, i2);
    REQUIRE(eq(*r1, *r2));

    r1 = zeta(zero, i2);
    r2 = div(integer(-3), i2);
    REQUIRE(eq(*r1, *r2));
}


TEST_CASE("Levi Civita: functions", "[functions]")
{
    RCP<const Symbol> i = symbol("i");
    RCP<const Symbol> j = symbol("j");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> r1;

    r1 = levi_civita({one, i2, i3});
    REQUIRE(eq(*r1, *one));

    r1 = levi_civita({one, i3, i2});
    REQUIRE(eq(*r1, *im1));

    r1 = levi_civita({one, one, i2});
    REQUIRE(eq(*r1, *zero));

    r1 = levi_civita({i, j, i});
    REQUIRE(eq(*r1, *zero));

    r1 = levi_civita({i2, i4});
    REQUIRE(eq(*r1, *i2));
}

TEST_CASE("Dirichlet Eta: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = dirichlet_eta(one);
    r2 = log(i2);
    REQUIRE(eq(*r1, *r2));

    r1 = dirichlet_eta(zero);
    r2 = div(one, i2);
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Gamma: functions", "[functions]")
{
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> sqrt_pi = sqrt(pi);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = gamma(one);
    r2 = one;
    REQUIRE(eq(*r1, *r2));

    r1 = gamma(mul(i2, i2));
    r2 = mul(i2, i3);
    REQUIRE(eq(*r1, *r2));

    r1 = gamma(div(i3, i2));
    r2 = div(sqrt(pi), i2);
    REQUIRE(eq(*r1, *r2));

    r1 = gamma(div(one, i2));
    r2 = sqrt(pi);
    REQUIRE(eq(*r1, *r2));

    r1 = gamma(div(im1, i2));
    r2 = mul(mul(im1, i2), sqrt(pi));
    REQUIRE(eq(*r1, *r2));

    r1 = gamma(div(integer(-15), i2));
    r2 = mul(div(integer(256), integer(2027025)), sqrt(pi));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Lowergamma: functions", "[functions]")
{
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> im1 = integer(-1);


    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = lowergamma(one, i2);
    r2 = sub(one, exp(mul(im1, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = lowergamma(i2, i2);
    r2 = sub(one, mul(i3, exp(mul(im1, i2))));
    REQUIRE(eq(*r1, *r2));

    r1 = lowergamma(mul(i2, i3), i2);
    r2 = sub(integer(120), mul(integer(872), exp(mul(im1, i2))));
    REQUIRE(eq(*expand(r1), *r2));
}

TEST_CASE("Uppergamma: functions", "[functions]")
{
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> im1 = integer(-1);


    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = uppergamma(one, i2);
    r2 = exp(mul(im1, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = uppergamma(i2, i2);
    r2 = mul(i3, exp(mul(im1, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = uppergamma(mul(i2, i3), i2);
    r2 = mul(integer(872), exp(mul(im1, i2)));
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Abs: functions", "[functions]")
{
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> sqrt_pi = sqrt(pi);
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    REQUIRE(eq(*abs(one), *one));
    REQUIRE(eq(*abs(i2), *i2));
    REQUIRE(eq(*abs(im1), *one));
    REQUIRE(eq(*abs(integer(-5)), *integer(5)));
    REQUIRE(neq(*abs(sqrt_pi), *sqrt_pi));
    REQUIRE(eq(*abs(sqrt_pi), *abs(sqrt_pi)));
    REQUIRE(eq(*abs(div(i2, i3)), *div(i2, i3)));
    REQUIRE(eq(*abs(neg(div(i2, i3))), *div(i2, i3)));
    REQUIRE(neq(*abs(x)->diff(x), *integer(0)));
    REQUIRE(eq(*abs(x)->diff(y), *integer(0)));
}

class MySin : public FunctionWrapper {
public :
    MySin(RCP<const Basic> arg) : FunctionWrapper("MySin", arg) {

    }
    RCP<const Number> eval(long bits) const {
        return real_double(::sin(eval_double(*arg_[0])));
    }
    RCP<const Basic> create(const vec_basic &v) const {
        if (eq(*zero, *v[0])) {
            return zero;
        } else {
            return make_rcp<MySin>(v[0]);
        }
    }
    RCP<const Basic> diff(const RCP<const Symbol> &x) const {
        return mul(cos(arg_[0]), arg_[0]->diff(x));
    }
};

TEST_CASE("FunctionWrapper: functions", "[functions]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> e = make_rcp<MySin>(x);
    RCP<const Basic> f;

    f = e->subs({{x, integer(0)}});
    REQUIRE(eq(*f, *zero));

    f = e->diff(x);
    REQUIRE(eq(*f, *cos(x)));

    f = e->subs({{x, integer(1)}});
    double d = eval_double(*f);
    REQUIRE(std::fabs(d - 0.84147098480789) < 1e-12);

    REQUIRE(e->__str__() == "MySin(x)");
}
/* ---------------------------- */

TEST_CASE("MPFR and MPC: functions", "[functions]")
{
#ifdef HAVE_SYMENGINE_MPFR
    RCP<const Basic> r1, r2;
    RCP<const Basic> i2 = integer(2);
    unsigned long p = 100000000000000000;

    mpfr_class a(60);
    mpfr_set_ui(a.get_mpfr_t(), 1, MPFR_RNDN);
    r1 = sin(real_mpfr(a));

    mpfr_set_ui(a.get_mpfr_t(), 2, MPFR_RNDN);
    r2 = sin(sub(div(pi, i2), real_mpfr(a)));
    REQUIRE(is_a<RealMPFR>(*r1));
    REQUIRE(is_a<RealMPFR>(*r2));

    mpfr_mul_ui(a.get_mpfr_t(), static_cast<const RealMPFR &>(*r1).i.get_mpfr_t(), p, MPFR_RNDN);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 84147098480789650) > 0);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 84147098480789651) < 0);
    mpfr_mul_ui(a.get_mpfr_t(), static_cast<const RealMPFR &>(*r2).i.get_mpfr_t(), p, MPFR_RNDN);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), -41614683654714239) > 0);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), -41614683654714238) < 0);

    mpfr_set_si(a.get_mpfr_t(), 0, MPFR_RNDN);
    r1 = asin(real_mpfr(a));
    REQUIRE(is_a<RealMPFR>(*r1));

#ifdef HAVE_SYMENGINE_MPC
    // Check asin(2.0)
    mpfr_set_si(a.get_mpfr_t(), 2, MPFR_RNDN);
    r1 = asin(real_mpfr(a));
    REQUIRE(is_a<ComplexMPC>(*r1));
    mpc_srcptr b = static_cast<const ComplexMPC &>(*r1).i.get_mpc_t();
    mpc_real(a.get_mpfr_t(), b, MPFR_RNDN);
    mpfr_mul_ui(a.get_mpfr_t(), a.get_mpfr_t(), p, MPFR_RNDN);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 157079632679489661) > 0);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 157079632679489662) < 0);
    mpc_imag(a.get_mpfr_t(), b, MPFR_RNDN);
    mpfr_mul_ui(a.get_mpfr_t(), a.get_mpfr_t(), p, MPFR_RNDN);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 131695789692481670) > 0);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 131695789692481671) < 0);

    //Check asin(1.0 + 1.0*I)
    mpc_class c(60);
    mpc_set_si_si(c.get_mpc_t(), 1, 1, MPFR_RNDN);
    r1 = asin(complex_mpc(c));
    REQUIRE(is_a<ComplexMPC>(*r1));
    b = static_cast<const ComplexMPC &>(*r1).i.get_mpc_t();
    mpc_real(a.get_mpfr_t(), b, MPFR_RNDN);
    mpfr_mul_ui(a.get_mpfr_t(), a.get_mpfr_t(), p, MPFR_RNDN);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 66623943249251525) > 0);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 66623943249251526) < 0);
    mpc_imag(a.get_mpfr_t(), b, MPFR_RNDN);
    mpfr_mul_ui(a.get_mpfr_t(), a.get_mpfr_t(), p, MPFR_RNDN);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 106127506190503565) > 0);
    REQUIRE(mpfr_cmp_si(a.get_mpfr_t(), 106127506190503566) < 0);
#else
    mpfr_set_si(a.get_mpfr_t(), 2, MPFR_RNDN);
    CHECK_THROWS_AS(asin(real_mpfr(a)), std::runtime_error);
#endif //HAVE_SYMENGINE_MPC
#endif //HAVE_SYMENGINE_MPFR
}
