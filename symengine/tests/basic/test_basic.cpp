#include "catch.hpp"
#include <cmath>
#include <iostream>

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/visitor.h>
#include <symengine/eval_double.h>

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::umap_basic_num;
using SymEngine::map_basic_basic;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Rational;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::Number;
using SymEngine::pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::Complex;
using SymEngine::has_symbol;
using SymEngine::is_a;
using SymEngine::rcp_static_cast;
using SymEngine::set_basic;
using SymEngine::free_symbols;
using SymEngine::function_symbol;

TEST_CASE("Symbol hash: Basic", "[basic]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> x2 = symbol("x");
    RCP<const Symbol> y  = symbol("y");

    REQUIRE(x->__eq__(*x));
    REQUIRE(x->__eq__(*x2));
    REQUIRE(!(x->__eq__(*y)));
    REQUIRE(x->__neq__(*y));

    std::hash<Basic> hash_fn;
    // Hashes of x and x2 must be the same:
    REQUIRE(hash_fn(*x) == hash_fn(*x2));
    // Hashes of x and y can but don't have to be different:
    if (hash_fn(*x) != hash_fn(*y)) REQUIRE(x->__neq__(*y));


    std::size_t seed1 = 0;
    hash_combine<std::string>(seed1, "x");
    hash_combine<std::string>(seed1, "y");

    std::size_t seed2 = 0;
    hash_combine<Basic>(seed2, *x);
    hash_combine<Basic>(seed2, *y);

    // This checks that the Symbols are hashed by their strings:
    REQUIRE(seed1 == seed2);
}

TEST_CASE("Symbol dict: Basic", "[basic]")
{
    umap_basic_num d;
    RCP<const Basic> x  = symbol("x");
    RCP<const Basic> x2 = symbol("x");
    RCP<const Basic> y  = symbol("y");
    REQUIRE( x !=  x2);  // The instances are different...
    REQUIRE(eq(x, x2));  // ...but equal in the SymPy sense

    insert(d, x, integer(2));
    insert(d, y, integer(3));

    // Test printing:
    std::cout << d << std::endl;
    std::cout << *x << std::endl;
}

TEST_CASE("Add: basic", "[basic]")
{
    umap_basic_num m, m2;
    RCP<const Basic> x  = symbol("x");
    RCP<const Basic> y  = symbol("y");
    insert(m, x, integer(2));
    insert(m, y, integer(3));

    m2 = m;
    RCP<const Add> a = make_rcp<const Add>(zero, std::move(m2));
    insert(m, x, integer(-2));
    RCP<const Add> b = make_rcp<const Add>(zero, std::move(m));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<const Basic> r = add(add(x, y), add(y, x));
    std::cout << *r << std::endl;

    r = add(x, x);
    std::cout << *r << std::endl;

    r = add(add(x, x), y);
    std::cout << *r << std::endl;
    std::cout << "----------------------" << std::endl;

    REQUIRE(vec_basic_eq_perm(r->get_args(), {mul(integer(2), x), y}));
    REQUIRE(!vec_basic_eq_perm(r->get_args(), {mul(integer(3), x), y}));

    r = add(mul(integer(5), x), integer(5));
    REQUIRE(vec_basic_eq_perm(r->get_args(), {mul(integer(5), x), integer(5)}));

    r = add(add(mul(mul(integer(2), x), y), integer(5)), pow(x, integer(2)));
    REQUIRE(vec_basic_eq_perm(r->get_args(),
                {integer(5), mul(mul(integer(2), x), y), pow(x, integer(2))}));
    std::cout << *r << std::endl;
}

