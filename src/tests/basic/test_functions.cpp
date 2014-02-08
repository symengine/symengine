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
using CSymPy::symbol;
using CSymPy::umap_basic_int;
using CSymPy::map_vec_int;
using CSymPy::Integer;
using CSymPy::integer;
using CSymPy::multinomial_coefficients;
using CSymPy::one;
using CSymPy::zero;
using CSymPy::sin;
using CSymPy::cos;
using CSymPy::function_symbol;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::rcp_dynamic_cast;
using CSymPy::print_stack_on_segfault;

void test_sin()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = sin(x);
    r2 = sin(x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = sin(zero);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = sin(x)->diff(x);
    r2 = cos(x);
    assert(eq(r1, r2));

    r1 = mul(i2,x)->diff(x);
    r2 = i2;
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = sin(mul(i2,x))->diff(x);
    r2 = mul(i2, cos(mul(i2,x)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = mul(x, sin(x))->diff(x);
    r2 = add(sin(x), mul(x, cos(x)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = mul(x, sin(x))->diff(x)->diff(x);
    r2 = add(mul(i2, cos(x)), neg(mul(x, sin(x))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = mul(sin(x), cos(x))->diff(x);
    r2 = sub(pow(cos(x), i2), pow(sin(x), i2));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));
}

void test_cos()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = cos(x);
    r2 = cos(x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = cos(zero);
    r2 = one;
    assert(eq(r1, r2));

    r1 = cos(x)->diff(x);
    r2 = mul(im1, sin(x));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));
}

void test_f()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = function_symbol("f", x);
    r2 = function_symbol("f", x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = function_symbol("f", x);
    r2 = function_symbol("g", x);
    assert(neq(r1, r2));

    r1 = function_symbol("f", x);
    r2 = function_symbol("f", y);
    assert(neq(r1, r2));

    r1 = function_symbol("f", zero);
    r2 = one;
    assert(neq(r1, r2));

    r1 = function_symbol("f", x)->diff(y);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = mul(i2, pow(function_symbol("f", add(add(x, y), z)), i2));
    r2 = mul(i2, pow(function_symbol("f", add(add(y, z), x)), i2));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_sin();
    test_cos();
    test_f();

    return 0;
}
