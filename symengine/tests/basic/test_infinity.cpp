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
#include <symengine/complex_double.h>

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
using SymEngine::I;
using SymEngine::Nan;
using SymEngine::erf;
using SymEngine::erfc;
using SymEngine::I;
using SymEngine::make_rcp;
using SymEngine::complex_double;

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

    CHECK(eq(*a, *Inf));
    CHECK(eq(*b, *NegInf));
    CHECK(eq(*c, *ComplexInf));

    CHECK_THROWS_AS(a->is_canonical(complex_double(std::complex<double>(2, 3))),
                    NotImplementedError);
    CHECK_THROWS_AS(
        a->is_canonical(Complex::from_two_nums(*integer(1), *integer(2))),
        NotImplementedError);

    REQUIRE(not(a->is_canonical(integer(2))));

    a = infty();
    b = infty(-1);
    c = infty(0);

    CHECK(eq(*a, *Inf));
    CHECK(eq(*b, *NegInf));
    CHECK(eq(*c, *ComplexInf));

    a = Infty::from_int(1);
    b = Infty::from_direction(im1);
    CHECK(eq(*a, *Inf));
    CHECK(eq(*b, *NegInf));

    //! Checking copy constructor
    Infty inf2 = Infty(*NegInf);
    CHECK(eq(inf2, *NegInf));

    // RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    // CHECK_THROWS_AS(Infty::from_direction(cx), SymEngineException);
}

TEST_CASE("Hash Size for Infinity", "[Infinity]")
{
    RCP<const Infty> a = infty(1);
    RCP<const Infty> b = infty(0);

    CHECK(not eq(*a, *b));
    CHECK(not(a->__hash__() == b->__hash__()));
    CHECK(eq(*a, *infty()));
    CHECK(a->__hash__() == infty(1)->__hash__());
}

TEST_CASE("Infinity Constants", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    CHECK(eq(*a, *Inf));
    CHECK(eq(*b, *NegInf));
    CHECK(eq(*c, *ComplexInf));
}

TEST_CASE("Boolean tests for Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    CHECK((not a->is_zero() && not a->is_one() && not a->is_minus_one()
           && not a->is_negative_infinity() && a->is_positive_infinity()
           && not a->is_unsigned_infinity() && a->is_positive()
           && not a->is_negative() && is_a<Infty>(*a)));
    CHECK((not b->is_zero() && not b->is_one() && not b->is_minus_one()
           && b->is_negative_infinity() && not b->is_positive_infinity()
           && not b->is_unsigned_infinity() && not b->is_positive()
           && b->is_negative() && is_a<Infty>(*b)));
    CHECK((not c->is_zero() && not c->is_one() && not c->is_minus_one()
           && not c->is_negative_infinity() && not c->is_positive_infinity()
           && c->is_unsigned_infinity() && not c->is_positive()
           && not c->is_negative() && c->is_complex() && is_a<Infty>(*c)));
}

TEST_CASE("Comparing Infinitys", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    CHECK(a->compare(*b) == 1);
    CHECK(c->compare(*c) == 0);
    CHECK(c->compare(*a) == -1);
    CHECK(a->compare(*c) == 1);
    CHECK(not c->__eq__(*a));
    CHECK(b->__eq__(*b));
    CHECK(not c->__eq__(*zero));
}

TEST_CASE("Checking arguments returned", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    CHECK(eq(*a->get_direction(), *one));
    CHECK(eq(*b->get_direction(), *minus_one));
    CHECK(eq(*c->get_direction(), *zero));
    CHECK(eq(*a->get_args()[0], *one));
    CHECK(eq(*b->get_args()[0], *minus_one));
    CHECK(eq(*c->get_args()[0], *zero));
}

TEST_CASE("Check Derivative", "[Infinity]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Infty> b = NegInf;
    CHECK(eq(*b->diff(x), *zero));
}

TEST_CASE("Adding to Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1 = a->add(*one);
    CHECK(eq(*n1, *Inf));
    n1 = b->add(*b);
    CHECK(eq(*n1, *NegInf));
    n1 = c->add(*minus_one);
    CHECK(eq(*n1, *ComplexInf));

    n1 = c->add(*a);
    CHECK(eq(*n1, *Nan));
    n1 = c->add(*c);
    CHECK(eq(*n1, *Nan));
    n1 = b->add(*a);
    CHECK(eq(*n1, *Nan));
}

TEST_CASE("Subtracting from Infinity", "[Infinity]")
{
    CHECK(eq(*Inf->sub(*NegInf), *Inf));
}