TEST_CASE("Integer: Basic", "[basic]")
{
    RCP<const Integer> i = integer(5);
    RCP<const Integer> j = integer(6);
    std::cout << *i << std::endl;
    std::cout << *j << std::endl;

    RCP<const Number> k = addnum(i, j);
    std::cout << *k << std::endl;
    REQUIRE(eq(k, integer(11)));
    REQUIRE(neq(k, integer(12)));

    k = subnum(i, j);
    std::cout << *k << std::endl;
    REQUIRE(eq(k, integer(-1)));
    REQUIRE(neq(k, integer(12)));

    k = mulnum(i, j);
    std::cout << *k << std::endl;
    REQUIRE(eq(k, integer(30)));
    REQUIRE(neq(k, integer(12)));

    k = divnum(i, j);
    REQUIRE(eq(k, Rational::from_two_ints(integer(5), integer(6))));
    std::cout << *k << std::endl;

    k = pownum(i, j);
    REQUIRE(eq(k, integer(15625)));
    std::cout << *k << std::endl;

    k = pownum(i, j->neg());
    REQUIRE(eq(k, Rational::from_two_ints(integer(1), integer(15625))));
    std::cout << *k << std::endl;

    k = i->neg();
    std::cout << *k << std::endl;
    REQUIRE(eq(k, integer(-5)));
    REQUIRE(neq(k, integer(12)));

    SYMENGINE_CHECK_THROW(divnum(i, zero), std::runtime_error)
}

