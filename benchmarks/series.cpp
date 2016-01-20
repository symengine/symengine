#include <iostream>
#include <chrono>

#include <symengine/series_generic.h>

using SymEngine::Basic;
using SymEngine::Symbol;
using SymEngine::UnivariateSeries;
using SymEngine::symbol;
using SymEngine::map_vec_int;
using SymEngine::integer;
using SymEngine::RCP;
using SymEngine::rcp_dynamic_cast;

int main(int argc, char* argv[])
{
    SymEngine::print_stack_on_segfault();

    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariateSeries> a, c;
    std::vector<mpz_class> v;
    int N;

    N = 1000;
    for (int i=0; i<N; ++i) {
        mpz_class coef(i);
        v.push_back(coef);
    }
    a = UnivariateSeries::create(x, N, v);
    auto t1 = std::chrono::high_resolution_clock::now();
    c = mul_uni_series(*a, *a);
    auto t2 = std::chrono::high_resolution_clock::now();
    //std::cout << *a << std::endl;
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;

    return 0;
}
