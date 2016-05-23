#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/pow.h>
#include <symengine/printer.h>

using SymEngine::Expression;
using SymEngine::UnivariateIntPolynomial;
using SymEngine::UnivariatePolynomial;
using SymEngine::univariate_int_polynomial;
using SymEngine::univariate_polynomial;
using SymEngine::UnivariateExprPolynomial;
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
using SymEngine::UIntDict;

using namespace SymEngine::literals;

TEST_CASE("Constructor of UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> none = symbol("");
    RCP<const UnivariateIntPolynomial> P
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(vec_basic_eq_perm(
        P->get_args(), {pow(x, integer(2)), mul(integer(2), x), integer(1)}));

    RCP<const UnivariateIntPolynomial> Q
        = UnivariateIntPolynomial::from_vec(x, {1_z, 0_z, 2_z, 1_z});

    REQUIRE(vec_basic_eq_perm(
        Q->get_args(),
        {pow(x, integer(3)), mul(integer(2), pow(x, integer(2))), integer(1)}));

    UnivariateIntPolynomial R(x, {1_z, 0_z, 2_z, 1_z});

    REQUIRE(vec_basic_eq_perm(
        R.get_args(),
        {pow(x, integer(3)), mul(integer(2), pow(x, integer(2))), integer(1)}));

    RCP<const UnivariateIntPolynomial> S
        = univariate_int_polynomial(none, {{0, 2_z}});

    REQUIRE(vec_basic_eq_perm(S->get_args(), {integer(2)}));

    RCP<const UnivariateIntPolynomial> T = univariate_int_polynomial(
        none, std::map<unsigned int, integer_class>{});
    REQUIRE(vec_basic_eq_perm(T->get_args(), {integer(0)}));
}

TEST_CASE("Adding two UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> none = symbol("");
    RCP<const Symbol> y = symbol("y");
    UIntDict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntDict bdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    UIntDict gdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    const UnivariateIntPolynomial a(x, 2, std::move(adict_));
    const UnivariateIntPolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = add_poly(a, b);

    UnivariateIntPolynomial q1(x, 2, UIntDict({{2, 5_z}, {1, 5_z}, {0, 3_z}}));
    REQUIRE(eq(*c, q1));

    RCP<const UnivariateIntPolynomial> d
        = univariate_int_polynomial(none, {{0, 1_z}});
    RCP<const Basic> e = add_poly(a, *d);
    RCP<const Basic> f = add_poly(*d, a);

    UnivariateIntPolynomial q2(x, 2, UIntDict({{2, 1_z}, {1, 2_z}, {0, 2_z}}));
    REQUIRE(eq(*e, q2));
    UnivariateIntPolynomial q3(x, 2, UIntDict({{2, 1_z}, {1, 2_z}, {0, 2_z}}));
    REQUIRE(eq(*f, q3));

    const UnivariateIntPolynomial g(y, 2, std::move(gdict_));
    CHECK_THROWS_AS(add_poly(a, g), std::runtime_error);
}

TEST_CASE("Negative of a UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    UIntDict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    const UnivariateIntPolynomial a(x, 2, std::move(adict_));

    RCP<const UnivariateIntPolynomial> b = neg_poly(a);
    UnivariateIntPolynomial q(x, 2,
                              UIntDict({{2, -1_z}, {1, -2_z}, {0, -1_z}}));
    REQUIRE(eq(*b, q));
}

TEST_CASE("Subtracting two UnivariateIntPolynomial",
          "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> none = symbol("");
    RCP<const Symbol> y = symbol("y");
    UIntDict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntDict bdict_({{0, 2_z}, {1, 3_z}, {2, 4_z}});
    UIntDict cdict_({{0, 2_z}});
    UIntDict fdict_({{0, 2_z}});

    const UnivariateIntPolynomial a(x, 2, std::move(adict_));
    const UnivariateIntPolynomial b(x, 2, std::move(bdict_));
    const UnivariateIntPolynomial c(none, 0, std::move(cdict_));
    const UnivariateIntPolynomial f(y, 0, std::move(fdict_));

    RCP<const Basic> d = sub_poly(b, a);
    UnivariateIntPolynomial q1(x, 2, UIntDict({{2, 3_z}, {1, 1_z}, {0, 1_z}}));
    REQUIRE(eq(*d, q1));
    d = sub_poly(c, a);
    UnivariateIntPolynomial q2(x, 2,
                               UIntDict({{2, -1_z}, {1, -2_z}, {0, 1_z}}));
    REQUIRE(eq(*d, q2));
    d = sub_poly(a, c);
    UnivariateIntPolynomial q3(x, 2, UIntDict({{2, 1_z}, {1, 2_z}, {0, -1_z}}));
    REQUIRE(eq(*d, q3));
    CHECK_THROWS_AS(sub_poly(a, f), std::runtime_error);
}

