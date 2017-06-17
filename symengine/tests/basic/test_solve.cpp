#include "catch.hpp"
#include <iostream>

#include <symengine/solve.h>
#include <symengine/mul.h>
#include <symengine/add.h>

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

TEST_CASE("test_solve", "[Solve]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> poly;
    RCP<const Set> reals = interval(NegInf, Inf, true, true);
    RCP<const Set> soln;

    auto sqx = mul(x, x), cbx = mul(sqx, x), qx = mul(cbx, x);

    // constants
    poly = one;
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *emptyset()));

    poly = zero;
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *reals));

    // linear
    poly = add(x, integer(3));
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *finiteset({integer(-3)})));

    poly = add(mul(integer(2), x), integer(3));
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *finiteset({rational(-3, 2)})));

    poly = add(mul(integer(2), x), integer(3));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({rational(-3, 2)})));

    soln = solve(poly, x, interval(zero, Inf, false, true));
    REQUIRE(eq(*soln, *emptyset()));

    poly = x;
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *finiteset({zero})));

    poly = sqx;
    CHECK_THROWS_AS(solve_poly_linear(poly, x, reals), SymEngineException);

    // Quadratic
    poly = add(sqx, one);
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({neg(I), I})));

    poly = add(add(sqx, mul(integer(-2), x)), one);
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({one})));

    poly = add(sqx, mul(x, integer(2)));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({zero, integer(-2)})));

    poly = add(sqx, sub(mul(integer(8), x), integer(5)));
    soln = solve(poly, x);
    REQUIRE(
        eq(*soln,
           *finiteset({add(integer(-4), div(sqrt(integer(84)), integer(2))),
                       sub(integer(-4), div(sqrt(integer(84)), integer(2)))})));

    poly = add(sqx, sub(integer(50), mul(integer(8), x)));
    soln = solve(poly, x);
    REQUIRE(eq(
        *soln,
        *finiteset(
            {add(integer(4), div(mul(sqrt(integer(136)), I), integer(2))),
             sub(integer(4), div(mul(sqrt(integer(136)), I), integer(2)))})));

    poly = cbx;
    CHECK_THROWS_AS(solve_poly_quadratic(poly, x, reals), SymEngineException);

    // cubic
    poly = sub(add(cbx, mul(x, integer(3))), add(mul(sqx, integer(3)), one));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({one})));

    // std::cout << *soln <<"::\n";
}
