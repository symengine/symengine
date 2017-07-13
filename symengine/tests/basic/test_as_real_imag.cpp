#include "catch.hpp"

#include <symengine/real_imag.cpp>
#include <symengine/symengine_casts.h>

using SymEngine::Basic;
using SymEngine::rational;
using SymEngine::one;
using SymEngine::Complex;
using SymEngine::Symbol;
using SymEngine::I;
using SymEngine::sqrt;
using SymEngine::RCP;
using SymEngine::zero;
using SymEngine::neg;
using SymEngine::integer;
using SymEngine::mul;
using SymEngine::add;
using SymEngine::Rational;
using SymEngine::symbol;
using SymEngine::SymEngineException;

TEST_CASE("RealImag: Number and Symbol", "[as_real_imag]")
{
    RCP<const Basic> re, im;
    auto i2 = integer(2), i3 = integer(3);
    auto r1 = Rational::from_two_ints(*i2, *i3);
    auto r2 = Rational::from_two_ints(*i3, *i2);

    as_real_imag(Complex::from_two_nums(*r1, *r2), outArg(re), outArg(im));
    REQUIRE(eq(*re, *r1));
    REQUIRE(eq(*im, *r2));

    as_real_imag(r1, outArg(re), outArg(im));
    REQUIRE(eq(*re, *r1));
    REQUIRE(eq(*im, *zero));

    as_real_imag(neg(i2), outArg(re), outArg(im));
    REQUIRE(eq(*re, *neg(i2)));
    REQUIRE(eq(*im, *zero));

    // Symbol
    CHECK_THROWS_AS(
        as_real_imag(mul(add(i2, I), symbol("x")), outArg(re), outArg(im)),
        SymEngineException);
}

TEST_CASE("RealImag: Mul", "[as_real_imag]")
{
    RCP<const Basic> re, im;
    auto i2 = integer(2), i3 = integer(-3);

    as_real_imag(neg(add(i2, I)), outArg(re), outArg(im));
    REQUIRE(eq(*re, *neg(i2)));
    REQUIRE(eq(*im, *neg(one)));

    CHECK_THROWS_AS(as_real_imag(mul(add(i2, I), pow(I, rational(2, 3))),
                                 outArg(re), outArg(im)),
                    SymEngineException);
}

TEST_CASE("RealImag: Add", "[as_real_imag]")
{
    RCP<const Basic> re, im;
    auto i2 = integer(2), i3 = integer(3);

    as_real_imag(add(add(i2, I), pow(I, rational(2, 3))), outArg(re),
                 outArg(im));
    REQUIRE(eq(*re, *rational(5, 2)));
    REQUIRE(eq(*im, *add(one, div(sqrt(i3), i2))));

    as_real_imag(add(add(i2, I), sqrt(I)), outArg(re), outArg(im));
    REQUIRE(eq(*re, *add(i2, div(sqrt(i2), i2))));
    REQUIRE(eq(*im, *add(one, div(sqrt(i2), i2))));
}

TEST_CASE("RealImag: Pow", "[as_real_imag]")
{
    auto sq = sqrt(neg(I));
    RCP<const Basic> re, im;
    auto i2 = integer(2);

    as_real_imag(sqrt(neg(I)), outArg(re), outArg(im));
    REQUIRE(eq(*re, *div(sqrt(i2), i2)));
    REQUIRE(eq(*im, *neg(div(sqrt(i2), i2))));

    as_real_imag(neg(sqrt(neg(I))), outArg(re), outArg(im));
    REQUIRE(eq(*re, *neg(div(sqrt(i2), i2))));
    REQUIRE(eq(*im, *div(sqrt(i2), i2)));

    CHECK_THROWS_AS(as_real_imag(pow(I, symbol("x")), outArg(re), outArg(im)),
                    SymEngineException);
}
