#include "catch.hpp"
#include <iostream>

#include <symengine/solve.h>
#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/polys/basic_conversions.h>

using SymEngine::solve;
using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::integer;
using SymEngine::rational;
using SymEngine::add;
using SymEngine::symbol;
using SymEngine::emptyset;
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
using SymEngine::FiniteSet;
using SymEngine::Eq;
using SymEngine::Ne;
using SymEngine::Ge;
using SymEngine::boolTrue;
using SymEngine::boolFalse;
using SymEngine::down_cast;
using SymEngine::ConditionSet;
using SymEngine::is_a;
using SymEngine::logical_and;
using SymEngine::Union;
using SymEngine::mul;

TEST_CASE("Trivial cases", "[Solve]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> poly;
    RCP<const Set> reals = interval(NegInf, Inf, true, true);
    RCP<const Set> soln;

    REQUIRE(eq(*solve(boolTrue, x, reals), *reals));
    REQUIRE(eq(*solve(boolFalse, x, reals), *emptyset()));

    // constants
    poly = one;
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *emptyset()));

    poly = zero;
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *reals));
}

TEST_CASE("linear and quadratic polynomials", "[Solve]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> poly;
    RCP<const Set> reals = interval(NegInf, Inf, true, true);
    RCP<const Set> soln;

    auto i2 = integer(2), i3 = integer(3), im2 = integer(-2), im3 = integer(-3);
    auto sqx = mul(x, x);

    // linear
    poly = add(x, i3);
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *finiteset({im3})));

    soln = solve(poly, x, interval(zero, Inf, false, true));
    REQUIRE(eq(*soln, *emptyset()));

    poly = add(div(x, i2), div(one, i3));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({neg(rational(2, 3))})));

    poly = x;
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *finiteset({zero})));

    auto y = symbol("y");
    soln = solve_poly_linear({y}, x);
    REQUIRE(eq(*soln, *finiteset({neg(y)})));

    CHECK_THROWS_AS(solve_poly_linear({one, zero}, x, reals),
                    SymEngineException);

    // Quadratic
    poly = add(sqx, one);
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({neg(I), I})));

    poly = add(add(sqx, mul(im2, x)), one);
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({one})));

    poly = sub(add(div(sqx, i3), x), div(i2, integer(5)));
    soln = solve(poly, x);
    REQUIRE(
        eq(*soln,
           *finiteset(
               {add(div(mul(sqrt(integer(69)), sqrt(integer(5))), integer(10)),
                    div(im3, i2)),
                sub(div(im3, i2), div(mul(sqrt(integer(69)), sqrt(integer(5))),
                                      integer(10)))}))); // Shouldn't
                                                         // sqrt(69)*sqrt(5)
                                                         // simplify into
                                                         // sqrt(345) ?

    poly = add(sqx, mul(x, i2));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({zero, im2})));

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

    auto b = symbol("b"), c = symbol("c");
    soln = solve_poly_quadratic({c, b}, x);
    REQUIRE(soln->__str__() == "{(-1/2)*b + (1/2)*sqrt(-4*c + b**2), (-1/2)*b "
                               "+ (-1/2)*sqrt(-4*c + b**2)}");

    CHECK_THROWS_AS(solve_poly_quadratic({one}, x, reals), SymEngineException);
}

