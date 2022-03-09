#include "catch.hpp"
#include <symengine/refine.h>

using SymEngine::Assumptions;
using SymEngine::infty;
using SymEngine::integer;
using SymEngine::integers;
using SymEngine::pi;
using SymEngine::Rational;
using SymEngine::reals;
using SymEngine::symbol;
using SymEngine::unevaluated_expr;

TEST_CASE("Test refine", "[refine]")
{
    auto x = symbol("x");
    auto y = symbol("y");

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

    expr = pow(x, integer(2));
    Assumptions a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = pow(pow(x, integer(2)),
               Rational::from_two_ints(*integer(1), *integer(2)));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));

    expr = pow(pow(x, integer(2)),
               Rational::from_two_ints(*integer(1), *integer(2)));
    a = Assumptions({reals()->contains(x)});
    REQUIRE(eq(*refine(expr, &a), *abs(x)));

    expr = pow(pow(x, integer(2)),
               Rational::from_two_ints(*integer(1), *integer(2)));
    a = Assumptions({Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *x));

    expr = pow(pow(x, integer(6)),
               Rational::from_two_ints(*integer(1), *integer(2)));
    a = Assumptions({reals()->contains(x)});
    REQUIRE(eq(*refine(expr, &a), *pow(abs(x), integer(3))));

    expr = log(pow(x, y));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *expr));
    a = Assumptions({reals()->contains(y), Gt(x, integer(0))});
    REQUIRE(eq(*refine(expr, &a), *mul(y, log(x))));

    expr = log(pow(integer(2), y));
    a = Assumptions({reals()->contains(y)});
    REQUIRE(eq(*refine(expr, &a), *mul(y, log(integer(2)))));

    expr = log(integer(8));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *mul(integer(3), log(integer(2)))));

    expr = log(integer(23));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *log(integer(23))));

    expr = log(integer(49));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *mul(integer(2), log(integer(7)))));

    expr = div(log(integer(4)), integer(2));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *log(integer(2))));

    expr = interval(infty(-1), infty(1));
    a = Assumptions({});
    REQUIRE(eq(*refine(expr, &a), *reals()));
    expr = interval(integer(1), integer(2));
    REQUIRE(eq(*refine(expr, &a), *expr));
}
