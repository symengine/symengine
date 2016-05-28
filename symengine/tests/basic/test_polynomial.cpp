#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::Expression;
using SymEngine::UIntPolyO;
using SymEngine::UExprPolyO;
using SymEngine::uint_poly;
using SymEngine::uexpr_poly;
using SymEngine::UExprODict;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::map_int_Expr;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;
using SymEngine::integer_class;
using SymEngine::UIntODict;

using namespace SymEngine::literals;

TEST_CASE("Constructor of UIntPolyO", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyO> P = uint_poly(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UIntPolyO> Q = UIntPolyO::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    UIntPolyO R(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(R.__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPolyO> S = uint_poly(x, {{0, 2_z}});
    REQUIRE(S->__str__() == "2");

    RCP<const UIntPolyO> T = uint_poly(x, map_uint_mpz{});
    REQUIRE(T->__str__() == "0");
}

TEST_CASE("Adding two UIntPolyO", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    UIntODict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntODict bdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    UIntODict gdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    const UIntPolyO a(x, std::move(adict_));
    const UIntPolyO b(x, std::move(bdict_));

    RCP<const Basic> c = add_upoly(a, b);
    REQUIRE(c->__str__() == "5*x**2 + 5*x + 3");

    RCP<const UIntPolyO> d = uint_poly(x, {{0, 1_z}});
    RCP<const Basic> e = add_upoly(a, *d);
    RCP<const Basic> f = add_upoly(*d, a);
    REQUIRE(e->__str__() == "x**2 + 2*x + 2");
    REQUIRE(f->__str__() == "x**2 + 2*x + 2");

    const UIntPolyO g(y, std::move(gdict_));
    CHECK_THROWS_AS(add_upoly(a, g), std::runtime_error);
}

TEST_CASE("Negative of a UIntPolyO", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    UIntODict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    const UIntPolyO a(x, std::move(adict_));

    RCP<const UIntPolyO> b = neg_upoly(a);
    REQUIRE(b->__str__() == "-x**2 - 2*x - 1");
}

TEST_CASE("Subtracting two UIntPolyO", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    UIntODict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntODict bdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    UIntODict cdict_({{0, 2_z}});
    UIntODict fdict_({{0, 2_z}});

    const UIntPolyO a(x, std::move(adict_));
    const UIntPolyO b(x, std::move(bdict_));
    const UIntPolyO c(x, std::move(cdict_));
    const UIntPolyO f(y, std::move(fdict_));

    RCP<const Basic> d = sub_upoly(b, a);
    REQUIRE(d->__str__() == "3*x**2 + x + 1");
    d = sub_upoly(c, a);
    REQUIRE(d->__str__() == "-x**2 - 2*x + 1");
    d = sub_upoly(a, c);
    REQUIRE(d->__str__() == "x**2 + 2*x - 1");
    CHECK_THROWS_AS(sub_upoly(a, f), std::runtime_error);
}

TEST_CASE("Multiplication of two UIntPolyO", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    UIntODict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntODict bdict_({{0, -1_z}, {1, -2_z}, {2, -1_z}});
    UIntODict edict_({{0, 5_z}, {1, -2_z}, {2, -1_z}});
    UIntODict fdict_({{0, 6_z}, {1, -2_z}, {2, 3_z}});
    UIntODict kdict_({{0, -1_z}, {1, -2_z}, {2, -100_z}});

    const UIntPolyO a(x, std::move(adict_));
    const UIntPolyO b(x, std::move(bdict_));
    const UIntPolyO e(x, std::move(edict_));
    const UIntPolyO f(x, std::move(fdict_));
    const UIntPolyO k(x, std::move(kdict_));

    RCP<const UIntPolyO> c = mul_upoly(a, a);
    RCP<const UIntPolyO> d = mul_upoly(a, b);
    RCP<const UIntPolyO> g = mul_upoly(e, e);
    RCP<const UIntPolyO> h = mul_upoly(e, f);
    RCP<const UIntPolyO> i = mul_upoly(f, f);
    RCP<const UIntPolyO> l = mul_upoly(k, f);
    RCP<const UIntPolyO> m = mul_upoly(k, k);

    REQUIRE(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
    REQUIRE(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
    REQUIRE(g->__str__() == "x**4 + 4*x**3 - 6*x**2 - 20*x + 25");
    REQUIRE(h->__str__() == "-3*x**4 - 4*x**3 + 13*x**2 - 22*x + 30");
    REQUIRE(i->__str__() == "9*x**4 - 12*x**3 + 40*x**2 - 24*x + 36");
    REQUIRE(l->__str__() == "-300*x**4 + 194*x**3 - 599*x**2 - 10*x - 6");
    REQUIRE(m->__str__() == "10000*x**4 + 400*x**3 + 204*x**2 + 4*x + 1");

    c = uint_poly(x, {{0, -1_z}});
    REQUIRE(mul_upoly(a, *c)->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(mul_upoly(*c, a)->__str__() == "-x**2 - 2*x - 1");

    c = uint_poly(y, {{0, -1_z}});
    CHECK_THROWS_AS(mul_upoly(a, *c), std::runtime_error);
}

TEST_CASE("Comparing two UIntPolyO", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UIntPolyO> P = uint_poly(x, {{0, 1_z}, {1, 2_z}});
    RCP<const UIntPolyO> Q = uint_poly(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(P->compare(*Q) == -1);

    P = uint_poly(x, {{0, 1_z}, {1, 2_z}, {2, 3_z}, {3, 2_z}});
    REQUIRE(P->compare(*Q) == 1);

    P = uint_poly(x, {{0, 1_z}, {1, 2_z}, {3, 3_z}});
    REQUIRE(P->compare(*Q) == 1);

    P = uint_poly(y, {{0, 1_z}, {1, 2_z}, {3, 3_z}});
    REQUIRE(P->compare(*Q) == 1);
}

TEST_CASE("UIntPolyO get_args", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyO> a = uint_poly(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(vec_basic_eq_perm(a->get_args(),
                              {one, mul(integer(2), x), pow(x, integer(2))}));
    REQUIRE(not vec_basic_eq_perm(
        a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));

    RCP<const UIntPolyO> b = uint_poly(x, {{0, 1_z}, {1, 1_z}, {2, 2_z}});
    REQUIRE(vec_basic_eq_perm(b->get_args(),
                              {one, x, mul(integer(2), pow(x, integer(2)))}));

    RCP<const UIntPolyO> c = uint_poly(x, map_uint_mpz{});
    REQUIRE(vec_basic_eq_perm(c->get_args(), {zero}));
}

TEST_CASE("Evaluation of UIntPolyO", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyO> a = uint_poly(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(a->eval(2_z) == 9);
}

TEST_CASE("Derivative of UIntPolyO", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");
    RCP<const UIntPolyO> a = uint_poly(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyO> b = uint_poly(none, {{0, 1_z}});

    REQUIRE(a->diff(x)->__str__() == "2*x + 2");
    REQUIRE(a->diff(y)->__str__() == "0");
    REQUIRE(b->diff(y)->__str__() == "0");

    a = uint_poly(none, {{0, 1_z}});
    REQUIRE(a->diff(y)->__str__() == "0");
    a = uint_poly(none, map_uint_mpz{});
    REQUIRE(a->diff(y)->__str__() == "0");
}

TEST_CASE("Bool checks specific UIntPolyO cases", "[UIntPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyO> z = uint_poly(x, {{0, 0_z}});
    RCP<const UIntPolyO> o = uint_poly(x, {{0, 1_z}});
    RCP<const UIntPolyO> mo = uint_poly(x, {{0, -1_z}});
    RCP<const UIntPolyO> i = uint_poly(x, {{0, 6_z}});
    RCP<const UIntPolyO> s = uint_poly(x, {{1, 1_z}});
    RCP<const UIntPolyO> m1 = uint_poly(x, {{1, 6_z}});
    RCP<const UIntPolyO> m2 = uint_poly(x, {{3, 5_z}});
    RCP<const UIntPolyO> po = uint_poly(x, {{5, 1_z}});
    RCP<const UIntPolyO> poly = uint_poly(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

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

TEST_CASE("UIntPolyO expand", "[UIntPolyO][expand]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyO> a = uint_poly(x, {{1, 1_z}, {2, 1_z}, {3, 1_z}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(x**3 + x**2 + x)**3");
    REQUIRE(c->__str__()
            == "x**9 + 3*x**8 + 6*x**7 + 7*x**6 + 6*x**5 + 3*x**4 + x**3");
}

TEST_CASE("Constructor of UExprPolyO", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> none = symbol("");
    Expression a(symbol("a"));
    Expression b(symbol("b"));
    Expression c(symbol("c"));
    Expression d(symbol("d"));
    Expression num2(integer(2));
    Expression num1(integer(1));

    RCP<const UExprPolyO> P = uexpr_poly(x, {{0, num1}, {1, num2}, {2, num1}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UExprPolyO> Q = UExprPolyO::from_vec(x, {1, 0, 2, 1});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UExprPolyO> R = uexpr_poly(x, {{0, d}, {1, c}, {2, b}, {3, a}});
    REQUIRE(R->__str__() == "a*x**3 + b*x**2 + c*x + d");

    UExprPolyO S(x, {1, 0, 2, 1});
    REQUIRE(S.__str__() == "x**3 + 2*x**2 + 1");

    R = uexpr_poly(x, {{-1, d}});
    REQUIRE(R->__str__() == "d*x**(-1)");
    REQUIRE(not(R->__str__() == "d*x**-1"));

    R = uexpr_poly(x, {{-2, d}, {-1, c}, {0, b}, {1, a}});
    REQUIRE(R->__str__() == "a*x + b + c*x**(-1) + d*x**(-2)");

    RCP<const UExprPolyO> T = uexpr_poly(none, map_int_Expr{});
    REQUIRE(T->__str__() == "0");
}

TEST_CASE("Adding two UExprPolyO", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    UExprODict adict_({{0, 1}, {1, 2}, {2, symbol("a")}});
    UExprODict bdict_({{0, 2}, {1, 3}, {2, symbol("b")}});
    const UExprPolyO a(x, std::move(adict_));
    const UExprPolyO b(x, std::move(bdict_));

    RCP<const Basic> c = add_upoly(a, b);
    REQUIRE(c->__str__() == "(a + b)*x**2 + 5*x + 3");

    RCP<const UExprPolyO> d = uexpr_poly(x, {{0, Expression(2)}});
    REQUIRE(add_upoly(a, *d)->__str__() == "a*x**2 + 2*x + 3");
    REQUIRE(add_upoly(*d, a)->__str__() == "a*x**2 + 2*x + 3");

    d = uexpr_poly(y, {{0, 2}, {1, 4}});
    CHECK_THROWS_AS(add_upoly(a, *d), std::runtime_error);
}

TEST_CASE("Negative of a UExprPolyO", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    UExprODict adict_({{0, 1}, {1, symbol("a")}, {2, symbol("c")}});
    const UExprPolyO a(x, std::move(adict_));

    RCP<const UExprPolyO> b = neg_upoly(a);
    REQUIRE(b->__str__() == "-c*x**2 - a*x - 1");

    RCP<const UExprPolyO> c = uexpr_poly(x, map_int_Expr{});
    REQUIRE(neg_upoly(*c)->__str__() == "0");

    c = uexpr_poly(x, {{0, Expression(2)}});
    REQUIRE(neg_upoly(*c)->__str__() == "-2");
}

TEST_CASE("Subtracting two UExprPolyO", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    UExprODict adict_({{0, 1}, {1, 2}, {2, 1}});
    UExprODict bdict_({{0, 2}, {1, symbol("b")}, {2, symbol("a")}});
    const UExprPolyO a(x, std::move(adict_));
    const UExprPolyO b(x, std::move(bdict_));

    RCP<const Basic> c = sub_upoly(b, a);
    REQUIRE(c->__str__() == "(-1 + a)*x**2 + (-2 + b)*x + 1");

    RCP<const UExprPolyO> d = uexpr_poly(x, {{0, Expression(2)}});
    REQUIRE(sub_upoly(a, *d)->__str__() == "x**2 + 2*x - 1");
    REQUIRE(sub_upoly(*d, a)->__str__() == "-x**2 - 2*x + 1");

    d = uexpr_poly(y, {{0, 2}, {1, 4}});
    CHECK_THROWS_AS(sub_upoly(a, *d), std::runtime_error);
}

TEST_CASE("Multiplication of two UExprPolyO", "[UExprPolyO]")
{

    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");
    RCP<const UExprPolyO> a
        = uexpr_poly(x, {{0, 1}, {1, symbol("b")}, {2, symbol("a")}});
    RCP<const UExprPolyO> b
        = uexpr_poly(x, {{0, -1}, {1, -2}, {2, mul(integer(-1), symbol("a"))}});

    RCP<const UExprPolyO> c = mul_upoly(*a, *a);
    RCP<const UExprPolyO> d = mul_upoly(*a, *b);

    REQUIRE(c->__str__()
            == "a**2*x**4 + 2*a*b*x**3 + (2*a + b**2)*x**2 + 2*b*x + 1");
    REQUIRE(d->__str__() == "-a**2*x**4 + (-2*a - a*b)*x**3 + (-2*a - "
                            "2*b)*x**2 + (-2 - b)*x - 1");

    RCP<const UExprPolyO> f = uexpr_poly(x, {{0, Expression(2)}});
    REQUIRE(mul_upoly(*a, *f)->__str__() == "2*a*x**2 + 2*b*x + 2");
    REQUIRE(mul_upoly(*f, *a)->__str__() == "2*a*x**2 + 2*b*x + 2");

    f = uexpr_poly(y, {{0, 2}, {1, 4}});
    CHECK_THROWS_AS(mul_upoly(*a, *f), std::runtime_error);

    f = uexpr_poly(x, map_int_Expr{});
    REQUIRE(mul_upoly(*a, *f)->__str__() == "0");

    a = uexpr_poly(x, {{-2, 5}, {-1, 3}, {0, 1}, {1, 2}});

    c = mul_upoly(*a, *b);
    REQUIRE(c->__str__() == "-2*a*x**3 + (-4 - a)*x**2 + (-4 - 3*a)*x + (-7 - "
                            "5*a) - 13*x**(-1) - 5*x**(-2)");
}

TEST_CASE("Comparing two UExprPolyO", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UExprPolyO> P = uexpr_poly(x, {{0, 1}, {1, 2}});
    RCP<const UExprPolyO> Q = uexpr_poly(x, {{0, 1}, {1, symbol("b")}, {2, 1}});

    REQUIRE(P->compare(*Q) == -1);

    P = uexpr_poly(x, {{0, 1}, {1, symbol("k")}, {2, 3}, {3, 2}});
    REQUIRE(P->compare(*Q) == 1);

    P = uexpr_poly(x, {{0, 1}, {1, 2}, {3, 3}});
    REQUIRE(P->compare(*Q) == -1);

    P = uexpr_poly(y, {{0, 1}, {1, 2}, {3, 3}});
    REQUIRE(P->compare(*Q) == 1);

    P = uexpr_poly(x, {{0, 1}, {1, symbol("b")}, {2, 1}});
    REQUIRE(P->compare(*Q) == 0);
    REQUIRE(P->__eq__(*Q) == true);

    P = uexpr_poly(x, {{0, 1}, {1, symbol("a")}, {2, 1}});
    REQUIRE(P->compare(*Q) == -1);
    REQUIRE(P->__eq__(*Q) == false);
}

TEST_CASE("UExprPolyO get_args", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UExprPolyO> a = uexpr_poly(x, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(vec_basic_eq_perm(a->get_args(),
                              {one, mul(integer(2), x), pow(x, integer(2))}));
    REQUIRE(not vec_basic_eq_perm(
        a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));

    a = uexpr_poly(x, {{0, 1}, {1, 1}, {2, 2}});
    REQUIRE(vec_basic_eq_perm(a->get_args(),
                              {one, x, mul(integer(2), pow(x, integer(2)))}));

    a = uexpr_poly(x, map_int_Expr{});
    REQUIRE(vec_basic_eq_perm(a->get_args(), {zero}));
}

TEST_CASE("UExprPolyO max_coef", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UExprPolyO> a = uexpr_poly(x, {{0, 1}, {1, 2}, {2, 4}});
    RCP<const UExprPolyO> b = uexpr_poly(x, {{0, 2}, {1, 2}, {2, symbol("b")}});

    Expression c(symbol("a"));
    Expression d(symbol("b"));

    REQUIRE(a->max_coef() == 4);
    REQUIRE(not(a->max_coef() == 2));
    REQUIRE(b->max_coef() == d);
    REQUIRE(not(b->max_coef() == c));
    REQUIRE(not(b->max_coef() == 2));
}

TEST_CASE("Evaluation of UExprPolyO", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UExprPolyO> a = uexpr_poly(x, {{0, 1}, {1, 2}, {2, symbol("a")}});

    REQUIRE(a->eval(2).get_basic()->__str__() == "5 + 4*a");

    a = uexpr_poly(x, {{-2, 5}, {-1, 3}, {0, 1}, {1, 2}});
    REQUIRE(a->eval(2).get_basic()->__str__() == "31/4");
}

TEST_CASE("Derivative of UExprPolyO", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");
    RCP<const UExprPolyO> a = uexpr_poly(x, {{0, 1}, {1, 2}, {2, symbol("a")}});
    RCP<const UExprPolyO> b = uexpr_poly(x, {{0, Expression(1)}});
    RCP<const UExprPolyO> c = uexpr_poly(none, {{0, Expression(5)}});

    REQUIRE(a->diff(x)->__str__() == "2*a*x + 2");
    REQUIRE(a->diff(y)->__str__() == "0");
    REQUIRE(b->diff(y)->__str__() == "0");

    a = uexpr_poly(x, {{-2, 5}, {-1, 3}, {0, 1}, {1, 2}, {2, symbol("a")}});
    REQUIRE(a->diff(x)->__str__() == "2*a*x + 2 - 3*x**(-2) - 10*x**(-3)");

    REQUIRE(c->diff(x)->__str__() == "0");

    c = uexpr_poly(none, map_int_Expr{});
    REQUIRE(c->diff(x)->__str__() == "0");
}

TEST_CASE("Bool checks specific UExprPolyO cases", "[UExprPolyO]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UExprPolyO> z = uexpr_poly(x, {{0, Expression(0)}});
    RCP<const UExprPolyO> o = uexpr_poly(x, {{0, Expression(1)}});
    RCP<const UExprPolyO> mo = uexpr_poly(x, {{0, Expression(-1)}});
    RCP<const UExprPolyO> i = uexpr_poly(x, {{0, Expression(6)}});
    RCP<const UExprPolyO> s = uexpr_poly(x, {{1, Expression(1)}});
    RCP<const UExprPolyO> m1 = uexpr_poly(x, {{1, Expression(6)}});
    RCP<const UExprPolyO> m2 = uexpr_poly(x, {{3, Expression(5)}});
    RCP<const UExprPolyO> po = uexpr_poly(x, {{5, Expression(1)}});
    RCP<const UExprPolyO> poly = uexpr_poly(x, {{0, 1}, {1, 2}, {2, 1}});
    RCP<const UExprPolyO> neg
        = uexpr_poly(x, {{-2, 5}, {-1, 3}, {0, 1}, {1, 2}});

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
    REQUIRE((not neg->is_zero() and not neg->is_one()
             and not neg->is_minus_one() and not neg->is_integer()
             and not neg->is_symbol() and not neg->is_mul()
             and not neg->is_pow()));
}

TEST_CASE("UExprPolyO expand", "[UExprPolyO][expand]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UExprPolyO> a = uexpr_poly(x, {{1, 1}, {2, 1}, {3, symbol("a")}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(a*x**3 + x**2 + x)**3");
    REQUIRE(c->__str__() == "a**3*x**9 + 3*a**2*x**8 + (2*a + a*(1 + 2*a) + "
                            "a**2)*x**7 + (1 + 6*a)*x**6 + (3 + 3*a)*x**5 + "
                            "3*x**4 + x**3");
}
