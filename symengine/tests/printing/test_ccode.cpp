#include "catch.hpp"

#include <symengine/logic.h>
#include <symengine/add.h>
#include <symengine/codegen.h>
#include <symengine/sets.h>

using SymEngine::Basic;
using SymEngine::E;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Interval;
using SymEngine::Eq;
using SymEngine::Ne;
using SymEngine::Le;
using SymEngine::Lt;
using SymEngine::interval;
using SymEngine::symbol;
using SymEngine::piecewise;
using SymEngine::add;
using SymEngine::ccode;
using SymEngine::Inf;
using SymEngine::NegInf;
using SymEngine::boolTrue;
using SymEngine::abs;
using SymEngine::sin;
using SymEngine::cos;
using SymEngine::tan;
using SymEngine::asin;
using SymEngine::acos;
using SymEngine::atan;
using SymEngine::atan2;
using SymEngine::exp;
using SymEngine::log;
using SymEngine::sinh;
using SymEngine::cosh;
using SymEngine::tanh;
using SymEngine::asinh;
using SymEngine::acosh;
using SymEngine::atanh;
using SymEngine::floor;
using SymEngine::ceiling;
using SymEngine::erf;
using SymEngine::erfc;
using SymEngine::gamma;
using SymEngine::loggamma;
using SymEngine::min;
using SymEngine::max;
using SymEngine::sqrt;
using SymEngine::rational;

TEST_CASE("Arithmetic", "[ccode]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto p = add(add(add(add(x, mul(x, y)), pow(x, y)), mul(x, x)),
                 sqrt(integer(2)));
    REQUIRE(ccode(*p) == "x + x*y + sqrt(2) + pow(x, 2) + pow(x, y)");
}

TEST_CASE("Rational", "[ccode]")
{
    auto p = rational(1, 3);
    REQUIRE(ccode(*p) == "1.0/3.0");

    p = rational(1, -3);
    REQUIRE(ccode(*p) == "-1.0/3.0");
}

TEST_CASE("Functions", "[ccode]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");
    auto p = function_symbol("f", x);

    REQUIRE(ccode(*p) == "f(x)");

    p = function_symbol("f", pow(integer(2), x));
    REQUIRE(ccode(*p) == "f(pow(2, x))");

    p = abs(x);
    REQUIRE(ccode(*p) == "fabs(x)");
    p = sin(x);
    REQUIRE(ccode(*p) == "sin(x)");
    p = cos(x);
    REQUIRE(ccode(*p) == "cos(x)");
    p = tan(x);
    REQUIRE(ccode(*p) == "tan(x)");
    p = atan2(x, y);
    REQUIRE(ccode(*p) == "atan2(x, y)");
    // p = exp(x);
    // REQUIRE(ccode(*p) == "exp(x)");  // currently pow(M_E, x) which is
    // technically correct
    p = log(x);
    REQUIRE(ccode(*p) == "log(x)");
    p = sinh(x);
    REQUIRE(ccode(*p) == "sinh(x)");
    p = cosh(x);
    REQUIRE(ccode(*p) == "cosh(x)");
    p = tanh(x);
    REQUIRE(ccode(*p) == "tanh(x)");
    p = asinh(x);
    REQUIRE(ccode(*p) == "asinh(x)");
    p = acosh(x);
    REQUIRE(ccode(*p) == "acosh(x)");
    p = atanh(x);
    REQUIRE(ccode(*p) == "atanh(x)");
    p = floor(x);
    REQUIRE(ccode(*p) == "floor(x)");
    p = ceiling(x);
    REQUIRE(ccode(*p) == "ceil(x)");
    p = erf(x);
    REQUIRE(ccode(*p) == "erf(x)");
    p = erfc(x);
    REQUIRE(ccode(*p) == "erfc(x)");
    p = gamma(x);
    REQUIRE(ccode(*p) == "tgamma(x)");
    p = loggamma(x);
    REQUIRE(ccode(*p) == "lgamma(x)");
    p = max({x, y, z});
    REQUIRE(ccode(*p) == "fmax(x, fmax(y, z))");
    p = min({x, y, z});
    REQUIRE(ccode(*p) == "fmin(x, fmin(y, z))");
}

TEST_CASE("Relationals", "[ccode]")
{
    auto x = symbol("x");
    auto y = symbol("y");

    auto p = Eq(x, y);
    CHECK(ccode(*p) == "x == y");

    p = Ne(x, y);
    CHECK(ccode(*p) == "x != y");

    p = Le(x, y);
    CHECK(ccode(*p) == "x <= y");

    p = Lt(x, y);
    CHECK(ccode(*p) == "x < y");
}

TEST_CASE("Piecewise", "[ccode]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto int1 = interval(NegInf, integer(2), true, false);
    auto int2 = interval(integer(2), integer(5), true, false);
    auto p = piecewise({{x, contains(x, int1)},
                        {y, contains(x, int2)},
                        {add(x, y), boolTrue}});

    REQUIRE(ccode(*p) == "((x <= 2) ? (\n   x\n)\n: ((x > 2 && x <= 5) ? (\n   "
                         "y\n)\n: (\n   x + y\n)))");
}
