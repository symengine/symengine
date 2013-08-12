#include <iostream>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "rational.h"
#include "mul.h"

using Teuchos::RCP;
using Teuchos::rcp;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Symbol;
using CSymPy::umap_basic_int;
using CSymPy::map_basic_basic;
using CSymPy::Rational;
using CSymPy::one;
using CSymPy::zero;

void test_symbol_hash()
{
    Symbol x  = Symbol("x");
    Symbol x2 = Symbol("x");
    Symbol y  = Symbol("y");

    assert(x.__eq__(x));
    assert(x.__eq__(x2));
    assert(!(x.__eq__(y)));
    assert(x.__neq__(y));

    std::hash<Basic> hash_fn;
    // Hashes of x and x2 must be the same:
    assert(hash_fn(x) == hash_fn(x2));
    // Hashes of x and y can but don't have to be different:
    if (hash_fn(x) != hash_fn(y)) assert(x.__neq__(y));


    std::size_t seed1 = 0;
    hash_combine<std::string>(seed1, "x");
    hash_combine<std::string>(seed1, "y");

    std::size_t seed2 = 0;
    hash_combine<Basic>(seed2, x);
    hash_combine<Basic>(seed2, y);

    // This checks that the Symbols are hashed by their strings:
    assert(seed1 == seed2);
}

void test_symbol_dict()
{
    umap_basic_int d;
    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> x2 = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));
    assert( x !=  x2);  // The instances are different...
    assert(eq(x, x2));  // ...but equal in the SymPy sense

    d[x] = rcp(new Rational(2));
    d[y] = rcp(new Rational(3));

    // Test printing:
    std::cout << d << std::endl;
    std::cout << *x << std::endl;
}

void test_add()
{
    umap_basic_int m;
    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));
    m[x] = rcp(new Rational(2));
    m[y] = rcp(new Rational(3));

    RCP<Add> a = rcp(new Add(zero, m));
    m[x] = rcp(new Rational(-2));
    RCP<Add> b = rcp(new Add(zero, m));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<Basic> r = add(add(x, y), add(y, x));
    std::cout << *r << std::endl;

    r = add(x, x);
    std::cout << *r << std::endl;

    r = add(add(x, x), y);
    std::cout << *r << std::endl;
    std::cout << "----------------------" << std::endl;
}

void test_integer()
{
    RCP<Rational> i = rcp(new Rational(5));
    RCP<Rational> j = rcp(new Rational(6));
    std::cout << *i << std::endl;
    std::cout << *j << std::endl;

    RCP<Rational> k = addint(i, j);
    std::cout << *k << std::endl;
    assert(eq(k, rcp(new Rational(11))));
    assert(neq(k, rcp(new Rational(12))));

    k = subint(i, j);
    std::cout << *k << std::endl;
    assert(eq(k, rcp(new Rational(-1))));
    assert(neq(k, rcp(new Rational(12))));

    k = mulint(i, j);
    std::cout << *k << std::endl;
    assert(eq(k, rcp(new Rational(30))));
    assert(neq(k, rcp(new Rational(12))));

    // FIXME: this should return a Rational
    k = divint(i, j);
    std::cout << *k << std::endl;

    k = negint(i);
    std::cout << *k << std::endl;
    assert(eq(k, rcp(new Rational(-5))));
    assert(neq(k, rcp(new Rational(12))));
}

void test_mul()
{
    map_basic_basic m;
    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));
    m[x] = rcp(new Rational(2));
    m[y] = rcp(new Rational(3));

    RCP<Mul> a = rcp(new Mul(one, m));
    m[x] = rcp(new Rational(-2));
    RCP<Mul> b = rcp(new Mul(one, m));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<Basic> r = mul(mul(x, y), mul(y, x));
    std::cout << *r << std::endl;
}

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    test_symbol_hash();

    test_symbol_dict();

    test_add();

    test_integer();

    test_mul();

    return 0;
}