TEST_CASE("Rational: Basic", "[basic]")
{
    RCP<const Number> r1, r2, r3;
    RCP<const Rational> r;
    mpq_class a, b;

    r1 = Rational::from_two_ints(integer(5), integer(6));
    std::cout << *r1 << std::endl;
    REQUIRE(eq(r1, Rational::from_two_ints(integer(5), integer(6))));
    REQUIRE(neq(r1, Rational::from_two_ints(integer(5), integer(7))));

    r1 = Rational::from_two_ints(integer(2), integer(4));
    r2 = Rational::from_two_ints(integer(1), integer(2));
    REQUIRE(eq(r1, r2));

    r1 = Rational::from_two_ints(integer(-2), integer(3));
    r2 = Rational::from_two_ints(integer(2), integer(-3));
    REQUIRE(eq(r1, r2));

    r1 = Rational::from_two_ints(integer(4), integer(2));
    r2 = integer(2);
    REQUIRE(eq(r1, r2));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(10), integer(21));
    REQUIRE(eq(mulnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = Rational::from_two_ints(integer(1), integer(2));
    r3 = Rational::from_two_ints(integer(1), integer(3));
    REQUIRE(eq(mulnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = Rational::from_two_ints(integer(9), integer(2));
    r3 = integer(3);
    REQUIRE(eq(mulnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(1), integer(2));
    r2 = integer(1);
    REQUIRE(eq(addnum(r1, r1), r2));

    r1 = Rational::from_two_ints(integer(1), integer(2));
    r2 = Rational::from_two_ints(integer(1), integer(3));
    r3 = Rational::from_two_ints(integer(1), integer(6));
    REQUIRE(eq(subnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(1), integer(6));
    r2 = Rational::from_two_ints(integer(1), integer(3));
    r3 = Rational::from_two_ints(integer(1), integer(2));
    REQUIRE(eq(divnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(2);
    r3 = Rational::from_two_ints(integer(4), integer(9));
    REQUIRE(eq(pownum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(-2);
    r3 = Rational::from_two_ints(integer(9), integer(4));
    REQUIRE(eq(pownum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = Rational::from_two_ints(integer(8), integer(27));
    REQUIRE(eq(pownum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(-3);
    r3 = Rational::from_two_ints(integer(27), integer(8));
    REQUIRE(eq(pownum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = integer(2);
    REQUIRE(eq(mulnum(r1, r2), r3));
    REQUIRE(eq(mulnum(r2, r1), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = Rational::from_two_ints(integer(11), integer(3));
    REQUIRE(eq(addnum(r1, r2), r3));
    REQUIRE(eq(addnum(r2, r1), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = Rational::from_two_ints(integer(-7), integer(3));
    REQUIRE(eq(subnum(r1, r2), r3));
    r3 = Rational::from_two_ints(integer(7), integer(3));
    REQUIRE(eq(subnum(r2, r1), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = Rational::from_two_ints(integer(2), integer(9));
    REQUIRE(eq(divnum(r1, r2), r3));
    r3 = Rational::from_two_ints(integer(9), integer(2));
    REQUIRE(eq(divnum(r2, r1), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = zero;
    SYMENGINE_CHECK_THROW(divnum(r1, r2), std::runtime_error)

    r1 = Rational::from_two_ints(integer(3), integer(5));
    REQUIRE(is_a<Rational>(*r1));
    r = rcp_static_cast<const Rational>(r1);
    a = mpq_class(3, 5);
    b =  r->as_mpq();
    REQUIRE(a == b);
}

TEST_CASE("Mul: Basic", "[basic]")
{
    map_basic_basic m, m2;
    RCP<const Basic> x  = symbol("x");
    RCP<const Basic> y  = symbol("y");
    insert(m, x, integer(2));
    insert(m, y, integer(3));

    m2 = m;
    RCP<const Mul> a = make_rcp<const Mul>(one, std::move(m2));
    insert(m, x, integer(-2));
    RCP<const Mul> b = make_rcp<const Mul>(one, std::move(m));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<const Basic> r = mul(mul(x, y), mul(y, x));
    std::cout << *r << std::endl;

    REQUIRE(vec_basic_eq_perm(r->get_args(),
                {pow(x, integer(2)), pow(y, integer(2))}));

    r = mul(mul(pow(x, integer(3)), integer(2)), y);
    REQUIRE(vec_basic_eq_perm(r->get_args(),
                {integer(2), pow(x, integer(3)), y}));

    r = add(x, x);
    REQUIRE(vec_basic_eq_perm(r->get_args(), {x, integer(2)}));

    r = sub(x, x);
    REQUIRE(vec_basic_eq(r->get_args(), {}));

    r = mul(x, x);
    REQUIRE(vec_basic_eq(r->get_args(), {x, integer(2)}));

    r = div(x, x);
    REQUIRE(vec_basic_eq(r->get_args(), {}));
}

TEST_CASE("Diff: Basic", "[basic]")
{
    RCP<const Basic> r1, r2;
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const Basic> i2  = integer(2);
    RCP<const Basic> i3  = integer(3);
    RCP<const Basic> i5  = integer(5);
    RCP<const Basic> i10  = integer(10);
    r1 = integer(5);
    r2 = r1->diff(x);
    REQUIRE(eq(r2, zero));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = r1->diff(x);
    REQUIRE(eq(r2, zero));

    r1 = pow(x, i3)->diff(x);
    r2 = mul(i3, pow(x, i2));
    REQUIRE(eq(r1, r2));

    r1 = pow(add(x, y), i2)->diff(x);
    r2 = mul(i2, add(x, y));
    REQUIRE(eq(r1, r2));

    r1 = add(add(i2, mul(i3, x)), mul(i5, pow(x, i2)));
    REQUIRE(eq(r1->diff(x), add(i3, mul(i10, x))));
    REQUIRE(eq(r1->diff(x)->diff(x), i10));

    r1 = add(mul(mul(pow(x, y), pow(y, x)), i2), one)->diff(x);
    r2 = add(mul(i2, mul(pow(x, y), mul(pow(y, x), log(y)))), mul(i2, mul(pow(x, y), mul(pow(y, x), div(y, x)))));
    REQUIRE(eq(r1, r2));
}

TEST_CASE("compare: Basic", "[basic]")
{
    RCP<const Basic> r1, r2;
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const Symbol> z  = symbol("z");
    RCP<const Basic> i2  = integer(2);
    RCP<const Basic> im2  = integer(-2);
    RCP<const Basic> i3  = integer(3);
    REQUIRE(x->compare(*x) == 0);
    REQUIRE(x->compare(*y) == -1);
    REQUIRE(x->compare(*z) == -1);
    REQUIRE(y->compare(*x) == 1);
    REQUIRE(y->compare(*z) == -1);
    REQUIRE(z->compare(*x) == 1);
    REQUIRE(z->compare(*y) == 1);

    REQUIRE(i2->compare(*i2) == 0);
    REQUIRE(i2->compare(*i3) == -1);
    REQUIRE(i3->compare(*i2) == 1);

    r1 = mul(x, y);
    r2 = mul(x, y);
    REQUIRE(r1->compare(*r2) == 0);
    REQUIRE(r2->compare(*r1) == 0);

    r1 = mul(x, y);
    r2 = mul(x, z);
    REQUIRE(r1->compare(*r2) == -1);
    REQUIRE(r2->compare(*r1) == 1);

    r1 = mul(y, x);
    r2 = mul(x, z);
    REQUIRE(r1->compare(*r2) == -1);
    REQUIRE(r2->compare(*r1) == 1);

    r1 = mul(mul(y, x), z);
    r2 = mul(x, z);
    REQUIRE(r1->compare(*r2) == 1);
    REQUIRE(r2->compare(*r1) == -1);

    r1 = add(add(y, x), z);
    r2 = add(x, z);
    REQUIRE(r1->compare(*r2) == 1);
    REQUIRE(r2->compare(*r1) == -1);

    r1 = pow(x, z);
    r2 = pow(y, x);
    REQUIRE(r1->compare(*r2) == -1);
    REQUIRE(r2->compare(*r1) == 1);

    r1 = pow(x, z);
    r2 = pow(x, x);
    REQUIRE(r1->compare(*r2) == 1);
    REQUIRE(r2->compare(*r1) == -1);

    r1 = add(add(x, y), z);
    r2 = add(x, y);
    REQUIRE(r1->compare(*r2) == 1);
    REQUIRE(r2->compare(*r1) == -1);

    r1 = add(add(x, y), i2);
    r2 = add(x, y);
    REQUIRE(r1->compare(*r2) == 1);
    REQUIRE(r2->compare(*r1) == -1);

    r1 = add(add(x, y), im2);
    r2 = add(x, y);
    REQUIRE(r1->compare(*r2) == -1);
    REQUIRE(r2->compare(*r1) == 1);

    r1 = add(x, y);
    r2 = add(x, z);
    REQUIRE(r1->compare(*r2) == -1);
    REQUIRE(r2->compare(*r1) == 1);

    r1 = add(x, y);
    r2 = add(x, y);
    REQUIRE(r1->compare(*r2) == 0);
    REQUIRE(r2->compare(*r1) == 0);

    r1 = add(add(x, y), z);
    r2 = add(add(x, z), y);
    REQUIRE(r1->compare(*r2) == 0);
    REQUIRE(r2->compare(*r1) == 0);

    r1 = sin(x);
    r2 = sin(y);
    REQUIRE(r1->compare(*r2) == -1);
    REQUIRE(r2->compare(*r1) == 1);
    REQUIRE(r1->compare(*r1) == 0);

    // These are specific to the order in the declaration of enum TypeID,
    // so we just make sure that if x < y, then y > x.
    r1 = add(x, z);
    r2 = mul(x, y);
    int cmp = r1->__cmp__(*r2);
    REQUIRE(cmp != 0);
    REQUIRE(r2->__cmp__(*r1) == -cmp);

    r1 = mul(x, pow(z, x));
    r2 = mul(x, y);
    cmp = r1->__cmp__(*r2);
    REQUIRE(cmp != 0);
    REQUIRE(r2->__cmp__(*r1) == -cmp);

    r1 = mul(x, pow(z, x));
    r2 = mul(x, z);
    cmp = r1->__cmp__(*r2);
    REQUIRE(cmp != 0);
    REQUIRE(r2->__cmp__(*r1) == -cmp);

    r1 = pow(z, x);
    r2 = pow(z, pow(x, y));
    cmp = r1->__cmp__(*r2);
    REQUIRE(cmp != 0);
    REQUIRE(r2->__cmp__(*r1) == -cmp);

    r1 = div(mul(x, y), i2);
    r2 = mul(x, y);
    cmp = r1->__cmp__(*r2);
    REQUIRE(cmp != 0);
    REQUIRE(r2->__cmp__(*r1) == -cmp);

    r1 = add(x, pow(z, x));
    r2 = add(x, y);
    cmp = r1->__cmp__(*r2);
    REQUIRE(cmp != 0);
    REQUIRE(r2->__cmp__(*r1) == -cmp);

    r1 = log(log(x));
    r2 = log(x);
    REQUIRE(r1->__cmp__(*r2) != 0);
    REQUIRE(r1->__cmp__(*r1) == 0);
}

TEST_CASE("Complex: Basic", "[basic]")
{
    RCP<const Number> r1, r2, r3, c1, c2, c3;
    RCP<const Complex> c;
    r1 = Rational::from_two_ints(integer(2), integer(4));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(-5), integer(7));

    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);

    // Basic check for equality in Complex::from_two_nums and Complex::from_two_rats
    REQUIRE(eq(c1, Complex::from_two_rats(static_cast<const Rational&>(*r1), static_cast<const Rational&>(*r2))));
    REQUIRE(neq(c2, Complex::from_two_rats(static_cast<const Rational&>(*r1), static_cast<const Rational&>(*r2))));

    // Checks for complex addition
    // Final result is int
    REQUIRE(eq(addnum(c1, c2), one));
    // Final result is complex
    r2 = Rational::from_two_ints(integer(1), integer(1));
    r3 = Rational::from_two_ints(integer(10), integer(7));
    c3 = Complex::from_two_nums(*r2, *r3);
    REQUIRE(eq(addnum(c1, c1), c3));
    // Final result is rational
    r1 = Rational::from_two_ints(integer(1), integer(4));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(-5), integer(7));
    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    REQUIRE(eq(addnum(c1, c2), div(one, integer(2))));

    // Checks for complex subtraction
    r1 = Rational::from_two_ints(integer(2), integer(4));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(-5), integer(7));

    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    // Final result is int
    REQUIRE(eq(subnum(c1, c1), zero));

    // Final result is rational
    r3 = Rational::from_two_ints(integer(1), integer(3));
    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r3, *r2);
    REQUIRE(eq(subnum(c1, c2), div(one, integer(6))));

    // Final result is complex
    r2 = Rational::from_two_ints(integer(1), integer(6));
    c1 = Complex::from_two_nums(*r1, *r1);
    c2 = Complex::from_two_nums(*r3, *r3);
    c3 = Complex::from_two_nums(*r2, *r2);
    REQUIRE(eq(subnum(c1, c2), c3));

    // Checks for complex multiplication
    r1 = Rational::from_two_ints(integer(2), integer(1));
    r2 = Rational::from_two_ints(integer(1), integer(1));
    r3 = Rational::from_two_ints(integer(-1), integer(1));
    // Final result is int
    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    REQUIRE(eq(mulnum(c1, c2), integer(5)));

    // Final result is rational
    r1 = Rational::from_two_ints(integer(1), integer(2));
    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    REQUIRE(eq(mulnum(c1, c2), div(integer(5), integer(4))));

    // Final result is complex
    c1 = Complex::from_two_nums(*r2, *r2);
    c2 = Complex::from_two_nums(*r3, *r3);
    c3 = Complex::from_two_nums(*(integer(0)), *(integer(-2)));
    REQUIRE(eq(mulnum(c1, c2), c3));

    // Check for complex division
    // Final result is complex
    c1 = Complex::from_two_nums(*r2, *r2);
    c2 = Complex::from_two_nums(*r2, *r3);
    c3 = Complex::from_two_nums(*(integer(0)), *(integer(1)));
    REQUIRE(eq(divnum(c1, c2), c3));

    // Final result is integer
    c1 = Complex::from_two_nums(*r2, *r2);
    c2 = Complex::from_two_nums(*r2, *r2);
    REQUIRE(eq(divnum(c1, c2), integer(1)));

    // Final result is rational
    r3 = Rational::from_two_ints(integer(2), integer(1));
    c1 = Complex::from_two_nums(*r2, *r2);
    c2 = Complex::from_two_nums(*r3, *r3);
    REQUIRE(eq(divnum(c1, c2), div(integer(1), integer(2))));

    r1 = Rational::from_two_ints(integer(1), integer(2));
    r2 = Rational::from_two_ints(integer(3), integer(4));
    c1 = Complex::from_two_nums(*r1, *r2);

    r1 = Rational::from_two_ints(integer(5), integer(6));
    r2 = Rational::from_two_ints(integer(7), integer(8));
    c2 = Complex::from_two_nums(*r1, *r2);

    r1 = Rational::from_two_ints(integer(618), integer(841));
    r2 = Rational::from_two_ints(integer(108), integer(841));
    c3 = Complex::from_two_nums(*r1, *r2);
    REQUIRE(eq(divnum(c1, c2), c3));

    r1 = Rational::from_two_ints(integer(-23), integer(96));
    r2 = Rational::from_two_ints(integer(17), integer(16));
    c3 = Complex::from_two_nums(*r1, *r2);
    REQUIRE(eq(mulnum(c1, c2), c3));

    r1 = Rational::from_two_ints(integer(4), integer(3));
    r2 = Rational::from_two_ints(integer(13), integer(8));
    c3 = Complex::from_two_nums(*r1, *r2);
    REQUIRE(eq(addnum(c1, c2), c3));

    r1 = Rational::from_two_ints(integer(-1), integer(3));
    r2 = Rational::from_two_ints(integer(-1), integer(8));
    c3 = Complex::from_two_nums(*r1, *r2);
    REQUIRE(eq(subnum(c1, c2), c3));

    REQUIRE(is_a<Complex>(*c3));
    c = rcp_static_cast<const Complex>(c3);
    REQUIRE(eq(c->real_part(), r1));
    REQUIRE(eq(c->imaginary_part(), r2));

    // Explicit division by zero checks
    SYMENGINE_CHECK_THROW(divnum(c1, integer(0)), std::runtime_error);

    r3 = Rational::from_two_ints(integer(0), integer(1));
    SYMENGINE_CHECK_THROW(divnum(c1, r3), std::runtime_error);

    c2 = Complex::from_two_nums(*r3, *r3);
    SYMENGINE_CHECK_THROW(divnum(c1, c2), std::runtime_error);
}

TEST_CASE("has_symbol: Basic", "[basic]")
{
    RCP<const Basic> r1;
    RCP<const Symbol> x, y, z;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");
    r1 = add(x, pow(y, integer(2)));
    REQUIRE(has_symbol(*r1, x));
    REQUIRE(has_symbol(*r1, y));
    REQUIRE(!has_symbol(*r1, z));

    r1 = sin(add(x, pow(y, integer(2))));
    REQUIRE(has_symbol(*r1, x));
    REQUIRE(has_symbol(*r1, y));
    REQUIRE(!has_symbol(*r1, z));
}

TEST_CASE("free_symbols: Basic", "[basic]")
{
    RCP<const Basic> r1;
    RCP<const Symbol> x, y, z;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");
    r1 = add(x, add(z, pow(y, x)));

    set_basic s = free_symbols(*r1);
    REQUIRE(s.size() == 3);
    REQUIRE(s.count(x) == 1);
    REQUIRE(s.count(y) == 1);
    REQUIRE(s.count(z) == 1);
    s.clear();

    r1 = function_symbol("f", mul(x, integer(2)))->diff(x);
    s = free_symbols(*r1);
    REQUIRE(s.size() == 1);
    REQUIRE(s.count(x) == 1);
}
