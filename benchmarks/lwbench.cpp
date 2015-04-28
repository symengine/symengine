#include <iostream>
#include <chrono>
#include <cstdlib>

#include "Teuchos_stacktrace.hpp"

#include "ntheory.h"
#include "mul.h"
#include "integer.h"
#include "basic.h"
#include "constants.h"
#include "add.h"
#include "pow.h"
#include "symbol.h"

using CSymPy::div;
using CSymPy::factorial;
using CSymPy::RCP;
using CSymPy::Basic;
using CSymPy::one;
using CSymPy::add;
using CSymPy::Integer;
using CSymPy::integer;
using CSymPy::gcd;
using CSymPy::rcp_static_cast;
using CSymPy::pow;
using CSymPy::symbol;

double A()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= 100; i++) {
        div(factorial(1000 + i), factorial(900 + i));
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
}


double B()
{
    RCP<const Basic> s = integer(0);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= 1000; i++) {
        s = add(s, div(one, integer(i)));
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
}

double C()
{
    RCP<const Integer> x = integer(13*17*31);
    RCP<const Integer> y = integer(13*19*29);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= 200; i++) {
        gcd(*rcp_static_cast<const Integer>(pow(x, integer(300 + i%181))),
            *rcp_static_cast<const Integer>(pow(y, integer(200 + i%183))));
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
}

double D()
{
    RCP<const Basic> s = integer(0);
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> t = symbol("t");

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= 1000; i++) {
        s = add(s, div(mul(integer(i), mul(y, pow(t, integer(i)))),
                    pow(add(y, mul(integer(i), t)), integer(i))));
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
}

double E()
{
    RCP<const Basic> s = integer(0);
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> t = symbol("t");

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= 1000; i++) {
        s = add(s, div(mul(integer(i), mul(y, pow(t, integer(i)))),
                    pow(add(y, mul(integer(abs(5 - i)), t)), integer(i))));
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()/1000000000.0;
}

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    std::cout << "Time for A : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << A() << std::endl;
    std::cout << "Time for B : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << B() << std::endl;
    std::cout << "Time for C : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << C() << std::endl;
    std::cout << "Time for D : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << D() << std::endl;
    std::cout << "Time for E : \t " << std::setw(15) << std::setprecision(9)  << std::fixed << E() << std::endl;

    return 0;
}
