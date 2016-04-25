#include "catch.hpp"

#include <symengine/lambda_double.h>

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::real_double;
using SymEngine::symbol;
using SymEngine::add;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::integer;
using SymEngine::vec_basic;
using SymEngine::complex_double;
using SymEngine::LambdaRealDoubleVisitor;
using SymEngine::LambdaComplexDoubleVisitor;
using SymEngine::max;
using SymEngine::E;
using SymEngine::gamma;
using SymEngine::loggamma;
using SymEngine::min;

TEST_CASE("Evaluate to double", "[lambda_double]")
{
    RCP<const Basic> x, y, z, r;
    double d;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");

    r = add(x, add(mul(y, z), pow(x, integer(2))));

    LambdaRealDoubleVisitor v;
    v.init({x, y, z}, *r);

    d = v.call({1.5, 2.0, 3.0});
    REQUIRE(::fabs(d - 9.75) < 1e-12);

    d = v.call({1.5, -1.0, 2.0});
    REQUIRE(::fabs(d - 1.75) < 1e-12);

    r = max({x, add(mul(y, z), integer(3))});
    v.init({x, y, z}, *r);

    d = v.call({4.0, 1.0, 2.5});
    REQUIRE(::fabs(d - 5.5) < 1e-12);

    r = min({pow(x, y), add(mul(y, z), integer(3))});
    v.init({x, y, z}, *r);

    d = v.call({4.0, 2.0, 2.5});
    REQUIRE(::fabs(d - 8.0) < 1e-12);

    // Evaluating to double when there are complex doubles raise an exception
    CHECK_THROWS_AS(
        v.init({x}, *add(complex_double(std::complex<double>(1, 2)), x)),
        std::runtime_error);

    // Undefined symbols raise an exception
    CHECK_THROWS_AS(v.init({x}, *r), std::runtime_error);
}

TEST_CASE("Evaluate to std::complex<double>", "[lambda_complex_double]")
{
    RCP<const Basic> x, y, z, r;
    std::complex<double> d;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");

    r = add(x,
            add(mul(y, z), pow(x, complex_double(std::complex<double>(3, 4)))));

    LambdaComplexDoubleVisitor v;
    v.init({x, y, z}, *r);

    d = v.call({std::complex<double>(1.5, 1.0), std::complex<double>(2.5, 4.0),
                std::complex<double>(-8.3, 3.2)});
    REQUIRE(::fabs(d.real() + 32.360749607381) < 1e-12);
    REQUIRE(::fabs(d.imag() + 24.6630395370884) < 1e-12);

    v.init({x, y, z}, *add(x, add(mul(y, z), pow(x, integer(2)))));
    d = v.call({std::complex<double>(1.5, 0.0), std::complex<double>(-1.0, 0.0),
                std::complex<double>(2.0, 0.0)});
    REQUIRE(::fabs(d.real() - 1.75) < 1e-12);
    REQUIRE(::fabs(d.imag() - 0.0) < 1e-12);

    // Undefined symbols raise an exception
    CHECK_THROWS_AS(v.init({x}, *r), std::runtime_error);
}

TEST_CASE("Evaluate functions", "[lambda_gamma]")
{
    RCP<const Basic> x, y, z, r;
    double d;
    x = symbol("x");

    r = gamma(x);

    LambdaRealDoubleVisitor v;
    v.init({x}, *r);

    d = v.call({1.1});
    REQUIRE(::fabs(d - 0.9513507698668) < 1e-12);

    r = loggamma(x);
    v.init({x}, *r);

    d = v.call({1.3});
    REQUIRE(::fabs(d + 0.10817480950786047) < 1e-12);

    r = add(gamma(x), loggamma(x));
    v.init({x}, *r);

    d = v.call({1.1});
    REQUIRE(::fabs(d - 0.901478328607033459) < 1e-12);

    r = erf(x);
    v.init({x}, *r);

    d = v.call({1.1});
    REQUIRE(::fabs(d - 0.88020506957408169) < 1e-12);
}