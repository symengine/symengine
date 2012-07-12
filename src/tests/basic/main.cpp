#include <iostream>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"

using Teuchos::RCP;
using Teuchos::rcp;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Symbol;
using CSymPy::Dict_int;

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

    d[x] = 2;
    d[y] = 3;

    // Test printing:
    std::cout << d << std::endl;
    std::cout << *x << std::endl;
}

void test_add()
{
    Dict_int m;
    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));
    m[x] = 2;
    m[y] = 3;

    RCP<Add> a = rcp(new Add(m));
    m[x] = -2;
    RCP<Add> b = rcp(new Add(m));
    std::cout << *a << std::endl;
    std::cout << *b << std::endl;

    RCP<Basic> r = (x + y) + (y + x);
    std::cout << *r << std::endl;
}

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    test_symbol_hash();

    test_symbol_dict();

    test_add();

    return 0;
}
