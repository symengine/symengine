#include "catch.hpp"
#include <symengine/basic.h>
#include <symengine/integration.h>

using SymEngine::integer;
using SymEngine::Integral;
using SymEngine::integrate;
using SymEngine::make_rcp;
using SymEngine::RCP;
using SymEngine::Symbol;
using SymEngine::symbol;

TEST_CASE("Test Integral", "[Integral]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    auto unsolved1 = integrate(sin(x), x);
    auto unsolved2 = integrate(sin(y), y);

    REQUIRE(unsolved1->compare(*unsolved2) == -1);
    REQUIRE(unsolved1->__eq__(*unsolved1));
    REQUIRE(not unsolved1->__eq__(*unsolved2));
    REQUIRE(not unsolved1->__eq__(*integer(23)));
}

TEST_CASE("Test integrate", "[integrate]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> p = symbol("p");

    // Integrals fully solved by symengine
    REQUIRE(eq(*integrate(a, x), *mul(a, x)));
    REQUIRE(eq(*integrate(sin(a), x), *mul(sin(a), x)));
    auto res = integrate(pow(div(a, x), p), x);
    auto correct
        = div(mul(mul(a, integer(-1)), pow(div(a, x), sub(p, integer(1)))),
              sub(p, integer(1)));
    REQUIRE(eq(*res, *correct));

    // Integrals partially solved by symengine
    REQUIRE(eq(*integrate(mul(x, a), x), *mul(a, make_rcp<Integral>(x, x))));
    REQUIRE(eq(*integrate(mul(mul(integer(2), x), a), x),
               *mul(mul(integer(2), a), make_rcp<Integral>(x, x))));

    // Integrals not yet solved by symengine
    REQUIRE(eq(*integrate(sin(x), x), Integral(sin(x), x)));
    REQUIRE(eq(*integrate(sin(x), x)->get_args()[0], *sin(x)));
    REQUIRE(eq(*integrate(sin(x), x)->get_args()[1], *x));
}
