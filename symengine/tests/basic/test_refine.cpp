#include "catch.hpp"
#include <symengine/refine.h>

using SymEngine::Assumptions;
using SymEngine::integer;
using SymEngine::integers;
using SymEngine::max;
using SymEngine::min;
using SymEngine::pi;
using SymEngine::reals;
using SymEngine::symbol;

TEST_CASE("Test refine", "[refine]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");

    auto expr = abs(x);
    auto a1 = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a1), *x));

    expr = abs(x);
    auto a2 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a2), *expr));

    expr = abs(x);
    auto a3 = Assumptions({Le(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a3), *neg(x)));

    expr = abs(conjugate(x));
    auto a3b = Assumptions({});
    REQUIRE(eq(*refine(expr, &a3b), *abs(x)));

    expr = sign(x);
    auto a4 = Assumptions({Lt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a4), *integer(-1)));

    expr = sign(x);
    auto a5 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a5), *expr));

    expr = sign(x);
    auto a6 = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a6), *integer(1)));

    expr = sign(x);
    auto a7 = Assumptions({Eq(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a7), *integer(0)));

    expr = sign(abs(x));
    auto a8 = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a8), *integer(1)));

    expr = floor(x);
    auto a9 = Assumptions({integers()->contains(x)});
    REQUIRE(eq(*refine(expr, &a9), *x));

    expr = floor(x);
    auto a10 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a10), *expr));

    expr = ceiling(x);
    auto a11 = Assumptions({integers()->contains(x)});
    REQUIRE(eq(*refine(expr, &a11), *x));

    expr = ceiling(x);
    auto a12 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a12), *expr));

    expr = ceiling(neg(x));
    auto a13 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a13), *neg(floor(x))));

    expr = floor(neg(x));
    auto a14 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a14), *neg(ceiling(x))));

    expr = conjugate(x);
    auto a15 = Assumptions({reals()->contains(x)});
    REQUIRE(eq(*refine(expr, &a15), *x));

    expr = conjugate(x);
    auto a16 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a16), *expr));

    expr = max({integer(-1), x});
    auto a17 = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a17), *x));

    expr = max({integer(-1), x});
    auto a18 = Assumptions({Ge(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a18), *x));

    expr = max({integer(-1), x});
    auto a19 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a19), *expr));

    expr = max({x, y, z});
    auto a20 = Assumptions(
        {Ge(x, integer(0)), Ge(y, integer(0)), Ge(z, integer(0))});
    REQUIRE(eq(*refine(expr, &a20), *expr));

    expr = max({x, y, z});
    auto a21 = Assumptions({Ge(z, integer(0))});
    REQUIRE(eq(*refine(expr, &a21), *expr));

    expr = min({integer(-1), x});
    auto a22 = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a22), *integer(-1)));

    expr = min({integer(-1), x});
    auto a23 = Assumptions({Ge(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a23), *integer(-1)));

    expr = min({integer(-1), x});
    auto a24 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a24), *expr));

    expr = min({x, y, z});
    auto a25 = Assumptions(
        {Ge(x, integer(0)), Ge(y, integer(0)), Ge(z, integer(0))});
    REQUIRE(eq(*refine(expr, &a25), *expr));

    expr = min({x, y, z});
    auto a26 = Assumptions({Ge(z, integer(0))});
    REQUIRE(eq(*refine(expr, &a26), *expr));
}
