#include <cmath>
#include <iostream>

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "visitor.h"
#include "eval_double.h"

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Symbol;
using CSymPy::symbol;
using CSymPy::umap_basic_num;
using CSymPy::map_basic_basic;
using CSymPy::Integer;
using CSymPy::integer;
using CSymPy::Rational;
using CSymPy::one;
using CSymPy::zero;
using CSymPy::Number;
using CSymPy::pow;
using CSymPy::RCP;
using CSymPy::print_stack_on_segfault;
using CSymPy::Complex;
using CSymPy::has_symbol;

void test_symbol_hash()
{
    RCP<const Symbol> x  = rcp(new Symbol("x"));
    RCP<const Symbol> x2 = rcp(new Symbol("x"));
    RCP<const Symbol> y  = rcp(new Symbol("y"));

    assert(x->__eq__(*x));
    assert(x->__eq__(*x2));
    assert(!(x->__eq__(*y)));
    assert(x->__neq__(*y));

    std::hash<Basic> hash_fn;
    // Hashes of x and x2 must be the same:
    assert(hash_fn(*x) == hash_fn(*x2));
    // Hashes of x and y can but don't have to be different:
    if (hash_fn(*x) != hash_fn(*y)) assert(x->__neq__(*y));


    std::size_t seed1 = 0;
    hash_combine<std::string>(seed1, "x");
    hash_combine<std::string>(seed1, "y");

    std::size_t seed2 = 0;
    hash_combine<Basic>(seed2, *x);
    hash_combine<Basic>(seed2, *y);

    // This checks that the Symbols are hashed by their strings:
    assert(seed1 == seed2);
}

void test_symbol_dict()
{
    umap_basic_num d;
    RCP<const Basic> x  = rcp(new Symbol("x"));
    RCP<const Basic> x2 = rcp(new Symbol("x"));
    RCP<const Basic> y  = rcp(new Symbol("y"));
    assert( x !=  x2);  // The instances are different...
    assert(eq(x, x2));  // ...but equal in the SymPy sense

    insert(d, x, rcp(new Integer(2)));
    insert(d, y, rcp(new Integer(3)));

    // Test printing:
    std::cout << d << std::endl;
    std::cout << *x << std::endl;
}

void test_add()
{
    umap_basic_num m, m2;
    RCP<const Basic> x  = rcp(new Symbol("x"));
    RCP<const Basic> y  = rcp(new Symbol("y"));
    insert(m, x, rcp(new Integer(2)));
    insert(m, y, rcp(new Integer(3)));

    m2 = m;
    RCP<const Add> a = rcp(new Add(zero, std::move(m2)));
    insert(m, x, rcp(new Integer(-2)));
    RCP<const Add> b = rcp(new Add(zero, std::move(m)));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<const Basic> r = add(add(x, y), add(y, x));
    std::cout << *r << std::endl;

    r = add(x, x);
    std::cout << *r << std::endl;

    r = add(add(x, x), y);
    std::cout << *r << std::endl;
    std::cout << "----------------------" << std::endl;

    assert(vec_basic_eq_perm(r->get_args(), {mul(integer(2), x), y}));
    assert(!vec_basic_eq_perm(r->get_args(), {mul(integer(3), x), y}));

    r = add(mul(integer(5), x), integer(5));
    assert(vec_basic_eq_perm(r->get_args(), {mul(integer(5), x), integer(5)}));

    r = add(add(mul(mul(integer(2), x), y), integer(5)), pow(x, integer(2)));
    assert(vec_basic_eq_perm(r->get_args(),
                {integer(5), mul(mul(integer(2), x), y), pow(x, integer(2))}));
    std::cout << *r << std::endl;
}

