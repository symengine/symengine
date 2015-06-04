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
    return 0;
}
