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
using CSymPy::tan;
using CSymPy::cot;
using CSymPy::csc;
using CSymPy::sec;
using CSymPy::asin;
using CSymPy::acos;
using CSymPy::asec;
using CSymPy::acsc;
using CSymPy::atan;
using CSymPy::acot;
using CSymPy::atan2;
using CSymPy::lambertw;
using CSymPy::log;
using CSymPy::exp;
using CSymPy::function_symbol;
using CSymPy::Derivative;
using CSymPy::pi;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::print_stack_on_segfault;
using CSymPy::sqrt;
using CSymPy::sinh;
using CSymPy::cosh;
using CSymPy::tanh;
using CSymPy::coth;
using CSymPy::asinh;
using CSymPy::acosh;
using CSymPy::atanh;
using CSymPy::acoth;

void test_sin()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");

    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = sin(x);
    r2 = sin(x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = sin(zero);
    r2 = zero;
    std::cout << *r1 << std::endl;
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

    // sin(-y) = -sin(y)
    r1 = sin(mul(im1, y));
    r2 = mul(im1, sin(y));
    assert(eq(r1, r2));

    // sin(pi - y) = sin(y)
    r1 = sin(sub(pi, y));
    r2 = sin(y);
    assert(eq(r1, r2));

    // sin(pi + y) = -sin(y)
    r1 = sin(add(pi, y));
    r2 = mul(im1, sin(y));
    assert(eq(r1, r2));

    // sin(2*pi - y) = -sin(y)
    r1 = sin(sub(mul(i2, pi), y));
    r2 = mul(im1, sin(y));
    assert(eq(r1, r2));

    // sin(12*pi + y) = sin(y)
    r1 = sin(add(mul(i12, pi), y));
    r2 = sin(y);
    assert(eq(r1, r2));

    // sin(3*pi - y) = sin(y)
    r1 = sin(sub(mul(i3, pi), y));
    r2 = sin(y);
    assert(eq(r1, r2));

    // sin(pi/2 + y) = cos(y)
    r1 = sin(add(div(pi, i2), y));
    r2 = cos(y);
    assert(eq(r1, r2));

    // sin(pi/2 - y) = cos(y)
    r1 = sin(sub(div(pi, i2), y));
    r2 = cos(y);
    assert(eq(r1, r2));

    // sin(12*pi + y + pi/2) = cos(y)
    r1 = sin(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = cos(y);
    assert(eq(r1, r2));

    // sin(12*pi - y + pi/2) = cos(y)
    r1 = sin(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = cos(y);
    assert(eq(r1, r2));
}

void test_cos()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

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

    // cos(-y) = cos(y)
    r1 = cos(mul(im1, y));
    r2 = cos(y);
    assert(eq(r1, r2));

    // cos(pi - y) = -cos(y)
    r1 = cos(sub(pi, y));
    r2 = mul(im1, cos(y));
    assert(eq(r1, r2));

    // cos(pi + y) = -cos(y)
    r1 = cos(add(pi, y));
    r2 = mul(im1, cos(y));
    assert(eq(r1, r2));

    // cos(2*pi - y) = cos(y)
    r1 = cos(sub(mul(i2, pi), y));
    r2 = cos(y);
    assert(eq(r1, r2));

    // cos(12*pi + y) = cos(y)
    r1 = cos(add(mul(i12, pi), y));
    r2 = cos(y);
    assert(eq(r1, r2));

    // cos(3*pi - y) = -cos(y)
    r1 = cos(sub(mul(i3, pi), y));
    r2 = mul(im1, cos(y));
    assert(eq(r1, r2));

    // cos(pi/2 + y) = -sin(y)
    r1 = cos(add(div(pi, i2), y));
    r2 = mul(im1, sin(y));
    assert(eq(r1, r2));

    // cos(pi/2 - y) = sin(y)
    r1 = cos(sub(div(pi, i2), y));
    r2 = sin(y);
    assert(eq(r1, r2));

    // cos(12*pi + y + pi/2) = -sin(y)
    r1 = cos(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = mul(im1, sin(y));
    assert(eq(r1, r2));

    // cos(12*pi - y + pi/2) = sin(y)
    r1 = cos(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = sin(y);
    assert(eq(r1, r2));
}

void test_tan()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);


    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = tan(x);
    r2 = tan(x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = tan(zero);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = tan(x)->diff(x);
    r2 = add(pow(tan(x), i2), i1);
    assert(eq(r1, r2));

    r1 = tan(mul(i2, x))->diff(x);
    r2 = mul(i2, add(pow(tan(mul(i2, x)), i2), i1));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = mul(x, tan(x))->diff(x);
    r2 = add(tan(x), mul(x, add(pow(tan(x), i2), i1)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    // tan(-y) = -tan(y)
    r1 = tan(mul(im1, y));
    r2 = mul(im1, tan(y));
    assert(eq(r1, r2));

    // tan(pi - y) = -tan(y)
    r1 = tan(sub(pi, y));
    r2 = mul(im1, tan(y));
    assert(eq(r1, r2));

    // tan(pi + y) = -tan(y)
    r1 = tan(add(pi, y));
    r2 = tan(y);
    assert(eq(r1, r2));

    // tan(2*pi - y) = -tan(y)
    r1 = tan(sub(mul(i2, pi), y));
    r2 = mul(im1, tan(y));
    assert(eq(r1, r2));

    // tan(12*pi + y) = tan(y)
    r1 = tan(add(mul(i12, pi), y));
    r2 = tan(y);
    assert(eq(r1, r2));

    // tan(3*pi - y) = -tan(y)
    r1 = tan(sub(mul(i3, pi), y));
    r2 = mul(im1, tan(y));
    assert(eq(r1, r2));

    // tan(pi/2 + y) = -cot(y)
    r1 = tan(add(div(pi, i2), y));
    r2 = mul(im1, cot(y));
    assert(eq(r1, r2));

    // tan(pi/2 - y) = cot(y)
    r1 = tan(sub(div(pi, i2), y));
    r2 = cot(y);
    assert(eq(r1, r2));

    // tan(12*pi + y + pi/2) = -cot(y)
    r1 = tan(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = mul(im1, cot(y));
    assert(eq(r1, r2));

    // tan(12*pi - y + pi/2) = cot(y)
    r1 = tan(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = cot(y);
    assert(eq(r1, r2));

}

void test_cot()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = cot(x);
    r2 = cot(x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = cot(x)->diff(x);
    r2 = mul(im1, add(pow(cot(x), i2), i1));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = cot(mul(i2, x))->diff(x);
    r2 = mul(integer(-2), add(pow(cot(mul(i2, x)), i2), i1));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = mul(x, cot(x))->diff(x);
    r2 = add(cot(x), mul(x, mul(add(pow(cot(x), i2), i1), im1)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    // cot(-y) = -cot(y)
    r1 = cot(mul(im1, y));
    r2 = mul(im1, cot(y));
    assert(eq(r1, r2));

    // cot(pi - y) = -cot(y)
    r1 = cot(sub(pi, y));
    r2 = mul(im1, cot(y));
    assert(eq(r1, r2));

    // cot(pi + y) = -cot(y)
    r1 = cot(add(pi, y));
    r2 = cot(y);
    assert(eq(r1, r2));

    // cot(2*pi - y) = -cot(y)
    r1 = cot(sub(mul(i2, pi), y));
    r2 = mul(im1, cot(y));
    assert(eq(r1, r2));

    // cot(12*pi + y) = cot(y)
    r1 = cot(add(mul(i12, pi), y));
    r2 = cot(y);
    assert(eq(r1, r2));

    // cot(3*pi - y) = -cot(y)
    r1 = cot(sub(mul(i3, pi), y));
    r2 = mul(im1, cot(y));
    assert(eq(r1, r2));

    // cot(pi/2 + y) = -tan(y)
    r1 = cot(add(div(pi, i2), y));
    r2 = mul(im1, tan(y));
    assert(eq(r1, r2));

    // cot(pi/2 - y) = cot(y)
    r1 = cot(sub(div(pi, i2), y));
    r2 = tan(y);
    assert(eq(r1, r2));

    // cot(12*pi + y + pi/2) = -tan(y)
    r1 = cot(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = mul(im1, tan(y));
    assert(eq(r1, r2));

    // cot(12*pi - y + pi/2) = tan(y)
    r1 = cot(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = tan(y);
    assert(eq(r1, r2));


}

void test_csc()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = csc(x);
    r2 = csc(x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = csc(x)->diff(x);
    r2 = mul(im1, mul(cot(x), csc(x)));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = csc(mul(i2, x))->diff(x);
    r2 = mul(integer(-2), mul(cot(mul(i2, x)), csc(mul(i2, x))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = mul(x, csc(x))->diff(x);
    r2 = add(csc(x), mul(x, mul(im1, mul(cot(x), csc(x)))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    // csc(-y) = -csc(y)
    r1 = csc(mul(im1, y));
    r2 = mul(im1, csc(y));
    assert(eq(r1, r2));

    // csc(pi - y) = csc(y)
    r1 = csc(sub(pi, y));
    r2 = csc(y);
    assert(eq(r1, r2));

    // csc(pi + y) = -csc(y)
    r1 = csc(add(pi, y));
    r2 = mul(im1, csc(y));
    assert(eq(r1, r2));

    // csc(2*pi - y) = -csc(y)
    r1 = csc(sub(mul(i2, pi), y));
    r2 = mul(im1, csc(y));
    assert(eq(r1, r2));

    // csc(12*pi + y) = csc(y)
    r1 = csc(add(mul(i12, pi), y));
    r2 = csc(y);
    assert(eq(r1, r2));

    // csc(3*pi - y) = csc(y)
    r1 = csc(sub(mul(i3, pi), y));
    r2 = csc(y);
    assert(eq(r1, r2));

    // csc(pi/2 + y) = sec(y)
    r1 = csc(add(div(pi, i2), y));
    r2 = sec(y);
    assert(eq(r1, r2));

    // csc(pi/2 - y) = sec(y)
    r1 = csc(sub(div(pi, i2), y));
    r2 = sec(y);
    assert(eq(r1, r2));

    // csc(12*pi + y + pi/2) = sec(y)
    r1 = csc(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = sec(y);
    assert(eq(r1, r2));

    // csc(12*pi - y + pi/2) = sec(y)
    r1 = csc(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = sec(y);
    assert(eq(r1, r2));

}

void test_sec()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = sec(x);
    r2 = sec(x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = sec(x)->diff(x);
    r2 = mul(tan(x), sec(x));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = sec(mul(i2, x))->diff(x);
    r2 = mul(i2, mul(tan(mul(i2, x)), sec(mul(i2, x))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = mul(x, sec(x))->diff(x);
    r2 = add(sec(x), mul(x, mul(tan(x), sec(x))));
    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    assert(eq(r1, r2));

    // sec(-y) = sec(y)
    r1 = sec(mul(im1, y));
    r2 = sec(y);
    assert(eq(r1, r2));

    // sec(pi - y) = -sec(y)
    r1 = sec(sub(pi, y));
    r2 = mul(im1, sec(y));
    assert(eq(r1, r2));

    // sec(pi + y) = -sec(y)
    r1 = sec(add(pi, y));
    r2 = mul(im1, sec(y));
    assert(eq(r1, r2));

    // sec(2*pi - y) = sec(y)
    r1 = sec(sub(mul(i2, pi), y));
    r2 = sec(y);
    assert(eq(r1, r2));

    // sec(12*pi + y) = sec(y)
    r1 = sec(add(mul(i12, pi), y));
    r2 = sec(y);
    assert(eq(r1, r2));

    // sec(3*pi - y) = -sec(y)
    r1 = sec(sub(mul(i3, pi), y));
    r2 = mul(im1, sec(y));
    assert(eq(r1, r2));

    // sec(pi/2 + y) = -csc(y)
    r1 = sec(add(div(pi, i2), y));
    r2 = mul(im1, csc(y));
    assert(eq(r1, r2));

    // sec(pi/2 - y) = csc(y)
    r1 = sec(sub(div(pi, i2), y));
    r2 = csc(y);
    assert(eq(r1, r2));

    // sec(12*pi + y + pi/2) = -csc(y)
    r1 = sec(add(add(mul(i12, pi), y), div(pi, i2)));
    r2 = mul(im1, csc(y));
    assert(eq(r1, r2));

    // sec(12*pi - y + pi/2) = csc(y)
    r1 = sec(add(sub(mul(i12, pi), y), div(pi, i2)));
    r2 = csc(y);
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

void test_Derivative()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Basic> f = function_symbol("f", x);

    RCP<const Basic> r1, r2, r3;

    r1 = f->diff(x);
    r2 = rcp(new Derivative(f, {x}));
    r3 = rcp(new Derivative(f, {y}));
    assert(eq(r1, r2));
    assert(neq(r1, r3));
    assert(neq(r2, r3));

    std::cout << *r1 << std::endl;
    std::cout << *r2 << std::endl;
    std::cout << *r3 << std::endl;
    assert(r1->__str__() == "Derivative(f(x), x)");
    assert(r2->__str__() == "Derivative(f(x), x)");
    assert(r3->__str__() == "Derivative(f(x), y)");

    r1 = f->diff(x)->diff(x);
    r2 = rcp(new Derivative(f, {x, x}));
    assert(eq(r1, r2));
    std::cout << *r1 << std::endl;
    assert(r1->__str__() == "Derivative(f(x), x, x)");

    f = function_symbol("f", pow(x, integer(2)));
    r1 = f->diff(x);
    std::cout << *f << " " << *r1 << std::endl;
    // NOTE: After we implement the Subs class, then f(x^2).diff(x) should
    // become 2*x*Subs(Derivative(f(_xi_1), _xi_1), _xi_1, x**2). For now we
    // don't simplify things:
    assert(eq(r1, rcp(new Derivative(f, {x}))));

    // Test is_canonical()
    RCP<const Derivative> r4 = rcp(new Derivative(f, {x}));
    assert(r4->is_canonical(x, {x}));
    assert(r4->is_canonical(x, {y}));
    assert(r4->is_canonical(x, {x, y, x, x}));
    assert(!(r4->is_canonical(x, {pow(x, integer(2))})));
}

void test_get_pi_shift()
{
    RCP<const Basic> r;
    RCP<const Basic> r1;
    RCP<const Integer> n;
    bool b;
    
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);
    RCP<const Basic> i8 = integer(8);

    RCP<const Basic> sq3 = sqrt(i3);
    RCP<const Basic> sq2 = sqrt(i2);
    
    RCP<const Symbol> x = symbol("x");
    
    // arg = k + n*pi
    r = add(i3, mul(i2, pi));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == true);
    assert(eq(n, integer(24)));
    assert(eq(r1, i3)); 
    
    // arg = n*pi/12
    r = mul(pi, div(one, integer(12)));
    get_pi_shift(r, outArg(n), outArg(r1));
    assert(eq(n, one));
    assert(eq(r1, zero)); 
    
    // arg = n*pi/12
    r = mul(pi, div(i2, integer(3)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(eq(n, i8) && (b == true) && eq(r1, zero));

	// arg neq n*pi/12 , n not an integer
    r = mul(pi, div(i2, integer(5)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == false);

	// arg neq theta + n*pi/12 (no pi symbol, pi as pow)
    r = mul(pow(pi, i2), div(i2, integer(3)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == false);

	// arg neq theta + n*pi/12 (no pi symbol, pi as mul form)
    r = mul(mul(pi, x), div(i2, integer(3)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == false);
    
    // arg = theta + n*pi/12 (theta is just another symbol)
    r = add(mul(i2, x), mul(pi, div(i2, integer(3))));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == true); 
    assert(eq(n, i8));
    assert(eq(r1, mul(i2, x)));

    // arg = theta + n*pi/12 (theta is constant plus a symbol)
    r = add(i2, add(x, mul(pi, div(i2, integer(3)))));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == true); 
    assert(eq(n, i8));
    assert(eq(r1, add(i2, x)));
    
    // arg = theta + n*pi/12 (theta is an expression)
    r = add(i2, add(mul(x, i2), mul(pi, div(i2, integer(3)))));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == true); 
    assert(eq(n, i8));
    assert(eq(r1, add(i2, mul(x, i2))));
   
    // arg neq n*pi/12 (n is not integer)
    r = mul(pi, div(i2, integer(5)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == false);

	// arg neq n*pi/12 (pi is not in form of symbol)
    r = mul(pow(pi, i2), div(i2, integer(3)));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == false);
    
    // arg = pi (it is neither of form add nor mul, just a symbol)
    b = get_pi_shift(pi, outArg(n), outArg(r1));
    assert((b == true) && eq(n, i12) && eq(r1, zero)) ;
    
    // arg = theta + n*pi/12 (theta is an expression of >1 symbols)
    r = add(add(mul(i2, x), mul(i2, symbol("y"))), mul(pi, div(i2, integer(3))));
    b = get_pi_shift(r, outArg(n), outArg(r1));
    assert(b == true); 
    assert(eq(n, i8));
    assert(eq(r1, add(mul(i2, x), mul(i2, symbol("y")))));
}

void test_sin_table()
{
    RCP<const Basic> r1;
    RCP<const Basic> r2;

    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i12 = integer(12);
    RCP<const Basic> im1 = integer(-1);

    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const Basic> sq3 = sqrt(i3);
    RCP<const Basic> sq2 = sqrt(i2);

    // sin(2pi + pi/6) = 1/2
    r1 = sin(add(mul(pi, i2), mul(div(pi, i12), i2)));
    r2 = div(one, i2);
    assert(eq(r1, r2));

    // sin(n*pi + pi/6) = 1/2
    r1 = sin(add(mul(pi, integer(10)), mul(div(pi, i12), i2)));
    r2 = div(one, i2);
    assert(eq(r1, r2));

    // sin(n*pi) = 0
    r1 = sin(mul(pi, i12));
    assert(eq(r1, zero));

    // sin(2pi + pi/2) = 1
    r1 = sin(add(mul(pi, i2), div(pi, i2)));
    assert(eq(r1, one));

    // sin(pi/3) = sqrt(3)/2
    r1 = sin(div(pi, integer(3)));
    r2 = div(sq3, i2);
    assert(eq(r1, r2));

    // sin(pi/4) = 1/sqrt(2)
    r1 = sin(div(pi, integer(4)));
    r2 = div(sq2, i2);
    assert(eq(r1, r2));

    // sin(pi/12) = (sqrt(3) - 1)/(2*sqrt(2))
    r1 = sin(div(pi, i12));
    r2 = div(sub(sq3, one), mul(i2, sq2));
    assert(eq(r1, r2));

    // sin(5*pi/12) = (sqrt(3) + 1)/(2*sqrt(2))
    r1 = sin(mul(div(pi, i12), integer(5)));
    r2 = div(add(sq3, one), mul(i2, sq2));
    assert(eq(r1, r2));

}

void test_could_extract_minus()
{
    RCP<const Basic> x = rcp(new Symbol("x"));
    RCP<const Basic> y = rcp(new Symbol("y"));
    
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> r;
    bool b;

    r = add(mul(im1, x), mul(im1, mul(i2, y)));
    b = could_extract_minus(r);
    assert(b == true); 

    r = add(mul(im1, x), mul(i2, y));
    b = could_extract_minus(r);
    assert(b == false); 

    r = mul(mul(x, integer(-10)), y);
    b = could_extract_minus(r);
    assert(b == true); 

    r = mul(mul(x, i2), y);
    b = could_extract_minus(r);
    assert(b == false);  

    r = add(mul(im1, x), mul(im1, div(mul(i2, y), integer(3))));
    b = could_extract_minus(r);
    assert(b == true);    

    r = mul(div(x, i2), y);
    b = could_extract_minus(r);
    assert(b == false);  

}

void test_asin()
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = asin(im1);
    r2 = mul(im1, div(pi, i2));
    assert(eq(r1, r2));

    r1 = asin(div(im1, i2));
    r2 = div(pi, mul(im2, i3));
    assert(eq(r1, r2));

    r1 = asin(div(sqrt(i2), i2));
    r2 = div(pi, mul(i2, i2));
    assert(eq(r1, r2));

    r1 = asin(div(add(sqrt(i3), i1), mul(i2, sqrt(i2))));
    r2 = div(pi, mul(i3, pow(i2, i2)));
    assert(eq(r1, r2));

    r1 = asin(div(sqrt(sub(i5, sqrt(i5))), integer(8)));
    r2 = div(pi, i5);
    assert(eq(r1, r2));

    r1 = asin(mul(div(sub(sqrt(i5), i1), integer(4)), im1));
    r2 = div(pi, mul(im2, i5));
    assert(eq(r1, r2));
}

void test_acos()
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acos(im1);
    r2 = pi;
    assert(eq(r1, r2));

    r1 = acos(div(im1, i2));
    r2 = mul(i2, div(pi,  i3));
    assert(eq(r1, r2));

    r1 = acos(div(sqrt(i2), i2));
    r2 = div(pi, mul(i2, i2));
    assert(eq(r1, r2));

    r1 = acos(div(add(sqrt(i3), i1), mul(i2, sqrt(i2))));
    r2 = mul(i5, div(pi, mul(i3, pow(i2, i2))));
    assert(eq(r1, r2));

    r1 = acos(div(sqrt(sub(i5, sqrt(i5))), integer(8)));
    r2 = mul(i3, div(pi, mul(i2, i5)));
    assert(eq(r1, r2));

    r1 = acos(mul(div(sub(sqrt(i5), i1), integer(4)), im1));
    r2 = mul(i3, div(pi, i5));
    assert(eq(r1, r2));

    r1 = acos(div(sub(sqrt(i5), i1), integer(4)));
    r2 = mul(i2, div(pi, i5));
    assert(eq(r1, r2));
}

void test_asec()
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = asec(im1);
    r2 = pi;
    assert(eq(r1, r2));

    r1 = asec(div(i2, im1));
    r2 = mul(i2, div(pi,  i3));
    assert(eq(r1, r2));
    
    r1 = asec(sqrt(i2));
    r2 = div(pi, mul(i2, i2));
    assert(eq(r1, r2));

    r1 = asec(div(mul(i2, sqrt(i2)), add(sqrt(i3), i1)));
    r2 = mul(i5, div(pi, mul(i3, pow(i2, i2))));
    std::cout<<*r1<<std::endl;
    std::cout<<*r2<<std::endl;
    assert(eq(r1, r2));

    r1 = asec(div(integer(8), sqrt(sub(i5, sqrt(i5)))));
    r2 = mul(i3, div(pi, mul(i2, i5)));
    assert(eq(r1, r2));

    r1 = asec(mul(div(integer(4), sub(sqrt(i5), i1)), im1));
    r2 = mul(i3, div(pi, i5));
    assert(eq(r1, r2));

    r1 = asec(div(integer(4), sub(sqrt(i5), i1)));
    r2 = mul(i2, div(pi, i5));
    assert(eq(r1, r2));
}

void test_acsc()
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acsc(im1);
    r2 = mul(im1, div(pi, i2));
    assert(eq(r1, r2));

    r1 = acsc(im2);
    r2 = div(pi, mul(im2, i3));
    assert(eq(r1, r2));

    r1 = acsc(sqrt(i2));
    r2 = div(pi, mul(i2, i2));
    assert(eq(r1, r2));

    r1 = acsc(div(mul(i2, sqrt(i2)), add(sqrt(i3), i1)));
    r2 = div(pi, mul(i3, pow(i2, i2)));
    assert(eq(r1, r2));

    r1 = acsc(div(integer(8), sqrt(sub(i5, sqrt(i5)))));
    r2 = div(pi, i5);
    assert(eq(r1, r2));

    r1 = acsc(mul(div(integer(4), sub(sqrt(i5), i1)), im1));
    r2 = div(pi, mul(im2, i5));
    assert(eq(r1, r2));
}

void test_atan()
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = atan(i1);
    r2 = div(pi, integer(4));
    assert(eq(r1, r2));

    r1 = atan(im1);
    r2 = div(pi, integer(-4));
    assert(eq(r1, r2));

    r1 = atan(div(one,sqrt(i3)));
    r2 = div(pi, integer(6));
    assert(eq(r1, r2));

    r1 = atan(mul(im1, add(one, sqrt(i2))));
    r2 = div(mul(pi, i3), integer(-8));
    assert(eq(r1, r2));

    r1 = atan(sub(sqrt(i2), one));
    r2 = div(pi, integer(8));
    assert(eq(r1, r2));

    r1 = atan(sub(i2, sqrt(i3)));
    r2 = div(pi, integer(12));
    assert(eq(r1, r2));

    r1 = atan(mul(im1, sqrt(add(i5, mul(i2, sqrt(i5))))));
    r2 = div(mul(pi, im2), i5);
    assert(eq(r1, r2));

}

void test_acot()
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acot(i1);
    r2 = div(pi, integer(4));
    assert(eq(r1, r2));

    r1 = acot(im1);
    r2 = mul(i3, div(pi, integer(4)));
    assert(eq(r1, r2));

    r1 = acot(div(one,sqrt(i3)));
    r2 = div(pi, i3);
    assert(eq(r1, r2));

    r1 = acot(mul(im1, add(one, sqrt(i2))));
    r2 = div(mul(pi, integer(7)), integer(8));
    assert(eq(r1, r2));

    r1 = acot(sub(sqrt(i2), one));
    r2 = mul(i3, div(pi, integer(8)));
    assert(eq(r1, r2));

    r1 = acot(sub(i2, sqrt(i3)));
    r2 = mul(i5, div(pi, integer(12)));
    assert(eq(r1, r2));

    r1 = acot(mul(im1, sqrt(add(i5, mul(i2, sqrt(i5))))));
    r2 = div(mul(pi, integer(9)), mul(i5, i2));
    assert(eq(r1, r2));
}

void test_atan2()
{
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i1 = integer(1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> im2 = integer(-2);
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = atan2(i1, i1);
    r2 = div(pi, integer(4));
    assert(eq(r1, r2));

    r1 = atan2(im1, i1);
    r2 = div(pi, integer(-4));
    assert(eq(r1, r2));

    r1 = atan2(i1, im1);
    r2 = div(mul(i3, pi), integer(4));
    assert(eq(r1, r2));

    r1 = atan2(im1, im1);
    r2 = div(mul(i3, pi), integer(-4));
    assert(eq(r1, r2));

    r1 = atan2(one,sqrt(i3));
    r2 = div(pi, integer(6));
    assert(eq(r1, r2));

    r1 = atan2(add(one, sqrt(i2)), im1);
    r2 = div(mul(pi, i3), integer(-8));
    assert(eq(r1, r2));

    r1 = atan2(sub(sqrt(i2), one), i1);
    r2 = div(pi, integer(8));
    assert(eq(r1, r2));

    r1 = atan2(sub(i2, sqrt(i3)), i1);
    r2 = div(pi, integer(12));
    assert(eq(r1, r2));

    r1 = atan2(sqrt(add(i5, mul(i2, sqrt(i5)))), im1);
    r2 = div(mul(pi, im2), i5);
    assert(eq(r1, r2));

    r1 = atan2(y, x)->diff(x);
    r2 = div(mul(im1, y), add(pow(x, i2), pow(y, i2)));
    assert(eq(r1, r2));

    r1 = atan2(y, x)->diff(y);
    r2 = div(x, add(pow(x, i2), pow(y, i2)));
    assert(eq(r1, r2));
}

void test_lambertw()
{
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> im2 = integer(-2);
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = lambertw(zero);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = lambertw(exp(im1));
    r2 = im1;
    assert(eq(r1, r2));

    r1 = lambertw(div(log(i2), im2));
    r2 = log(div(one, i2));
    assert(eq(r1, r2));

    r1 = lambertw(x)->diff(y);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = lambertw(mul(i2, x))->diff(x);
    r2 = div(lambertw(mul(i2, x)), mul(x, add(lambertw(mul(i2, x)), one)));
    assert(eq(r1, r2));
}

void test_sinh()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = sinh(zero);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = sinh(im1);
    r2 = mul(im1, sinh(one));
    assert(eq(r1, r2));

    r1 = sinh(x)->expand_as_exp();
    r2 = div(add(exp(x), mul(im1, exp(mul(im1, x)))), i2);
    assert(eq(r1, r2));
    // tests cosh(-x) = cosh(x) and sinh(x)->diff(x) = cosh(x)
    r1 = sinh(mul(im1, x))->diff(x);
    r2 = mul(im1, cosh(x));
    assert(eq(r1, r2));

}

void test_cosh()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = cosh(zero);
    r2 = one;
    assert(eq(r1, r2));

    r1 = cosh(im1);
    r2 = cosh(one);
    assert(eq(r1, r2));

    r1 = cosh(x)->expand_as_exp();
    r2 = div(add(exp(x), exp(mul(im1, x))), i2);
    assert(eq(r1, r2));
    // tests sinh(-x) = -sinh(x) and cosh(x)->diff(x) = sinh(x)
    r1 = cosh(mul(im1, x))->diff(x);
    r2 = mul(im1, sinh(mul(im1, x)));
    assert(eq(r1, r2));
}

void test_tanh()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = tanh(zero);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = tanh(im1);
    r2 = mul(im1, tanh(one));
    assert(eq(r1, r2));

    r1 = tanh(x)->expand_as_exp();
    r2 = div(sub(exp(x), exp(mul(im1, x))), add(exp(x), exp(mul(im1, x))));
    assert(eq(r1, r2));

    r1 = tanh(mul(im1, x))->diff(x);
    r2 = add(pow(tanh(x), i2), im1);
    std::cout<<*r1<<std::endl;
    std::cout<<*r2<<std::endl;
    // assert(eq(r1, r2));
}

void test_coth()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> im2 = integer(-2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = coth(im1);
    r2 = mul(im1, coth(one));
    assert(eq(r1, r2));

    r1 = coth(x)->expand_as_exp();
    r2 = div(add(exp(x), exp(mul(im1, x))), sub(exp(x), exp(mul(im1, x))));
    assert(eq(r1, r2));

    r1 = coth(mul(im1, x))->diff(x);
    r2 = pow(sinh(x), im2);
    assert(eq(r1, r2));
}

void test_asinh()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = asinh(zero);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = asinh(one);
    r2 = log(add(sqrt(i2), one));
    assert(eq(r1, r2));

    r1 = asinh(im1);
    r2 = log(add(sqrt(i2), im1));
    assert(eq(r1, r2));

    r1 = asinh(mul(im1, x))->diff(x);
    r2 = div(im1, sqrt(add(pow(x, i2), one)));
    assert(eq(r1, r2));
}

void test_acosh()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acosh(one);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = acosh(mul(im1, x))->diff(x);
    r2 = div(im1, sqrt(add(pow(x, i2), im1)));
    assert(eq(r1, r2));
}

void test_atanh()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = atanh(zero);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = atanh(mul(im1, x))->diff(x);
    r2 = div(im1, sub(one, pow(x, i2)));
    assert(eq(r1, r2));

    r1 = atanh(mul(im1, x));
    r2 = mul(im1, atanh(x));
    assert(eq(r1, r2));
}

void test_acoth()
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = acoth(mul(im1, x))->diff(x);
    r2 = div(im1, sub(one, pow(x, i2)));
    assert(eq(r1, r2));

    r1 = acoth(mul(im1, x));
    r2 = mul(im1, acoth(x));
    assert(eq(r1, r2));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();    
    test_sin();
    test_cos();
    test_tan();
    test_cot();
    test_csc();
    test_sec();
    test_f();
    test_Derivative();
    test_get_pi_shift();
    test_sin_table();
    test_could_extract_minus();
    test_asin();
    test_acos();
    test_asec();
    test_acsc();
    test_atan();
    test_acot();
    test_atan2();
    test_lambertw();
    test_sinh();
    test_cosh();
    test_tanh();
    test_coth();
    test_asinh();
    test_acosh();
    test_atanh();
    test_acoth();
    return 0;
}
