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
using SymEngine::map_int_Expr;
using SymEngine::pow;

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();

    RCP<const Symbol> x = symbol("x");
    // std::vector<Expression> v, v2;
    map_int_Expr p, q;
    int N = 100000;
    int N2 = 1000;
    for (int i = 0; i < N; ++i) {
        // Expression coef(i);
        Expression coef(pow(x, integer(i)));
        // v.push_back(coef);
        p[i] = coef;
    }
    for (int j = 0; j < N2; ++j) {
        // Expression coef(j);
        Expression coef(pow(x, integer(j)));
        // v2.push_back(coef);
        q[j] = coef;
    }

    // UnivariateExprPolynomial c, p(UnivariatePolynomial::create(x, v));
    UnivariateExprPolynomial c, d, a, b, f, g, s, r, t, u, n, ep(p), epq(q);
    UnivariateExprPolynomial e({{0, Expression(7)}});
    Expression coeff(7);

    // ep += ep
    c = ep;
    auto t1 = std::chrono::high_resolution_clock::now();
    c += ep;
    // d = UnivariateSeries::mul(ep, ep, 1000);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "ep += ep "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // ep + ep
    t1 = std::chrono::high_resolution_clock::now();
    d = ep + ep;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "ep + ep "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // ep -= ep
    t = ep;
    t1 = std::chrono::high_resolution_clock::now();
    t -= ep;
    // d = UnivariateSeries::mul(ep, ep, 1000);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "ep -= ep "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // ep - ep
    t1 = std::chrono::high_resolution_clock::now();
    u = ep - ep;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "ep - ep "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // -ep
    t1 = std::chrono::high_resolution_clock::now();
    n = -ep;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "-ep "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // epq * epq
    t1 = std::chrono::high_resolution_clock::now();
    b = epq * epq;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "epq * epq "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // epq *= epq
    r = epq;
    t1 = std::chrono::high_resolution_clock::now();
    r *= epq;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "epq *= epq "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // ep * e single constant
    t1 = std::chrono::high_resolution_clock::now();
    s = ep * e;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "ep * e single constant "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // ep *= e single constant
    a = ep;
    t1 = std::chrono::high_resolution_clock::now();
    a *= e;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "ep *= e single constant "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    // ep / coeff
    t1 = std::chrono::high_resolution_clock::now();
    f = a / coeff;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "ep / coeff "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;
    // std::cout << f.get_dict().at(0) << " + " << f.get_dict().at(1) << " + "
    // << f.get_dict().at(2) << std::endl;

    // ep /= coeff
    g = a;
    t1 = std::chrono::high_resolution_clock::now();
    g /= coeff;
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "ep /= coeff "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;
    // std::cout << g.get_dict().at(0) << " + " << g.get_dict().at(1) << " + "
    // << g.get_dict().at(2) << std::endl;

    return 0;
}
