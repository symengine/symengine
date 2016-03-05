#include "catch.hpp"
#include <iostream>

#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/sets.h>

using SymEngine::Basic;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Rational;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::Number;
using SymEngine::RCP;
using SymEngine::Interval;

TEST_CASE("Interval : Basic", "[basic]")
{
	RCP<const Interval> r1, r2, r3, r4;
	RCP<const Number> i2 = integer(2);
	RCP<const Number> i20 = integer(20);
	RCP<const Number> im5 = integer(-5);

	r1 = Interval::from_two_nums(*zero, *i20); // [0, 20]
	r2 = Interval::from_two_nums(*im5, *i2); // [-5, 2]

	r3 = r1->interval_intersection(*r2); // [0, 2]
	r4 = Interval::from_two_nums(*zero, *i2); // [0, 2]
	REQUIRE(eq(*r3, *r4));

	r3 = Interval::from_two_nums(*im5, *i2, true, true); // (-5, 2)
	r4 = r3->interval_intersection(*r2);
	REQUIRE(eq(*r3, *r4));

	r3 = r1->interval_union(*r2); // [-5, 20]
	r4 = Interval::from_two_nums(*im5, *i20);
	REQUIRE(eq(*r3, *r4));

	r3 = Interval::from_two_nums(*im5, *i2, false, false); // (-5, 2)
	REQUIRE(r3->is_subset(*r2));
	REQUIRE(not r3->is_proper_subset(*r2));
	REQUIRE(not r3->is_proper_superset(*r2));

	r3 = Interval::from_two_nums(*im5, *i20);
	r4 = Interval::from_two_nums(*zero, *i2);
	REQUIRE(r3->is_superset(*r4));
	REQUIRE(r3->is_proper_superset(*r4));

}
