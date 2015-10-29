#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/polynomial.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::UnivariatePolynomial;
using SymEngine::univariate_polynomial;
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

TEST_CASE("Constructor of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> P = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UnivariatePolynomial> Q = UnivariatePolynomial::create(x, {1, 0, 2, 1});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");
}

TEST_CASE("Adding two UnivariatePolynomial", "[UnivariatePolynomial]")
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

TEST_CASE("Negative of a UnivariatePolynomial", "[UnivariatePolynomial]")
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

TEST_CASE("Multiplication of two UnivariatePolynomial", "[UnivariatePolynomial]")
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

TEST_CASE("UnivariatePolynomial get_args", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(vec_basic_eq_perm(a->get_args(), {one, mul(integer(2), x), pow(x, integer(2))}));
    REQUIRE(!vec_basic_eq_perm(a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));
}

TEST_CASE("Evaluation of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(a->eval(2) == 9);
    REQUIRE(a->eval_bit(3) == 81);
}

TEST_CASE("Derivative of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(a->diff(x)->__str__() == "2*x + 2");
	//std::cout<<a->diff(x)->__str__()<<std::endl;
    REQUIRE(a->diff(y)->__str__() == "0");
	//std::cout<<a->diff(y)->__str__()<<std::endl;
}

TEST_CASE("Bool checks specific UnivariatePolynomial cases", "[UnivariatePolynomial]")
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

    REQUIRE((z->is_zero() and !z->is_one() and !z->is_minus_one() and z->is_integer() and !z->is_symbol() and
			!z->is_mul() and !z->is_pow()));
    REQUIRE((!o->is_zero() and o->is_one() and !o->is_minus_one() and o->is_integer() and !o->is_symbol() and
			!o->is_mul() and !o->is_pow()));
    REQUIRE((!mo->is_zero() and !mo->is_one() and mo->is_minus_one() and mo->is_integer() and !mo->is_symbol() and
			!mo->is_mul() and !mo->is_pow()));
    REQUIRE((!i->is_zero() and !i->is_one() and !i->is_minus_one() and i->is_integer() and !i->is_symbol() and
			!i->is_mul() and !i->is_pow()));
    REQUIRE((!s->is_zero() and !s->is_one() and !s->is_minus_one() and !s->is_integer() and s->is_symbol() and
			!s->is_mul() and !s->is_pow()));
    REQUIRE((!m1->is_zero() and !m1->is_one() and !m1->is_minus_one() and !m1->is_integer() and !m1->is_symbol() and
			m1->is_mul() and !m1->is_pow()));
    REQUIRE((!m2->is_zero() and !m2->is_one() and !m2->is_minus_one() and !m2->is_integer() and !m2->is_symbol() and
			m2->is_mul() and !m2->is_pow()));
    REQUIRE((!po->is_zero() and !po->is_one() and !po->is_minus_one() and !po->is_integer() and !po->is_symbol() and
			!po->is_mul() and po->is_pow()));
    REQUIRE((!poly->is_zero() and !poly->is_one() and !poly->is_minus_one() and !poly->is_integer() and !poly->is_symbol() and
			!poly->is_mul() and !poly->is_pow()));
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
