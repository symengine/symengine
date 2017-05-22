#include "catch.hpp"
#include <iostream>
#include <symengine/basic.h>
#include <symengine/relationals.h>

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::Eq;
using SymEngine::Ne;
using SymEngine::Le;
using SymEngine::Lt;
using SymEngine::Equality;
using SymEngine::zero;
using SymEngine::one;
using SymEngine::integer;
using SymEngine::gamma;
using SymEngine::I;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Inf;
using SymEngine::NegInf;
using SymEngine::ComplexInf;
using SymEngine::boolTrue;
using SymEngine::boolFalse;
using SymEngine::Nan;
using SymEngine::SymEngineException;

TEST_CASE("Hash Size for Relationals", "[Relationals]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const Basic> a = Eq(x, y);
    RCP<const Basic> b = Eq(x, x);
    CHECK(a->__hash__() != b->__hash__());

    b = Eq(x, y);
    CHECK(a->__hash__() == b->__hash__());

    a = Eq(one, zero);
    b = Eq(one);
    CHECK(a->__hash__() == b->__hash__());
}

TEST_CASE("String Printing", "[Relationals]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const Basic> a = Eq(x, y);
    CHECK(a->__str__() == "Eq(x, y)");

    a = Ne(x, y);
    CHECK(a->__str__() == "Ne(x, y)");

    a = Le(x, y);
    CHECK(a->__str__() == "Le(x, y)");

    a = Lt(x, y);
    CHECK(a->__str__() == "Lt(x, y)");
}

TEST_CASE("Comparing Relationals", "[Relationals]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const Basic> a = Eq(x, y);
    RCP<const Basic> b = Eq(x, y);
    CHECK(a->compare(*b) == 0);
    CHECK(eq(*a, *b));

    b = Eq(y, x);
    CHECK(a->compare(*b) == -1);
    CHECK(eq(*a, *b));

    a = Ne(x, y);
    b = Ne(y, x);
    CHECK(eq(*a, *b));

    a = Eq(one, zero);
    b = Ne(one, one);
    CHECK(eq(*a, *b));

    a = Le(x, y);
    b = Le(x, y);
    CHECK(eq(*a, *b));

    a = Lt(x, y);
    b = Lt(x, y);
    CHECK(eq(*a, *b));
}

TEST_CASE("Canonicalization", "[Relationals]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Equality> r = make_rcp<Equality>(x, y);
    CHECK(not(r->is_canonical(zero, one)));
    CHECK(not(r->is_canonical(gamma(integer(2)), one)));
    CHECK(not(r->is_canonical(boolTrue, boolTrue)));
}

TEST_CASE("Eq", "[Relationals]")
{
    RCP<const Symbol> x = symbol("x");

    RCP<const Basic> a = Eq(x);
    RCP<const Basic> b = Eq(x, zero);
    CHECK(eq(*a, *b));
}

TEST_CASE("Infinity", "[Relationals]")
{
    RCP<const Basic> a = Eq(Inf, Inf);
    CHECK(eq(*a, *boolTrue));

    a = Ne(Inf, Inf);
    CHECK(eq(*a, *boolFalse));

    a = Lt(Inf, Inf);
    CHECK(eq(*a, *boolFalse));

    a = Lt(Inf, NegInf);
    CHECK(eq(*a, *boolFalse));

    a = Lt(Inf, one);
    CHECK(eq(*a, *boolFalse));

    a = Le(Inf, Inf);
    CHECK(eq(*a, *boolTrue));

    a = Le(Inf, NegInf);
    CHECK(eq(*a, *boolFalse));

    a = Le(Inf, one);
    CHECK(eq(*a, *boolFalse));

    a = Lt(NegInf, Inf);
    CHECK(eq(*a, *boolTrue));

    a = Lt(NegInf, NegInf);
    CHECK(eq(*a, *boolFalse));

    a = Lt(NegInf, one);
    CHECK(eq(*a, *boolTrue));

    a = Le(NegInf, Inf);
    CHECK(eq(*a, *boolTrue));

    a = Le(NegInf, NegInf);
    CHECK(eq(*a, *boolTrue));

    a = Le(NegInf, one);
    CHECK(eq(*a, *boolTrue));

    CHECK_THROWS_AS(Lt(ComplexInf, zero), SymEngineException);
    CHECK_THROWS_AS(Le(ComplexInf, zero), SymEngineException);
}

TEST_CASE("Boolean Values", "[Relationals]")
{
    RCP<const Basic> a = Eq(zero, zero);
    CHECK(eq(*a, *boolTrue));

    a = Eq(boolTrue, boolTrue);
    CHECK(eq(*a, *boolTrue));

    a = Eq(boolFalse, boolTrue);
    CHECK(eq(*a, *boolFalse));

    a = Eq(boolTrue, boolFalse);
    CHECK(eq(*a, *boolFalse));

    a = Eq(boolFalse, boolFalse);
    CHECK(eq(*a, *boolTrue));

    a = Eq(one, zero);
    CHECK(eq(*a, *boolFalse));

    a = Ne(zero, zero);
    CHECK(eq(*a, *boolFalse));

    a = Eq(I, one);
    CHECK(eq(*a, *boolFalse));

    a = Ne(I, one);
    CHECK(eq(*a, *boolTrue));

    a = Ne(one, zero);
    CHECK(eq(*a, *boolTrue));

    a = Lt(zero, one);
    CHECK(eq(*a, *boolTrue));

    a = Lt(one, zero);
    CHECK(eq(*a, *boolFalse));

    a = Le(zero, zero);
    CHECK(eq(*a, *boolTrue));

    a = Le(zero, one);
    CHECK(eq(*a, *boolTrue));

    a = Le(one, zero);
    CHECK(eq(*a, *boolFalse));

    CHECK_THROWS_AS(Lt(I, one), SymEngineException);
    CHECK_THROWS_AS(Le(I, one), SymEngineException);
}

TEST_CASE("Nan Exceptions", "[Relationals]")
{
    RCP<const Basic> a = Eq(Nan, Nan);
    CHECK(eq(*a, *boolFalse));

    a = Ne(Nan, Nan);
    CHECK(eq(*a, *boolTrue));

    CHECK_THROWS_AS(Lt(Nan, one), SymEngineException);
    CHECK_THROWS_AS(Le(Nan, one), SymEngineException);
}

TEST_CASE("Boolean Exceptions", "[Relationals]")
{
    CHECK_THROWS_AS(Lt(boolFalse, boolTrue), SymEngineException);
    CHECK_THROWS_AS(Le(boolTrue, boolTrue), SymEngineException);
}