void test_integer()
{
    RCP<const Integer> i = rcp(new Integer(5));
    RCP<const Integer> j = rcp(new Integer(6));
    std::cout << *i << std::endl;
    std::cout << *j << std::endl;

    RCP<const Number> k = addnum(i, j);
    std::cout << *k << std::endl;
    assert(eq(k, rcp(new Integer(11))));
    assert(neq(k, rcp(new Integer(12))));

    k = subnum(i, j);
    std::cout << *k << std::endl;
    assert(eq(k, rcp(new Integer(-1))));
    assert(neq(k, rcp(new Integer(12))));

    k = mulnum(i, j);
    std::cout << *k << std::endl;
    assert(eq(k, rcp(new Integer(30))));
    assert(neq(k, rcp(new Integer(12))));

    k = divnum(i, j);
    assert(eq(k, Rational::from_two_ints(integer(5), integer(6))));
    std::cout << *k << std::endl;

    k = pownum(i, j);
    assert(eq(k, integer(15625)));
    std::cout << *k << std::endl;

    k = pownum(i, j->neg());
    assert(eq(k, Rational::from_two_ints(integer(1), integer(15625))));
    std::cout << *k << std::endl;

    k = i->neg();
    std::cout << *k << std::endl;
    assert(eq(k, rcp(new Integer(-5))));
    assert(neq(k, rcp(new Integer(12))));

    CSYMPY_CHECK_THROW(divnum(i, zero), std::runtime_error)
}

