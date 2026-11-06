#include "catch.hpp"
#include <symengine/simplify.h>

using SymEngine::Assumptions;
using SymEngine::integer;
using SymEngine::integers;
using SymEngine::pi;
using SymEngine::reals;
using SymEngine::symbol;
using SymEngine::unevaluated_expr;

TEST_CASE("Test simplify", "[simplify]")
{
    auto x = symbol("x");

    // The following tests visits Mul
    auto expr = div(integer(2), csc(x));
    REQUIRE(eq(*simplify(expr), *mul(integer(2), sin(x))));
    expr = div(integer(2), sec(x));
    REQUIRE(eq(*simplify(expr), *mul(integer(2), cos(x))));
    expr = div(integer(2), cot(x));
    REQUIRE(eq(*simplify(expr), *mul(integer(2), tan(x))));
    expr = div(integer(2), csc(div(integer(2), csc(x))));
    REQUIRE(
        eq(*simplify(expr), *mul(integer(2), sin(mul(integer(2), sin(x))))));

    expr = div(integer(2), csc(abs(x)));
    Assumptions a1 = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*simplify(expr, &a1), *mul(integer(2), sin(x))));

    // The following tests visits Pow
    expr = div(integer(1), csc(x));
    REQUIRE(eq(*simplify(expr), *sin(x)));
    expr = div(integer(1), sec(x));
    REQUIRE(eq(*simplify(expr), *cos(x)));
    expr = div(integer(1), cot(x));
    REQUIRE(eq(*simplify(expr), *tan(x)));

    // No simplifications
    expr = x;
    REQUIRE(eq(*simplify(expr), *expr));
    expr = sin(x);
    REQUIRE(eq(*simplify(expr), *expr));
    expr = div(integer(1), sin(x));
    REQUIRE(eq(*simplify(expr), *expr));
    expr = div(integer(2), sin(x));
    REQUIRE(eq(*simplify(expr), *expr));
}

