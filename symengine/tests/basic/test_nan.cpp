#include "catch.hpp"
#include <iostream>
#include <symengine/basic.h>
#include <symengine/nan.h>
#include <symengine/symengine_rcp.h>
#include <symengine/constants.h>
#include <symengine/functions.h>
#include <symengine/pow.h>

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
using SymEngine::gamma;

TEST_CASE("Hash Size for NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;
    RCP<const NaN> b = Nan;

    CHECK(a->__hash__() == b->__hash__());
}

TEST_CASE("NaN Constants", "[NaN]")
{
    CHECK(Nan->__str__() == "nan");
}

TEST_CASE("Boolean tests for NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    CHECK((not a->is_zero() && not a->is_one() && not a->is_minus_one()
           && not a->is_positive() && not a->is_negative()
           && not a->is_complex() && not a->is_exact() && is_a<NaN>(*a)));
}

TEST_CASE("Comparing NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;
    RCP<const NaN> b = Nan;
    RCP<const Basic> i1 = integer(1);

    CHECK(a->compare(*b) == 0);
    CHECK(eq(*a, *b));
    CHECK(neq(*a, *i1));
}

TEST_CASE("Check Derivative", "[NaN]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const NaN> b = Nan;
    CHECK(eq(*b->diff(x), *zero));
}

TEST_CASE("Adding to NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    RCP<const Basic> n1 = a->add(*one);
    CHECK(eq(*n1, *Nan));
}

TEST_CASE("Subtracting from NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;
    RCP<const Basic> r1 = a->sub(*a);
    CHECK(eq(*r1, *Nan));
}

TEST_CASE("Multiplication with NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    RCP<const Basic> n1;
    n1 = a->mul(*integer(-10));
    CHECK(eq(*n1, *Nan));
}

TEST_CASE("Powers of NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    RCP<const Basic> n1;
    n1 = a->pow(*integer(-10));
    CHECK(eq(*n1, *Nan));
}

TEST_CASE("Powers to NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;

    RCP<const Basic> n1;
    n1 = integer(-10)->pow(*a);
    CHECK(eq(*n1, *Nan));
}

TEST_CASE("Evaluate Class of NaN", "[NaN]")
{
    RCP<const NaN> a = Nan;
    RCP<const Basic> n1;

    n1 = sin(a);
    CHECK(eq(*n1, *Nan));
    n1 = cos(a);
    CHECK(eq(*n1, *Nan));
    n1 = tan(a);
    CHECK(eq(*n1, *Nan));
    n1 = csc(a);
    CHECK(eq(*n1, *Nan));
    n1 = sec(a);
    CHECK(eq(*n1, *Nan));
    n1 = cot(a);
    CHECK(eq(*n1, *Nan));
    n1 = asin(a);
    CHECK(eq(*n1, *Nan));
    n1 = acos(a);
    CHECK(eq(*n1, *Nan));
    n1 = atan(a);
    CHECK(eq(*n1, *Nan));
    n1 = acsc(a);
    CHECK(eq(*n1, *Nan));
    n1 = asec(a);
    CHECK(eq(*n1, *Nan));
    n1 = acot(a);
    CHECK(eq(*n1, *Nan));
    n1 = sinh(a);
    CHECK(eq(*n1, *Nan));
    n1 = cosh(a);
    CHECK(eq(*n1, *Nan));
    n1 = tanh(a);
    CHECK(eq(*n1, *Nan));
    n1 = csch(a);
    CHECK(eq(*n1, *Nan));
    n1 = sech(a);
    CHECK(eq(*n1, *Nan));
    n1 = coth(a);
    CHECK(eq(*n1, *Nan));
    n1 = asinh(a);
    CHECK(eq(*n1, *Nan));
    n1 = acosh(a);
    CHECK(eq(*n1, *Nan));
    n1 = atanh(a);
    CHECK(eq(*n1, *Nan));
    n1 = acsch(a);
    CHECK(eq(*n1, *Nan));
    n1 = asech(a);
    CHECK(eq(*n1, *Nan));
    n1 = acoth(a);
    CHECK(eq(*n1, *Nan));
    n1 = log(a);
    CHECK(eq(*n1, *Nan));
    n1 = gamma(a);
    CHECK(eq(*n1, *Nan));
    n1 = abs(a);
    CHECK(eq(*n1, *Nan));
    n1 = exp(a);
    CHECK(eq(*n1, *Nan));
    n1 = erf(a);
    CHECK(eq(*n1, *Nan));
    n1 = erfc(a);
    CHECK(eq(*n1, *Nan));
}