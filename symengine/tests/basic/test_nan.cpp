#include "catch.hpp"
#include <iostream>
#include <symengine/basic.h>
#include <symengine/nan.h>
#include <symengine/symengine_rcp.h>
#include <symengine/constants.h>

using SymEngine::Basic;
using SymEngine::is_a;
using SymEngine::Integer;
using SymEngine::Number;
using SymEngine::integer;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::minus_one;
using SymEngine::RCP;
using SymEngine::rcp_dynamic_cast;
using SymEngine::NaN;
using SymEngine::Nan;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Inf;

TEST_CASE("Hash Size for NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;
    RCP<const NaN> b = Nan;

    REQUIRE(a->__hash__() == b->__hash__());
}

TEST_CASE("NaN Constants", "[NaN]")
{
    REQUIRE(Nan->__str__() == "nan");
}

TEST_CASE("Boolean tests for NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    REQUIRE((not a->is_zero() && not a->is_one() && not a->is_minus_one()
             && not a->is_positive() && not a->is_negative()
             && not a->is_complex() && not a->is_exact() && is_a<NaN>(*a)));
}

TEST_CASE("Comparing NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;
    RCP<const NaN> b = Nan;
    RCP<const Basic> i1 = integer(1);

    REQUIRE(a->compare(*b) == 0);
    REQUIRE(eq(*a, *b));
    REQUIRE(neq(*a, *i1));
}

TEST_CASE("Check Derivative", "[NaN]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const NaN> b = Nan;
    REQUIRE(eq(*b->diff(x), *zero));
}

TEST_CASE("Adding to NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    RCP<const Basic> n1 = a->add(*one);
    REQUIRE(eq(*n1, *Nan));
}

TEST_CASE("Subtracting from NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;
    RCP<const Basic> r1 = a->sub(*a);
    REQUIRE(eq(*r1, *Nan));
}

TEST_CASE("Multiplication with NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    RCP<const Basic> n1;
    n1 = a->mul(*integer(-10));
    REQUIRE(eq(*n1, *Nan));
}

TEST_CASE("Powers of NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    RCP<const Basic> n1;
    n1 = a->pow(*integer(-10));
    REQUIRE(eq(*n1, *Nan));
}

TEST_CASE("Powers to NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    RCP<const Basic> n1;
    n1 = integer(-10)->pow(*a);
    REQUIRE(eq(*n1, *Nan));
}