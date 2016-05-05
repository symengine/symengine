#include "catch.hpp"
#include <iostream>

#include <symengine/complex.h>
#include <symengine/basic.h>
#include <symengine/infinity.h>
#include <symengine/symengine_rcp.h>
#include <symengine/constants.h>

using SymEngine::Basic;
using SymEngine::Number;
using SymEngine::is_a;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Rational;
using SymEngine::rational;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::minus_one;
using SymEngine::RCP;
using SymEngine::Infinit;
using SymEngine::infinit;
using SymEngine::rcp_dynamic_cast;
using SymEngine::Complex;
using SymEngine::Inf;
using SymEngine::NegInf;
using SymEngine::ComplexInf;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Complex;

TEST_CASE("Constructors for Infinity", "[Infinity]")
{
    RCP<const Number> r1 = rational(1, 1);
    RCP<const Number> rm1 = rational(-1, 1);
    RCP<const Number> r0 = rational(0, 1);

    RCP<const Integer> im1 = integer(-1);
    RCP<const Integer> i0 = integer(0);

    RCP<const Infinit> a = Infinit::from_direction(r1);
    RCP<const Infinit> b = Infinit::from_direction(rm1);
    RCP<const Infinit> c = Infinit::from_direction(r0);

    REQUIRE(a->__str__() == "+oo");
    REQUIRE(b->__str__() == "-oo");
    REQUIRE(c->__str__() == "zoo");

    a = infinit();
    b = infinit(-1);
    c = infinit(0);

    REQUIRE(a->__str__() == "+oo");
    REQUIRE(b->__str__() == "-oo");
    REQUIRE(c->__str__() == "zoo");

    a = Infinit::from_int(1);
    b = Infinit::from_direction(im1);
    REQUIRE(a->__str__() == "+oo");
    REQUIRE(b->__str__() == "-oo");

    Infinit inf = Infinit();
    REQUIRE(inf.__str__() == "+oo");

    //! Checking copy constructor
    Infinit inf2 = Infinit(*NegInf);
    REQUIRE(inf2.__str__() == "-oo");

    RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    CHECK_THROWS_AS(Infinit::from_direction(cx), std::runtime_error);
}

TEST_CASE("Hash Size for Infinity", "[Infinity]")
{
    RCP<const Infinit> a = infinit(1);
    RCP<const Infinit> b = infinit(0);

    REQUIRE(not eq(*a, *b));
    REQUIRE(not(a->__hash__() == b->__hash__()));
    REQUIRE(eq(*a, *infinit()));
    REQUIRE(a->__hash__() == infinit(1)->__hash__());
}

TEST_CASE("Infinity Constants", "[Infinity]")
{
    RCP<const Infinit> a = Inf;
    RCP<const Infinit> b = NegInf;
    RCP<const Infinit> c = ComplexInf;

    REQUIRE(a->__str__() == "+oo");
    REQUIRE(b->__str__() == "-oo");
    REQUIRE(c->__str__() == "zoo");
}

TEST_CASE("Boolean tests for Infinity", "[Infinity]")
{
    RCP<const Infinit> a = Inf;
    RCP<const Infinit> b = NegInf;
    RCP<const Infinit> c = ComplexInf;

    REQUIRE((not a->is_zero() && not a->is_one() && not a->is_minus_one()
             && not a->is_negative_infinity() && a->is_positive_infinity()
             && not a->is_unsigned_infinity() && a->is_positive()
             && not a->is_negative() && is_a<Infinit>(*a)));
    REQUIRE((not b->is_zero() && not b->is_one() && not b->is_minus_one()
             && b->is_negative_infinity() && not b->is_positive_infinity()
             && not b->is_unsigned_infinity() && not b->is_positive()
             && b->is_negative() && is_a<Infinit>(*b)));
    REQUIRE((not c->is_zero() && not c->is_one() && not c->is_minus_one()
             && not c->is_negative_infinity() && not c->is_positive_infinity()
             && c->is_unsigned_infinity() && not c->is_positive()
             && not c->is_negative() && is_a<Infinit>(*c)));
}

TEST_CASE("Comparing Infinitys", "[Infinity]")
{
    RCP<const Infinit> a = Inf;
    RCP<const Infinit> b = NegInf;
    RCP<const Infinit> c = ComplexInf;

    REQUIRE(a->compare(*b) == 1);
    REQUIRE(c->compare(*c) == 0);
    REQUIRE(c->compare(*a) == -1);
    REQUIRE(a->compare(*c) == 1);
    REQUIRE(not c->__eq__(*a));
    REQUIRE(b->__eq__(*b));
    REQUIRE(not c->__eq__(*zero));
}

TEST_CASE("Checking arguments returned", "[Infinity]")
{
    RCP<const Infinit> a = Inf;
    RCP<const Infinit> b = NegInf;
    RCP<const Infinit> c = ComplexInf;

    REQUIRE(eq(*a->get_direction(), *one));
    REQUIRE(eq(*b->get_direction(), *minus_one));
    REQUIRE(eq(*c->get_direction(), *zero));
    REQUIRE(eq(*a->get_args()[0], *one));
    REQUIRE(eq(*b->get_args()[0], *minus_one));
    REQUIRE(eq(*c->get_args()[0], *zero));
}

TEST_CASE("Check Derivative", "[Infinity]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Infinit> b = NegInf;
    REQUIRE(eq(*b->diff(x), *zero));
}

TEST_CASE("Adding to Infinity", "[Infinity]")
{
    RCP<const Infinit> a = Inf;
    RCP<const Infinit> b = NegInf;
    RCP<const Infinit> c = ComplexInf;

    RCP<const Number> n1 = a->add(*one);
    REQUIRE(n1->__str__() == "+oo");
    n1 = b->add(*b);
    REQUIRE(n1->__str__() == "-oo");
    n1 = c->add(*minus_one);
    REQUIRE(n1->__str__() == "zoo");
    CHECK_THROWS_AS(c->add(*c), std::runtime_error);
    CHECK_THROWS_AS(c->add(*a), std::runtime_error);
    CHECK_THROWS_AS(b->add(*a), std::runtime_error);
}

TEST_CASE("Subtracting from Infinity", "[Infinity]")
{
    REQUIRE(eq(*Inf->sub(*NegInf), *Inf));
}

TEST_CASE("Multiplication with Infinity", "[Infinity]")
{
    RCP<const Infinit> a = Inf;
    RCP<const Infinit> b = NegInf;
    RCP<const Infinit> c = ComplexInf;

    RCP<const Number> n1 = b->mul(*integer(-10));
    REQUIRE(n1->__str__() == "+oo");
    n1 = c->mul(*integer(5));
    REQUIRE(n1->__str__() == "zoo");
    n1 = c->mul(*integer(-5));
    REQUIRE(n1->__str__() == "zoo");

    RCP<const Number> n2 = a->mul(*a);
    REQUIRE(n2->__str__() == "+oo");
    n2 = b->mul(*a);
    REQUIRE(n2->__str__() == "-oo");
    n2 = b->mul(*c);
    REQUIRE(n2->__str__() == "zoo");
    n2 = b->mul(*b);
    REQUIRE(n2->__str__() == "+oo");
    n2 = c->mul(*c);
    REQUIRE(n2->__str__() == "zoo");

    CHECK_THROWS_AS(a->mul(*zero), std::runtime_error);
    CHECK_THROWS_AS(b->mul(*zero), std::runtime_error);
    CHECK_THROWS_AS(c->mul(*zero), std::runtime_error);

    RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    CHECK_THROWS_AS(c->mul(*cx), std::runtime_error);
}
