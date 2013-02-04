#include <iostream>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "mul.h"

using Teuchos::RCP;
using Teuchos::rcp;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Symbol;
using CSymPy::Dict_int;
using CSymPy::Integer;

void test_symbol_hash()
{
    Symbol x  = Symbol("x");
    Symbol x2 = Symbol("x");
    Symbol y  = Symbol("y");

    assert(x == x);
    assert(x == x2);
    assert(!(x == y));
    assert(x != y);

    std::hash<Basic> hash_fn;
    // Hashes of x and x2 must be the same:
    assert(hash_fn(x) == hash_fn(x2));
    // Hashes of x and y can but don't have to be different:
    if (hash_fn(x) != hash_fn(y)) assert(x != y);


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
    Dict_int d;
    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> x2 = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));
    assert( x !=  x2);  // The instances are different...
    assert(*x == *x2);  // ...but equal in the SymPy sense

    d[x] = rcp(new Integer(2));
    d[y] = rcp(new Integer(3));

    // Test printing:
    std::cout << d << std::endl;
    std::cout << x << std::endl;
}

void test_add()
{
    Dict_int m;
    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));
    m[x] = rcp(new Integer(2));
    m[y] = rcp(new Integer(3));

    RCP<Add> a = rcp(new Add(m));
    m[x] = rcp(new Integer(-2));
    RCP<Add> b = rcp(new Add(m));
    std::cout << a << std::endl;
    std::cout << b << std::endl;

    RCP<Basic> r = (x + y) + (y + x);
    std::cout << r << std::endl;

    r = x + x;
    std::cout << r << std::endl;

    r = x + x + y;
    std::cout << r << std::endl;
    std::cout << "----------------------" << std::endl;
}

void test_integer()
{
    RCP<Integer> i = rcp(new Integer(5));
    RCP<Integer> j = rcp(new Integer(6));
    std::cout << i << std::endl;
    std::cout << j << std::endl;

    RCP<Integer> k = i + j;
    std::cout << k << std::endl;

    k = i - j;
    std::cout << k << std::endl;

    k = i * j;
    std::cout << k << std::endl;

    k = i / j;
    std::cout << k << std::endl;

    k = -i;
    std::cout << k << std::endl;
}

void test_mul()
{
    Dict_int m;
    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));
    m[x] = rcp(new Integer(2));
    m[y] = rcp(new Integer(3));

    RCP<Mul> a = rcp(new Mul(rcp(new Integer(1)), m));
    m[x] = rcp(new Integer(-2));
    RCP<Mul> b = rcp(new Mul(rcp(new Integer(1)), m));
    std::cout << a << std::endl;
    std::cout << b << std::endl;

    RCP<Basic> r = (x * y) * (y * x);
    std::cout << r << std::endl;
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
