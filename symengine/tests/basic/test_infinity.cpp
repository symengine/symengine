#include "catch.hpp"
#include <iostream>

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
using SymEngine::symbol;

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
}