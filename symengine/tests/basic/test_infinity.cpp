#include "catch.hpp"
#include <iostream>

#include <symengine/complex.h>
#include <symengine/basic.h>
#include <symengine/infinity.h>
#include <symengine/symengine_rcp.h>
#include <symengine/constants.h>
#include <symengine/symengine_exception.h>
#include <symengine/functions.h>
#include <symengine/pow.h>

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
using SymEngine::Infty;
using SymEngine::infty;
using SymEngine::rcp_dynamic_cast;
using SymEngine::Complex;
using SymEngine::Inf;
using SymEngine::NegInf;
using SymEngine::ComplexInf;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Complex;
using SymEngine::NotImplementedError;
using SymEngine::SymEngineException;
using SymEngine::DomainError;
using SymEngine::pi;
using SymEngine::gamma;

TEST_CASE("Constructors for Infinity", "[Infinity]")
{
    RCP<const Number> r1 = rational(1, 1);
    RCP<const Number> rm1 = rational(-1, 1);
    RCP<const Number> r0 = rational(0, 1);

    RCP<const Integer> im1 = integer(-1);
    RCP<const Integer> i0 = integer(0);

    RCP<const Infty> a = Infty::from_direction(r1);
    RCP<const Infty> b = Infty::from_direction(rm1);
    RCP<const Infty> c = Infty::from_direction(r0);

    REQUIRE(a->__str__() == "oo");
    REQUIRE(b->__str__() == "-oo");
    REQUIRE(c->__str__() == "zoo");

    a = infty();
    b = infty(-1);
    c = infty(0);

    REQUIRE(a->__str__() == "oo");
    REQUIRE(b->__str__() == "-oo");
    REQUIRE(c->__str__() == "zoo");

    a = Infty::from_int(1);
    b = Infty::from_direction(im1);
    REQUIRE(a->__str__() == "oo");
    REQUIRE(b->__str__() == "-oo");

    //! Checking copy constructor
    Infty inf2 = Infty(*NegInf);
    REQUIRE(inf2.__str__() == "-oo");

    // RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    // CHECK_THROWS_AS(Infty::from_direction(cx), SymEngineException);
}

TEST_CASE("Hash Size for Infinity", "[Infinity]")
{
    RCP<const Infty> a = infty(1);
    RCP<const Infty> b = infty(0);

    REQUIRE(not eq(*a, *b));
    REQUIRE(not(a->__hash__() == b->__hash__()));
    REQUIRE(eq(*a, *infty()));
    REQUIRE(a->__hash__() == infty(1)->__hash__());
}

TEST_CASE("Infinity Constants", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    REQUIRE(a->__str__() == "oo");
    REQUIRE(b->__str__() == "-oo");
    REQUIRE(c->__str__() == "zoo");
}

TEST_CASE("Boolean tests for Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    REQUIRE((not a->is_zero() && not a->is_one() && not a->is_minus_one()
             && not a->is_negative_infinity() && a->is_positive_infinity()
             && not a->is_unsigned_infinity() && a->is_positive()
             && not a->is_negative() && is_a<Infty>(*a)));
    REQUIRE((not b->is_zero() && not b->is_one() && not b->is_minus_one()
             && b->is_negative_infinity() && not b->is_positive_infinity()
             && not b->is_unsigned_infinity() && not b->is_positive()
             && b->is_negative() && is_a<Infty>(*b)));
    REQUIRE((not c->is_zero() && not c->is_one() && not c->is_minus_one()
             && not c->is_negative_infinity() && not c->is_positive_infinity()
             && c->is_unsigned_infinity() && not c->is_positive()
             && not c->is_negative() && is_a<Infty>(*c)));
}

TEST_CASE("Comparing Infinitys", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

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
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

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
    RCP<const Infty> b = NegInf;
    REQUIRE(eq(*b->diff(x), *zero));
}

TEST_CASE("Adding to Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1 = a->add(*one);
    REQUIRE(n1->__str__() == "oo");
    n1 = b->add(*b);
    REQUIRE(n1->__str__() == "-oo");
    n1 = c->add(*minus_one);
    REQUIRE(n1->__str__() == "zoo");
    CHECK_THROWS_AS(c->add(*c), SymEngineException);
    CHECK_THROWS_AS(c->add(*a), SymEngineException);
    CHECK_THROWS_AS(b->add(*a), SymEngineException);
}

TEST_CASE("Subtracting from Infinity", "[Infinity]")
{
    REQUIRE(eq(*Inf->sub(*NegInf), *Inf));
}

TEST_CASE("Multiplication with Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1 = b->mul(*integer(-10));
    REQUIRE(n1->__str__() == "oo");
    n1 = c->mul(*integer(5));
    REQUIRE(n1->__str__() == "zoo");
    n1 = c->mul(*integer(-5));
    REQUIRE(n1->__str__() == "zoo");

    RCP<const Number> n2 = a->mul(*a);
    REQUIRE(n2->__str__() == "oo");
    n2 = b->mul(*a);
    REQUIRE(n2->__str__() == "-oo");
    n2 = b->mul(*c);
    REQUIRE(n2->__str__() == "zoo");
    n2 = b->mul(*b);
    REQUIRE(n2->__str__() == "oo");
    n2 = c->mul(*c);
    REQUIRE(n2->__str__() == "zoo");

    CHECK_THROWS_AS(a->mul(*zero), SymEngineException);
    CHECK_THROWS_AS(b->mul(*zero), SymEngineException);
    CHECK_THROWS_AS(c->mul(*zero), SymEngineException);

    RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    CHECK_THROWS_AS(c->mul(*cx), NotImplementedError);
}

