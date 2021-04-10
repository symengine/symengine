#include "catch.hpp"
#include <symengine/assumptions.h>
#include <symengine/sets.h>

using SymEngine::RCP;
using SymEngine::symbol;
using SymEngine::Set;
using SymEngine::Basic;
using SymEngine::reals;
using SymEngine::rationals;
using SymEngine::integers;
using SymEngine::Assumptions;

TEST_CASE("Test assumptions", "[assumptions]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Set> s1 = reals();
    RCP<const Set> s2 = integers();
    RCP<const Set> s3 = rationals();

    auto a1 = Assumptions({s1->contains(x)});
    REQUIRE(is_true(a1.is_real(x)));
    REQUIRE(is_indeterminate(a1.is_integer(x)));
    REQUIRE(is_indeterminate(a1.is_real(y)));

    auto a2 = Assumptions({s2->contains(x)});
    REQUIRE(is_true(a2.is_real(x)));
    REQUIRE(is_true(a2.is_integer(x)));
    REQUIRE(is_indeterminate(a2.is_real(y)));
    REQUIRE(is_indeterminate(a2.is_integer(y)));

    auto a3 = Assumptions({s1->contains(x), s2->contains(y)});
    REQUIRE(is_indeterminate(a3.is_integer(x)));
    REQUIRE(is_true(a3.is_integer(y)));
    REQUIRE(is_true(a3.is_real(x)));
    REQUIRE(is_true(a3.is_real(y)));

    auto a4 = Assumptions({s3->contains(x)});
    REQUIRE(is_true(a4.is_rational(x)));
    REQUIRE(is_indeterminate(a4.is_rational(y)));
}
