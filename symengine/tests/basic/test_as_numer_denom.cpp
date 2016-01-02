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

TEST_CASE("NumerDenom: Basic", "[as_numer_denom]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const Basic> r1, num, den;

    r1 = add(x, y);
    r1->as_numer_denom(outArg(num), outArg(den));

    REQUIRE(eq(*num, *r1));
    REQUIRE(eq(*den, *one));

    r1 = add(x, mul(y, pow(x, y)));
    r1->as_numer_denom(outArg(num), outArg(den));
}

TEST_CASE("NumerDenom: Rational", "[as_numer_denom]")
{
    RCP<const Basic> num, den, r2_5, rm6_2;

    r2_5 = Rational::from_two_ints(*integer(2), *integer(5));
    rm6_2 = Rational::from_two_ints(*integer(-6), *integer(2));
    
    r2_5->as_numer_denom(outArg(num), outArg(den));

    REQUIRE(eq(*num, *integer(2)));
    REQUIRE(eq(*den, *integer(5)));
    REQUIRE(is_a<Integer>(*num));

    rm6_2->as_numer_denom(outArg(num), outArg(den));

    REQUIRE(eq(*num, *integer(-3)));
    REQUIRE(eq(*den, *integer(1)));
    REQUIRE(is_a<Integer>(*num));
}