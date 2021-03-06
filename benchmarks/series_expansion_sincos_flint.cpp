#include <symengine/symengine_config.h>

#include <symengine/series_flint.h>

#include <iostream>
#include <chrono>

#include <symengine/functions.h>
#include <symengine/symbol.h>
#include <symengine/mul.h>
#include <symengine/series.h>

using SymEngine::add;
using SymEngine::Basic;
using SymEngine::cos;
using SymEngine::integer;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::RCP;
using SymEngine::rcp_dynamic_cast;
using SymEngine::series;
using SymEngine::sin;
using SymEngine::Symbol;
using SymEngine::symbol;

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();

    RCP<const Symbol> x = symbol("x");
    int N = 1000;
    auto arg = add(x, pow(x, integer(2)));
    auto ex = mul(sin(arg), cos(arg));

    auto t1 = std::chrono::high_resolution_clock::now();
    auto res = SymEngine::URatPSeriesFlint::series(ex, "x", N);
    auto t2 = std::chrono::high_resolution_clock::now();
    // std::cout << *res[N-1] << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    return 0;
}