TEST_CASE("Division of Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1 = b->div(*integer(-10));
    REQUIRE(n1->__str__() == "oo");
    n1 = b->div(*integer(10));
    REQUIRE(n1->__str__() == "-oo");
    n1 = c->div(*minus_one);
    REQUIRE(n1->__str__() == "zoo");
    n1 = a->div(*zero);
    REQUIRE(n1->__str__() == "zoo");
    n1 = b->div(*zero);
    REQUIRE(n1->__str__() == "zoo");
    n1 = c->div(*zero);
    REQUIRE(n1->__str__() == "zoo");

    CHECK_THROWS_AS(a->div(*b), SymEngineException);
    CHECK_THROWS_AS(b->div(*c), SymEngineException);
    CHECK_THROWS_AS(c->div(*c), SymEngineException);
}

TEST_CASE("Powers of Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1 = a->pow(*integer(-10));
    REQUIRE(eq(*n1, *zero));
    n1 = a->pow(*integer(10));
    REQUIRE(n1->__str__() == "oo");
    n1 = a->pow(*zero);
    REQUIRE(eq(*n1, *one));
    n1 = a->pow(*b);
    REQUIRE(eq(*n1, *zero));
    n1 = a->pow(*a);
    REQUIRE(n1->__str__() == "oo");
    n1 = b->pow(*integer(-10));
    REQUIRE(eq(*n1, *zero));
    n1 = b->pow(*zero);
    REQUIRE(eq(*n1, *one));
    n1 = c->pow(*a);
    REQUIRE(n1->__str__() == "zoo");
    n1 = c->pow(*b);
    REQUIRE(eq(*n1, *zero));
    n1 = c->pow(*integer(-10));
    REQUIRE(eq(*n1, *zero));
    n1 = c->pow(*zero);
    REQUIRE(eq(*n1, *one));
    n1 = c->pow(*integer(10));
    REQUIRE(n1->__str__() == "zoo");

    RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    CHECK_THROWS_AS(a->pow(*c), SymEngineException);
    CHECK_THROWS_AS(b->pow(*integer(2)), NotImplementedError);
    CHECK_THROWS_AS(b->pow(*a), NotImplementedError);
    CHECK_THROWS_AS(b->pow(*c), NotImplementedError);
    CHECK_THROWS_AS(b->pow(*cx), NotImplementedError);
    CHECK_THROWS_AS(c->pow(*c), SymEngineException);
}

TEST_CASE("Powers to Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1;
    n1 = integer(10)->pow(*a);
    REQUIRE(n1->__str__() == "oo");
    n1 = rational(2, 5)->pow(*a);
    REQUIRE(eq(*n1, *zero));
    n1 = rational(5, 2)->pow(*a);
    REQUIRE(n1->__str__() == "oo");
    n1 = integer(10)->pow(*b);
    REQUIRE(eq(*n1, *zero));
    n1 = rational(2, 5)->pow(*b);
    REQUIRE(n1->__str__() == "zoo");
    n1 = rational(5, 2)->pow(*b);
    REQUIRE(eq(*n1, *zero));

    RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    CHECK_THROWS_AS(integer(-10)->pow(*a), NotImplementedError);
    CHECK_THROWS_AS(integer(0)->pow(*b), SymEngineException);
    CHECK_THROWS_AS(integer(1)->pow(*c), SymEngineException);
    CHECK_THROWS_AS(rational(3, 3)->pow(*c), SymEngineException);
    CHECK_THROWS_AS(integer(-3)->pow(*c), SymEngineException);
    CHECK_THROWS_AS(cx->pow(*c), NotImplementedError);
}

TEST_CASE("Evaluate Class of Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;
    RCP<const Basic> r;

    CHECK_THROWS_AS(sin(Inf), DomainError);
    CHECK_THROWS_AS(cos(Inf), DomainError);
    CHECK_THROWS_AS(tan(Inf), DomainError);
    CHECK_THROWS_AS(csc(Inf), DomainError);
    CHECK_THROWS_AS(sec(Inf), DomainError);
    CHECK_THROWS_AS(cot(Inf), DomainError);
    CHECK_THROWS_AS(asin(Inf), DomainError);
    CHECK_THROWS_AS(acos(Inf), DomainError);
    CHECK_THROWS_AS(acsc(Inf), DomainError);
    CHECK_THROWS_AS(asec(Inf), DomainError);
    CHECK_THROWS_AS(sin(ComplexInf), DomainError);

    r = atan(Inf);
    REQUIRE(eq(*r, *div(pi, integer(2))));

    r = acot(Inf);
    REQUIRE(eq(*r, *zero));

    r = abs(ComplexInf);
    REQUIRE(eq(*r, *a));

    r = gamma(Inf);
    REQUIRE(eq(*r, *a));

    r = sinh(NegInf);
    REQUIRE(eq(*r, *b));

    r = exp(NegInf);
    REQUIRE(eq(*r, *zero));
}