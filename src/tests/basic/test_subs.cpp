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
#include "constants.h"

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Pow;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::umap_basic_num;
using SymEngine::map_vec_int;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::multinomial_coefficients;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::sin;
using SymEngine::RCP;
using SymEngine::rcp;
using SymEngine::rcp_dynamic_cast;
using SymEngine::map_basic_basic;
using SymEngine::print_stack_on_segfault;

void test_symbol()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = x;
    RCP<const Basic> r2 = y;
    map_basic_basic d;
    d[x] = y;
    assert(eq(r1->subs(d), r2));
    assert(neq(r1->subs(d), r1));
}

void test_add()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = add(x, y);
    RCP<const Basic> r2 = mul(i2, y);
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
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = mul(x, y);
    RCP<const Basic> r2 = pow(y, i2);
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
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = pow(x, y);
    RCP<const Basic> r2 = pow(y, y);
    map_basic_basic d;
    d[x] = y;
    assert(eq(r1->subs(d), r2));

    d[x] = z;
    d[y] = w;
    r1 = pow(x, y);
    r2 = pow(z, w);
    assert(eq(r1->subs(d), r2));

    r1 = pow(x, i2);
    r2 = pow(z, i2);
    assert(eq(r1->subs(d), r2));

    d.clear();
    d[pow(x, y)] = z;
    r1 = pow(x, y);
    r2 = z;
    assert(eq(r1->subs(d), r2));
}

void test_trig()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = sin(x);
    RCP<const Basic> r2 = zero;
    map_basic_basic d;
    d[x] = zero;
    assert(eq(r1->subs(d), r2));

    r1 = cos(x);
    r2 = one;
    assert(eq(r1->subs(d), r2));

    d[x] = y;
    r1 = sin(pow(x, i2));
    r2 = sin(pow(y, i2));
    assert(eq(r1->subs(d), r2));

    d.clear();
    d[sin(x)] = z;
    r1 = sin(x);
    r2 = z;
    assert(eq(r1->subs(d), r2));

    r1 = mul(i2, sin(x));
    r2 = mul(i2, z);
    assert(eq(r1->subs(d), r2));
}


int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_symbol();
    test_add();
    test_mul();
    test_pow();
    test_trig();

    return 0;
}
