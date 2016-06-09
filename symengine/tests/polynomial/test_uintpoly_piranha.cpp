#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/polys/uintpoly_piranha.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::UIntPolyPiranha;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;
using SymEngine::integer_class;

using namespace SymEngine::literals;

TEST_CASE("Constructor of UIntPolyPiranha", "[UIntPolyPiranha]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyPiranha> P
        = UIntPolyPiranha::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UIntPolyPiranha> Q
        = UIntPolyPiranha::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPolyPiranha> R
        = UIntPolyPiranha::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(R->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPolyPiranha> S = UIntPolyPiranha::from_dict(x, {{0, 2_z}});
    REQUIRE(S->__str__() == "2");

    RCP<const UIntPolyPiranha> T = UIntPolyPiranha::from_dict(x, {});
    REQUIRE(T->__str__() == "0");

    RCP<const UIntPolyPiranha> U
        = UIntPolyPiranha::from_dict(x, {{0, 2_z}, {1, 0_z}, {2, 0_z}});
    REQUIRE(U->__str__() == "2");
}

TEST_CASE("Adding two UIntPolyPiranha", "[UIntPolyPiranha]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPolyPiranha> a
        = UIntPolyPiranha::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyPiranha> b
        = UIntPolyPiranha::from_dict(x, {{0, 2_z}, {1, 3_z}, {2, 4_z}});

    RCP<const Basic> c = add_upoly(*a, *b);
    REQUIRE(c->__str__() == "5*x**2 + 5*x + 3");

    RCP<const UIntPolyPiranha> d = UIntPolyPiranha::from_dict(x, {{0, 1_z}});
    RCP<const Basic> e = add_upoly(*a, *d);
    RCP<const Basic> f = add_upoly(*d, *a);
    REQUIRE(e->__str__() == "x**2 + 2*x + 2");
    REQUIRE(f->__str__() == "x**2 + 2*x + 2");

    RCP<const UIntPolyPiranha> g
        = UIntPolyPiranha::from_dict(y, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    CHECK_THROWS_AS(add_upoly(*a, *g), std::runtime_error);
}

TEST_CASE("Negative of a UIntPolyPiranha", "[UIntPolyPiranha]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyPiranha> a
        = UIntPolyPiranha::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyPiranha> b = UIntPolyPiranha::from_dict(x, {});

    RCP<const UIntPolyPiranha> c = neg_upoly(*a);
    RCP<const UIntPolyPiranha> d = neg_upoly(*b);
    REQUIRE(c->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(d->__str__() == "0");
}

TEST_CASE("Subtracting two UIntPolyPiranha", "[UIntPolyPiranha]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPolyPiranha> a
        = UIntPolyPiranha::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyPiranha> b
        = UIntPolyPiranha::from_dict(x, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    RCP<const UIntPolyPiranha> c = UIntPolyPiranha::from_dict(x, {{0, 2_z}});
    RCP<const UIntPolyPiranha> f = UIntPolyPiranha::from_dict(y, {{0, 2_z}});

    RCP<const Basic> d = sub_upoly(*b, *a);
    REQUIRE(d->__str__() == "3*x**2 + x + 1");
    d = sub_upoly(*c, *a);
    REQUIRE(d->__str__() == "-x**2 - 2*x + 1");
    d = sub_upoly(*a, *c);
    REQUIRE(d->__str__() == "x**2 + 2*x - 1");
    CHECK_THROWS_AS(sub_upoly(*a, *f), std::runtime_error);
}

TEST_CASE("Multiplication of two UIntPolyPiranha", "[UIntPolyPiranha]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPolyPiranha> a
        = UIntPolyPiranha::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyPiranha> b
        = UIntPolyPiranha::from_dict(x, {{0, -1_z}, {1, -2_z}, {2, -1_z}});
    RCP<const UIntPolyPiranha> e
        = UIntPolyPiranha::from_dict(x, {{0, 5_z}, {1, -2_z}, {2, -1_z}});
    RCP<const UIntPolyPiranha> f
        = UIntPolyPiranha::from_dict(x, {{0, 6_z}, {1, -2_z}, {2, 3_z}});
    RCP<const UIntPolyPiranha> k
        = UIntPolyPiranha::from_dict(x, {{0, -1_z}, {1, -2_z}, {2, -100_z}});

    RCP<const UIntPolyPiranha> c = mul_upoly(*a, *a);
    RCP<const UIntPolyPiranha> d = mul_upoly(*a, *b);
    RCP<const UIntPolyPiranha> g = mul_upoly(*e, *e);
    RCP<const UIntPolyPiranha> h = mul_upoly(*e, *f);
    RCP<const UIntPolyPiranha> i = mul_upoly(*f, *f);
    RCP<const UIntPolyPiranha> l = mul_upoly(*k, *f);
    RCP<const UIntPolyPiranha> m = mul_upoly(*k, *k);

    REQUIRE(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
    REQUIRE(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
    REQUIRE(g->__str__() == "x**4 + 4*x**3 - 6*x**2 - 20*x + 25");
    REQUIRE(h->__str__() == "-3*x**4 - 4*x**3 + 13*x**2 - 22*x + 30");
    REQUIRE(i->__str__() == "9*x**4 - 12*x**3 + 40*x**2 - 24*x + 36");
    REQUIRE(l->__str__() == "-300*x**4 + 194*x**3 - 599*x**2 - 10*x - 6");
    REQUIRE(m->__str__() == "10000*x**4 + 400*x**3 + 204*x**2 + 4*x + 1");

    c = UIntPolyPiranha::from_dict(x, {{0, -1_z}});
    REQUIRE(mul_upoly(*a, *c)->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(mul_upoly(*c, *a)->__str__() == "-x**2 - 2*x - 1");

    c = UIntPolyPiranha::from_dict(y, {{0, -1_z}});
    CHECK_THROWS_AS(mul_upoly(*a, *c), std::runtime_error);
}