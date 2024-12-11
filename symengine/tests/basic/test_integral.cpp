#include <symengine/integral.h>
#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/pow.h>
#include <symengine/rational.h>
#include <symengine/symengine_casts.h>
#include "catch.hpp"

using SymEngine::Basic;
using SymEngine::symbol;
using SymEngine::integer;
using SymEngine::real_double;
using SymEngine::Rational;
using SymEngine::RCP;
using SymEngine::Symbol;
using SymEngine::add;
using SymEngine::sin;
using SymEngine::cos;
using SymEngine::pow;
using SymEngine::div;
using SymEngine::integrate;

TEST_CASE("Integration of basic expressions", "[integrals]") {
    RCP<const Symbol> x = symbol("x");

    SECTION("Constant integration") {
        RCP<const Basic> expr = integer(5);
        RCP<const Basic> result = integrate(expr, x);
        REQUIRE(result->__eq__(*mul(integer(5), x)));
    }

    SECTION("Linear integration") {
        RCP<const Basic> expr = x;
        RCP<const Basic> result = integrate(expr, x);
        REQUIRE(result->__eq__(*div(pow(x, integer(2)), integer(2))));
    }

    SECTION("Polynomial integration") {
        RCP<const Basic> expr = add(pow(x, integer(2)), x);
        RCP<const Basic> result = integrate(expr, x);
        REQUIRE(result->__eq__(*add(div(pow(x, integer(3)), integer(3)), div(pow(x, integer(2)), integer(2)))));
    }

    SECTION("Exponential integration") {
        RCP<const Basic> expr = pow(x, integer(2));
        RCP<const Basic> result = integrate(expr, x);
        REQUIRE(result->__eq__(*div(pow(x, integer(3)), integer(3))));
    }

    SECTION("Trigonometric integration: sin(x)") {
        RCP<const Basic> expr = sin(x);
        RCP<const Basic> result = integrate(expr, x);
        REQUIRE(result->__eq__(*mul(SymEngine::minus_one, cos(x))));
    }

    SECTION("Trigonometric integration: cos(x)") {
        RCP<const Basic> expr = cos(x);
        RCP<const Basic> result = integrate(expr, x);
        REQUIRE(result->__eq__(*sin(x)));
    }
}
