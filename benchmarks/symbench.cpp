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

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::rcp_static_cast;
using SymEngine::RCPBasicKeyLess;
using SymEngine::vec_basic;
using SymEngine::integer;
using SymEngine::Integer;
using SymEngine::symbol;
using SymEngine::Symbol;
using SymEngine::sin;
using SymEngine::cos;
using SymEngine::I;
using SymEngine::pi;
using SymEngine::add;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::div;
using SymEngine::one;
using SymEngine::expand;
using SymEngine::sub;
using SymEngine::zero;

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
    std::cout << "Time for R1 : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << R1() << std::endl;
    std::cout << "Time for R2 : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << R2() << std::endl;
    std::cout << "Time for R3 : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << R3() << std::endl;
    std::cout << "Time for R4 : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << R5() << std::endl;
    std::cout << "Time for S1 : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << S1() << std::endl;
    std::cout << "Time for S2 : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << S2() << std::endl;
    std::cout << "Time for S3 : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << S3() << std::endl;
    std::cout << "Time for S3a : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << S3a() << std::endl;

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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
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
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
}
