#include "catch.hpp"

#include <symengine/logic.h>
#include <symengine/add.h>
#include <symengine/real_double.h>

using SymEngine::Basic;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::real_double;
using SymEngine::Interval;
using SymEngine::interval;
using SymEngine::symbol;
using SymEngine::piecewise;
using SymEngine::contains;
using SymEngine::Contains;
using SymEngine::add;
using SymEngine::boolTrue;
using SymEngine::boolFalse;
using SymEngine::is_a;
using SymEngine::vec_basic;
using SymEngine::unified_eq;
using SymEngine::zero;
using SymEngine::one;
using SymEngine::eq;
using SymEngine::boolean;

TEST_CASE("BooleanAtom : Basic", "[basic]")
{
    REQUIRE(boolTrue->__str__() == "True");
    REQUIRE(boolFalse->__str__() == "False");

    vec_basic v = boolTrue->get_args();
    vec_basic u;
    REQUIRE(unified_eq(v, u));

    auto x = symbol("x");
    CHECK_THROWS_AS(boolTrue->diff(x), std::runtime_error);

    REQUIRE(not eq(*boolTrue, *boolFalse));
    REQUIRE(eq(*boolFalse, *boolean(false)));
}

TEST_CASE("Contains", "[logic]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto int1 = interval(integer(1), integer(2), false, false);

    auto p = contains(real_double(1.5), int1);
    REQUIRE(eq(*p, *boolTrue));

    p = contains(integer(3), int1);
    REQUIRE(eq(*p, *boolFalse));

    p = contains(x, int1);
    REQUIRE(is_a<Contains>(*p));
    REQUIRE(p->__str__() == "Contains(x, [1, 2])");
    REQUIRE(eq(*p, *p));

    vec_basic v = p->get_args();
    vec_basic u = {x, int1};
    REQUIRE(unified_eq(v, u));

    CHECK_THROWS_AS(p->diff(x), std::runtime_error);
}

TEST_CASE("Piecewise", "[logic]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto int1 = interval(integer(1), integer(2), true, false);
    auto int2 = interval(integer(2), integer(5), true, false);
    auto int3 = interval(integer(5), integer(10), true, false);
    auto p = piecewise({{x, contains(x, int1)},
                        {y, contains(x, int2)},
                        {add(x, y), contains(x, int3)}});

    std::string s = "Piecewise((x, Contains(x, (1, 2])), (y, Contains(x, (2, "
                    "5])), (x + y, Contains(x, (5, 10])))";
    REQUIRE(s == p->__str__());

    auto q = piecewise({{one, contains(x, int1)},
                        {zero, contains(x, int2)},
                        {one, contains(x, int3)}});

    REQUIRE(eq(*p->diff(x), *q));
}
