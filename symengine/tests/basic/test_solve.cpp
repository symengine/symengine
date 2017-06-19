#include "catch.hpp"
#include <iostream>

#include <symengine/solve.h>
#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/pow.h>

using SymEngine::solve;
using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::integer;
using SymEngine::rational;
using SymEngine::add;
using SymEngine::symbol;
using SymEngine::emptyset;
using SymEngine::solve_poly_linear;
using SymEngine::interval;
using SymEngine::Interval;
using SymEngine::emptyset;
using SymEngine::finiteset;
using SymEngine::Set;
using SymEngine::Symbol;
using SymEngine::Inf;
using SymEngine::NegInf;
using SymEngine::I;
using SymEngine::SymEngineException;
using SymEngine::neg;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::pow;
using SymEngine::map_basic_basic;
using SymEngine::FiniteSet;
using SymEngine::down_cast;

TEST_CASE("test_solve", "[Solve]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> poly;
    RCP<const Set> reals = interval(NegInf, Inf, true, true);
    RCP<const Set> soln;

    auto sqx = mul(x, x), cbx = mul(sqx, x), qx = mul(cbx, x);
    auto i2 = integer(2), i3 = integer(3), im2 = integer(-2), im3 = integer(-3);

    // constants
    poly = one;
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *emptyset()));

    poly = zero;
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *reals));

    // linear
    poly = add(x, i3);
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *finiteset({im3})));

    soln = solve(poly, x, interval(zero, Inf, false, true));
    REQUIRE(eq(*soln, *emptyset()));

    // poly = x;
    // soln = solve(poly, x, reals);
    // REQUIRE(eq(*soln, *finiteset({zero})));

    // poly = sqx;
    // CHECK_THROWS_AS(solve_poly_linear(poly, x, reals), SymEngineException);

    // Quadratic
    poly = add(sqx, one);
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({neg(I), I})));

    poly = add(add(sqx, mul(im2, x)), one);
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({one})));

    std::cout << "!!!\n";
    poly = sub(add(div(sqx, i3), x), div(i2, integer(5)));
    soln = solve(poly, x);
    REQUIRE(eq(
        *soln,
        *finiteset({add(div(sqrt(integer(345)), integer(10)), div(im3, i2)),
                    sub(div(im3, i2), div(sqrt(integer(345)), integer(10)))})));

    // poly = add(sqx, mul(x, i2));
    // soln = solve(poly, x);
    // REQUIRE(eq(*soln, *finiteset({zero, im2})));

    poly = add(sqx, sub(mul(integer(8), x), integer(5)));
    soln = solve(poly, x);
    REQUIRE(
        eq(*soln, *finiteset({add(integer(-4), div(sqrt(integer(84)), i2)),
                              sub(integer(-4), div(sqrt(integer(84)), i2))})));

    poly = add(sqx, sub(integer(50), mul(integer(8), x)));
    soln = solve(poly, x);
    REQUIRE(
        eq(*soln,
           *finiteset({add(integer(4), div(mul(sqrt(integer(136)), I), i2)),
                       sub(integer(4), div(mul(sqrt(integer(136)), I), i2))})));

    poly = cbx;
    // CHECK_THROWS_AS(solve_poly_quadratic(poly, x, reals),
    // SymEngineException);

    // cubic
    poly = sub(add(cbx, mul(x, i3)), add(mul(sqx, i3), one));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({one})));

    // poly = mul(cbx, i3);
    // soln = solve(poly, x, reals);
    // REQUIRE(eq(*soln, *finiteset({zero})));

    poly = add(cbx, sub(add(mul(sqx, i3), mul(i3, x)), one));
    soln = solve(poly, x);
    auto r1 = neg(div(add(i3, pow(integer(-54), div(one, i3))), i3));
    auto r2 = neg(div(add(i3, mul(add(div(one, im2), div(mul(I, sqrt(i3)), i2)),
                                  pow(integer(-54), div(one, i3)))),
                      i3));
    auto r3 = neg(div(add(i3, mul(sub(div(one, im2), div(mul(I, sqrt(i3)), i2)),
                                  pow(integer(-54), div(one, i3)))),
                      i3));
    REQUIRE(eq(*soln, *finiteset({r1, r2, r3})));

    poly = sub(add(cbx, mul(integer(201), x)),
               add(integer(288), mul(sqx, integer(38))));
    soln = solve(poly, x);
    r1 = i3;
    r2 = integer(32);
    REQUIRE(eq(*soln, *finiteset({r1, r2})));

    poly = add(cbx, one);
    soln = solve(poly, x);
    r1 = neg(one);
    r2 = add(div(one, i2), div(mul(I, sqrt(i3)), i2));
    r3 = sub(div(one, i2), div(mul(I, sqrt(i3)), i2));
    REQUIRE(eq(*soln, *finiteset({r1, r2, r3})));

    poly = sqx;
    // CHECK_THROWS_AS(solve_poly_cubic(poly, x, reals), SymEngineException);

    // Quartic
    // poly = qx;
    // soln = solve(poly,x);
    // REQUIRE(eq(*soln,*finiteset({zero})));

    // poly = add(qx,cbx);
    // soln = solve(poly,x);
    // REQUIRE(eq(*soln,*finiteset({zero,neg(one)})));

    poly = sub(add({mul(integer(6), qx), mul(rational(96, 5), sqx),
                    mul(rational(97, 5), x), rational(6, 5)}),
               mul(cbx, rational(133, 5)));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({i2, i3, neg(rational(1, 15)),
                                  neg(rational(1, 2))})));

    // poly = qx;
    // soln = solve(poly,x);
    // REQUIRE(eq(*soln,*finiteset({zero})));
}
