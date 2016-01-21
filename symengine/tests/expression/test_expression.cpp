#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/expression.h>

using SymEngine::Expression;
using SymEngine::symbol;

TEST_CASE("Constructors of Expression", "[Expression]")
{
    Expression e0 = symbol("x");
}

TEST_CASE("Printing of Expression", "[Expression]")
{
    Expression e0 = symbol("x");
    std::cout << e0 << std::endl;
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
    std::cout
        << std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()
        << "ns" << std::endl;
    std::cout << res << std::endl;
}
