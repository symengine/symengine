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

    // Evaluating to double when there are complex doubles raise an exception
    SYMENGINE_CHECK_THROW(v.init({x}, *add(complex_double(std::complex<double>(1, 2)), x)), std::runtime_error);

    // When raise_error is False, exception is raised when called only.
    v.init({x}, *add(complex_double(std::complex<double>(1, 2)), x), false);
    SYMENGINE_CHECK_THROW(v.call({1.0}), std::runtime_error);

    // Undefined symbols raise an exception
    SYMENGINE_CHECK_THROW(v.init({x}, *r), std::runtime_error);
}

TEST_CASE("Evaluate to std::complex<double>", "[lambda_complex_double]")
{
    RCP<const Basic> x, y, z, r;
    std::complex<double> d;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");

    r = add(x, add(mul(y, z), pow(x, complex_double(std::complex<double>(3, 4)))));

    LambdaComplexDoubleVisitor v;
    v.init({x, y, z}, *r);

    d = v.call({std::complex<double>(1.5, 1.0), std::complex<double>(2.5, 4.0), std::complex<double>(-8.3, 3.2)});
    REQUIRE(::fabs(d.real() + 32.360749607381) < 1e-12);
    REQUIRE(::fabs(d.imag() + 24.6630395370884) < 1e-12);

    v.init({x, y, z}, *add(x, add(mul(y, z), pow(x, integer(2)))));
    d = v.call({std::complex<double>(1.5, 0.0), std::complex<double>(-1.0, 0.0), std::complex<double>(2.0, 0.0)});
    REQUIRE(::fabs(d.real() - 1.75) < 1e-12);
    REQUIRE(::fabs(d.imag() - 0.0) < 1e-12);

    // Undefined symbols raise an exception
    SYMENGINE_CHECK_THROW(v.init({x}, *r), std::runtime_error);
}
