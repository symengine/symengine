#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/polynomial.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::Expression;
using SymEngine::UnivariateIntPolynomial;
using SymEngine::UnivariatePolynomial;
using SymEngine::univariate_int_polynomial;
using SymEngine::univariate_polynomial;
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
    REQUIRE(c->__str__() == "5*x**2 + 5*x + 3");
}

TEST_CASE("Negative of a UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 1_z}};
    const UnivariateIntPolynomial a(x, 2, std::move(adict_));

    RCP<const UnivariateIntPolynomial> b = neg_poly(a);
    REQUIRE(b->__str__() == "-x**2 - 2*x - 1");
}

TEST_CASE("Subtracting two UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> none  = symbol("");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 1_z}};
    map_uint_mpz bdict_ = {{0, 2_z}, {1, 3_z}, {2, 4_z}};
    map_uint_mpz cdict_ = {{0, 2_z}};

    const UnivariateIntPolynomial a(x, 2, std::move(adict_));
    const UnivariateIntPolynomial b(x, 2, std::move(bdict_));
    const UnivariateIntPolynomial c(none, 0, std::move(cdict_));

    RCP<const Basic> d = sub_poly(b, a);
    REQUIRE(d->__str__() == "3*x**2 + x + 1");
    d = sub_poly(c, a);
    REQUIRE(d->__str__() == "-x**2 - 2*x + 1");
}

TEST_CASE("Multiplication of two UnivariateIntPolynomial", "[UnivariateIntPolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateIntPolynomial> a = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UnivariateIntPolynomial> b = univariate_int_polynomial(x, {{0, -1_z}, {1, -2_z}, {2, -1_z}});

    RCP<const UnivariateIntPolynomial> c = mul_poly(a, a);
    RCP<const UnivariateIntPolynomial> d = mul_poly(a, b);

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
    REQUIRE(a->diff(y)->__str__() == "0");
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

TEST_CASE("Univariate Int Polynomial expand", "[UnivariateIntPolynomial][expand]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateIntPolynomial> a = univariate_int_polynomial(x, {{1, 1_z}, {2, 1_z}, {3, 1_z}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(x**3 + x**2 + x)**3");
    REQUIRE(c->__str__() == "x**9 + 3*x**8 + 6*x**7 + 7*x**6 + 6*x**5 + 3*x**4 + x**3");
}

TEST_CASE("Constructor of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    Expression a(symbol("a"));
    Expression b(symbol("b"));
    Expression c(symbol("c"));
    Expression d(symbol("d"));
    Expression num2(integer(2));
    Expression num1(integer(1));
    RCP<const UnivariatePolynomial> P = univariate_polynomial(x, 2, {{0, num1}, {1, num2}, {2, num1}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UnivariatePolynomial> Q = UnivariatePolynomial::create(x, {1, 0, 2, 1});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UnivariatePolynomial> R = univariate_polynomial(x, 3, {{0, d}, {1, c}, {2, b}, {3, a}});
    REQUIRE(R->__str__() == "a*x**3 + b*x**2 + c*x + d"); 
}

TEST_CASE("Adding two UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, symbol("a")}};
    map_int_Expr bdict_ = {{0, 2}, {1, 3}, {2, symbol("b")}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));
    const UnivariatePolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = add_uni_poly(a, b);
    REQUIRE(c->__str__() == "(a + b)*x**2 + 5*x + 3");
}

TEST_CASE("Negative of a UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, symbol("a")}, {2, symbol("c")}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));

    RCP<const UnivariatePolynomial> b = neg_uni_poly(a);
    REQUIRE(b->__str__() == "-c*x**2 - a*x - 1");
}

TEST_CASE("Subtracting two UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, 1}};
    map_int_Expr bdict_ = {{0, 2}, {1, symbol("b")}, {2, symbol("a")}};
    const UnivariatePolynomial a(x, 2, std::move(adict_));
    const UnivariatePolynomial b(x, 2, std::move(bdict_));

    RCP<const Basic> c = sub_uni_poly(b, a);
    REQUIRE(c->__str__() == "(-1 + a)*x**2 + (-2 + b)*x + 1");
}

TEST_CASE("Multiplication of two UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, symbol("b")}, {2, symbol("a")}});
    RCP<const UnivariatePolynomial> b = univariate_polynomial(x, 2, {{0, -1}, {1, -2}, {2, mul(integer(-1), symbol("a"))}});

    RCP<const UnivariatePolynomial> c = mul_uni_poly(a, a);
    RCP<const UnivariatePolynomial> d = mul_uni_poly(a, b);

    REQUIRE(c->__str__() == "a**2*x**4 + 2*a*b*x**3 + (2*a + b**2)*x**2 + 2*b*x + 1");
    REQUIRE(d->__str__() == "-a**2*x**4 + (-2*a - a*b)*x**3 + (-2*a - 2*b)*x**2 + (-2 - b)*x - 1");
}

TEST_CASE("UnivariatePolynomial get_args", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}});

    REQUIRE(vec_basic_eq_perm(a->get_args(), {one, mul(integer(2), x), pow(x, integer(2))}));
    //REQUIRE(not vec_basic_eq_perm(a->get_args(), {one, mul(integer(3), x), pow(x, integer(2))}));
}

