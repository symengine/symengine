#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/expression.h>
#include <symengine/functions.h>

using SymEngine::Expression;
using SymEngine::symbol;
using SymEngine::eq;
using SymEngine::integer;
using SymEngine::real_double;
using SymEngine::complex_double;
using SymEngine::sin;
using SymEngine::pi;

TEST_CASE("Constructors of Expression", "[Expression]")
{
    Expression e0 = symbol("x");
    REQUIRE(eq(*e0.get_basic(), *symbol("x")));

    Expression e1 = 20;
    REQUIRE(eq(*e1.get_basic(), *integer(20)));

    Expression e2 = 10.0;
    REQUIRE(eq(*e2.get_basic(), *real_double(10.0)));

    Expression e3 = std::complex<double>(1.0, 2.0);
    REQUIRE(
        eq(*e3.get_basic(), *complex_double(std::complex<double>(1.0, 2.0))));
}

TEST_CASE("Printing of Expression", "[Expression]")
{
    Expression e0 = symbol("x");
    std::stringstream s;
    s << e0;
    REQUIRE(s.str() == "x");
}

TEST_CASE("Arithmetic of Expression", "[Expression]")
{
    Expression x = symbol("x"), y = symbol("y");
    auto z = x + y;
    std::cout << z << std::endl;
    z += y;
    std::cout << z << std::endl;
    REQUIRE(z == x + y + y);
    REQUIRE(z == x + 2 * y);
    std::cout << pow_ex(z, z) << std::endl;
    std::cout << pow_ex(z, 45) << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    auto res = expand(pow_ex(z, 45) * pow_ex(z, 45));
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1)
                     .count()
              << "ns" << std::endl;
    std::cout << res << std::endl;
}
