#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"

using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Pow;
using CSymPy::Symbol;
using CSymPy::Dict_int;
using CSymPy::map_vec_int;
using CSymPy::Integer;
using CSymPy::multinomial_coefficients;

void test_add()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1 = add(x, x);
    RCP<Basic> r2 = mul(i2, x);
    RCP<Basic> r3 = mul(i3, x);
    assert(eq(r1, r2));
    assert(neq(r1, r3));

    r3 = mul(i2, y);
    assert(neq(r1, r3));
    assert(neq(r2, r3));

    r1 = add(mul(y, x), mul(mul(i2, x), y));
    r2 = mul(mul(i3, x), y);
    assert(eq(r1, r2));

    r1 = add(add(x, x), x);
    r2 = mul(i3, x);
    assert(eq(r1, r2));

    r1 = add(add(x, x), x);
    r2 = mul(x, i3);
    assert(eq(r1, r2));
}

void test_pow()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1;
    RCP<Basic> r2;

    r1 = mul(x, x);
    r2 = pow(x, i2);
    assert(eq(r1, r2));

    r1 = mul(mul(x, x), x);
    r2 = pow(x, i3);
    assert(eq(r1, r2));

    r1 = mul(mul(mul(x, x), x), x);
    r2 = pow(x, i4);
    assert(eq(r1, r2));

    r1 = mul(mul(add(x, y), add(x, y)), add(x, y));
    r2 = pow(add(x, y), i3);
    assert(eq(r1, r2));

    r1 = mul(mul(add(x, y), add(y, x)), add(x, y));
    r2 = pow(add(x, y), i3);
    assert(eq(r1, r2));
}

void test_multinomial()
{
    map_vec_int r;
    auto t1 = std::chrono::high_resolution_clock::now();
    multinomial_coefficients(4, 40, r);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
}

void test_expand()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(40));

    RCP<Basic> r1;
    RCP<Basic> r2;

    r1 = pow(add(add(add(x, y), z), w), i4);

    std::cout << *r1 << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    r2 = expand(rcp_dynamic_cast<Pow>(r1));
    auto t2 = std::chrono::high_resolution_clock::now();
    //std::cout << *r2 << std::endl;
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
    std::cout << "number of terms: "
        << rcp_dynamic_cast<Add>(r2)->dict.size() << std::endl;
}

void test_expand2()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));

    RCP<Basic> r1;
    RCP<Basic> r2;

    r1 = mul(w, add(add(x, y), z)); // w*(x+y+z)
    std::cout << *r1 << std::endl;

    r2 = mul_expand(rcp_dynamic_cast<Mul>(r1));
    std::cout << *r2 << std::endl;
}


int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    test_add();
    test_pow();
    test_multinomial();
    test_expand();
    test_expand2();

    return 0;
}
