#include "catch.hpp"
#include <symengine/refine.h>

using SymEngine::Assumptions;
using SymEngine::integer;
using SymEngine::integers;
using SymEngine::reals;
using SymEngine::symbol;

TEST_CASE("Test refine", "[refine]")
{
    auto x = symbol("x");

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

    expr = sign(abs(x));
    auto a7 = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a7), *integer(1)));

    expr = floor(x);
    auto a8 = Assumptions({integers()->contains(x)});
    REQUIRE(eq(*refine(expr, &a8), *x));

    expr = floor(x);
    auto a9 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a9), *expr));

    expr = ceiling(x);
    auto a10 = Assumptions({integers()->contains(x)});
    REQUIRE(eq(*refine(expr, &a10), *x));

    expr = ceiling(x);
    auto a11 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a11), *expr));

    expr = ceiling(neg(x));
    auto a12 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a12), *neg(floor(x))));

    expr = floor(neg(x));
    auto a13 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a13), *neg(ceiling(x))));

    expr = conjugate(x);
    auto a14 = Assumptions({reals()->contains(x)});
    REQUIRE(eq(*refine(expr, &a14), *x));

    expr = conjugate(x);
    auto a15 = Assumptions({});
    REQUIRE(eq(*refine(expr, &a15), *expr));
}