TEST_CASE("Evaluation of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, symbol("a")}});

    REQUIRE(a->eval(2).get_basic()->__str__() == "5 + 4*a");
}

TEST_CASE("Derivative of UnivariatePolynomial", "[UnivariatePolynomial]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, symbol("a")}});
    RCP<const UnivariatePolynomial> b = univariate_polynomial(x, 0, {{0, 1}});
    
    REQUIRE(a->diff(x)->__str__() == "2*a*x + 2");
    REQUIRE(a->diff(y)->__str__() == "0");
    REQUIRE(b->diff(y)->__str__() == "0");
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

TEST_CASE("Univariate Polynomial expand", "[UnivariateIntPolynomial][expand]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariatePolynomial> a = univariate_polynomial(x, 3, {{1, 1}, {2, 1}, {3, symbol("a")}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(a*x**3 + x**2 + x)**3");
    REQUIRE(c->__str__() == "a**3*x**9 + 3*a**2*x**8 + (2*a + a*(1 + 2*a) + a**2)*x**7 + (1 + 6*a)*x**6 + (3 + 3*a)*x**5 + 3*x**4 + x**3");
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

TEST_CASE("Testing MultivariateIntPolynomial::__hash__() and compare", "[MultivariateIntPolynomial]"){
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::from_dict({x,y}, {{{2,0},1_z}, {{1,1},1_z}, {{0,2},1_z}  });
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::from_dict({x,y}, {{{2,0},1_z}, {{1,1},-1_z}, {{0,2},1_z}  });
    RCP<const MultivariateIntPolynomial> p3 = MultivariateIntPolynomial::from_dict({x,y}, {{{2,0},2_z}, {{0,2},2_z}  });
    RCP<const MultivariateIntPolynomial> p4 = MultivariateIntPolynomial::from_dict({a,b}, {{{2,0},2_z}, {{0,2},2_z}  });

    //Only requre that the same polynomial hash to the same value and that different polynomials hash to different values
    //Don't want to require a polynomial to have a particular hash in case someone comes up with a better hash function
    REQUIRE(p3->__hash__() == add_mult_poly(*p1, *p2)->__hash__());
    REQUIRE(p1->__hash__() != p2->__hash__());
    REQUIRE(p3->__hash__() != p4->__hash__());

    //Same for compare.
    REQUIRE(0 == p3->compare(*add_mult_poly(*p1, *p2)));
    REQUIRE(0 != p1->compare(*p2));
    REQUIRE(0 != p3->compare(*p4));
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
    RCP<const MultivariateIntPolynomial> p = MultivariateIntPolynomial::from_dict( {x,y,z}, { {{2,0,0},1_z}, {{0,2,0},2_z}, {{0,0,2},3_z}, {{1,1,1},4_z}, {{1,1,0},1_z}, {{0,1,1},2_z}, {{1,0,0},1_z}, {{0,1,0},2_z}, {{0,0,1}, 3_z} , {{0,0,0},5_z} });
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m1 = {{x,1_z}, {y,2_z}, {z,5_z}};
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m2 = {{x,0_z}, {y,0_z}, {z,0_z}};
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m3 = {{x,-1_z}, {y,-2_z}, {z,-5_z}};

    REQUIRE(171_z == p->eval(m1));
    REQUIRE(5_z == p->eval(m2));
    REQUIRE(51_z == p->eval(m3));
}

TEST_CASE("Testing MultivariateIntPolynomial neg_mult_poly", "[MultivariateIntPolynomial]"){
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p = MultivariateIntPolynomial::from_dict( {x,y,z}, { {{1,0,0}, 1_z}, {{0,1,0},-2_z}, {{0,0,1}, 3_z }  });
    RCP<const MultivariateIntPolynomial> p2 = neg_mult_poly(*p);
    REQUIRE(p2->__str__() == "- x + 2*y - 3*z");
}

TEST_CASE("Testing addition, subtraction, multiplication of MultivariateIntPolynomials with the same set of variables", "[MultivariateIntPolynomial]"){
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::from_dict({x,y,z}, {{{1,2,3},1_z}, {{3,2,1},2_z}, {{4,1,0},3_z },{{0,0,0},4_z} });
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::from_dict({x,y,z}, {{{1,2,3},1_z},{{3,2,1},-2_z}, {{0,1,2},1_z}, {{0,0,0},3_z}});

    REQUIRE(add_mult_poly(*p1,*p2)->__str__() == "2*x y**2 z**3 + 3*x**4 y + y z**2 + 7");
    REQUIRE(add_mult_poly(*p2,*p1)->__str__() == "2*x y**2 z**3 + 3*x**4 y + y z**2 + 7");
    REQUIRE(sub_mult_poly(*p1,*p2)->__str__() == "4*x**3 y**2 z + 3*x**4 y - y z**2 + 1");
    REQUIRE(mul_mult_poly(*p1,*p2)->__str__() == "- 4*x**6 y**4 z**2 + x**2 y**4 z**6 - 6*x**7 y**3 z + 3*x**5 y**3 z**3 + 2*x**3 y**3 z**3 + x y**3 z**5 + 3*x**4 y**2 z**2 - 2*x**3 y**2 z + 7*x y**2 z**3 + 9*x**4 y + 4*y z**2 + 12");
    REQUIRE(mul_mult_poly(*p2,*p1)->__str__() == "- 4*x**6 y**4 z**2 + x**2 y**4 z**6 - 6*x**7 y**3 z + 3*x**5 y**3 z**3 + 2*x**3 y**3 z**3 + x y**3 z**5 + 3*x**4 y**2 z**2 - 2*x**3 y**2 z + 7*x y**2 z**3 + 9*x**4 y + 4*y z**2 + 12");
}

TEST_CASE("Testing addition, subtraction, multiplication of MultivariteIntPolynomials with disjoint sets of varables", "[MultivariateIntPolynomial]"){
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::from_dict({a,b,c}, {{{1,2,3},1_z}, {{3,2,1},2_z}, {{4,1,0},3_z },{{0,0,0},4_z} });
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::from_dict({x,y,z}, {{{1,2,3},1_z},{{3,2,1},-2_z}, {{0,1,2},1_z}, {{0,0,0},3_z}});

    REQUIRE(add_mult_poly(*p1,*p2)->__str__() == "2*a**3 b**2 c + a b**2 c**3 - 2*x**3 y**2 z + x y**2 z**3 + 3*a**4 b + y z**2 + 7");
    REQUIRE(add_mult_poly(*p2,*p1)->__str__() == "2*a**3 b**2 c + a b**2 c**3 - 2*x**3 y**2 z + x y**2 z**3 + 3*a**4 b + y z**2 + 7");
    REQUIRE(sub_mult_poly(*p1,*p2)->__str__() == "2*a**3 b**2 c + a b**2 c**3 + 2*x**3 y**2 z - x y**2 z**3 + 3*a**4 b - y z**2 + 1");
    REQUIRE(sub_mult_poly(*p2,*p1)->__str__() == "- 2*a**3 b**2 c - a b**2 c**3 - 2*x**3 y**2 z + x y**2 z**3 - 3*a**4 b + y z**2 - 1");
    REQUIRE(mul_mult_poly(*p1,*p2)->__str__() == "- 4*a**3 b**2 c x**3 y**2 z + 2*a**3 b**2 c x y**2 z**3 - 2*a b**2 c**3 x**3 y**2 z + a b**2 c**3 x y**2 z**3 - 6*a**4 b x**3 y**2 z + 3*a**4 b x y**2 z**3 + 2*a**3 b**2 c y z**2 + a b**2 c**3 y z**2 + 3*a**4 b y z**2 + 6*a**3 b**2 c + 3*a b**2 c**3 - 8*x**3 y**2 z + 4*x y**2 z**3 + 9*a**4 b + 4*y z**2 + 12");
    REQUIRE(mul_mult_poly(*p2,*p1)->__str__() == "- 4*a**3 b**2 c x**3 y**2 z + 2*a**3 b**2 c x y**2 z**3 - 2*a b**2 c**3 x**3 y**2 z + a b**2 c**3 x y**2 z**3 - 6*a**4 b x**3 y**2 z + 3*a**4 b x y**2 z**3 + 2*a**3 b**2 c y z**2 + a b**2 c**3 y z**2 + 3*a**4 b y z**2 + 6*a**3 b**2 c + 3*a b**2 c**3 - 8*x**3 y**2 z + 4*x y**2 z**3 + 9*a**4 b + 4*y z**2 + 12");
}

TEST_CASE("Testing addition, subtraction, multiplication of MultivariateIntPolynomials with an overlapping set of variables", "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::from_dict({x,y}, { {{1,2},1_z}, {{4,0},3_z},{{0,3},4_z} });
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::from_dict({y,z}, { {{2,1},-2_z}, {{0,2},1_z}, {{1,0},3_z} });

    REQUIRE(add_mult_poly(*p1,*p2)->__str__() == "3*x**4 + x y**2 + 4*y**3 - 2*y**2 z + z**2 + 3*y");
    REQUIRE(add_mult_poly(*p2,*p1)->__str__() == "3*x**4 + x y**2 + 4*y**3 - 2*y**2 z + z**2 + 3*y");
    REQUIRE(sub_mult_poly(*p1,*p2)->__str__() == "3*x**4 + x y**2 + 4*y**3 + 2*y**2 z - z**2 - 3*y");
    REQUIRE(sub_mult_poly(*p2,*p1)->__str__() == "- 3*x**4 - x y**2 - 4*y**3 - 2*y**2 z + z**2 + 3*y");
    REQUIRE(mul_mult_poly(*p1,*p2)->__str__() == "- 6*x**4 y**2 z + 3*x**4 z**2 - 2*x y**4 z - 8*y**5 z + 9*x**4 y + x y**2 z**2 + 4*y**3 z**2 + 3*x y**3 + 12*y**4");
    REQUIRE(mul_mult_poly(*p2,*p1)->__str__() == "- 6*x**4 y**2 z + 3*x**4 z**2 - 2*x y**4 z - 8*y**5 z + 9*x**4 y + x y**2 z**2 + 4*y**3 z**2 + 3*x y**3 + 12*y**4");
}

TEST_CASE("Testing derivative of MultivariateIntPolynomial", "[MultivariateIntPolynomial]"){
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p = MultivariateIntPolynomial::from_dict({x,y}, { {{2,1},3_z}, {{1,2},2_z}, {{2,0},3_z}, {{0,2},2_z}, {{1,0}, 3_z}, {{0,1},2_z}, {{0,2},2_z}, {{0,0},5_z}  });
    REQUIRE(p->diff(x)->__str__() == "6*x y + 2*y**2 + 6*x + 3");
    REQUIRE(p->diff(y)->__str__() == "3*x**2 + 4*x y + 4*y + 2");
    REQUIRE(p->diff(z)->__str__() == "0");
}

TEST_CASE("Testing addition, subtraction, multiplication of MultivariteIntPolynomials with a UnivariateIntPolynomial whose variable are in the variable set", "[MultivariateIntPolynomial][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::from_dict({x,y,z}, { {{1,2,3}, 1_z}, {{3,2,1}, -2_z}, {{0,1,2}, 1_z}, {{0,0,0}, 3_z}, {{2,0,0}, 2_z}, {{1,0,0},1_z} });
    RCP<const UnivariateIntPolynomial> p2 = univariate_int_polynomial(x, {{1, 1_z}, {2, 1_z}});
    RCP<const UnivariateIntPolynomial> p3 = univariate_int_polynomial(y, {{1, 1_z}, {2, 1_z}});
    REQUIRE(add_mult_poly(*p1,*p2)->__str__() == "- 2*x**3 y**2 z + x y**2 z**3 + y z**2 + 3*x**2 + 2*x + 3");
    REQUIRE(add_mult_poly(*p2,*p1)->__str__() == "- 2*x**3 y**2 z + x y**2 z**3 + y z**2 + 3*x**2 + 2*x + 3");
    REQUIRE(sub_mult_poly(*p1,*p2)->__str__() == "- 2*x**3 y**2 z + x y**2 z**3 + y z**2 + x**2 + 3");
    REQUIRE(sub_mult_poly(*p2,*p1)->__str__() == "2*x**3 y**2 z - x y**2 z**3 - y z**2 - x**2 - 3");
    REQUIRE(mul_mult_poly(*p1,*p2)->__str__() == "- 2*x**5 y**2 z + x**3 y**2 z**3 - 2*x**4 y**2 z + x**2 y**2 z**3 + x**2 y z**2 + 2*x**4 + x y z**2 + 3*x**3 + 4*x**2 + 3*x");
    REQUIRE(mul_mult_poly(*p2,*p1)->__str__() == "- 2*x**5 y**2 z + x**3 y**2 z**3 - 2*x**4 y**2 z + x**2 y**2 z**3 + x**2 y z**2 + 2*x**4 + x y z**2 + 3*x**3 + 4*x**2 + 3*x");

    REQUIRE(add_mult_poly(*p1,*p3)->__str__() == "- 2*x**3 y**2 z + x y**2 z**3 + y z**2 + 2*x**2 + y**2 + x + y + 3");
}

TEST_CASE("Testing addition, subtraction, multiplication of MultivariteIntPolynomials with a UnivariateIntPolynomial whose variables are not in the variable set", "[MultivariateIntPolynomial][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::from_dict({x,y}, { {{1,2}, 1_z}, {{2,1}, -2_z}, {{0,1}, 1_z}, {{0,0}, 3_z} });
    RCP<const UnivariateIntPolynomial> p2 = univariate_int_polynomial(z, {{1, 1_z}, {2, 1_z}});
    REQUIRE(add_mult_poly(*p1,*p2)->__str__() == "- 2*x**2 y + x y**2 + z**2 + y + z + 3");
    REQUIRE(sub_mult_poly(*p1,*p2)->__str__() == "- 2*x**2 y + x y**2 - z**2 + y - z + 3");
    REQUIRE(sub_mult_poly(*p2,*p1)->__str__() == "2*x**2 y - x y**2 + z**2 - y + z - 3");
    REQUIRE(mul_mult_poly(*p1,*p2)->__str__() == "- 2*x**2 y z**2 + x y**2 z**2 - 2*x**2 y z + x y**2 z + y z**2 + y z + 3*z**2 + 3*z");	
}

TEST_CASE("Testing addition, subtraction, multiplication of two UnivariateIntPolynomials with different variables", "[MultivariateIntPolynomial][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UnivariateIntPolynomial> p1 = univariate_int_polynomial(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UnivariateIntPolynomial> p2 = univariate_int_polynomial(y, {{0, 1_z}, {1, 1_z}});
    REQUIRE(add_mult_poly(*p1,*p2)->__str__() == "3*x**2 - x + y + 1");
    REQUIRE(sub_mult_poly(*p1,*p2)->__str__() == "3*x**2 - x - y - 1");
    REQUIRE(sub_mult_poly(*p2,*p1)->__str__() == "- 3*x**2 + x + y + 1");
    REQUIRE(mul_mult_poly(*p1,*p2)->__str__() == "3*x**2 y + 3*x**2 - x y - x");
    REQUIRE(mul_mult_poly(*p2,*p1)->__str__() == "3*x**2 y + 3*x**2 - x y - x");
}

TEST_CASE("Testing addition, subtraction, multiplication of two UnivariateIntPolynomials with the same variables", "[MultivariateIntPolynomial][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariateIntPolynomial> p1 = univariate_int_polynomial(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UnivariateIntPolynomial> p2 = univariate_int_polynomial(x, {{0, 1_z}, {1, 1_z}});
    REQUIRE(add_mult_poly(*p1,*p2)->__str__() == "3*x**2 + 1");
    REQUIRE(sub_mult_poly(*p1,*p2)->__str__() == "3*x**2 - 2*x - 1");
    REQUIRE(sub_mult_poly(*p2,*p1)->__str__() == "- 3*x**2 + 2*x + 1");
    REQUIRE(mul_mult_poly(*p1,*p2)->__str__() == "3*x**3 + 2*x**2 - x");	
}

TEST_CASE("Testing MultivariateIntPolynomial::get_args()"){
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p = MultivariateIntPolynomial::from_dict({x,y,z},{ {{0,0,0},1_z}, {{1,1,1},2_z}, {{0,0,2},1_z} });
    REQUIRE(vec_basic_eq_perm(p->get_args(), {mul(integer(2),mul(x,mul(y,z))),pow(z,integer(2)), one}));
}
