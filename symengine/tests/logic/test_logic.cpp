#include "catch.hpp"

#include <symengine/logic.h>
#include <symengine/add.h>

using SymEngine::Basic;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Interval;
using SymEngine::interval;
using SymEngine::symbol;
using SymEngine::piecewise;
using SymEngine::add;
using SymEngine::rcp_static_cast;

TEST_CASE("Piecewise : Basic", "[basic]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto int1 = rcp_static_cast<const Interval>(
        interval(integer(1), integer(2), true, false));
    auto int2 = rcp_static_cast<const Interval>(
        interval(integer(2), integer(5), true, false));
    auto int3 = rcp_static_cast<const Interval>(
        interval(integer(5), integer(10), true, false));
    auto p = piecewise({{int1, x}, {int2, y}, {int3, add(x, y)}}, x);

    std::string s
        = "Piecewise((1 <= x < 2, x), (2 <= x < 5, y), (5 <= x < 10, x + y))";
    REQUIRE(s == p->__str__());
}
