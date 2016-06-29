#include "catch.hpp"
#include <chrono>

#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/pow.h>

using SymEngine::UIntPoly;
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
using SymEngine::UIntDict;
using SymEngine::add;
using SymEngine::vec_integer_class;

using namespace SymEngine::literals;

TEST_CASE("Constructor of UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> P
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UIntPoly> Q = UIntPoly::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPoly> R = UIntPoly::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(R->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPoly> S = UIntPoly::from_dict(x, {{0, 2_z}});
    REQUIRE(S->__str__() == "2");

    RCP<const UIntPoly> T = UIntPoly::from_dict(x, map_uint_mpz{});
    REQUIRE(T->__str__() == "0");

    RCP<const UIntPoly> U
        = UIntPoly::from_dict(x, {{0, 2_z}, {1, 0_z}, {2, 0_z}});
    REQUIRE(U->__str__() == "2");

    REQUIRE(is_a_UPoly(*U));
    REQUIRE(not is_a_UPoly(*x));
}

TEST_CASE("Adding two UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    UIntDict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntDict bdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    UIntDict gdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    const UIntPoly a(x, std::move(adict_));
    const UIntPoly b(x, std::move(bdict_));

    RCP<const Basic> c = add_upoly(a, b);
    REQUIRE(c->__str__() == "5*x**2 + 5*x + 3");

    RCP<const UIntPoly> d = UIntPoly::from_dict(x, {{0, 1_z}});
    RCP<const Basic> e = add_upoly(a, *d);
    RCP<const Basic> f = add_upoly(*d, a);
    REQUIRE(e->__str__() == "x**2 + 2*x + 2");
    REQUIRE(f->__str__() == "x**2 + 2*x + 2");

    const UIntPoly g(y, std::move(gdict_));
    CHECK_THROWS_AS(add_upoly(a, g), std::runtime_error);
}

TEST_CASE("Negative of a UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    UIntDict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    const UIntPoly a(x, std::move(adict_));

    RCP<const UIntPoly> b = neg_upoly(a);
    REQUIRE(b->__str__() == "-x**2 - 2*x - 1");
}

TEST_CASE("Subtracting two UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    UIntDict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntDict bdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    UIntDict cdict_({{0, 2_z}});
    UIntDict fdict_({{0, 2_z}});

    const UIntPoly a(x, std::move(adict_));
    const UIntPoly b(x, std::move(bdict_));
    const UIntPoly c(x, std::move(cdict_));
    const UIntPoly f(y, std::move(fdict_));

    RCP<const Basic> d = sub_upoly(b, a);
    REQUIRE(d->__str__() == "3*x**2 + x + 1");
    d = sub_upoly(c, a);
    REQUIRE(d->__str__() == "-x**2 - 2*x + 1");
    d = sub_upoly(a, c);
    REQUIRE(d->__str__() == "x**2 + 2*x - 1");
    CHECK_THROWS_AS(sub_upoly(a, f), std::runtime_error);
}

TEST_CASE("Multiplication of two UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    UIntDict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntDict bdict_({{0, -1_z}, {1, -2_z}, {2, -1_z}});
    UIntDict edict_({{0, 5_z}, {1, -2_z}, {2, -1_z}});
    UIntDict fdict_({{0, 6_z}, {1, -2_z}, {2, 3_z}});
    UIntDict kdict_({{0, -1_z}, {1, -2_z}, {2, -100_z}});

    const UIntPoly a(x, std::move(adict_));
    const UIntPoly b(x, std::move(bdict_));
    const UIntPoly e(x, std::move(edict_));
    const UIntPoly f(x, std::move(fdict_));
    const UIntPoly k(x, std::move(kdict_));

    RCP<const UIntPoly> c = mul_upoly(a, a);
    RCP<const UIntPoly> d = mul_upoly(a, b);
    RCP<const UIntPoly> g = mul_upoly(e, e);
    RCP<const UIntPoly> h = mul_upoly(e, f);
    RCP<const UIntPoly> i = mul_upoly(f, f);
    RCP<const UIntPoly> l = mul_upoly(k, f);
    RCP<const UIntPoly> m = mul_upoly(k, k);

    REQUIRE(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
    REQUIRE(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
    REQUIRE(g->__str__() == "x**4 + 4*x**3 - 6*x**2 - 20*x + 25");
    REQUIRE(h->__str__() == "-3*x**4 - 4*x**3 + 13*x**2 - 22*x + 30");
    REQUIRE(i->__str__() == "9*x**4 - 12*x**3 + 40*x**2 - 24*x + 36");
    REQUIRE(l->__str__() == "-300*x**4 + 194*x**3 - 599*x**2 - 10*x - 6");
    REQUIRE(m->__str__() == "10000*x**4 + 400*x**3 + 204*x**2 + 4*x + 1");

    c = UIntPoly::from_dict(x, {{0, -1_z}});
    REQUIRE(mul_upoly(a, *c)->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(mul_upoly(*c, a)->__str__() == "-x**2 - 2*x - 1");

    c = UIntPoly::from_dict(y, {{0, -1_z}});
    CHECK_THROWS_AS(mul_upoly(a, *c), std::runtime_error);
}

TEST_CASE("Comparing two UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UIntPoly> P = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}});
    RCP<const UIntPoly> Q
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(P->compare(*Q) == -1);

    P = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 3_z}, {3, 2_z}});
    REQUIRE(P->compare(*Q) == 1);

    P = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {3, 3_z}});
    REQUIRE(P->compare(*Q) == 1);

    P = UIntPoly::from_dict(y, {{0, 1_z}, {1, 2_z}, {3, 3_z}});
    REQUIRE(P->compare(*Q) == 1);
}