TEST_CASE("Multiplication of two UnivariateIntPolynomial",
          "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");

    UIntDict adict_({{0, 1_z}, {1, 2_z}, {2, 1_z}});
    UIntDict bdict_({{0, -1_z}, {1, -2_z}, {2, -1_z}});
    UIntDict edict_({{0, 5_z}, {1, -2_z}, {2, -1_z}});
    UIntDict fdict_({{0, 6_z}, {1, -2_z}, {2, 3_z}});
    UIntDict kdict_({{0, -1_z}, {1, -2_z}, {2, -100_z}});

    const UnivariateIntPolynomial a(x, 2, std::move(adict_));
    const UnivariateIntPolynomial b(x, 2, std::move(bdict_));
    const UnivariateIntPolynomial e(x, 2, std::move(edict_));
    const UnivariateIntPolynomial f(x, 2, std::move(fdict_));
    const UnivariateIntPolynomial k(x, 2, std::move(kdict_));

    RCP<const UnivariateIntPolynomial> c = mul_poly(a, a);
    RCP<const UnivariateIntPolynomial> d = mul_poly(a, b);
    RCP<const UnivariateIntPolynomial> g = mul_poly(e, e);
    RCP<const UnivariateIntPolynomial> h = mul_poly(e, f);
    RCP<const UnivariateIntPolynomial> i = mul_poly(f, f);
    RCP<const UnivariateIntPolynomial> l = mul_poly(k, f);
    RCP<const UnivariateIntPolynomial> m = mul_poly(k, k);

    UnivariateIntPolynomial q1(
        x, 4, UIntDict({{4, 1_z}, {3, 4_z}, {2, 6_z}, {1, 4_z}, {0, 1_z}}));
    REQUIRE(eq(*c, q1));
    UnivariateIntPolynomial q2(
        x, 4,
        UIntDict({{4, -1_z}, {3, -4_z}, {2, -6_z}, {1, -4_z}, {0, -1_z}}));
    REQUIRE(eq(*d, q2));
    UnivariateIntPolynomial q3(
        x, 4, UIntDict({{4, 1_z}, {3, 4_z}, {2, -6_z}, {1, -20_z}, {0, 25_z}}));
    REQUIRE(eq(*g, q3));
    UnivariateIntPolynomial q4(
        x, 4,
        UIntDict({{4, -3_z}, {3, -4_z}, {2, 13_z}, {1, -22_z}, {0, 30_z}}));
    REQUIRE(eq(*h, q4));
    UnivariateIntPolynomial q5(
        x, 4,
        UIntDict({{4, 9_z}, {3, -12_z}, {2, 40_z}, {1, -24_z}, {0, 36_z}}));
    REQUIRE(eq(*i, q5));
    UnivariateIntPolynomial q6(
        x, 4,
        UIntDict(
            {{4, -300_z}, {3, 194_z}, {2, -599_z}, {1, -10_z}, {0, -6_z}}));
    REQUIRE(eq(*l, q6));
    UnivariateIntPolynomial q7(
        x, 4,
        UIntDict({{4, 10000_z}, {3, 400_z}, {2, 204_z}, {1, 4_z}, {0, 1_z}}));
    REQUIRE(eq(*m, q7));

    c = univariate_int_polynomial(none, {{0, -1_z}});
    UnivariateIntPolynomial q8(x, 2,
                               UIntDict({{2, -1_z}, {1, -2_z}, {0, -1_z}}));
    REQUIRE(eq(*mul_poly(a, *c), q8));
    UnivariateIntPolynomial q9(x, 2,
                               UIntDict({{2, -1_z}, {1, -2_z}, {0, -1_z}}));
    REQUIRE(eq(*mul_poly(*c, a), q9));

    c = univariate_int_polynomial(y, {{0, -1_z}});
    CHECK_THROWS_AS(mul_poly(a, *c), std::runtime_error);
}

