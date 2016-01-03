#include "catch.hpp"

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/visitor.h>
#include <symengine/eval_double.h>
#include <symengine/numer_denom.cpp>

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Rational;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::Number;
using SymEngine::pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::has_symbol;
using SymEngine::is_a;
using SymEngine::as_numer_denom;

TEST_CASE("NumerDenom: Basic", "[as_numer_denom]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const Basic> r1, num, den;

    r1 = add(x, y);
    as_numer_denom(r1, outArg(num), outArg(den));
    REQUIRE(eq(*num, *r1));
    REQUIRE(eq(*den, *one));

    r1 = add(x, mul(y, pow(x, y)));
    as_numer_denom(r1, outArg(num), outArg(den));
    REQUIRE(eq(*num, *r1));
    REQUIRE(eq(*den, *one));
}

TEST_CASE("NumerDenom: Rational", "[as_numer_denom]")
{
    RCP<const Basic> num, den, r2_5, r6_m2;

    r2_5 = Rational::from_two_ints(*integer(2), *integer(5));
    r6_m2 = Rational::from_two_ints(*integer(6), *integer(-2));

    as_numer_denom(r2_5, outArg(num), outArg(den));
    REQUIRE(eq(*num, *integer(2)));
    REQUIRE(eq(*den, *integer(5)));
    REQUIRE(is_a<Integer>(*num));

    as_numer_denom(r6_m2, outArg(num), outArg(den));
    REQUIRE(eq(*num, *integer(-3)));
    REQUIRE(eq(*den, *integer(1)));
    REQUIRE(is_a<Integer>(*num));
}

TEST_CASE("NumerDenom: Mul", "[as_numer_denom]")
{
    RCP<const Basic> num, den, r2_5, rm6_2, r1;
    RCP<const Symbol> x = symbol("x");

    r2_5 = Rational::from_two_ints(*integer(2), *integer(5));
    rm6_2 = Rational::from_two_ints(*integer(-6), *integer(2));

    r1 = mul(x, r2_5);
    as_numer_denom(r1, outArg(num), outArg(den));
    REQUIRE(eq(*num, *mul(x, integer(2))));
    REQUIRE(eq(*den, *integer(5)));

    r1 = mul(r1, rm6_2);
    as_numer_denom(r1, outArg(num), outArg(den));
    REQUIRE(eq(*num, *mul(x, integer(-6))));
    REQUIRE(eq(*den, *integer(5)));
}

TEST_CASE("NumerDenom: Pow", "[as_numer_denom]")
{
    RCP<const Basic> num, den, r2_5, i3, im3, r1;
    RCP<const Symbol> x = symbol("x");

    i3 = integer(3);
    im3 = integer(-3);
    r2_5 = Rational::from_two_ints(*integer(2), *integer(5));

    r1 = pow(i3, r2_5);
    as_numer_denom(r1, outArg(num), outArg(den));
    REQUIRE(eq(*num, *r1));
    REQUIRE(eq(*den, *one));

    r1 = pow(r2_5, i3);
    as_numer_denom(r1, outArg(num), outArg(den));
    REQUIRE(eq(*num, *pow(integer(2), i3)));
    REQUIRE(eq(*den, *pow(integer(5), i3)));

    r1 = pow(r2_5, im3);
    as_numer_denom(r1, outArg(num), outArg(den));
    REQUIRE(eq(*num, *pow(integer(5), i3)));
    REQUIRE(eq(*den, *pow(integer(2), i3)));

    r1 = pow(r2_5, mul(im3, x));
    as_numer_denom(r1, outArg(num), outArg(den));
    REQUIRE(eq(*num, *pow(integer(5), mul(i3, x))));
    REQUIRE(eq(*den, *pow(integer(2), mul(i3, x))));
}
