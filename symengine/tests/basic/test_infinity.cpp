#include "catch.hpp"
#include <iostream>

#include <SymEngine/complex.h>
#include <symengine/basic.h>
#include <symengine/infinity.h>
#include <symengine/symengine_rcp.h>

using SymEngine::Basic;
using SymEngine::Number;
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

TEST_CASE("Boolean tests for Infinity", "[Infinity")
{
    RCP<const Infinit> a = Inf;
    RCP<const Infinit> b = NegInf;
    RCP<const Infinit> c = ComplexInf;

    REQUIRE((not a->is_zero() && not a->is_one() && not a->is_minus_one()
             && not a->is_negative_infinity() && a->is_positive_infinity()
             && not a->is_unsigned_infinity() && a->is_positive()
             && not a->is_negative()));
    REQUIRE((not b->is_zero() && not b->is_one() && not b->is_minus_one()
             && b->is_negative_infinity() && not b->is_positive_infinity()
             && not b->is_unsigned_infinity() && not b->is_positive()
             && b->is_negative()));
    REQUIRE((not c->is_zero() && not c->is_one() && not c->is_minus_one()
             && not c->is_negative_infinity() && not c->is_positive_infinity()
             && c->is_unsigned_infinity() && not c->is_positive()
             && not c->is_negative()));
}
