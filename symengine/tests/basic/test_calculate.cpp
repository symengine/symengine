#include "catch.hpp"
#include <symengine/calculate.h>

using SymEngine::Assumptions;
using SymEngine::Basic;
using SymEngine::calculate;
using SymEngine::complex_double;
using SymEngine::ComplexInf;
using SymEngine::down_cast;
using SymEngine::eq;
using SymEngine::gamma;
using SymEngine::Inf;
using SymEngine::infty;
using SymEngine::Infty;
using SymEngine::integer;
using SymEngine::integers;
using SymEngine::is_a;
using SymEngine::minus_one;
using SymEngine::Nan;
using SymEngine::NegInf;
using SymEngine::NotImplementedError;
using SymEngine::one;
using SymEngine::pi;
using SymEngine::Rational;
using SymEngine::rational;
using SymEngine::RCP;
using SymEngine::real_double;
using SymEngine::RealDouble;
using SymEngine::reals;
using SymEngine::symbol;

TEST_CASE("Test calculate gamma", "[calculate gamma]")
{
    RCP<const Basic> r1;
    RCP<const Basic> r2;
    RCP<const Infty> a = Inf;
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> x = symbol("x");

    r1 = calculate(gamma(Inf));
    REQUIRE(eq(*r1, *Inf));

    r1 = calculate(gamma(NegInf));
    REQUIRE(eq(*r1, *ComplexInf));

    r1 = calculate(gamma(Nan));
    REQUIRE(eq(*r1, *Nan));

    r1 = calculate(gamma(one));
    r2 = one;
    REQUIRE(eq(*r1, *r2));

    r1 = calculate(gamma(minus_one));
    REQUIRE(eq(*r1, *ComplexInf));

    r1 = calculate(gamma(mul(i2, i2)));
    r2 = mul(i2, i3);
    REQUIRE(eq(*r1, *r2));

    r1 = calculate(gamma(div(i3, i2)));
    r2 = div(sqrt(pi), i2);
    REQUIRE(eq(*r1, *r2));

    r1 = calculate(gamma(div(one, i2)));
    r2 = sqrt(pi);
    REQUIRE(eq(*r1, *r2));

    r1 = calculate(gamma(div(im1, i2)));
    r2 = mul(mul(im1, i2), sqrt(pi));
    REQUIRE(eq(*r1, *r2));

    r1 = calculate(gamma(real_double(3.7)));
    REQUIRE(is_a<RealDouble>(*r1));
    REQUIRE(std::abs(down_cast<const RealDouble &>(*r1).i - 4.17065178379660)
            < 1e-12);

    CHECK_THROWS_AS(
        calculate(gamma(complex_double(std::complex<double>(1, 1)))),
        NotImplementedError);

    r1 = calculate(gamma(div(integer(-15), i2)));
    r2 = mul(div(integer(256), integer(2027025)), sqrt(pi));
    REQUIRE(eq(*r1, *r2));

    r1 = calculate(gamma(x));
    REQUIRE(eq(*r1, *gamma(x)));
}
