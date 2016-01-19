#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/polynomial.h>
#include <symengine/polys.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/dict.h>
#include <symengine/symengine_config.h>

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
using SymEngine::vec_symbol;
using SymEngine::Polynomial;
using SymEngine::polynomial;
using SymEngine::umap_basic_num;

#if defined(HAVE_SYMENGINE_PIRANHA)
using SymEngine::hash_set;
using SymEngine::m_pair;
#endif

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
    REQUIRE(not vec_basic_eq_perm(a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));
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

    REQUIRE((z->is_zero() and not z->is_one() and not z->is_minus_one() and z->is_integer() and not z->is_symbol() and
			not z->is_mul() and not z->is_pow()));
    REQUIRE((not o->is_zero() and o->is_one() and not o->is_minus_one() and o->is_integer() and not o->is_symbol() and
			not o->is_mul() and not o->is_pow()));
    REQUIRE((not mo->is_zero() and not mo->is_one() and mo->is_minus_one() and mo->is_integer() and not mo->is_symbol() and
			not mo->is_mul() and not mo->is_pow()));
    REQUIRE((not i->is_zero() and not i->is_one() and not i->is_minus_one() and i->is_integer() and not i->is_symbol() and
			not i->is_mul() and not i->is_pow()));
    REQUIRE((not s->is_zero() and not s->is_one() and not s->is_minus_one() and not s->is_integer() and s->is_symbol() and
			not s->is_mul() and not s->is_pow()));
    REQUIRE((not m1->is_zero() and not m1->is_one() and not m1->is_minus_one() and not m1->is_integer() and not m1->is_symbol() and
			m1->is_mul() and not m1->is_pow()));
    REQUIRE((not m2->is_zero() and not m2->is_one() and not m2->is_minus_one() and not m2->is_integer() and not m2->is_symbol() and
			m2->is_mul() and not m2->is_pow()));
    REQUIRE((not po->is_zero() and not po->is_one() and not po->is_minus_one() and not po->is_integer() and not po->is_symbol() and
			not po->is_mul() and po->is_pow()));
    REQUIRE((not poly->is_zero() and not poly->is_one() and not poly->is_minus_one() and not poly->is_integer() and not poly->is_symbol() and
			not poly->is_mul() and not poly->is_pow()));
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

#if defined(HAVE_SYMENGINE_PIRANHA)
TEST_CASE("Testing hash_set", "[hash_set]")
{
    m_pair t1, t2;
    hash_set p, q, s;

    t1.first = 3;
    t1.second = 2;
    t2.first = 2;
    t2.second = 1;

    s.insert(t1);
    s.insert(t2);

    q = s;

    std::stringstream o;
    o << s;
    REQUIRE(o.str() == "{2: 1, 3: 2}");

    REQUIRE(hash_set_eq(s, q) == 1);
    REQUIRE(hash_set_eq(s, p) == 0);

    REQUIRE(hash_set_compare(s, p) == 1);
    REQUIRE(hash_set_compare(p, s) == -1);
    REQUIRE(hash_set_compare(s, q) == 0);
}

TEST_CASE("Constructor of Polynomial", "[Polynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    vec_symbol vars;
    vars.push_back(x);

    m_pair t1, t2;
    hash_set s;

    t1.second = 2;
    t2.second = 1;

    std::vector<long long> temp {3};
    using ka = piranha::kronecker_array<long long>;
    t1.first = ka::encode(temp);
    temp = {2};
    t2.first = ka::encode(temp);

    s.insert(t1);
    s.insert(t2);

    RCP<const Polynomial> P = polynomial(vars, s);
    REQUIRE(P->__str__() == "x**2 + 2*x**3");

    RCP<const Basic> y  = symbol("y");
    RCP<const Basic> r = add(add(x, y), add(y, x));

    umap_basic_num syms;
    insert(syms, x, integer(0));
    insert(syms, y, integer(1));

    const Polynomial Q(r, syms);
    REQUIRE(Q.__str__() == "2*y**1*x**0 + 2*y**0*x**1");
}

TEST_CASE("Arithmetic of Polynomial", "[Polynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Basic> y  = symbol("y");
    vec_symbol vars;
    vars.push_back(x);

    m_pair t1, t2;
    hash_set s;

    t1.second = 2;
    t2.second = 1;

    std::vector<long long> temp {3};
    using ka = piranha::kronecker_array<long long>;
    t1.first = ka::encode(temp);
    temp = {2};
    t2.first = ka::encode(temp);

    s.insert(t1);
    s.insert(t2);

    RCP<const Polynomial> P = polynomial(vars, s);
    RCP<const Polynomial> R = neg_poly(*P);
    REQUIRE(R->__str__() == "-x**2 - 2*x**3");

    RCP<const Polynomial> Q = add_poly(*P, *P);
    REQUIRE(Q->__str__() == "2*x**2 + 4*x**3");

    RCP<const Polynomial> S = sub_poly(*Q, *P);
    REQUIRE(S->__str__() == "x**2 + 2*x**3");

    RCP<const Polynomial> T = mul_poly(*S, *S);
    REQUIRE(T->__str__() == "x**4 + 4*x**5 + 4*x**6");

    RCP<const Basic> r = add(add(x, y), add(y, x));

    umap_basic_num syms;
    insert(syms, x, integer(0));
    insert(syms, y, integer(1));

    RCP<const Polynomial> W = make_rcp<const Polynomial>(r, syms);
    RCP<const Polynomial> V = mul_poly(*W, *W);
    //std::cout<<V->__str__()<<std::endl;
    REQUIRE(V->__str__() == "8*y**1*x**1 + 4*y**2*x**0 + 4*y**0*x**2");

    RCP<const Polynomial> X = mul_poly(*V, *S);
    std::cout<<X->__str__()<<std::endl;
}
#endif