TEST_CASE("UIntPoly as_symbolic", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(eq(*a->as_symbolic(),
               *add({one, mul(integer(2), x), pow(x, integer(2))})));
    REQUIRE(not eq(*a->as_symbolic(),
                   *add({one, mul(integer(3), x), pow(x, integer(2))})));

    RCP<const UIntPoly> b
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 1_z}, {2, 2_z}});
    REQUIRE(eq(*b->as_symbolic(),
               *add({one, x, mul(integer(2), pow(x, integer(2)))})));

    RCP<const UIntPoly> c = UIntPoly::from_dict(x, map_uint_mpz{});
    REQUIRE(eq(*c->as_symbolic(), *zero));
}

TEST_CASE("Evaluation of UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPoly> b
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 0_z}, {2, -1_z}});

    REQUIRE(a->eval(2_z) == 9);
    REQUIRE(a->eval(10_z) == 121);
    REQUIRE(b->eval(-1_z) == 0);
    REQUIRE(b->eval(0_z) == 1);

    vec_integer_class resa = {9_z, 121_z, 0_z, 1_z};
    vec_integer_class resb = {-3_z, -99_z, 0_z, 1_z};
    REQUIRE(a->multieval({2_z, 10_z, -1_z, 0_z}) == resa);
    REQUIRE(b->multieval({2_z, 10_z, -1_z, 0_z}) == resb);
}

TEST_CASE("Derivative of UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPoly> b = UIntPoly::from_dict(y, {{2, 4_z}});

    REQUIRE(a->diff(x)->__str__() == "2*x + 2");
    REQUIRE(a->diff(y)->__str__() == "0");
    REQUIRE(b->diff(y)->__str__() == "8*y");
}

