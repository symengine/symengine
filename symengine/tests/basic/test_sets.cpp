#include "catch.hpp"
#include <iostream>

#include <symengine/basic.h>
#include <symengine/sets.h>
#include <symengine/symengine_rcp.h>

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
using SymEngine::rcp_dynamic_cast;
using SymEngine::Complex;
using SymEngine::symbol;

TEST_CASE("Interval : Basic", "[basic]")
{
    RCP<const Set> r1, r2, r3, r4;
    RCP<const Number> i2 = integer(2);
    RCP<const Number> i20 = integer(20);
    RCP<const Number> im5 = integer(-5);
    RCP<const Number> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Number> rat2
        = Rational::from_two_ints(*integer(500), *integer(6));

    r1 = interval(zero, i20); // [0, 20]
    r2 = interval(im5, i2);   // [-5, 2]

    REQUIRE(r1->is_Interval());
    REQUIRE(not r1->is_EmptySet());
    REQUIRE(not r1->is_FiniteSet());

    r3 = r1->set_intersection(r2); // [0, 2]
    r4 = interval(zero, i2);       // [0, 2]
    REQUIRE(eq(*r3, *r4));
    r3 = interval(im5, i2, true, true); // (-5, 2)
    r4 = r3->set_intersection(r2);
    REQUIRE(eq(*r3, *r4));
    r3 = r1->set_union(r2); // [-5, 20]
    r4 = interval(im5, i20);
    REQUIRE(eq(*r3, *r4));
    r3 = r2->set_union(r1); // [-5, 20]
    REQUIRE(eq(*r3, *r4));
    r3 = interval(integer(21), integer(22));
    r4 = r1->set_intersection(r3);
    REQUIRE(eq(*r4, *emptyset()));
    r3 = interval(im5, i2, false, false); // [-5, 2]
    r4 = interval(integer(3), i20, false, false);
    REQUIRE(r3->compare(*r4) == -1);
    CHECK_THROWS_AS(r3->set_union(r4), std::runtime_error);
    CHECK_THROWS_AS(r4->set_union(r3), std::runtime_error);

    r3 = interval(im5, i2, false, false); // [-5, 2]
    REQUIRE(r3->is_subset(r2));
    REQUIRE(not r3->is_subset(emptyset()));
    REQUIRE(not r3->is_proper_subset(emptyset()));
    REQUIRE(not r3->is_proper_subset(r2));
    REQUIRE(not r3->is_proper_superset(r2));
    r3 = interval(im5, i20);
    r4 = interval(zero, i2);
    REQUIRE(r3->is_superset(r4));
    REQUIRE(r3->is_proper_superset(r4));

    r1 = interval(rat1, rat2); // [5/6, 500/6]
    r2 = interval(im5, i2);    // [-5, 2]
    r3 = r1->set_intersection(r2);
    r4 = interval(rat1, i2);
    REQUIRE(eq(*r3, *r4));
    r3 = r2->set_intersection(r1);
    REQUIRE(eq(*r3, *r4));
    REQUIRE(eq(*emptyset(), *r1->set_intersection(emptyset())));
    REQUIRE(eq(*r1, *r1->set_union(emptyset())));

    REQUIRE(r4->__str__() == "[5/6, 2]");
    REQUIRE(r4->compare(*r3) == 0);
    r4 = interval(rat1, i2, true, true);
    REQUIRE(r4->__str__() == "(5/6, 2)");

    r1 = interval(one, zero);
    REQUIRE(eq(*r1, *emptyset()));
    r1 = interval(one, one, true, true);
    REQUIRE(eq(*r1, *emptyset()));

    r1 = interval(zero, one);
    RCP<const Interval> r5 = rcp_dynamic_cast<const Interval>(r1);

    r2 = interval(zero, one, false, false);
    REQUIRE(eq(*r5->close(), *r1));
    r2 = interval(zero, one, true, false);
    REQUIRE(eq(*r5->Lopen(), *r2));
    r2 = interval(zero, one, false, true);
    REQUIRE(eq(*r5->Ropen(), *r2));
    r2 = interval(zero, one, true, true);
    REQUIRE(eq(*r5->open(), *r2));

    REQUIRE(not r5->__eq__(*r2));
    REQUIRE(r5->__hash__() != emptyset()->__hash__());
    REQUIRE(not r5->__eq__(*emptyset()));

    REQUIRE(r5->Lopen()->compare(*r5) == -1);
    REQUIRE(r5->compare(*r5->Lopen()) == 1);
    REQUIRE(r5->Ropen()->compare(*r5) == 1);
    REQUIRE(r5->compare(*r5->Ropen()) == -1);

    REQUIRE(eq(*r5->get_args()[0], *r5->start_));
    REQUIRE(eq(*r5->get_args()[1], *r5->end_));
    RCP<const Number> c1 = Complex::from_two_nums(*i2, *i20);
    CHECK_THROWS_AS(interval(c1, one), std::runtime_error);
    CHECK_THROWS_AS(r5->diff(symbol("x")), std::runtime_error);
}

TEST_CASE("EmptySet : Basic", "[basic]")
{
    RCP<const Set> r1 = emptyset();
    RCP<const Set> r2 = interval(zero, one);

    REQUIRE(r1->is_EmptySet());
    REQUIRE(not r1->is_Interval());
    REQUIRE(r1->is_FiniteSet());
    REQUIRE(r1->is_subset(r2));
    REQUIRE(r1->is_proper_subset(r2));
    REQUIRE(not r1->is_proper_superset(r2));
    REQUIRE(r1->is_superset(r1));
    REQUIRE(not r1->is_superset(r2));
    REQUIRE(eq(*r1, *r1->set_intersection(r2)));
    REQUIRE(eq(*r2, *r1->set_union(r2)));
    REQUIRE(r1->__str__() == "EmptySet");
    REQUIRE(r1->__hash__() == emptyset()->__hash__());
    REQUIRE(not r1->is_proper_subset(r1));
    REQUIRE(not r1->__eq__(*r2));
    REQUIRE(r1->compare(*emptyset()) == 0);
    CHECK_THROWS_AS(r1->diff(symbol("x")), std::runtime_error);
}