TEST_CASE("Comparing two UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UnivariateIntPolynomial> P
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}});
    RCP<const UnivariateIntPolynomial> Q
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(P->compare(*Q) == -1);

    P = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 3_z}, {3, 2_z}});
    REQUIRE(P->compare(*Q) == 1);

    P = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {3, 3_z}});
    REQUIRE(P->compare(*Q) == 1);

    P = univariate_int_polynomial(y, {{0, 1_z}, {1, 2_z}, {3, 3_z}});
    REQUIRE(P->compare(*Q) == 1);
}

TEST_CASE("UnivariateIntPolynomial get_args", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariateIntPolynomial> a
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(vec_basic_eq_perm(a->get_args(),
                              {one, mul(integer(2), x), pow(x, integer(2))}));
    REQUIRE(not vec_basic_eq_perm(
        a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));

    RCP<const UnivariateIntPolynomial> b
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 1_z}, {2, 2_z}});
    REQUIRE(vec_basic_eq_perm(b->get_args(),
                              {one, x, mul(integer(2), pow(x, integer(2)))}));

    RCP<const UnivariateIntPolynomial> c
        = univariate_int_polynomial(x, map_uint_mpz{});
    REQUIRE(vec_basic_eq_perm(c->get_args(), {zero}));
}

TEST_CASE("Evaluation of UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariateIntPolynomial> a
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(a->eval(2_z) == 9);
}

TEST_CASE("Derivative of UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");
    RCP<const UnivariateIntPolynomial> a
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UnivariateIntPolynomial> b
        = univariate_int_polynomial(none, {{0, 1_z}});

    RCP<const UnivariateIntPolynomial> q1
        = univariate_int_polynomial(x, {{1, 2_z}, {0, 2_z}});
    REQUIRE(eq(*a->diff(x), *q1));
    RCP<const UnivariateIntPolynomial> q2
        = univariate_int_polynomial(x, {{0, 0_z}});
    REQUIRE(eq(*a->diff(y), *q2));
    RCP<const UnivariateIntPolynomial> q3
        = univariate_int_polynomial(none, {{0, 0_z}});
    REQUIRE(eq(*b->diff(y), *q3));

    a = univariate_int_polynomial(none, {{0, 1_z}});
    REQUIRE(eq(*a->diff(y), *q3));
    a = univariate_int_polynomial(none, map_uint_mpz{});
    REQUIRE(eq(*a->diff(y), *q3));
}

