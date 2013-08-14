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
using CSymPy::umap_basic_int;
using CSymPy::map_vec_int;
using CSymPy::Integer;
using CSymPy::integer;
using CSymPy::multinomial_coefficients;
using CSymPy::one;
using CSymPy::zero;

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

    r1 = add(x, one);
    r2 = add(one, x);
    assert(eq(r1, r2));

    r1 = add(pow(x, y), z);
    r2 = add(z, pow(x, y));
    assert(eq(r1, r2));
}

void test_mul()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1, r2;

    r1 = mul(pow(x, y), z);
    r2 = mul(z, pow(x, y));
    assert(eq(r1, r2));

    r1 = mul(mul(x, y), mul(y, x));
    r2 = mul(pow(x, i2), pow(y, i2));
    assert(eq(r1, r2));

    r1 = mul(pow(x, add(y, z)), z);
    r2 = mul(z, pow(x, add(z, y)));
    assert(eq(r1, r2));

    r1 = mul(pow(x, y), pow(x, z));
    r2 = pow(x, add(y, z));
    assert(eq(r1, r2));

    r1 = mul(mul(pow(x, y), pow(x, z)), pow(x, x));
    r2 = pow(x, add(add(x, y), z));
    assert(eq(r1, r2));

    r1 = mul(mul(mul(pow(x, y), pow(x, z)), pow(x, x)), y);
    r2 = mul(pow(x, add(add(x, y), z)), y);
    assert(eq(r1, r2));
}

void test_sub()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> im1 = rcp(new Integer(-1));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1, r2;

    r1 = sub(i3, i2);
    r2 = one;
    assert(eq(r1, r2));

    r1 = sub(x, x);
    r2 = zero;
    assert(eq(r1, r2));

    r1 = sub(mul(i2, x), x);
    r2 = x;
    assert(eq(r1, r2));

    r1 = add(mul(mul(i2, x), y), mul(x, y));
    r2 = mul(i3, mul(x, y));
    assert(eq(r1, r2));

    r1 = add(mul(mul(i2, x), y), mul(x, y));
    r2 = mul(mul(x, y), i3);
    assert(eq(r1, r2));

    r1 = sub(mul(mul(i2, x), y), mul(x, y));
    r2 = mul(x, y);
    assert(eq(r1, r2));

    r1 = sub(add(x, one), x);
    r2 = one;
    assert(eq(r1, r2));

    r1 = add(add(x, one), add(x, i2));
    r2 = add(mul(i2, x), i3);
    assert(eq(r1, r2));

    r1 = sub(add(x, one), add(x, i2));
    r1 = expand(r1);
    r2 = im1;
    assert(eq(r1, r2));
}

void test_div()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> im1 = rcp(new Integer(-1));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    assert(integer(2)->is_positive());
    assert(integer(0)->is_zero());
    assert(integer(1)->is_one());
    assert(!(integer(-1)->is_positive()));
    assert(integer(-1)->is_negative());

    RCP<Basic> r1, r2;

    r1 = pow(i3, i2);
    r2 = integer(9);
    assert(eq(r1, r2));

    r1 = div(i4, i2);
    r2 = i2;
    assert(eq(r1, r2));

    r1 = div(x, x);
    r2 = one;
    assert(eq(r1, r2));

    r1 = div(mul(i2, x), x);
    r2 = i2;
    assert(eq(r1, r2));

    r1 = div(pow(x, i2), x);
    r2 = x;
    assert(eq(r1, r2));

    r1 = div(mul(mul(i2, x), y), mul(x, y));
    r2 = i2;
    std::cout << "r1: " << *r1 << std::endl;
    std::cout << "r2: " << *r2 << std::endl;
    assert(eq(r1, r2));

    r1 = div(mul(mul(y, x), i2), mul(x, y));
    r2 = i2;
    assert(eq(r1, r2));

    r1 = div(mul(x, i2), x);
    r2 = i2;
    assert(eq(r1, r2));

    r1 = div(mul(x, i4), mul(x, i2));
    r2 = i2;
    assert(eq(r1, r2));
}

