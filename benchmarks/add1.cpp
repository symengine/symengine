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

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Pow;
using CSymPy::Symbol;
using CSymPy::map_vec_int;
using CSymPy::integer;
using CSymPy::multinomial_coefficients;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::rcp_dynamic_cast;

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    RCP<const Basic> x = rcp(new Symbol("x"));
    RCP<const Basic> a, c;
    int N;

    N = 3000; a = x; c = integer(1);
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        a = add(a, mul(c, pow(x, integer(i))));
        c = mul(c, integer(-1));
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    //std::cout << *a << std::endl;
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
    std::cout << "number of terms: "
        << rcp_dynamic_cast<const Add>(a)->dict_.size() << std::endl;

    return 0;
}