TEST_CASE("Bool checks specific UnivariateIntPolynomial cases",
          "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariateIntPolynomial> z
        = univariate_int_polynomial(x, {{0, 0_z}});
    RCP<const UnivariateIntPolynomial> o
        = univariate_int_polynomial(x, {{0, 1_z}});
    RCP<const UnivariateIntPolynomial> mo
        = univariate_int_polynomial(x, {{0, -1_z}});
    RCP<const UnivariateIntPolynomial> i
        = univariate_int_polynomial(x, {{0, 6_z}});
    RCP<const UnivariateIntPolynomial> s
        = univariate_int_polynomial(x, {{1, 1_z}});
    RCP<const UnivariateIntPolynomial> m1
        = univariate_int_polynomial(x, {{1, 6_z}});
    RCP<const UnivariateIntPolynomial> m2
        = univariate_int_polynomial(x, {{3, 5_z}});
    RCP<const UnivariateIntPolynomial> po
        = univariate_int_polynomial(x, {{5, 1_z}});
    RCP<const UnivariateIntPolynomial> poly
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

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

TEST_CASE("UnivariateIntPolynomial expand", "[UnivariateIntPolynomial][expand]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariateIntPolynomial> a
        = univariate_int_polynomial(x, {{1, 1_z}, {2, 1_z}, {3, 1_z}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(x**3 + x**2 + x)**3");
    REQUIRE(c->__str__()
            == "x**9 + 3*x**8 + 6*x**7 + 7*x**6 + 6*x**5 + 3*x**4 + x**3");
}

TEST_CASE("Constructor of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> none = symbol("");
    Expression a(symbol("a"));
    Expression b(symbol("b"));
    Expression c(symbol("c"));
    Expression d(symbol("d"));
    Expression num2(integer(2));
    Expression num1(integer(1));

    RCP<const UnivariatePolynomial> P = univariate_polynomial(
        x, UnivariateExprPolynomial({{0, num1}, {1, num2}, {2, num1}}));
    REQUIRE(vec_basic_eq_perm(
        P->get_args(), {pow(x, integer(2)), mul(integer(2), x), integer(1)}));

    RCP<const UnivariatePolynomial> Q
        = UnivariatePolynomial::from_vec(x, {1, 0, 2, 1});
    REQUIRE(vec_basic_eq_perm(
        Q->get_args(),
        {pow(x, integer(3)), mul(integer(2), pow(x, integer(2))), integer(1)}));

    RCP<const UnivariatePolynomial> R = univariate_polynomial(
        x, UnivariateExprPolynomial({{0, d}, {1, c}, {2, b}, {3, a}}));
    REQUIRE(vec_basic_eq_perm(R->get_args(),
                              {mul(a.get_basic(), pow(x, integer(3))),
                               mul(b.get_basic(), pow(x, integer(2))),
                               mul(c.get_basic(), x), d.get_basic()}));

    UnivariatePolynomial S(x, {1, 0, 2, 1});
    REQUIRE(vec_basic_eq_perm(
        S.get_args(),
        {pow(x, integer(3)), mul(integer(2), pow(x, integer(2))), integer(1)}));

    R = univariate_polynomial(x, UnivariateExprPolynomial({{-1, d}}));
    REQUIRE(vec_basic_eq_perm(R->get_args(),
                              {mul(d.get_basic(), pow(x, integer(-1)))}));

    R = univariate_polynomial(
        x, UnivariateExprPolynomial({{-2, d}, {-1, c}, {0, b}, {1, a}}));
    REQUIRE(vec_basic_eq_perm(R->get_args(),
                              {mul(a.get_basic(), x), b.get_basic(),
                               mul(c.get_basic(), pow(x, integer(-1))),
                               mul(d.get_basic(), pow(x, integer(-2)))}));

    RCP<const UnivariatePolynomial> T
        = univariate_polynomial(none, UnivariateExprPolynomial({}));
    REQUIRE(vec_basic_eq_perm(T->get_args(), {integer(0)}));
}

TEST_CASE("Adding two UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");
    UnivariateExprPolynomial adict_({{0, 1}, {1, 2}, {2, symbol("a")}});
    UnivariateExprPolynomial bdict_({{0, 2}, {1, 3}, {2, symbol("b")}});
    const UnivariatePolynomial a(x, 2, std::move(adict_));
    const UnivariatePolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = add_uni_poly(a, b);
    UnivariatePolynomial q1(
        x, 2, UnivariateExprPolynomial(
                  {{2, add(symbol("a"), symbol("b"))}, {1, 5}, {0, 3}}));
    REQUIRE(eq(*c, q1));

    RCP<const UnivariatePolynomial> d = univariate_polynomial(
        none, UnivariateExprPolynomial({{0, Expression(2)}}));
    UnivariatePolynomial q2(
        x, 2, UnivariateExprPolynomial({{2, symbol("a")}, {1, 2}, {0, 3}}));
    REQUIRE(eq(*add_uni_poly(a, *d), q2));
    REQUIRE(eq(*add_uni_poly(*d, a), q2));

    d = univariate_polynomial(y, {{0, 2}, {1, 4}});
    CHECK_THROWS_AS(add_uni_poly(a, *d), std::runtime_error);
}

TEST_CASE("Negative of a UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    UnivariateExprPolynomial adict_(
        {{0, 1}, {1, symbol("a")}, {2, symbol("c")}});
    const UnivariatePolynomial a(x, 2, std::move(adict_));

    RCP<const UnivariatePolynomial> b = neg_uni_poly(a);
    UnivariatePolynomial q1(
        x, 2, UnivariateExprPolynomial({{2, mul(integer(-1), symbol("c"))},
                                        {1, mul(integer(-1), symbol("a"))},
                                        {0, -1}}));
    REQUIRE(eq(*b, q1));

    RCP<const UnivariatePolynomial> c
        = univariate_polynomial(x, UnivariateExprPolynomial({}));
    UnivariatePolynomial q2(x, 0,
                            UnivariateExprPolynomial({{0, Expression(0)}}));
    REQUIRE(eq(*neg_uni_poly(*c), q2));

    c = univariate_polynomial(x,
                              UnivariateExprPolynomial({{0, Expression(2)}}));
    UnivariatePolynomial q3(x, 0,
                            UnivariateExprPolynomial({{0, Expression(-2)}}));
    REQUIRE(eq(*neg_uni_poly(*c), q3));
}

TEST_CASE("Subtracting two UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");
    UnivariateExprPolynomial adict_({{0, 1}, {1, 2}, {2, 1}});
    UnivariateExprPolynomial bdict_(
        {{0, 2}, {1, symbol("b")}, {2, symbol("a")}});
    const UnivariatePolynomial a(x, 2, std::move(adict_));
    const UnivariatePolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = sub_uni_poly(b, a);
    UnivariatePolynomial q1(
        x, 2, UnivariateExprPolynomial({{2, add(integer(-1), symbol("a"))},
                                        {1, add(integer(-2), symbol("b"))},
                                        {0, 1}}));
    REQUIRE(eq(*c, q1));

    RCP<const UnivariatePolynomial> d = univariate_polynomial(
        none, UnivariateExprPolynomial({{0, Expression(2)}}));
    UnivariatePolynomial q2(
        x, 2, UnivariateExprPolynomial({{2, 1}, {1, 2}, {0, -1}}));
    UnivariatePolynomial q3(
        x, 2, UnivariateExprPolynomial({{2, -1}, {1, -2}, {0, 1}}));
    REQUIRE(eq(*sub_uni_poly(a, *d), q2));
    REQUIRE(eq(*sub_uni_poly(*d, a), q3));

    d = univariate_polynomial(y, {{0, 2}, {1, 4}});
    CHECK_THROWS_AS(sub_uni_poly(a, *d), std::runtime_error);
}

