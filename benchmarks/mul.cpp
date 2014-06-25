#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "mul.h"
#include "integer.h"
#include "pow.h"

using CSymPy::Basic;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::mul;
using CSymPy::integer;
using CSymPy::pow;

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    RCP<const Basic> mhalf = div(integer(-1), integer(2));
    RCP<const Basic> r1 = mul(integer(12), pow(integer(196), mhalf));
    RCP<const Basic> r2 = mul(integer(294), pow(integer(196), mhalf));

    std::cout << "Multiplying 12*196^(-1/2) * 294*196^(-1/2)" << std::endl;

    unsigned N = 10000;
    auto t1 = std::chrono::high_resolution_clock::now();

    for (unsigned i = 0; i < N; i++)
        mul(r1, r2);

    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout
        << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()/N
        << " microseconds" << std::endl;

    return 0;
}
