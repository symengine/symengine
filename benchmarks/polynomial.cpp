#include <iostream>
#include <chrono>
#include <functional>
#include <symengine/basic.h>
#include <symengine/polynomial.h>
#include <symengine/dict.h>
#include <random>
#include <ctime>

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Pow;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::RCP;
using SymEngine::UnivariateIntPolynomial;
using SymEngine::map_uint_mpz;
using SymEngine::eq;

// sparseness;      // number of terms in dict / degree
// coef_len;        // in bits
// poly_len;        // number of terms

void benchmark_multiplication(double sparseness, int coef_len, int poly_len)
{
    int t1 = 0, t2 = 0;
    int iterations;
    if (poly_len < 201)
        iterations = 50;
    else if (poly_len < 501)
        iterations = 25;
    else if (poly_len < 1001)
        iterations = 10;
    else if (poly_len < 3001)
        iterations = 5;
    else
        iterations = 3;

    for (int i = 0; i < iterations; ++i) {
        int deg = ((double)(poly_len - 1)) / sparseness + 1;
        int max_coeff = (1 << (coef_len + 1)) - 1;
        int min_coeff = 1 << coef_len;
        int diff = max_coeff - min_coeff;
        srand(time(NULL));

        std::vector<int> nums(deg - 1);
        for (unsigned int i = 0; i < nums.size(); ++i)
            nums[i] = i;
        std::shuffle(nums.begin(), nums.end(),
                     std::default_random_engine(time(0)));

        map_uint_mpz a_dict, b_dict;
        for (int i = 0; i < poly_len - 1; ++i)
            a_dict[nums[i]]
                = ((rand() % 2 == 0) ? 1 : -1) * ((rand() % diff) + min_coeff);
        a_dict[deg]
            = ((rand() % 2 == 0) ? 1 : -1) * ((rand() % diff) + min_coeff);

        std::shuffle(nums.begin(), nums.end(),
                     std::default_random_engine(time(0)));
        for (int i = 0; i < poly_len - 1; ++i)
            b_dict[nums[i]]
                = ((rand() % 2 == 0) ? 1 : -1) * ((rand() % diff) + min_coeff);
        b_dict[deg]
            = ((rand() % 2 == 0) ? 1 : -1) * ((rand() % diff) + min_coeff);

        RCP<const Symbol> x = symbol("x");
        UnivariateIntPolynomial a(x, deg, std::move(a_dict));
        UnivariateIntPolynomial b(x, deg, std::move(b_dict));

        auto ts = std::chrono::high_resolution_clock::now();
        RCP<const UnivariateIntPolynomial> c = mul_poly(a, b);
        auto te = std::chrono::high_resolution_clock::now();

        t1 += std::chrono::duration_cast<std::chrono::microseconds>(te - ts)
                  .count();

        ts = std::chrono::high_resolution_clock::now();
        RCP<const UnivariateIntPolynomial> d = mul_poly_ks2(a, b);
        te = std::chrono::high_resolution_clock::now();

        t2 += std::chrono::duration_cast<std::chrono::microseconds>(te - ts)
                  .count();
    }
    float ratio = ((float)t2) / ((float)t1);
    printf("%.1f %d %d %.6f\n", sparseness, coef_len, poly_len, ratio);

    return;
};

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();

    double sparsities[4] = {0.25, 0.5, 0.75, 1};
    int coeffs[4] = {5, 10, 15, 20};

    std::vector<int> lens;
    for (int i = 1; i < 251; i = i + 1)
        lens.push_back(i);
    for (int i = 253; i < 501; i = i + 3)
        lens.push_back(i);
    for (int i = 525; i < 1001; i = i + 25)
        lens.push_back(i);
    for (int i = 1050; i < 3001; i = i + 50)
        lens.push_back(i);
    for (int i = 3050; i < 7501; i = i + 150)
        lens.push_back(i);
    for (int i = 7500; i < 10001; i = i + 250)
        lens.push_back(i);

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            for (int k = 0; k < lens.size(); ++k)
                benchmark_multiplication(sparsities[i], coeffs[j], lens[k]);
    return 0;
}
