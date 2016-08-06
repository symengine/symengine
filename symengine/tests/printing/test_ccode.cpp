#include "catch.hpp"

#include <symengine/logic.h>
#include <symengine/add.h>
#include <symengine/codegen.h>
#include <symengine/sets.h>

using SymEngine::Basic;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Interval;
using SymEngine::interval;
using SymEngine::symbol;
using SymEngine::piecewise;
using SymEngine::add;
using SymEngine::rcp_static_cast;
using SymEngine::ccode;
using SymEngine::Inf;
using SymEngine::NegInf;
using SymEngine::boolTrue;
using SymEngine::sin;
using SymEngine::sqrt;

TEST_CASE("Arithmetic", "[ccode]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto p = add(add(add(add(x, mul(x, y)), pow(x, y)), mul(x, x)),
                 sqrt(integer(2)));
    REQUIRE(ccode(*p) == "x + x*y + sqrt(2) + pow(x, 2) + pow(x, y)");
}

TEST_CASE("Functions", "[ccode]")
{
    auto x = symbol("x");
    auto p = sin(x);
    REQUIRE(ccode(*p) == "sin(x)");
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
