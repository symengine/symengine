#include <iostream>
#include <chrono>

#include "polynomial.h"
#include "add.h"
#include "mul.h"
#include "pow.h"
#include "dict.h"
#include "rings.h"

using SymEngine::UnivariatePolynomial;
using SymEngine::univariate_polynomial;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::umap_ull_mpz;
using SymEngine::umap_vec_mpz;
using SymEngine::umap_basic_num;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;	
using SymEngine::vec_int;
using SymEngine::pack_vec_int;
using SymEngine::if_pack_vec_int;
using SymEngine::poly_mul2;

void uni_poly_constructor()
{   
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> P = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});
    assert(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UnivariatePolynomial> Q = rcp(new UnivariatePolynomial(x, {1, 0, 2, 1}));
    assert(Q->__str__() == "x**3 + 2*x**2 + 1");
}

void test_add_uni_poly()
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
    map_uint_mpz bdict_ = {{0, 2}, {1, 3}, {2, 4}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));
    const UnivariatePolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = add_uni_poly(a, b);
    //std::cout<<c->__str__();
    assert(c->__str__() == "5*x**2 + 5*x + 3");
}

void test_neg_uni_poly()
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));

    RCP<const UnivariatePolynomial> b = neg_uni_poly(a);
    //std::cout<<b->__str__()<<std::endl;
    assert(b->__str__() == "-x**2 - 2*x - 1");
}

void test_sub_uni_poly()
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
    map_uint_mpz bdict_ = {{0, 2}, {1, 3}, {2, 4}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));
    const UnivariatePolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = sub_uni_poly(b, a);
    //std::cout<<c->__str__();
    assert(c->__str__() == "3*x**2 + x + 1");
}

void test_mul_uni_poly()
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});
    RCP<const UnivariatePolynomial> b = univariate_polynomial(x, 2, {{0, -1}, {1, -2}, {2, -1}});

    RCP<const UnivariatePolynomial> c = mul_uni_poly(a, a);
    //std::cout<<c->__str__();
    RCP<const UnivariatePolynomial> d = mul_uni_poly(a, b);
    //std::cout<<c->__str__();

    assert(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
    assert(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
}

void test_get_args()
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    assert(vec_basic_eq_perm(a->get_args(), {one, mul(integer(2), x), pow(x, integer(2))}));
    assert(!vec_basic_eq_perm(a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));
}

void test_eval()
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    assert(a->eval(2) == 9);
    assert(a->eval_bit(3) == 81);
}

void test_diff()
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    assert(a->diff(x)->__str__() == "2*x + 2");
	//std::cout<<a->diff(x)->__str__()<<std::endl;
    assert(a->diff(y)->__str__() == "0");
	//std::cout<<a->diff(y)->__str__()<<std::endl;
}

void test_bool_checks()
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

    assert(z->is_zero() && !z->is_one() && !z->is_minus_one() && z->is_integer() && !z->is_symbol() &&
			!z->is_mul() && !z->is_pow());
    assert(!o->is_zero() && o->is_one() && !o->is_minus_one() && o->is_integer() && !o->is_symbol() &&
			!o->is_mul() && !o->is_pow());
    assert(!mo->is_zero() && !mo->is_one() && mo->is_minus_one() && mo->is_integer() && !mo->is_symbol() &&
			!mo->is_mul() && !mo->is_pow());
    assert(!i->is_zero() && !i->is_one() && !i->is_minus_one() && i->is_integer() && !i->is_symbol() &&
			!i->is_mul() && !i->is_pow());
    assert(!s->is_zero() && !s->is_one() && !s->is_minus_one() && !s->is_integer() && s->is_symbol() &&
			!s->is_mul() && !s->is_pow());
    assert(!m1->is_zero() && !m1->is_one() && !m1->is_minus_one() && !m1->is_integer() && !m1->is_symbol() &&
			m1->is_mul() && !m1->is_pow());
    assert(!m2->is_zero() && !m2->is_one() && !m2->is_minus_one() && !m2->is_integer() && !m2->is_symbol() &&
			m2->is_mul() && !m2->is_pow());
    assert(!po->is_zero() && !po->is_one() && !po->is_minus_one() && !po->is_integer() && !po->is_symbol() &&
			!po->is_mul() && po->is_pow());
    assert(!poly->is_zero() && !poly->is_one() && !poly->is_minus_one() && !poly->is_integer() && !poly->is_symbol() &&
			!poly->is_mul() && !poly->is_pow());
}

void test_expand()
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 3, {{1, 1}, {2, 1}, {3, 1}});
    RCP<const Basic> b = rcp(new Pow(a, integer(3)));
    RCP<const Basic> c = expand(b);

    assert(b->__str__() == "(x**3 + x**2 + x)**3");
    //std::cout<<b->__str__()<<std::endl;
    assert(c->__str__() == "x**9 + 3*x**8 + 6*x**7 + 7*x**6 + 6*x**5 + 3*x**4 + x**3");
    //std::cout<<c->__str__()<<std::endl;
}

void test_pack_vec_int()
{
    vec_int a = {1, 3, 1};
    vec_int b = {0, 3, 2};
    vec_int c = {3, 0, 0};
    vec_int d = {0, 0, 0};

    assert(if_pack_vec_int(a) && 
        (pack_vec_int(a) == 5*std::pow(2, 48) + 1*std::pow(2, 32) + 3*std::pow(2, 16) + 1));
    assert(if_pack_vec_int(b) &&
        (pack_vec_int(b) == 5*std::pow(2, 48) + 2*std::pow(2, 32) + 3*std::pow(2, 16)));
    assert(if_pack_vec_int(c) &&
        (pack_vec_int(c) == 3*std::pow(2, 48) + 3));
    assert(if_pack_vec_int(b) &&
        (pack_vec_int(d) == 0));

    assert(!if_pack_vec_int({-1, 0 ,1}));
    
    int n = 1<<15;
    assert(!if_pack_vec_int({n, n ,n}));
}

void test_poly2packed()
{

    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> i9 = integer(9);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> e;

    e = add(mul(i9, mul(mul(x, z), pow(y, i3))), mul(i3, x));

    umap_basic_num syms;
    insert(syms, x, integer(0));
    insert(syms, y, integer(1));
    insert(syms, z, integer(2));
    umap_vec_mpz P, Q;
    umap_ull_mpz p;

    expr2poly(e, syms, P);
    poly2packed(P, p);
    packed2poly(p, syms, Q);

    assert(P == Q);
}

void test_poly_mul2()
{

    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> i9 = integer(9);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> e;

    e = add(mul(i9, mul(mul(x, z), pow(y, i3))), mul(i3, x));

    umap_basic_num syms;
    insert(syms, x, integer(0));
    insert(syms, y, integer(1));
    insert(syms, z, integer(2));
    umap_vec_mpz P, Q;
    umap_ull_mpz p, q;

    expr2poly(e, syms, P);
    poly2packed(P, p);
    poly_mul2(p, p, q);
    packed2poly(q, syms, Q);

    //std::cout<<Q<<std::endl;
    //TODO:Tested only by looking at output
    //Assertion test needed
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    uni_poly_constructor();

    test_add_uni_poly();

    test_neg_uni_poly();

    test_sub_uni_poly();

    test_mul_uni_poly();

    test_get_args();

    test_eval();

    test_diff();

    test_bool_checks();

    test_expand();

    test_pack_vec_int();

    test_poly2packed();

    test_poly_mul2();
    return 0;
}