TEST_CASE("Multiplication of two UnivariatePolynomial",
          "[UnivariatePolynomial]")
{

    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(
        x, {{0, 1}, {1, symbol("b")}, {2, symbol("a")}});
    RCP<const UnivariatePolynomial> b = univariate_polynomial(
        x, {{0, -1}, {1, -2}, {2, mul(integer(-1), symbol("a"))}});

    RCP<const UnivariatePolynomial> c = mul_uni_poly(*a, *a);
    RCP<const UnivariatePolynomial> d = mul_uni_poly(*a, *b);

    UnivariatePolynomial q1(
        x, 4, UnivariateExprPolynomial(
                  {{4, pow(symbol("a"), integer(2))},
                   {3, mul(integer(2), mul(symbol("a"), symbol("b")))},
                   {2, add(mul(integer(2), symbol("a")),
                           pow(symbol("b"), integer(2)))},
                   {1, mul(integer(2), symbol("b"))},
                   {0, 1}}));
    REQUIRE(eq(*c, q1));
    UnivariatePolynomial q2(
        x, 4, UnivariateExprPolynomial(
                  {{4, mul(integer(-1), pow(symbol("a"), integer(2)))},
                   {3, sub(mul(integer(-2), symbol("a")),
                           mul(symbol("a"), symbol("b")))},
                   {2, sub(mul(integer(-2), symbol("a")),
                           mul(integer(2), symbol("b")))},
                   {1, sub(integer(-2), symbol("b"))},
                   {0, -1}}));
    REQUIRE(eq(*d, q2));

    RCP<const UnivariatePolynomial> f = univariate_polynomial(
        x, UnivariateExprPolynomial({{0, Expression(2)}}));
    UnivariatePolynomial q3(
        x, 2, UnivariateExprPolynomial({{2, mul(integer(2), symbol("a"))},
                                        {1, mul(integer(2), symbol("b"))},
                                        {0, 2}}));
    REQUIRE(eq(*mul_uni_poly(*a, *f), q3));
    REQUIRE(eq(*mul_uni_poly(*f, *a), q3));

    f = univariate_polynomial(y, {{0, 2}, {1, 4}});
    CHECK_THROWS_AS(mul_uni_poly(*a, *f), std::runtime_error);

    f = univariate_polynomial(x, UnivariateExprPolynomial({}));
    UnivariatePolynomial q4(x, 0,
                            UnivariateExprPolynomial({{0, Expression(0)}}));
    REQUIRE(eq(*mul_uni_poly(*a, *f), q4));

    a = univariate_polynomial(x, {{-2, 5}, {-1, 3}, {0, 1}, {1, 2}});

    c = mul_uni_poly(*a, *b);
    UnivariatePolynomial q5(
        x, 3, UnivariateExprPolynomial(
                  {{3, mul(integer(-2), symbol("a"))},
                   {2, sub(integer(-4), symbol("a"))},
                   {1, sub(integer(-4), mul(integer(3), symbol("a")))},
                   {0, add(integer(-7), mul(integer(-5), symbol("a")))},
                   {-1, Expression(-13)},
                   {-2, Expression(-5)}}));
    REQUIRE(eq(*c, q5));
}

