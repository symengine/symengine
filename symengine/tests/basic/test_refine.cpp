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
    Assumptions a = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *x));

    expr = abs(x);
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = abs(x);
    a = Assumptions({Le(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *neg(x)));

    expr = abs(conjugate(x));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *abs(x)));

    expr = sign(x);
    a = Assumptions({Lt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *integer(-1)));

    expr = sign(x);
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = sign(x);
    a = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *integer(1)));

    expr = sign(x);
    a = Assumptions({Eq(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *integer(0)));

    expr = sign(abs(x));
    a = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *integer(1)));

    expr = floor(x);
    a = Assumptions({integers()->contains(x)});
    REQUIRE(eq(*refine(expr, &a), *x));

    expr = floor(x);
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = ceiling(x);
    a = Assumptions({integers()->contains(x)});
    REQUIRE(eq(*refine(expr, &a), *x));

    expr = ceiling(x);
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = ceiling(neg(x));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *neg(floor(x))));

    expr = floor(neg(x));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *neg(ceiling(x))));

    expr = conjugate(x);
    a = Assumptions({reals()->contains(x)});
    REQUIRE(eq(*refine(expr, &a), *x));

    expr = conjugate(x);
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = max({integer(-1), x});
    a = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *x));

    expr = max({integer(-1), x});
    a = Assumptions({Ge(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *x));

    expr = max({integer(-1), x});
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = max({x, y, z});
    a = Assumptions({Ge(x, integer(0)), Ge(y, integer(0)), Ge(z, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = max({x, y, z});
    a = Assumptions({Ge(z, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = max({integer(1), x});
    a = Assumptions({Le(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *integer(1)));

    expr = max({integer(-1), x});
    a = Assumptions({Le(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = min({integer(-1), x});
    a = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *integer(-1)));

    expr = min({integer(-1), x});
    a = Assumptions({Ge(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *integer(-1)));

    expr = min({integer(-1), x});
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = min({x, y, z});
    a = Assumptions({Ge(x, integer(0)), Ge(y, integer(0)), Ge(z, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = min({x, y, z});
    a = Assumptions({Ge(z, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = min({integer(1), x});
    a = Assumptions({Le(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *x));

    expr = min({integer(-1), x});
    a = Assumptions({Le(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *expr));
}
