#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/symengine_rcp.h>
#include <symengine/expression.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>

using namespace SymEngine;

TEST_CASE("Constructors of expression", "[expression]")
{
	expression e0 = symbol("x");
}

TEST_CASE("Printing of expression", "[expression]")
{
	expression e0 = symbol("x");
	std::cout << e0 << '\n';
}

TEST_CASE("Arithmetic of expression", "[expression]")
{
	expression x = symbol("x"), y = symbol("y");
	auto z = x + y;
	std::cout << z << '\n';
	z += y;
	std::cout << z << '\n';
	REQUIRE(z == x + y + y);
	REQUIRE(z == x + 2 * y);
	std::cout << pow(z,z) << '\n';
	std::cout << pow(z,45) << '\n';
	auto t1 = std::chrono::high_resolution_clock::now();
	auto res = expand(pow(z,45) * pow(z,45));
	std::cout
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-t1).count()
		<< "ns" << std::endl;
	std::cout << res << '\n';
}