TEST_CASE("Multiplication with Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1 = b->mul(*integer(-10));
    CHECK(eq(*n1, *Inf));
    n1 = c->mul(*integer(5));
    CHECK(eq(*n1, *ComplexInf));
    n1 = c->mul(*integer(-5));
    CHECK(eq(*n1, *ComplexInf));

    RCP<const Number> n2 = a->mul(*a);
    CHECK(eq(*n2, *Inf));
    n2 = b->mul(*a);
    CHECK(eq(*n2, *NegInf));
    n2 = b->mul(*c);
    CHECK(eq(*n2, *ComplexInf));
    n2 = b->mul(*b);
    CHECK(eq(*n2, *Inf));
    n2 = c->mul(*c);
    CHECK(eq(*n2, *ComplexInf));

    n2 = a->mul(*zero);
    CHECK(eq(*n2, *Nan));
    n2 = b->mul(*zero);
    CHECK(eq(*n2, *Nan));
    n2 = c->mul(*zero);
    CHECK(eq(*n2, *Nan));

    RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    CHECK_THROWS_AS(c->mul(*cx), NotImplementedError);
}

TEST_CASE("Division of Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1 = b->div(*integer(-10));
    CHECK(eq(*n1, *Inf));
    n1 = b->div(*integer(10));
    CHECK(eq(*n1, *NegInf));
    n1 = c->div(*minus_one);
    CHECK(eq(*n1, *ComplexInf));
    n1 = a->div(*zero);
    CHECK(eq(*n1, *ComplexInf));
    n1 = b->div(*zero);
    CHECK(eq(*n1, *ComplexInf));
    n1 = c->div(*zero);
    CHECK(eq(*n1, *ComplexInf));

    n1 = a->div(*b);
    CHECK(eq(*n1, *Nan));
    n1 = b->div(*c);
    CHECK(eq(*n1, *Nan));
    n1 = c->div(*c);
    CHECK(eq(*n1, *Nan));
}

TEST_CASE("Powers of Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1 = a->pow(*integer(-10));
    CHECK(eq(*n1, *zero));
    n1 = a->pow(*integer(10));
    CHECK(eq(*n1, *Inf));
    n1 = a->pow(*zero);
    CHECK(eq(*n1, *one));
    n1 = a->pow(*b);
    CHECK(eq(*n1, *zero));
    n1 = a->pow(*a);
    CHECK(eq(*n1, *Inf));
    n1 = b->pow(*integer(-10));
    CHECK(eq(*n1, *zero));
    n1 = b->pow(*zero);
    CHECK(eq(*n1, *one));
    n1 = c->pow(*a);
    CHECK(eq(*n1, *ComplexInf));
    n1 = c->pow(*b);
    CHECK(eq(*n1, *zero));
    n1 = c->pow(*integer(-10));
    CHECK(eq(*n1, *zero));
    n1 = c->pow(*zero);
    CHECK(eq(*n1, *one));
    n1 = c->pow(*integer(10));
    CHECK(eq(*n1, *ComplexInf));
    n1 = a->pow(*c);
    CHECK(eq(*n1, *Nan));
    n1 = b->pow(*a);
    CHECK(eq(*n1, *Nan));
    n1 = b->pow(*c);
    CHECK(eq(*n1, *Nan));
    n1 = c->pow(*c);
    CHECK(eq(*n1, *Nan));

    RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    CHECK_THROWS_AS(b->pow(*integer(2)), NotImplementedError);
    CHECK_THROWS_AS(b->pow(*cx), NotImplementedError);
}

TEST_CASE("Powers to Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;

    RCP<const Number> n1;
    n1 = integer(10)->pow(*a);
    CHECK(eq(*n1, *Inf));
    n1 = rational(2, 5)->pow(*a);
    CHECK(eq(*n1, *zero));
    n1 = rational(5, 2)->pow(*a);
    CHECK(eq(*n1, *Inf));
    n1 = integer(10)->pow(*b);
    CHECK(eq(*n1, *zero));
    n1 = rational(2, 5)->pow(*b);
    CHECK(eq(*n1, *ComplexInf));
    n1 = rational(5, 2)->pow(*b);
    CHECK(eq(*n1, *zero));

    n1 = integer(1)->pow(*c);
    CHECK(eq(*n1, *Nan));
    n1 = rational(3, 3)->pow(*c);

    RCP<const Number> cx = Complex::from_two_nums(*integer(1), *integer(1));
    CHECK_THROWS_AS(integer(-10)->pow(*a), NotImplementedError);
    CHECK_THROWS_AS(integer(0)->pow(*b), SymEngineException);
    CHECK_THROWS_AS(integer(10)->pow(*c), SymEngineException);
    CHECK_THROWS_AS(integer(-3)->pow(*c), SymEngineException);
    CHECK_THROWS_AS(cx->pow(*c), NotImplementedError);
}

