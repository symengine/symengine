#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/series.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::UnivariateSeries;
using SymEngine::univariate_series;
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

TEST_CASE("Constructor of UnivariateSeries", "[UnivariateSeries")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
    RCP<const UnivariateSeries> P = univariate_series(x, 2, adict_);
    REQUIRE(P->__str__() == "1 + 2*x + O(x**2)");

    RCP<const UnivariateSeries> Q = UnivariateSeries::create(x, 5, {1, 0, 2, 1});
    REQUIRE(Q->__str__() == "1 + 2*x**2 + x**3 + O(x**5)");
}
/*
TEST_CASE("Adding two UnivariatePolynomial", "[UnivariatePolynomial")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
    map_uint_mpz bdict_ = {{0, 2}, {1, 3}, {2, 4}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));
    const UnivariatePolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = add_uni_poly(a, b);
    //std::cout<<c->__str__();
    REQUIRE(c->__str__() == "5*x**2 + 5*x + 3");
}

TEST_CASE("Negative of a UnivariatePolynomial", "[UnivariatePolynomial")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));

    RCP<const UnivariatePolynomial> b = neg_uni_poly(a);
    //std::cout<<b->__str__()<<std::endl;
    REQUIRE(b->__str__() == "-x**2 - 2*x - 1");
}

TEST_CASE("Subtracting two UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
    map_uint_mpz bdict_ = {{0, 2}, {1, 3}, {2, 4}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));
    const UnivariatePolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = sub_uni_poly(b, a);
    //std::cout<<c->__str__();
    REQUIRE(c->__str__() == "3*x**2 + x + 1");
}

TEST_CASE("Multiplication of two UnivariatePolynomial", "[UnivariatePolynomial")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});
    RCP<const UnivariatePolynomial> b = univariate_polynomial(x, 2, {{0, -1}, {1, -2}, {2, -1}});

    RCP<const UnivariatePolynomial> c = mul_uni_poly(a, a);
    //std::cout<<c->__str__();
    RCP<const UnivariatePolynomial> d = mul_uni_poly(a, b);
    //std::cout<<c->__str__();

    REQUIRE(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
    REQUIRE(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
}

TEST_CASE("UnivariatePolynomial get_args", "[UnivariatePolynomial")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(vec_basic_eq_perm(a->get_args(), {one, mul(integer(2), x), pow(x, integer(2))}));
    REQUIRE(!vec_basic_eq_perm(a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));
}

TEST_CASE("Evaluation of UnivariatePolynomial", "[UnivariatePolynomial")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(a->eval(2) == 9);
    REQUIRE(a->eval_bit(3) == 81);
}

TEST_CASE("Derivative of UnivariatePolynomial", "[UnivariatePolynomial")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(a->diff(x)->__str__() == "2*x + 2");
	//std::cout<<a->diff(x)->__str__()<<std::endl;
    REQUIRE(a->diff(y)->__str__() == "0");
	//std::cout<<a->diff(y)->__str__()<<std::endl;
}

TEST_CASE("Bool checks specific UnivariatePolynomial cases", "[UnivariatePolynomial")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> z = univariate_polynomial(x, 0, {{0, 0}});
    RCP<const UnivariatePolynomial> o = univariate_polynomial(x, 0, {{0, 1}});
    RCP<const UnivariatePolynomial> mo = univariate_polynomial(x, 0, {{0, -1}});
    RCP<const UnivariatePolynomial> i = univariate_polynomial(x, 0, {{0, 6}});
    RCP<const UnivariatePolynomial> s = univariate_polynomial(x, 1, {{1, 1}});
    RCP<const UnivariatePolynomial> m1 = univariate_polynomial(x, 1, {{1, 6}});
    RCP<const UnivariatePolynomial> m2 = univariate_polynomial(x, 3, {{3, 5}});
    RCP<const UnivariatePolynomial> po = univariate_polynomial(x, 5, {{5, 1}});
    RCP<const UnivariatePolynomial> poly = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE((z->is_zero() && !z->is_one() && !z->is_minus_one() && z->is_integer() && !z->is_symbol() &&
			!z->is_mul() && !z->is_pow()));
    REQUIRE((!o->is_zero() && o->is_one() && !o->is_minus_one() && o->is_integer() && !o->is_symbol() &&
			!o->is_mul() && !o->is_pow()));
    REQUIRE((!mo->is_zero() && !mo->is_one() && mo->is_minus_one() && mo->is_integer() && !mo->is_symbol() &&
			!mo->is_mul() && !mo->is_pow()));
    REQUIRE((!i->is_zero() && !i->is_one() && !i->is_minus_one() && i->is_integer() && !i->is_symbol() &&
			!i->is_mul() && !i->is_pow()));
    REQUIRE((!s->is_zero() && !s->is_one() && !s->is_minus_one() && !s->is_integer() && s->is_symbol() &&
			!s->is_mul() && !s->is_pow()));
    REQUIRE((!m1->is_zero() && !m1->is_one() && !m1->is_minus_one() && !m1->is_integer() && !m1->is_symbol() &&
			m1->is_mul() && !m1->is_pow()));
    REQUIRE((!m2->is_zero() && !m2->is_one() && !m2->is_minus_one() && !m2->is_integer() && !m2->is_symbol() &&
			m2->is_mul() && !m2->is_pow()));
    REQUIRE((!po->is_zero() && !po->is_one() && !po->is_minus_one() && !po->is_integer() && !po->is_symbol() &&
			!po->is_mul() && po->is_pow()));
    REQUIRE((!poly->is_zero() && !poly->is_one() && !poly->is_minus_one() && !poly->is_integer() && !poly->is_symbol() &&
			!poly->is_mul() && !poly->is_pow()));
}

TEST_CASE("Univariate Polynomial expand", "[UnivariatePolynomial][expand]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 3, {{1, 1}, {2, 1}, {3, 1}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(x**3 + x**2 + x)**3");
    //std::cout<<b->__str__()<<std::endl;
    REQUIRE(c->__str__() == "x**9 + 3*x**8 + 6*x**7 + 7*x**6 + 6*x**5 + 3*x**4 + x**3");
    //std::cout<<c->__str__()<<std::endl;
}
*/