TEST_CASE("Bool checks specific UIntPoly cases", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> z = UIntPoly::from_dict(x, {{0, 0_z}});
    RCP<const UIntPoly> o = UIntPoly::from_dict(x, {{0, 1_z}});
    RCP<const UIntPoly> mo = UIntPoly::from_dict(x, {{0, -1_z}});
    RCP<const UIntPoly> i = UIntPoly::from_dict(x, {{0, 6_z}});
    RCP<const UIntPoly> s = UIntPoly::from_dict(x, {{1, 1_z}});
    RCP<const UIntPoly> m1 = UIntPoly::from_dict(x, {{1, 6_z}});
    RCP<const UIntPoly> m2 = UIntPoly::from_dict(x, {{3, 5_z}});
    RCP<const UIntPoly> po = UIntPoly::from_dict(x, {{5, 1_z}});
    RCP<const UIntPoly> poly
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE((z->is_zero() and not z->is_one() and not z->is_minus_one()
             and z->is_integer() and not z->is_symbol() and not z->is_mul()
             and not z->is_pow()));
    REQUIRE((not o->is_zero() and o->is_one() and not o->is_minus_one()
             and o->is_integer() and not o->is_symbol() and not o->is_mul()
             and not o->is_pow()));
    REQUIRE((not mo->is_zero() and not mo->is_one() and mo->is_minus_one()
             and mo->is_integer() and not mo->is_symbol() and not mo->is_mul()
             and not mo->is_pow()));
    REQUIRE((not i->is_zero() and not i->is_one() and not i->is_minus_one()
             and i->is_integer() and not i->is_symbol() and not i->is_mul()
             and not i->is_pow()));
    REQUIRE((not s->is_zero() and not s->is_one() and not s->is_minus_one()
             and not s->is_integer() and s->is_symbol() and not s->is_mul()
             and not s->is_pow()));
    REQUIRE((not m1->is_zero() and not m1->is_one() and not m1->is_minus_one()
             and not m1->is_integer() and not m1->is_symbol() and m1->is_mul()
             and not m1->is_pow()));
    REQUIRE((not m2->is_zero() and not m2->is_one() and not m2->is_minus_one()
             and not m2->is_integer() and not m2->is_symbol() and m2->is_mul()
             and not m2->is_pow()));
    REQUIRE((not po->is_zero() and not po->is_one() and not po->is_minus_one()
             and not po->is_integer() and not po->is_symbol()
             and not po->is_mul() and po->is_pow()));
    REQUIRE((not poly->is_zero() and not poly->is_one()
             and not poly->is_minus_one() and not poly->is_integer()
             and not poly->is_symbol() and not poly->is_mul()
             and not poly->is_pow()));
}

TEST_CASE("UIntPoly expand", "[UIntPoly][expand]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{1, 1_z}, {2, 1_z}, {3, 1_z}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(x**3 + x**2 + x)**3");
    REQUIRE(c->__str__()
            == "x**9 + 3*x**8 + 6*x**7 + 7*x**6 + 6*x**5 + 3*x**4 + x**3");
}

TEST_CASE("UIntPoly pow", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a = UIntPoly::from_dict(x, {{0, 1_z}, {1, 1_z}});
    RCP<const UIntPoly> b = UIntPoly::from_dict(x, {{0, 3_z}, {2, 1_z}});

    RCP<const UIntPoly> aaa = pow_upoly(*a, 3);
    RCP<const UIntPoly> bb = pow_upoly(*b, 2);

    REQUIRE(aaa->__str__() == "x**3 + 3*x**2 + 3*x + 1");
    REQUIRE(bb->__str__() == "x**4 + 6*x**2 + 9");
}

TEST_CASE("UIntPoly divides", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a = UIntPoly::from_dict(x, {{0, 1_z}, {1, 1_z}});
    RCP<const UIntPoly> b = UIntPoly::from_dict(x, {{0, 4_z}});
    RCP<const UIntPoly> c = UIntPoly::from_dict(x, {{0, 8_z}, {1, 8_z}});
    RCP<const UIntPoly> res;

    REQUIRE(divides_upoly(*a, *c, outArg(res)));
    REQUIRE(res->__str__() == "8");
    REQUIRE(divides_upoly(*b, *c, outArg(res)));
    REQUIRE(res->__str__() == "2*x + 2");
    REQUIRE(!divides_upoly(*b, *a, outArg(res)));
}