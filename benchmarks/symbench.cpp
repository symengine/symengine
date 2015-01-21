#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "constants.h"
#include "functions.h"

using CSymPy::Basic;
using CSymPy::RCP;
using CSymPy::rcp_static_cast;
using CSymPy::RCPBasicKeyLess;
using CSymPy::vec_basic;
using CSymPy::integer;
using CSymPy::Integer;
using CSymPy::symbol;
using CSymPy::Symbol;
using CSymPy::sin;
using CSymPy::cos;
using CSymPy::I;
using CSymPy::pi;
using CSymPy::add;
using CSymPy::mul;
using CSymPy::pow;
using CSymPy::div;
using CSymPy::one;
using CSymPy::expand;
using CSymPy::sub;
using CSymPy::zero;

double R1();
double R2();
double R3();
double R5();
double S1();
double S2();
double S3();
double S3a();

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();
    std::cout << R1() << "us" <<std::endl;
    std::cout << R2() << "us" <<std::endl;
    std::cout << R3() << "us" <<std::endl;
    std::cout << R5() << "us" <<std::endl;
    std::cout << S1() << "us" <<std::endl;
    std::cout << S2() << "us" <<std::endl;
    std::cout << S3() << "us" <<std::endl;
    std::cout << S3a() << "us" <<std::endl;

    return 0;
}

RCP<const Basic> f(RCP<const Basic> z) {
    return add(mul(sqrt(div(one, integer(3))), pow(z, integer(2))), div(I, integer(3)));
}

double R1()
{
    RCP<const Basic> g;
    RCP<const Basic> h = div(I, integer(2));
    auto t1 = std::chrono::high_resolution_clock::now();
    g = expand(f(f(f(f(f(f(f(f(f(f(h)))))))))));
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
}

RCP<const Basic> hermite(RCP<const Integer> n, RCP<const Basic> y)
{
    if (eq(n, one)) return mul(y, integer(2));
    if (eq(n, zero)) return one;
    return expand(sub(mul(mul(integer(2), y), hermite(n->subint(*one), y)), 
        mul(integer(2), mul(n->subint(*one), hermite(n->subint(*integer(2)), y)))));
}

double R2()
{
    RCP<const Basic> g;
    RCP<const Integer> n = integer(15);
    RCP<const Basic> y = symbol("y");
    auto t1 = std::chrono::high_resolution_clock::now();
    g = hermite(n, y);
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
}

double R3()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> f = add(x, add(y, z));
    std::vector<bool> vec(10);
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        vec.push_back(eq(f, f));
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
}

double R5()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> f = add(x, add(y, z));
    vec_basic v;

    v.push_back(x);
    v.push_back(y);
    v.push_back(z);
    for (int i = 0; i < 8; i++) {
        v.push_back(add(v[i], add(v[i + 1], v[i + 2])));
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    std::set<RCP<const Basic>, RCPBasicKeyLess> s(v.begin(), v.end());
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
}

double S1()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> e;
    RCP<const Basic> f;

    e = pow(add(x, add(y, add(z, one))), integer(7));
    f = mul(e, add(e, one));

    auto t1 = std::chrono::high_resolution_clock::now();
    f = expand(f);
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
}

double S2()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> e;
    RCP<const Basic> f;

    e = pow(add(pow(x, sin(x)), add(pow(y, cos(y)), pow(z, add(x, y)))), integer(100));

    auto t1 = std::chrono::high_resolution_clock::now();
    f = expand(e);
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
}

double S3()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> e;
    RCP<const Basic> f;

    e = pow(add(pow(x, y), add(pow(y, z), pow(z, x))), integer(50));
    e = expand(e);

    auto t1 = std::chrono::high_resolution_clock::now();
    f = e->diff(rcp_static_cast<const Symbol>(x));
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
}

double S3a()
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> e;
    RCP<const Basic> f;

    e = pow(add(pow(x, y), add(pow(y, z), pow(z, x))), integer(500));
    e = expand(e);

    auto t1 = std::chrono::high_resolution_clock::now();
    f = e->diff(rcp_static_cast<const Symbol>(x));
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
}
