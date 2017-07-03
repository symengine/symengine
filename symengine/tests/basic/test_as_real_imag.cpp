#include "catch.hpp"

#include <symengine/real_imag.cpp>

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Complex;
using SymEngine::Symbol;
using SymEngine::I;
using SymEngine::sqrt;
using SymEngine::RCP;
using SymEngine::zero;
using SymEngine::neg;
using SymEngine::integer;

TEST_CASE("RealImag: Pow", "[as_real_imag]")
{
	auto sq = sqrt(I);
    RCP<const Basic> re, im;
    auto i2 = integer(2);

	as_real_imag(sq, outArg(re), outArg(im));
	REQUIRE(eq(*re, *div(sqrt(i2), i2)));
	REQUIRE(eq(*im, *neg(div(sqrt(i2), i2))));

	// as_real_imag(exp(mul(I, x)), outArg(re), outArg(im));
	// REQUIRE(eq(*re, *div(sqrt(i2), i2)));
	// REQUIRE(eq(*im, *neg(div(sqrt(i2), i2))));

}