#include "catch.hpp"

#include <symengine/mp_class.h>

using SymEngine::integer_class;
using SymEngine::mp_and;
using SymEngine::mp_fdiv_r;

TEST_CASE("integer_class: helper functions", "[mp_class]")
{
	integer_class i = 9, j = 12; // i == 1001, j==1100
	integer_class k;
	mp_and(k,i,j);
	REQUIRE(k == 8); //mp_and is bitwise and

	j = -12;
	mp_fdiv_r(k, j, i);
	REQUIRE(k == 6);
}