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
using SymEngine::interval;
using SymEngine::Set;
using SymEngine::EmptySet;
using SymEngine::emptyset;

TEST_CASE("Interval : Basic", "[basic]")
{
	RCP<const Set> r1, r2, r3, r4;
	RCP<const Number> i2 = integer(2);
	RCP<const Number> i20 = integer(20);
	RCP<const Number> im5 = integer(-5);
	RCP<const Number> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
	RCP<const Number> rat2 = Rational::from_two_ints(*integer(500), *integer(6));

	r1 = interval(zero, i20); // [0, 20]
	r2 = interval(im5, i2); // [-5, 2]

	r3 = r1->_intersection(*r2); // [0, 2]
	r4 = interval(zero, i2); // [0, 2]
	REQUIRE(eq(*r3, *r4));

	r3 = interval(im5, i2, true, true); // (-5, 2)
	r4 = r3->_intersection(*r2);
	REQUIRE(eq(*r3, *r4));

	r3 = r1->_union(*r2); // [-5, 20]
	r4 = interval(im5, i20);
	REQUIRE(eq(*r3, *r4));

	r3 = interval(integer(21), integer(22));
	r4 = r1->_intersection(*r3);
	REQUIRE(eq(*r4, *emptyset()));

	r3 = interval(im5, i2, false, false); // (-5, 2)
	REQUIRE(r3->is_subset(*r2));
	REQUIRE(not r3->is_proper_subset(*r2));
	REQUIRE(not r3->is_proper_superset(*r2));

	r3 = interval(im5, i20);
	r4 = interval(zero, i2);
	REQUIRE(r3->is_superset(*r4));
	REQUIRE(r3->is_proper_superset(*r4));

	r1 = interval(rat1, rat2);// [5/6, 500/6]
	r2 = interval(im5, i2); // [-5, 2]
	r3 = r1->_intersection(*r2);
	r4 = interval(rat1, i2);
	REQUIRE(eq(*r3, *r4));
	REQUIRE(r4->__str__() == "[5/6, 2]");
}

TEST_CASE("EmptySet : Basic", "[basic]")
{
	RCP<const Set> r1;
	r1 = emptyset();

	REQUIRE(r1->is_EmptySet());
	REQUIRE(not r1->is_Interval());
	REQUIRE(r1->__str__() == "EmptySet");
}