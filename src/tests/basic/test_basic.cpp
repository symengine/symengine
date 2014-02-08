#include <iostream>

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "rational.h"
#include "mul.h"
#include "pow.h"

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Symbol;
using CSymPy::symbol;
using CSymPy::umap_basic_int;
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
    umap_basic_int d;
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
    umap_basic_int m;
    RCP<const Basic> x  = rcp(new Symbol("x"));
    RCP<const Basic> y  = rcp(new Symbol("y"));
    insert(m, x, rcp(new Integer(2)));
    insert(m, y, rcp(new Integer(3)));

    RCP<const Add> a = rcp(new Add(zero, m));
    insert(m, x, rcp(new Integer(-2)));
    RCP<const Add> b = rcp(new Add(zero, m));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<const Basic> r = add(add(x, y), add(y, x));
    std::cout << *r << std::endl;

    r = add(x, x);
    std::cout << *r << std::endl;

    r = add(add(x, x), y);
    std::cout << *r << std::endl;
    std::cout << "----------------------" << std::endl;
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
}

void test_mul()
{
    map_basic_basic m;
    RCP<const Basic> x  = rcp(new Symbol("x"));
    RCP<const Basic> y  = rcp(new Symbol("y"));
    insert(m, x, rcp(new Integer(2)));
    insert(m, y, rcp(new Integer(3)));

    RCP<const Mul> a = rcp(new Mul(one, m));
    insert(m, x, rcp(new Integer(-2)));
    RCP<const Mul> b = rcp(new Mul(one, m));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<const Basic> r = mul(mul(x, y), mul(y, x));
    std::cout << *r << std::endl;
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

    // These are compiler implementation specific, so we just make sure that if
    // x < y, then y > x.
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

    return 0;
}