void test_rational()
{
    RCP<const Number> r1, r2, r3;
    r1 = Rational::from_two_ints(integer(5), integer(6));
    std::cout << *r1 << std::endl;
    assert(eq(r1, Rational::from_two_ints(integer(5), integer(6))));
    assert(neq(r1, Rational::from_two_ints(integer(5), integer(7))));

    r1 = Rational::from_two_ints(integer(2), integer(4));
    r2 = Rational::from_two_ints(integer(1), integer(2));
    assert(eq(r1, r2));

    r1 = Rational::from_two_ints(integer(-2), integer(3));
    r2 = Rational::from_two_ints(integer(2), integer(-3));
    assert(eq(r1, r2));

    r1 = Rational::from_two_ints(integer(4), integer(2));
    r2 = integer(2);
    assert(eq(r1, r2));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(10), integer(21));
    assert(eq(mulnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = Rational::from_two_ints(integer(1), integer(2));
    r3 = Rational::from_two_ints(integer(1), integer(3));
    assert(eq(mulnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = Rational::from_two_ints(integer(9), integer(2));
    r3 = integer(3);
    assert(eq(mulnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(1), integer(2));
    r2 = integer(1);
    assert(eq(addnum(r1, r1), r2));

    r1 = Rational::from_two_ints(integer(1), integer(2));
    r2 = Rational::from_two_ints(integer(1), integer(3));
    r3 = Rational::from_two_ints(integer(1), integer(6));
    assert(eq(subnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(1), integer(6));
    r2 = Rational::from_two_ints(integer(1), integer(3));
    r3 = Rational::from_two_ints(integer(1), integer(2));
    assert(eq(divnum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(2);
    r3 = Rational::from_two_ints(integer(4), integer(9));
    assert(eq(pownum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(-2);
    r3 = Rational::from_two_ints(integer(9), integer(4));
    assert(eq(pownum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = Rational::from_two_ints(integer(8), integer(27));
    assert(eq(pownum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(-3);
    r3 = Rational::from_two_ints(integer(27), integer(8));
    assert(eq(pownum(r1, r2), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = integer(2);
    assert(eq(mulnum(r1, r2), r3));
    assert(eq(mulnum(r2, r1), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = Rational::from_two_ints(integer(11), integer(3));
    assert(eq(addnum(r1, r2), r3));
    assert(eq(addnum(r2, r1), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = Rational::from_two_ints(integer(-7), integer(3));
    assert(eq(subnum(r1, r2), r3));
    r3 = Rational::from_two_ints(integer(7), integer(3));
    assert(eq(subnum(r2, r1), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = integer(3);
    r3 = Rational::from_two_ints(integer(2), integer(9));
    assert(eq(divnum(r1, r2), r3));
    r3 = Rational::from_two_ints(integer(9), integer(2));
    assert(eq(divnum(r2, r1), r3));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = zero;
    CSYMPY_CHECK_THROW(divnum(r1, r2), std::runtime_error)
}

void test_mul()
{
    map_basic_basic m, m2;
    RCP<const Basic> x  = rcp(new Symbol("x"));
    RCP<const Basic> y  = rcp(new Symbol("y"));
    insert(m, x, rcp(new Integer(2)));
    insert(m, y, rcp(new Integer(3)));

    m2 = m;
    RCP<const Mul> a = rcp(new Mul(one, std::move(m2)));
    insert(m, x, rcp(new Integer(-2)));
    RCP<const Mul> b = rcp(new Mul(one, std::move(m)));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<const Basic> r = mul(mul(x, y), mul(y, x));
    std::cout << *r << std::endl;

    assert(vec_basic_eq_perm(r->get_args(),
                {pow(x, integer(2)), pow(y, integer(2))}));

    r = mul(mul(pow(x, integer(3)), integer(2)), y);
    assert(vec_basic_eq_perm(r->get_args(),
                {integer(2), pow(x, integer(3)), y}));

    r = add(x, x);
    assert(vec_basic_eq_perm(r->get_args(), {x, integer(2)}));

    r = sub(x, x);
    assert(vec_basic_eq(r->get_args(), {}));

    r = mul(x, x);
    assert(vec_basic_eq(r->get_args(), {x, integer(2)}));

    r = div(x, x);
    assert(vec_basic_eq(r->get_args(), {}));
}

void test_diff()
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
    assert(eq(r2, zero));

    r1 = Rational::from_two_ints(integer(2), integer(3));
    r2 = r1->diff(x);
    assert(eq(r2, zero));

    r1 = pow(x, i3)->diff(x);
    r2 = mul(i3, pow(x, i2));
    assert(eq(r1, r2));

    r1 = pow(add(x, y), i2)->diff(x);
    r2 = mul(i2, add(x, y));
    assert(eq(r1, r2));

    r1 = add(add(i2, mul(i3, x)), mul(i5, pow(x, i2)));
    assert(eq(r1->diff(x), add(i3, mul(i10, x))));
    assert(eq(r1->diff(x)->diff(x), i10));

    r1 = add(mul(mul(pow(x, y), pow(y, x)), i2), one)->diff(x);
    r2 = add(mul(i2, mul(pow(x, y), mul(pow(y, x), log(y)))), mul(i2, mul(pow(x, y), mul(pow(y, x), div(y, x)))));
    assert(eq(r1, r2));
}

void test_compare()
{
    RCP<const Basic> r1, r2;
    RCP<const Symbol> x  = symbol("x");
    RCP<const Symbol> y  = symbol("y");
    RCP<const Symbol> z  = symbol("z");
    RCP<const Basic> i2  = integer(2);
    RCP<const Basic> im2  = integer(-2);
    RCP<const Basic> i3  = integer(3);
    assert(x->compare(*x) == 0);
    assert(x->compare(*y) == -1);
    assert(x->compare(*z) == -1);
    assert(y->compare(*x) == 1);
    assert(y->compare(*z) == -1);
    assert(z->compare(*x) == 1);
    assert(z->compare(*y) == 1);

    assert(i2->compare(*i2) == 0);
    assert(i2->compare(*i3) == -1);
    assert(i3->compare(*i2) == 1);

    r1 = mul(x, y);
    r2 = mul(x, y);
    assert(r1->compare(*r2) == 0);
    assert(r2->compare(*r1) == 0);

    r1 = mul(x, y);
    r2 = mul(x, z);
    assert(r1->compare(*r2) == -1);
    assert(r2->compare(*r1) == 1);

    r1 = mul(y, x);
    r2 = mul(x, z);
    assert(r1->compare(*r2) == -1);
    assert(r2->compare(*r1) == 1);

    r1 = mul(mul(y, x), z);
    r2 = mul(x, z);
    assert(r1->compare(*r2) == 1);
    assert(r2->compare(*r1) == -1);

    r1 = add(add(y, x), z);
    r2 = add(x, z);
    assert(r1->compare(*r2) == 1);
    assert(r2->compare(*r1) == -1);

    r1 = pow(x, z);
    r2 = pow(y, x);
    assert(r1->compare(*r2) == -1);
    assert(r2->compare(*r1) == 1);

    r1 = pow(x, z);
    r2 = pow(x, x);
    assert(r1->compare(*r2) == 1);
    assert(r2->compare(*r1) == -1);

    r1 = add(add(x, y), z);
    r2 = add(x, y);
    assert(r1->compare(*r2) == 1);
    assert(r2->compare(*r1) == -1);

    r1 = add(add(x, y), i2);
    r2 = add(x, y);
    assert(r1->compare(*r2) == 1);
    assert(r2->compare(*r1) == -1);

    r1 = add(add(x, y), im2);
    r2 = add(x, y);
    assert(r1->compare(*r2) == -1);
    assert(r2->compare(*r1) == 1);

    r1 = add(x, y);
    r2 = add(x, z);
    assert(r1->compare(*r2) == -1);
    assert(r2->compare(*r1) == 1);

    r1 = add(x, y);
    r2 = add(x, y);
    assert(r1->compare(*r2) == 0);
    assert(r2->compare(*r1) == 0);

    r1 = add(add(x, y), z);
    r2 = add(add(x, z), y);
    assert(r1->compare(*r2) == 0);
    assert(r2->compare(*r1) == 0);

    r1 = sin(x);
    r2 = sin(y);
    assert(r1->compare(*r2) == -1);
    assert(r2->compare(*r1) == 1);
    assert(r1->compare(*r1) == 0);

    // These are specific to the order in the declaration of enum TypeID,
    // so we just make sure that if x < y, then y > x.
    r1 = add(x, z);
    r2 = mul(x, y);
    int cmp = r1->__cmp__(*r2);
    assert(cmp != 0);
    assert(r2->__cmp__(*r1) == -cmp);

    r1 = mul(x, pow(z, x));
    r2 = mul(x, y);
    cmp = r1->__cmp__(*r2);
    assert(cmp != 0);
    assert(r2->__cmp__(*r1) == -cmp);

    r1 = mul(x, pow(z, x));
    r2 = mul(x, z);
    cmp = r1->__cmp__(*r2);
    assert(cmp != 0);
    assert(r2->__cmp__(*r1) == -cmp);

    r1 = pow(z, x);
    r2 = pow(z, pow(x, y));
    cmp = r1->__cmp__(*r2);
    assert(cmp != 0);
    assert(r2->__cmp__(*r1) == -cmp);

    r1 = div(mul(x, y), i2);
    r2 = mul(x, y);
    cmp = r1->__cmp__(*r2);
    assert(cmp != 0);
    assert(r2->__cmp__(*r1) == -cmp);

    r1 = add(x, pow(z, x));
    r2 = add(x, y);
    cmp = r1->__cmp__(*r2);
    assert(cmp != 0);
    assert(r2->__cmp__(*r1) == -cmp);
}

void test_complex()
{
    RCP<const Number> r1, r2, r3, c1, c2, c3;
    r1 = Rational::from_two_ints(integer(2), integer(4));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(-5), integer(7));

    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);

    // Basic check for equality in Complex::from_two_nums and Complex::from_two_rats
    assert(eq(c1, Complex::from_two_rats(static_cast<const Rational&>(*r1), static_cast<const Rational&>(*r2))));
    assert(neq(c2, Complex::from_two_rats(static_cast<const Rational&>(*r1), static_cast<const Rational&>(*r2))));

    // Checks for complex addition
    // Final result is int
    assert(eq(addnum(c1, c2), one));
    // Final result is complex
    r2 = Rational::from_two_ints(integer(1), integer(1));
    r3 = Rational::from_two_ints(integer(10), integer(7));
    c3 = Complex::from_two_nums(*r2, *r3);
    assert(eq(addnum(c1, c1), c3));
    // Final result is rational
    r1 = Rational::from_two_ints(integer(1), integer(4));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(-5), integer(7));
    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    assert(eq(addnum(c1, c2), div(one, integer(2))));

    // Checks for complex subtraction
    r1 = Rational::from_two_ints(integer(2), integer(4));
    r2 = Rational::from_two_ints(integer(5), integer(7));
    r3 = Rational::from_two_ints(integer(-5), integer(7));

    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    // Final result is int
    assert(eq(subnum(c1, c1), zero));

    // Final result is rational
    r3 = Rational::from_two_ints(integer(1), integer(3));
    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r3, *r2);
    assert(eq(subnum(c1, c2), div(one, integer(6))));

    // Final result is complex
    r2 = Rational::from_two_ints(integer(1), integer(6));
    c1 = Complex::from_two_nums(*r1, *r1);
    c2 = Complex::from_two_nums(*r3, *r3);
    c3 = Complex::from_two_nums(*r2, *r2);
    assert(eq(subnum(c1, c2), c3));

    // Checks for complex multiplication
    r1 = Rational::from_two_ints(integer(2), integer(1));
    r2 = Rational::from_two_ints(integer(1), integer(1));
    r3 = Rational::from_two_ints(integer(-1), integer(1));
    // Final result is int
    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    assert(eq(mulnum(c1, c2), integer(5)));

    // Final result is rational
    r1 = Rational::from_two_ints(integer(1), integer(2));
    c1 = Complex::from_two_nums(*r1, *r2);
    c2 = Complex::from_two_nums(*r1, *r3);
    assert(eq(mulnum(c1, c2), div(integer(5), integer(4))));

    // Final result is complex
    c1 = Complex::from_two_nums(*r2, *r2);
    c2 = Complex::from_two_nums(*r3, *r3);
    c3 = Complex::from_two_nums(*(integer(0)), *(integer(-2)));
    assert(eq(mulnum(c1, c2), c3));

    // Check for complex division
    // Final result is complex
    c1 = Complex::from_two_nums(*r2, *r2);
    c2 = Complex::from_two_nums(*r2, *r3);
    c3 = Complex::from_two_nums(*(integer(0)), *(integer(1)));
    assert(eq(divnum(c1, c2), c3));

    // Final result is integer
    c1 = Complex::from_two_nums(*r2, *r2);
    c2 = Complex::from_two_nums(*r2, *r2);
    assert(eq(divnum(c1, c2), integer(1)));

    // Final result is rational
    r3 = Rational::from_two_ints(integer(2), integer(1));
    c1 = Complex::from_two_nums(*r2, *r2);
    c2 = Complex::from_two_nums(*r3, *r3);
    assert(eq(divnum(c1, c2), div(integer(1), integer(2))));

    r1 = Rational::from_two_ints(integer(1), integer(2));
    r2 = Rational::from_two_ints(integer(3), integer(4));
    c1 = Complex::from_two_nums(*r1, *r2);

    r1 = Rational::from_two_ints(integer(5), integer(6));
    r2 = Rational::from_two_ints(integer(7), integer(8));
    c2 = Complex::from_two_nums(*r1, *r2);

    r1 = Rational::from_two_ints(integer(618), integer(841));
    r2 = Rational::from_two_ints(integer(108), integer(841));
    c3 = Complex::from_two_nums(*r1, *r2);
    assert(eq(divnum(c1, c2), c3));

    r1 = Rational::from_two_ints(integer(-23), integer(96));
    r2 = Rational::from_two_ints(integer(17), integer(16));
    c3 = Complex::from_two_nums(*r1, *r2);
    assert(eq(mulnum(c1, c2), c3));

    r1 = Rational::from_two_ints(integer(4), integer(3));
    r2 = Rational::from_two_ints(integer(13), integer(8));
    c3 = Complex::from_two_nums(*r1, *r2);
    assert(eq(addnum(c1, c2), c3));

    r1 = Rational::from_two_ints(integer(-1), integer(3));
    r2 = Rational::from_two_ints(integer(-1), integer(8));
    c3 = Complex::from_two_nums(*r1, *r2);
    assert(eq(subnum(c1, c2), c3));

    // Explicit division by zero checks
    CSYMPY_CHECK_THROW(divnum(c1, integer(0)), std::runtime_error);

    r3 = Rational::from_two_ints(integer(0), integer(1));
    CSYMPY_CHECK_THROW(divnum(c1, r3), std::runtime_error);

    c2 = Complex::from_two_nums(*r3, *r3);
    CSYMPY_CHECK_THROW(divnum(c1, c2), std::runtime_error);
}

void test_has()
{
    RCP<const Basic> r1;
    RCP<const Symbol> x, y, z;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");
    r1 = add(x, pow(y, integer(2)));
    assert(has_symbol(*r1, x));
    assert(has_symbol(*r1, y));
    assert(!has_symbol(*r1, z));

    r1 = sin(add(x, pow(y, integer(2))));
    assert(has_symbol(*r1, x));
    assert(has_symbol(*r1, y));
    assert(!has_symbol(*r1, z));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_symbol_hash();

    test_symbol_dict();

    test_add();

    test_integer();

    test_rational();

    test_mul();

    test_diff();

    test_compare();

    test_complex();

    test_has();

    return 0;
}