TEST_CASE("cubic and quartic polynomials", "[Solve]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> poly;
    RCP<const Set> reals = interval(NegInf, Inf, true, true);
    RCP<const Set> soln;

    auto sqx = mul(x, x), cbx = mul(sqx, x), qx = mul(cbx, x);
    auto i2 = integer(2), i3 = integer(3), im2 = integer(-2);

    // cubic
    poly = Eq(add({cbx, mul(x, i3), mul(sqx, i3)}), neg(one));
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *finiteset({neg(one)})));

    poly = Ne(add({cbx, mul(x, i3), mul(sqx, i3)}), neg(one));
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *SymEngine::set_union(
                          {interval(NegInf, integer(-1), true, true),
                           interval(integer(-1), Inf, true, true)})));

    poly = Ge(add({cbx, mul(x, i3), mul(sqx, i3)}), one);
    soln = solve(poly, x, reals);
    REQUIRE(is_a<ConditionSet>(*soln));

    REQUIRE(eq(*down_cast<const ConditionSet &>(*soln).get_condition(),
               *logical_and({Ge(add({cbx, mul(x, i3), mul(sqx, i3)}), one),
                             reals->contains(x)})));

    poly = mul(cbx, i3);
    soln = solve(poly, x, reals);
    REQUIRE(eq(*soln, *finiteset({zero})));

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

    poly = Eq(cbx, neg(one));
    soln = solve(poly, x);
    r1 = neg(one);
    r2 = add(div(one, i2), div(mul(I, sqrt(i3)), i2));
    r3 = sub(div(one, i2), div(mul(I, sqrt(i3)), i2));
    // -(-1/2 - 1/2*I*sqrt(3)) != 1/2 + 1/2*I*sqrt(3) ?
    // REQUIRE(eq(*soln, *finiteset({r1, r2, r3})));

    CHECK_THROWS_AS(solve_poly_cubic({one}, x, reals), SymEngineException);

    // Quartic
    poly = qx;
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({zero})));

    poly = add(qx, cbx);
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({zero, neg(one)})));

    poly = add({qx, mul(cbx, i2), mul(integer(-41), sqx), mul(x, integer(-42)),
                integer(360)});
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({i3, integer(-4), integer(5), integer(-6)})));

    poly = add({qx, mul(cbx, rational(5, 2)), mul(integer(-19), sqx),
                mul(x, integer(14)), integer(12)});
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({i2, rational(-1, 2), integer(-6)})));

    poly = mul(add(x, i2), add(x, i3));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({neg(i2), neg(i3)})));
}

TEST_CASE("Higher order(degree >=5) polynomials", "[Solve]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> poly;
    RCP<const Set> soln;

    auto sqx = mul(x, x), cbx = mul(sqx, x), qx = mul(cbx, x);
    auto i2 = integer(2), i3 = integer(3);

#if defined(HAVE_SYMENGINE_FLINT) and __FLINT_RELEASE > 20502

    poly = add({mul(qx, x), mul(qx, rational(-538, 15)),
                mul(cbx, rational(1003, 3)), mul(sqx, rational(-13436, 15)),
                mul(x, rational(740, 3)), rational(4784, 5)});
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({i2, i3, rational(26, 3), rational(-4, 5),
                                  integer(23)})));

    poly = add({mul(qx, qx), mul({i3, qx, cbx}), mul({i2, qx, sqx}), mul(qx, x),
                mul(i3, qx), mul(i2, cbx), sqx, mul(i3, x), i2});
    soln = solve(poly, x);
    REQUIRE(is_a<Union>(*soln));
    auto &temp = down_cast<const Union &>(*soln).get_container();
    REQUIRE(temp.size() == 2);
    REQUIRE(temp.find(finiteset({neg(one), neg(i2)})) != temp.end());
    REQUIRE(temp.find(conditionset(x, Eq(add({mul(cbx, cbx), cbx, one}), zero)))
            != temp.end());

#else

    poly = add({mul(qx, x), mul(qx, rational(-538, 15)),
                mul(cbx, rational(1003, 3)), mul(sqx, rational(-13436, 15)),
                mul(x, rational(740, 3)), rational(4784, 5)});
    soln = solve(poly, x);
    REQUIRE(is_a<ConditionSet>(*soln));
    REQUIRE(eq(*down_cast<const ConditionSet &>(*soln).get_condition(),
               *Eq(poly, zero)));

    poly = add({mul(qx, qx), mul({i3, qx, cbx}), mul({i2, qx, sqx}), mul(qx, x),
                mul(i3, qx), mul(i2, cbx), sqx, mul(i3, x), i2});
    soln = solve(poly, x);
    REQUIRE(is_a<ConditionSet>(*soln));
    REQUIRE(eq(*down_cast<const ConditionSet &>(*soln).get_condition(),
               *Eq(poly, zero)));

#endif

    poly = add({mul(cbx, cbx), cbx, one});
    soln = solve(poly, x);
    REQUIRE(is_a<ConditionSet>(*soln));
    REQUIRE(eq(*down_cast<const ConditionSet &>(*soln).get_condition(),
               *Eq(poly, zero)));
}

TEST_CASE("solve_poly_rational", "[Solve]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> poly;
    RCP<const Set> soln;
    auto i2 = integer(2), i3 = integer(3);

    poly = div(add(x, i2), add(x, i3));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({neg(i2)})));

    poly = div(mul(add(x, i2), add(x, i3)), add(x, i2));
    soln = solve(poly, x);
    REQUIRE(eq(*soln, *finiteset({neg(i3)})));
}
