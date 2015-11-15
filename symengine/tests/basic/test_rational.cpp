#include "catch.hpp"

#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/number.h>

using SymEngine::print_stack_on_segfault;
using SymEngine::RCP;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Rational;
using SymEngine::rational;
using SymEngine::Number;

TEST_CASE("Rational", "[rational]")
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> i25 = integer(25);
    
    RCP<const Number> q10_25 = Rational::from_two_ints(*i10, *i25);
    RCP<const Number> q2_5 = Rational::from_two_ints(*i2, *i5);
    RCP<const Number> q = rational(2, 5);

    REQUIRE(q10_25->__eq__(*q2_5));
    REQUIRE(q10_25->__eq__(*q));
}

