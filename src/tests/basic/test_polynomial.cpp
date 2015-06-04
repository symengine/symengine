#include <iostream>
#include <chrono>

#include "polynomial.h"
#include "mul.h"
#include "pow.h"
#include "dict.h"

using SymEngine::Polynomial;
using SymEngine::polynomial;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;

void polynomial_constructor()
{   
    RCP<const Symbol> x  = symbol("x");
    RCP<const Polynomial> P = polynomial(x, {{0, 1}, {1, 2}, {2, 1}});
    assert(P->__str__() == "x**2 + 2*x + 1");

    RCP<const Polynomial> Q = rcp(new Polynomial(x, {1, 0, 2, 1}));
    assert(Q->__str__() == "x**3 + 2*x**2 + 1");
}

void test_add_poly()
{
	RCP<const Symbol> x  = symbol("x");
	map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
	map_uint_mpz bdict_ = {{0, 2}, {1, 3}, {2, 4}};
	const Polynomial a(x, std::move(adict_));
	const Polynomial b(x, std::move(bdict_));

	RCP<const Basic> c = add_poly(a, b);
	//std::cout<<c->__str__();
	assert(c->__str__() == "5*x**2 + 5*x + 3");
}

void test_neg_poly()
{
	RCP<const Symbol> x  = symbol("x");
	map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
	const Polynomial a(x, std::move(adict_));

	RCP<const Polynomial> b = neg_poly(a);
	//std::cout<<b->__str__()<<std::endl;
	assert(b->__str__() == "-x**2 - 2*x - 1");
}

void test_sub_poly()
{
	RCP<const Symbol> x  = symbol("x");
	map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
	map_uint_mpz bdict_ = {{0, 2}, {1, 3}, {2, 4}};
	const Polynomial a(x, std::move(adict_));
	const Polynomial b(x, std::move(bdict_));

	RCP<const Basic> c = sub_poly(b, a);
	//std::cout<<c->__str__();
	assert(c->__str__() == "3*x**2 + x + 1");
}

void test_mul_poly()
{
	RCP<const Symbol> x  = symbol("x");
    RCP<const Polynomial> a = polynomial(x, {{0, 1}, {1, 2}, {2, 1}});
    RCP<const Polynomial> b = polynomial(x, {{0, -1}, {1, -2}, {2, -1}});

	RCP<const Polynomial> c = mul_poly(a, a);
	//std::cout<<c->__str__();
	RCP<const Polynomial> d = mul_poly(a, b);
	//std::cout<<c->__str__();

	assert(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
	assert(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
}

void test_get_args()
{
	RCP<const Symbol> x  = symbol("x");
	RCP<const Polynomial> a = polynomial(x, {{0, 1}, {1, 2}, {2, 1}});

    assert(vec_basic_eq_perm(a->get_args(), {one, mul(integer(2), x), pow(x, integer(2))}));
    assert(!vec_basic_eq_perm(a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));
}

void test_eval()
{
	RCP<const Symbol> x  = symbol("x");
	RCP<const Polynomial> a = polynomial(x, {{0, 1}, {1, 2}, {2, 1}});

	assert(a->eval(2) == 9);
	assert(a->eval_bit(3) == 81);
}

void test_diff()
{
	RCP<const Symbol> x  = symbol("x");
	RCP<const Symbol> y  = symbol("y");
	RCP<const Polynomial> a = polynomial(x, {{0, 1}, {1, 2}, {2, 1}});

	assert(a->diff(x)->__str__() == "2*x + 2");
	//std::cout<<a->diff(x)->__str__()<<std::endl;
	assert(a->diff(y)->__str__() == "0");
	//std::cout<<a->diff(y)->__str__()<<std::endl;
}

void test_bool_checks()
{
	RCP<const Symbol> x  = symbol("x");
	RCP<const Polynomial> z = polynomial(x, {{0, 0}});
	RCP<const Polynomial> o = polynomial(x, {{0, 1}});
	RCP<const Polynomial> mo = polynomial(x, {{0, -1}});
	RCP<const Polynomial> i = polynomial(x, {{0, 6}});
	RCP<const Polynomial> s = polynomial(x, {{1, 1}});
	RCP<const Polynomial> m1 = polynomial(x, {{1, 6}});
	RCP<const Polynomial> m2 = polynomial(x, {{3, 5}});
	RCP<const Polynomial> po = polynomial(x, {{5, 1}});
	RCP<const Polynomial> poly = polynomial(x, {{0, 1}, {1, 2}, {2, 1}});

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

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    polynomial_constructor();

    test_add_poly();

    test_neg_poly();

    test_sub_poly();

    test_mul_poly();

    test_get_args();

    test_eval();

    test_diff();

    test_bool_checks();
    return 0;
}