TEST_CASE("Evaluate Class of Infinity", "[Infinity]")
{
    RCP<const Infty> a = Inf;
    RCP<const Infty> b = NegInf;
    RCP<const Infty> c = ComplexInf;
    RCP<const Basic> r1, r2;

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
    CHECK_THROWS_AS(asech(ComplexInf), DomainError);
    CHECK_THROWS_AS(erfc(ComplexInf), DomainError);
    CHECK_THROWS_AS(atan(ComplexInf), DomainError);
    CHECK_THROWS_AS(acot(ComplexInf), DomainError);
    CHECK_THROWS_AS(sinh(ComplexInf), DomainError);
    CHECK_THROWS_AS(csch(ComplexInf), DomainError);
    CHECK_THROWS_AS(cosh(ComplexInf), DomainError);
    CHECK_THROWS_AS(sech(ComplexInf), DomainError);
    CHECK_THROWS_AS(tanh(ComplexInf), DomainError);
    CHECK_THROWS_AS(coth(ComplexInf), DomainError);
    CHECK_THROWS_AS(asinh(ComplexInf), DomainError);
    CHECK_THROWS_AS(acosh(ComplexInf), DomainError);
    CHECK_THROWS_AS(atanh(ComplexInf), DomainError);
    CHECK_THROWS_AS(acoth(ComplexInf), DomainError);
    CHECK_THROWS_AS(acsch(ComplexInf), DomainError);
    CHECK_THROWS_AS(exp(ComplexInf), DomainError);
    CHECK_THROWS_AS(erf(ComplexInf), DomainError);

    r1 = atan(Inf);
    CHECK(eq(*r1, *div(pi, integer(2))));

    r1 = acot(Inf);
    CHECK(eq(*r1, *zero));

    r1 = abs(ComplexInf);
    CHECK(eq(*r1, *a));

    r1 = atan(NegInf);
    CHECK(eq(*r1, *mul(minus_one, (div(pi, integer(2))))));

    r1 = acot(Inf);
    CHECK(eq(*r1, *zero));

    r1 = csch(Inf);
    CHECK(eq(*r1, *zero));

    r1 = cosh(Inf);
    CHECK(eq(*r1, *Inf));

    r1 = sech(Inf);
    CHECK(eq(*r1, *zero));

    r1 = tanh(Inf);
    CHECK(eq(*r1, *one));

    r1 = tanh(NegInf);
    CHECK(eq(*r1, *minus_one));

    r1 = coth(Inf);
    CHECK(eq(*r1, *one));

    r1 = coth(NegInf);
    CHECK(eq(*r1, *minus_one));

    r1 = asinh(Inf);
    CHECK(eq(*r1, *Inf));

    r1 = acsch(Inf);
    REQUIRE(eq(*r1, *zero));

    r1 = acosh(NegInf);
    CHECK(eq(*r1, *Inf));

    r1 = atanh(Inf);
    CHECK(eq(*r1, *mul(minus_one, div(mul(pi, I), integer(2)))));

    r1 = atanh(NegInf);
    CHECK(eq(*r1, *div(mul(pi, I), integer(2))));

    r1 = acoth(NegInf);
    CHECK(eq(*r1, *zero));

    r1 = abs(ComplexInf);
    CHECK(eq(*r1, *a));

    r1 = gamma(Inf);
    CHECK(eq(*r1, *a));

    r1 = sinh(NegInf);
    CHECK(eq(*r1, *b));

    r1 = exp(NegInf);
    CHECK(eq(*r1, *zero));

    r1 = asech(Inf);
    r2 = mul(mul(I, pi), div(one, integer(2)));
    CHECK(eq(*r1, *r2));

    r1 = exp(NegInf);
    CHECK(eq(*r1, *zero));

    r1 = erf(Inf);
    CHECK(eq(*r1, *one));

    r1 = erfc(Inf);
    CHECK(eq(*r1, *zero));

    r1 = erf(NegInf);
    CHECK(eq(*r1, *minus_one));

    r1 = erfc(NegInf);
    REQUIRE(eq(*r1, *integer(2)));

    r1 = exp(Inf);
    CHECK(eq(*r1, *Inf));

    r1 = gamma(NegInf);
    CHECK(eq(*r1, *ComplexInf));
}
