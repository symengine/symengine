#include "catch.hpp"

#include <symengine/mp_class.h>
#include <symengine/pow.h>
#include <symengine/integer.h>

using SymEngine::integer_class;
using SymEngine::mp_and;
using SymEngine::mp_fdiv_r;
using SymEngine::Integer;
using SymEngine::make_rcp;
using SymEngine::RCP;

TEST_CASE("integer_class: helper functions", "[mp_class]")
{
	integer_class i = 9, j = 12; // i == 1001, j==1100
	integer_class k;
	mp_and(k,i,j);
	REQUIRE(k == 8); //mp_and is bitwise and

	j = -12;
	mp_fdiv_r(k, j, i);
	REQUIRE(k == 6);

	RCP<const Integer> a = make_rcp<const Integer>(2);
	RCP<const Integer> b = make_rcp<const Integer>(3);
	pow(a,b);
}