void test_pow()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> im1 = rcp(new Integer(-1));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> im3 = rcp(new Integer(-3));
    RCP<Basic> i4 = rcp(new Integer(4));
    RCP<Basic> i6 = rcp(new Integer(6));
    RCP<Basic> i9 = rcp(new Integer(9));
    RCP<Basic> i27 = rcp(new Integer(27));

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

    r1 = sub(pow(x, y), pow(x, y));
    r2 = zero;
    assert(eq(r1, r2));

    /* Test (x*y)^2 -> x^2*y^2 type of simplifications */

    r1 = pow(mul(x, y), i2);
    r2 = mul(pow(x, i2), pow(y, i2));
    assert(eq(r1, r2));

    r1 = pow(mul(i2, mul(x, y)), i2);
    r2 = mul(i4, mul(pow(x, i2), pow(y, i2)));
    assert(eq(r1, r2));

    r1 = pow(mul(i3, mul(x, y)), i2);
    r2 = mul(i9, mul(pow(x, i2), pow(y, i2)));
    assert(eq(r1, r2));

    r1 = pow(mul(i3, mul(x, y)), im1);
    r2 = mul(div(one, i3), mul(pow(x, im1), pow(y, im1)));
    assert(eq(r1, r2));

    r1 = pow(mul(i3, mul(pow(x, i2), pow(y, i3))), i2);
    r2 = mul(i9, mul(pow(x, i4), pow(y, i6)));
    assert(eq(r1, r2));

    r1 = pow(mul(i3, mul(pow(x, i2), pow(y, im1))), i3);
    r2 = mul(i27, mul(pow(x, i6), pow(y, im3)));
    assert(eq(r1, r2));
}

void test_multinomial()
{
    map_vec_int r;
    auto t1 = std::chrono::high_resolution_clock::now();
    multinomial_coefficients(4, 20, r);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
}

void test_expand1()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(10));

    RCP<Basic> r1;
    RCP<Basic> r2;

    r1 = pow(add(add(add(x, y), z), w), i4);

    std::cout << *r1 << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    r2 = expand(r1);
    auto t2 = std::chrono::high_resolution_clock::now();
    //std::cout << *r2 << std::endl;
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
    std::cout << "number of terms: "
        << rcp_dynamic_cast<Add>(r2)->dict_.size() << std::endl;
}

void test_expand2()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> im1 = rcp(new Integer(-1));
    RCP<Basic> im2 = rcp(new Integer(-2));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));
    RCP<Basic> i5 = rcp(new Integer(5));
    RCP<Basic> i6 = rcp(new Integer(6));
    RCP<Basic> i9 = rcp(new Integer(9));
    RCP<Basic> i12 = rcp(new Integer(12));
    RCP<Basic> i25 = rcp(new Integer(25));
    RCP<Basic> i30 = rcp(new Integer(30));

    RCP<Basic> r1;
    RCP<Basic> r2;

    r1 = mul(w, add(add(x, y), z)); // w*(x+y+z)
    std::cout << *r1 << std::endl;

    r2 = expand(r1);
    std::cout << *r2 << std::endl;

    assert( eq(r2, add(add(mul(w, x), mul(w, y)), mul(w, z))));
    assert(neq(r2, add(add(mul(w, x), mul(w, w)), mul(w, z))));

    r1 = mul(add(x, y), add(z, w)); // (x+y)*(z+w)
    std::cout << *r1 << std::endl;

    r2 = expand(r1);
    std::cout << *r2 << std::endl;

    assert( eq(r2, add(add(add(mul(x, z), mul(y, z)), mul(x, w)), mul(y, w))));
    assert(neq(r2, add(add(add(mul(y, z), mul(y, z)), mul(x, w)), mul(y, w))));

    r1 = mul(im1, add(x, i2));
    r1 = expand(r1);
    r2 = add(mul(im1, x), im2);
    assert(eq(r1, r2));

    r1 = pow(add(x, one), i2);
    r1 = expand(r1);
    r2 = add(add(pow(x, i2), mul(i2, x)), one);
    assert(eq(r1, r2));

    r1 = pow(add(x, i2), i2);
    r1 = expand(r1);
    r2 = add(add(pow(x, i2), mul(i4, x)), i4);
    assert(eq(r1, r2));

    r1 = pow(add(x, i3), i2);
    r1 = expand(r1);
    r2 = add(add(pow(x, i2), mul(i6, x)), i9);
    assert(eq(r1, r2));

    r1 = pow(add(mul(i3, x), i5), i2);
    r1 = expand(r1);
    r2 = add(add(mul(i9, pow(x, i2)), mul(i30, x)), i25);
    assert(eq(r1, r2));

    r1 = pow(add(mul(i2, pow(x, i2)), mul(i3, y)), i2);
    r1 = expand(r1);
    r2 = add(add(mul(i4, pow(x, i4)), mul(i12, mul(pow(x, i2), y))),
            mul(i9, pow(y, i2)));
    assert(eq(r1, r2));
}

void test_expand3()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i4 = rcp(new Integer(2));

    RCP<Basic> e, f, r;

    e = pow(add(add(add(x, y), z), w), i4);
    f = mul(e, add(e, w));

    std::cout << *f << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    r = expand(f);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << *r << std::endl;
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
    std::cout << "number of terms: "
        << rcp_dynamic_cast<Add>(r)->dict_.size() << std::endl;
}

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    test_add();
    test_mul();
    test_pow();
    test_sub();
    test_div();
    test_multinomial();
    test_expand1();
    test_expand2();
    test_expand3();

    return 0;
}
