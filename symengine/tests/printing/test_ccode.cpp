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
using SymEngine::sin;
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
    auto p = sin(x);
    REQUIRE(ccode(*p) == "sin(x)");

    p = function_symbol("f", x);
    REQUIRE(ccode(*p) == "f(x)");

    p = function_symbol("f", pow(integer(2), x));
    REQUIRE(ccode(*p) == "f(pow(2, x))");
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
