#include <iostream>
#include <chrono>

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Pow;
using CSymPy::Symbol;
using CSymPy::umap_basic_int;
using CSymPy::map_vec_int;
using CSymPy::Integer;
using CSymPy::integer;
using CSymPy::multinomial_coefficients;
using CSymPy::one;
using CSymPy::zero;
using CSymPy::sin;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::rcp_dynamic_cast;
using CSymPy::map_basic_basic;
using CSymPy::print_stack_on_segfault;

void test_symbol()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1 = x;
    RCP<Basic> r2 = y;
    map_basic_basic d;
    d[x] = y;
    assert(eq(r1->subs(d), r2));
    assert(neq(r1->subs(d), r1));
}

void test_add()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1 = add(x, y);
    RCP<Basic> r2 = mul(i2, y);
    map_basic_basic d;
    d[x] = y;
    assert(eq(r1->subs(d), r2));

    d[x] = z;
    d[y] = w;
    r1 = add(x, y);
    r2 = add(z, w);
    assert(eq(r1->subs(d), r2));

    d.clear();
    d[add(x, y)] = z;
    r1 = add(x, y);
    r2 = z;
    assert(eq(r1->subs(d), r2));

    d.clear();
    d[pow(x, y)] = z;
    d[pow(x, i2)] = y;
    d[pow(i2, y)] = x;
    r1 = add(add(pow(x, y), pow(x, i2)), pow(i2, y));
    r2 = add(add(x, y), z);
    assert(eq(r1->subs(d), r2));

    r1 = add(add(add(add(pow(x, y), pow(x, i2)), pow(i2, y)), x), i3);
    r2 = add(add(add(mul(i2, x), y), z), i3);
    assert(eq(r1->subs(d), r2));
}

void test_mul()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1 = mul(x, y);
    RCP<Basic> r2 = pow(y, i2);
    map_basic_basic d;
    d[x] = y;
    assert(eq(r1->subs(d), r2));

    d[x] = z;
    d[y] = w;
    r1 = mul(x, y);
    r2 = mul(z, w);
    assert(eq(r1->subs(d), r2));

    d.clear();
    d[mul(x, y)] = z;
    r1 = mul(x, y);
    r2 = z;
    assert(eq(r1->subs(d), r2));

    d.clear();
    d[pow(x, y)] = z;
    r1 = mul(i2, pow(x, y));
    r2 = mul(i2, z);
    assert(eq(r1->subs(d), r2));
}

void test_pow()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1 = pow(x, y);
    RCP<Basic> r2 = pow(y, y);
    map_basic_basic d;
    d[x] = y;
    assert(eq(r1->subs(d), r2));

    d[x] = z;
    d[y] = w;
    r1 = pow(x, y);
    r2 = pow(z, w);
    assert(eq(r1->subs(d), r2));

    d.clear();
    d[pow(x, y)] = z;
    r1 = pow(x, y);
    r2 = z;
    assert(eq(r1->subs(d), r2));
}


int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_symbol();
    test_add();
    test_mul();
    test_pow();

    return 0;
}
