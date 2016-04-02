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
using SymEngine::is_a;

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

    RCP<const Number> r1 = rational(2, 1);
    CHECK(is_a<Integer>(*r1));
    CHECK(r1->__eq__(*integer(2)));

    r1 = rational(9, 3);
    CHECK(is_a<Integer>(*r1));
    CHECK(r1->__eq__(*integer(3)));
}

TEST_CASE("Rational compare", "[rational compare]")
{
    RCP<const Number> q2_5 = rational(2, 5);
    RCP<const Number> q2_5a = rational(2, 5);
    RCP<const Number> qm1_27 = rational(-1, 27);
    RCP<const Integer> im2 = integer(-2);
    RCP<const Integer> i0 = integer(0);
    RCP<const Integer> i2 = integer(2);

    REQUIRE(q2_5->compare(*q2_5a) == 0);
    REQUIRE(q2_5->compare(*qm1_27) == 1);
    REQUIRE(qm1_27->compare(*q2_5) == -1);
    REQUIRE(q2_5->compare(*im2) == 1);
    REQUIRE(q2_5->compare(*i0) == 1);
    REQUIRE(q2_5->compare(*i2) == -1);
    REQUIRE(qm1_27->compare(*im2) == 1);
    REQUIRE(qm1_27->compare(*i0) == -1);
    REQUIRE(qm1_27->compare(*i2) == -1);
}

TEST_CASE("Rational is_power, nth root", "[rational is_power, nth root]")
{
    RCP<const Number> q2_5 = rational(2, 5);
    RCP<const Number> qm1_27 = rational(-1, 27);
    RCP<const Number> q9_25 = rational(9, 25);
    RCP<const Number> qm1_3 = rational(-1, 3);
    RCP<const Number> q3_5 = rational(3, 5);
    RCP<const Number> res;

    REQUIRE(not q2_5->is_perfect_power());
    REQUIRE(qm1_27->is_perfect_power());
    REQUIRE(q9_25->is_perfect_power());
    REQUIRE(not q2_5->nth_root(outArg(res), 2));
    REQUIRE(qm1_27->nth_root(outArg(res), 3));
    REQUIRE(res->__eq__(*qm1_3));
    REQUIRE(q9_25->nth_root(outArg(res), 2));
    REQUIRE(res->__eq__(*q3_5));
    CHECK_THROWS_AS(q9_25->nth_root(outArg(res), 0), std::runtime_error);
}
