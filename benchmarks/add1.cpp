#include <iostream>
#include <chrono>

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>

using SymEngine::Add;
using SymEngine::Basic;
using SymEngine::integer;
using SymEngine::is_a;
using SymEngine::Mul;
using SymEngine::multinomial_coefficients;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::rcp_static_cast;
using SymEngine::Symbol;
using SymEngine::symbol;

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();

    RCP<const Basic> x = symbol("x");
    RCP<const Basic> a, c;
    int N;

    N = 3000;
    a = x;
    c = integer(1);
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        a = add(a, mul(c, pow(x, integer(i))));
        c = mul(c, integer(-1));
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    // std::cout << *a << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;
    assert(is_a<Add>(*a));
    std::cout << "number of terms: "
              << rcp_static_cast<const Add>(a)->get_dict().size() << std::endl;

    return 0;
}