TEST_CASE("Comparing two UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UnivariatePolynomial> P
        = univariate_polynomial(x, {{0, 1}, {1, 2}});
    RCP<const UnivariatePolynomial> Q
        = univariate_polynomial(x, {{0, 1}, {1, symbol("b")}, {2, 1}});

    REQUIRE(P->compare(*Q) == -1);

    P = univariate_polynomial(x, {{0, 1}, {1, symbol("k")}, {2, 3}, {3, 2}});
    REQUIRE(P->compare(*Q) == 1);

    P = univariate_polynomial(x, {{0, 1}, {1, 2}, {3, 3}});
    REQUIRE(P->compare(*Q) == -1);

    P = univariate_polynomial(y, {{0, 1}, {1, 2}, {3, 3}});
    REQUIRE(P->compare(*Q) == 1);

    P = univariate_polynomial(x, {{0, 1}, {1, symbol("b")}, {2, 1}});
    REQUIRE(P->compare(*Q) == 0);
    REQUIRE(P->__eq__(*Q) == true);

    P = univariate_polynomial(x, {{0, 1}, {1, symbol("a")}, {2, 1}});
    REQUIRE(P->compare(*Q) == -1);
    REQUIRE(P->__eq__(*Q) == false);
}

TEST_CASE("UnivariatePolynomial get_args", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariatePolynomial> a
        = univariate_polynomial(x, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(vec_basic_eq_perm(a->get_args(),
                              {one, mul(integer(2), x), pow(x, integer(2))}));
    REQUIRE(not vec_basic_eq_perm(
        a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));

    a = univariate_polynomial(x, {{0, 1}, {1, 1}, {2, 2}});
    REQUIRE(vec_basic_eq_perm(a->get_args(),
                              {one, x, mul(integer(2), pow(x, integer(2)))}));

    a = univariate_polynomial(x, map_int_Expr{});
    REQUIRE(vec_basic_eq_perm(a->get_args(), {zero}));
}

TEST_CASE("UnivariatePolynomial max_coef", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariatePolynomial> a
        = univariate_polynomial(x, {{0, 1}, {1, 2}, {2, 4}});
    RCP<const UnivariatePolynomial> b
        = univariate_polynomial(x, {{0, 2}, {1, 2}, {2, symbol("b")}});

    Expression c(symbol("a"));
    Expression d(symbol("b"));

    REQUIRE(a->max_coef() == 4);
    REQUIRE(not(a->max_coef() == 2));
    REQUIRE(b->max_coef() == d);
    REQUIRE(not(b->max_coef() == c));
    REQUIRE(not(b->max_coef() == 2));
}

TEST_CASE("Evaluation of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariatePolynomial> a
        = univariate_polynomial(x, {{0, 1}, {1, 2}, {2, symbol("a")}});

    REQUIRE(a->eval(2).get_basic()->__str__() == "5 + 4*a");

    a = univariate_polynomial(x, {{-2, 5}, {-1, 3}, {0, 1}, {1, 2}});
    REQUIRE(a->eval(2).get_basic()->__str__() == "31/4");
}

TEST_CASE("Derivative of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> none = symbol("");
    RCP<const UnivariatePolynomial> a
        = univariate_polynomial(x, {{0, 1}, {1, 2}, {2, symbol("a")}});
    RCP<const UnivariatePolynomial> b
        = univariate_polynomial(x, {{0, Expression(1)}});
    RCP<const UnivariatePolynomial> c
        = univariate_polynomial(none, {{0, Expression(5)}});

    RCP<const UnivariatePolynomial> q1 = univariate_polynomial(
        x,
        UnivariateExprPolynomial({{1, mul(integer(2), symbol("a"))}, {0, 2}}));
    REQUIRE(eq(*a->diff(x), *q1));
    RCP<const UnivariatePolynomial> q2 = univariate_polynomial(
        x, UnivariateExprPolynomial({{0, Expression(0)}}));
    REQUIRE(eq(*a->diff(y), *q2));
    REQUIRE(eq(*b->diff(y), *q2));

    a = univariate_polynomial(
        x, UnivariateExprPolynomial(
               {{-2, 5}, {-1, 3}, {0, 1}, {1, 2}, {2, symbol("a")}}));
    RCP<const UnivariatePolynomial> q3 = univariate_polynomial(
        x,
        UnivariateExprPolynomial(
            {{1, mul(integer(2), symbol("a"))}, {0, 2}, {-2, -3}, {-3, -10}}));
    REQUIRE(eq(*a->diff(x), *q3));

    RCP<const UnivariatePolynomial> q4 = univariate_polynomial(
        none, UnivariateExprPolynomial({{0, Expression(0)}}));
    REQUIRE(eq(*c->diff(x), *q4));

    c = univariate_polynomial(none, UnivariateExprPolynomial({}));
    REQUIRE(eq(*c->diff(x), *q4));
}

TEST_CASE("Bool checks specific UnivariatePolynomial cases",
          "[UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariatePolynomial> z
        = univariate_polynomial(x, {{0, Expression(0)}});
    RCP<const UnivariatePolynomial> o
        = univariate_polynomial(x, {{0, Expression(1)}});
    RCP<const UnivariatePolynomial> mo
        = univariate_polynomial(x, {{0, Expression(-1)}});
    RCP<const UnivariatePolynomial> i
        = univariate_polynomial(x, {{0, Expression(6)}});
    RCP<const UnivariatePolynomial> s
        = univariate_polynomial(x, {{1, Expression(1)}});
    RCP<const UnivariatePolynomial> m1
        = univariate_polynomial(x, {{1, Expression(6)}});
    RCP<const UnivariatePolynomial> m2
        = univariate_polynomial(x, {{3, Expression(5)}});
    RCP<const UnivariatePolynomial> po
        = univariate_polynomial(x, {{5, Expression(1)}});
    RCP<const UnivariatePolynomial> poly
        = univariate_polynomial(x, {{0, 1}, {1, 2}, {2, 1}});
    RCP<const UnivariatePolynomial> neg
        = univariate_polynomial(x, {{-2, 5}, {-1, 3}, {0, 1}, {1, 2}});

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

TEST_CASE("UnivariatePolynomial expand", "[UnivariatePolynomial][expand]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariatePolynomial> a
        = univariate_polynomial(x, {{1, 1}, {2, 1}, {3, symbol("a")}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(a*x**3 + x**2 + x)**3");
    REQUIRE(c->__str__() == "(a**3)*x**9 + (3*a**2)*x**8 + (2*a + a*(1 + 2*a) "
                            "+ a**2)*x**7 + (1 + 6*a)*x**6 + (3 + 3*a)*x**5 + "
                            "3*x**4 + x**3");
}
