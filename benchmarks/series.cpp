#include <iostream>
#include <chrono>

#include <symengine/series_generic.h>

using SymEngine::Basic;
using SymEngine::Symbol;
using SymEngine::UnivariateSeries;
using SymEngine::symbol;
using SymEngine::map_vec_int;
using SymEngine::integer;
using SymEngine::integer_class;
using SymEngine::RCP;
using SymEngine::rcp_dynamic_cast;
using SymEngine::Expression;
using SymEngine::UnivariatePolynomial;
using SymEngine::UnivariateExprPolynomial;

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();

    RCP<const Symbol> x = symbol("x");
    std::vector<Expression> v;
    int N;

    N = 1000;
    for (int i = 0; i < N; ++i) {
        Expression coef(i);
        v.push_back(coef);
    }

    UnivariateExprPolynomial c, p(UnivariatePolynomial::create(x, v));
    auto t1 = std::chrono::high_resolution_clock::now();
    c = UnivariateSeries::mul(p, p, 1000);
    auto t2 = std::chrono::high_resolution_clock::now();
    // std::cout << *a << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    return 0;
}