TEST_CASE("Test piecewise_fold", "[simplify]")
{
    using SymEngine::Basic;
    using SymEngine::boolTrue;
    using SymEngine::function_symbol;
    using SymEngine::logical_and;
    using SymEngine::piecewise;
    using SymEngine::piecewise_fold;
    using SymEngine::RCP;

    auto x = symbol("x");
    auto y = symbol("y");
    auto i0 = integer(0);
    auto i1 = integer(1);
    auto i2 = integer(2);
    RCP<const Basic> p, e, r;

    // no Piecewise
    e = add(sin(x), y);
    REQUIRE(eq(*piecewise_fold(e), *e));

    p = piecewise({{x, Lt(x, i0)}, {i1, boolTrue}});

    // Piecewise alone is unchanged
    REQUIRE(eq(*piecewise_fold(p), *p));

    // one arg function
    r = piecewise_fold(sin(p));
    REQUIRE(eq(*r, *piecewise({{sin(x), Lt(x, i0)}, {sin(i1), boolTrue}})));

    // Add and Mul
    r = piecewise_fold(add(p, y));
    REQUIRE(
        eq(*r, *piecewise({{add(x, y), Lt(x, i0)}, {add(i1, y), boolTrue}})));
    r = piecewise_fold(mul(i2, p));
    REQUIRE(eq(*r, *piecewise({{mul(i2, x), Lt(x, i0)}, {i2, boolTrue}})));

    // Repeated Piecewise in an Add, i.e. 2*p + y
    r = piecewise_fold(SymEngine::add({p, p, y}));
    REQUIRE(eq(*r, *piecewise({{add(mul(i2, x), y), Lt(x, i0)},
                               {add(i2, y), boolTrue}})));

    // Pow
    r = piecewise_fold(pow(p, i2));
    REQUIRE(eq(*r, *piecewise({{pow(x, i2), Lt(x, i0)}, {i1, boolTrue}})));

    // Nested inside other functions
    r = piecewise_fold(cos(add(p, y)));
    REQUIRE(eq(*r, *piecewise({{cos(add(x, y)), Lt(x, i0)},
                               {cos(add(i1, y)), boolTrue}})));

    // Multi arg function
    auto f = function_symbol("f", {p, y});
    r = piecewise_fold(f);
    REQUIRE(eq(*r, *piecewise({{function_symbol("f", {x, y}), Lt(x, i0)},
                               {function_symbol("f", {i1, y}), boolTrue}})));

    // Two arg function
    r = piecewise_fold(atan2(p, y));
    REQUIRE(eq(
        *r, *piecewise({{atan2(x, y), Lt(x, i0)}, {atan2(i1, y), boolTrue}})));

    // Piecewise with same conditions are zipped
    auto q = piecewise({{y, Lt(x, i0)}, {i2, boolTrue}});
    r = piecewise_fold(add(p, q));
    REQUIRE(
        eq(*r, *piecewise({{add(x, y), Lt(x, i0)}, {integer(3), boolTrue}})));

    // Piecewise with different conditions take the cartesian product
    q = piecewise({{y, Lt(y, i0)}, {i2, boolTrue}});
    r = piecewise_fold(add(p, q));
    REQUIRE(eq(*r, *piecewise({{add(x, y), logical_and({Lt(x, i0), Lt(y, i0)})},
                               {add(i1, y), Lt(y, i0)},
                               {add(x, i2), Lt(x, i0)},
                               {integer(3), boolTrue}})));

    // Nested Piecewise are flattened
    e = piecewise({{p, Lt(y, i0)}, {y, boolTrue}});
    r = piecewise_fold(e);
    REQUIRE(eq(*r, *piecewise({{x, logical_and({Lt(y, i0), Lt(x, i0)})},
                               {i1, Lt(y, i0)},
                               {y, boolTrue}})));

    // Piecewise inside a branch of a Piecewise
    e = piecewise({{add(p, i1), Lt(y, i0)}, {y, boolTrue}});
    r = piecewise_fold(e);
    REQUIRE(
        eq(*r, *piecewise({{add(x, i1), logical_and({Lt(y, i0), Lt(x, i0)})},
                           {i2, Lt(y, i0)},
                           {y, boolTrue}})));

    // Relationals are folded
    r = piecewise_fold(Lt(p, y));
    REQUIRE(eq(*r, *piecewise({{Lt(x, y), Lt(x, i0)}, {Lt(i1, y), boolTrue}})));
    r = piecewise_fold(Eq(p, i0));
    REQUIRE(eq(*r, *piecewise({{Eq(x, i0), Lt(x, i0)},
                               {SymEngine::boolFalse, boolTrue}})));

    // Boolean functions are folded
    using SymEngine::contains;
    using SymEngine::interval;
    using SymEngine::logical_not;
    using SymEngine::logical_or;
    r = piecewise_fold(logical_and({Lt(p, i0), Lt(y, i0)}));
    REQUIRE(eq(*r, *piecewise({{logical_and({Lt(x, i0), Lt(y, i0)}), Lt(x, i0)},
                               {SymEngine::boolFalse, boolTrue}})));
    r = piecewise_fold(logical_or({Lt(p, i0), Lt(y, i0)}));
    REQUIRE(eq(*r, *piecewise({{logical_or({Lt(x, i0), Lt(y, i0)}), Lt(x, i0)},
                               {Lt(y, i0), boolTrue}})));
    r = piecewise_fold(logical_not(Lt(p, y)));
    REQUIRE(eq(*r, *piecewise({{logical_not(Lt(x, y)), Lt(x, i0)},
                               {logical_not(Lt(i1, y)), boolTrue}})));
    r = piecewise_fold(contains(p, interval(i0, i2)));
    REQUIRE(eq(*r, *piecewise({{contains(x, interval(i0, i2)), Lt(x, i0)},
                               {boolTrue, boolTrue}})));

    // Relationals inside conditions are not folded
    e = piecewise({{x, Lt(p, i0)}, {y, boolTrue}});
    REQUIRE(eq(*piecewise_fold(e), *e));
}
