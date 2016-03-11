#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/polynomial.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::UnivariateIntPolynomial;
using SymEngine::univariate_int_polynomial;
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
using SymEngine::MultivariateIntPolynomial;
using SymEngine::RCPSymbolCompare;
using namespace SymEngine::literals;

TEST_CASE("Constructor of UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateIntPolynomial> P = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UnivariateIntPolynomial> Q = UnivariateIntPolynomial::create(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");
}

TEST_CASE("Adding two UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 1_z}};
    map_uint_mpz bdict_ = {{0, 2_z}, {1, 3_z}, {2, 4_z}};
    const UnivariateIntPolynomial a(x, 2, std::move(adict_));
    const UnivariateIntPolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = add_poly(a, b);
    //std::cout<<c->__str__();
    REQUIRE(c->__str__() == "5*x**2 + 5*x + 3");
}

TEST_CASE("Negative of a UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 1_z}};
    const UnivariateIntPolynomial a(x, 2, std::move(adict_));

    RCP<const UnivariateIntPolynomial> b = neg_poly(a);
    //std::cout<<b->__str__()<<std::endl;
    REQUIRE(b->__str__() == "-x**2 - 2*x - 1");
}

TEST_CASE("Subtracting two UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 1_z}};
    map_uint_mpz bdict_ = {{0, 2_z}, {1, 3_z}, {2, 4_z}};
    const UnivariateIntPolynomial a(x, 2, std::move(adict_));
    const UnivariateIntPolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = sub_poly(b, a);
    //std::cout<<c->__str__();
    REQUIRE(c->__str__() == "3*x**2 + x + 1");
}

TEST_CASE("Multiplication of two UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateIntPolynomial> a = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UnivariateIntPolynomial> b = univariate_int_polynomial(x, {{0, -1_z}, {1, -2_z}, {2, -1_z}});

    RCP<const UnivariateIntPolynomial> c = mul_poly(a, a);
    //std::cout<<c->__str__();
    RCP<const UnivariateIntPolynomial> d = mul_poly(a, b);
    //std::cout<<c->__str__();

    REQUIRE(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
    REQUIRE(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
}

TEST_CASE("UnivariateIntPolynomial get_args", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateIntPolynomial> a = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(vec_basic_eq_perm(a->get_args(), {one, mul(integer(2), x), pow(x, integer(2))}));
    REQUIRE(not vec_basic_eq_perm(a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));
}

TEST_CASE("Evaluation of UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateIntPolynomial> a = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(a->eval(2_z) == 9);
    REQUIRE(a->eval_bit(3) == 81);
}

TEST_CASE("Derivative of UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const UnivariateIntPolynomial> a = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(a->diff(x)->__str__() == "2*x + 2");
	//std::cout<<a->diff(x)->__str__()<<std::endl;
    REQUIRE(a->diff(y)->__str__() == "0");
	//std::cout<<a->diff(y)->__str__()<<std::endl;
}

TEST_CASE("Bool checks specific UnivariateIntPolynomial cases", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateIntPolynomial> z = univariate_int_polynomial(x, {{0, 0_z}});
    RCP<const UnivariateIntPolynomial> o = univariate_int_polynomial(x, {{0, 1_z}});
    RCP<const UnivariateIntPolynomial> mo = univariate_int_polynomial(x, {{0, -1_z}});
    RCP<const UnivariateIntPolynomial> i = univariate_int_polynomial(x, {{0, 6_z}});
    RCP<const UnivariateIntPolynomial> s = univariate_int_polynomial(x, {{1, 1_z}});
    RCP<const UnivariateIntPolynomial> m1 = univariate_int_polynomial(x, {{1, 6_z}});
    RCP<const UnivariateIntPolynomial> m2 = univariate_int_polynomial(x, {{3, 5_z}});
    RCP<const UnivariateIntPolynomial> po = univariate_int_polynomial(x, {{5, 1_z}});
    RCP<const UnivariateIntPolynomial> poly = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

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

TEST_CASE("Univariate Polynomial expand", "[UnivariateIntPolynomial][expand]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateIntPolynomial> a = univariate_int_polynomial(x, {{1, 1_z}, {2, 1_z}, {3, 1_z}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(x**3 + x**2 + x)**3");
    //std::cout<<b->__str__()<<std::endl;
    REQUIRE(c->__str__() == "x**9 + 3*x**8 + 6*x**7 + 7*x**6 + 6*x**5 + 3*x**4 + x**3");
    //std::cout<<c->__str__()<<std::endl;
}

TEST_CASE("Constructing MultivariateIntPolynomial using from_dict", "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const MultivariateIntPolynomial> P = MultivariateIntPolynomial::from_dict({x,y}, { {{1,2}, 1_z}, {{1,1}, 2_z}, {{0,1}, 2_z}, {{1,0}, 3_z} , {{0,0}, 0_z} });
    REQUIRE(P->__str__() == "x y**2 + 2*x y + 3*x + 2*y");

    RCP<const MultivariateIntPolynomial> P2 = MultivariateIntPolynomial::from_dict({x,y}, { {{0,0}, 0_z} });
    REQUIRE(P2->__str__() == "0");

}

TEST_CASE("Testing MultivariateIntPolynomial::__eq__(const Basic &o)", "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::from_dict({x,y}, {{{2,0},1_z}, {{1,1},1_z}, {{0,2},1_z}  });
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::from_dict({x,y}, {{{2,0},1_z}, {{1,1},-1_z}, {{0,2},1_z}  });
    RCP<const MultivariateIntPolynomial> p3 = MultivariateIntPolynomial::from_dict({x,y}, {{{2,0},2_z}, {{0,2},2_z}  });

    REQUIRE(p1->__eq__(*p1));
    REQUIRE(!(p2->__eq__(*p1)));
    REQUIRE(p3->__eq__( *add_mult_poly(*p1, *p2) ));

}

TEST_CASE("Testing MultivariateIntPolynomial::eval((std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> &vals)", "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p = MultivariateIntPolynomial::from_dict( {x,y,z}, { {{2,0,0},1_z}, {{0,2,0},2_z}, {{0,0,2},3_z}, {{1,1,1},4_z}, {{1,1,0},1_z}, {{0,1,1},2_z}, {{1,0,0},1_z}, {{0,1,0},2_z}, {{0,0,1}, 3_z} , {{0,0,0},5} });
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m1 = {{x,1}, {y,2}, {z,5}};
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m2 = {{x,0}, {y,0}, {z,0}};
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m3 = {{x,-1}, {y,-2}, {z,-5}};

    REQUIRE(171_z == p->eval(m1));
    REQUIRE(5_z == p->eval(m2));
    REQUIRE(51_z == p->eval(m3));
}

TEST_CASE("Testing MultivariateIntPolynomail neg_mult_poly", "[MultivariateIntPolynomial]"){
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p = MultivariateIntPolynomial::from_dict( {x,y,z}, { {{1,0,0}, 1_z}, {{0,1,0},-2_z}, {{0,0,1}, 3_z }  });
    RCP<const MultivariateIntPolynomial> p2 = neg_mult_poly(*p);
    REQUIRE(p2->__str__() == "- x + 2*y - 3*z");
  
}
