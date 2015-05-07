#include <iostream>
#include <chrono>

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "rings.h"
#include "monomials.h"

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Pow;
using SymEngine::Symbol;
using SymEngine::umap_basic_num;
using SymEngine::map_vec_int;
using SymEngine::Integer;
using SymEngine::multinomial_coefficients;
using SymEngine::map_vec_mpz;
using SymEngine::expr2poly;
using SymEngine::vec_int;
using SymEngine::monomial_mul;
using SymEngine::poly_mul;
using SymEngine::umap_vec_mpz;
using SymEngine::RCP;
using SymEngine::rcp;
using SymEngine::rcp_dynamic_cast;
using SymEngine::print_stack_on_segfault;

void test_monomial_mul()
{
    vec_int a, b, c, d;
    a = {1, 2, 3, 4};
    b = {2, 3, 2, 5};
    c = {0, 0, 0, 0};

    monomial_mul(a, b, c);

    d = {3, 5, 5, 9};
    assert(c == d);
    d = {5, 6, 5, 5};
    assert(c != d);

    umap_vec_mpz m;
    m[a] = 4;
}

void test_expand()
{
    RCP<const Basic> x = rcp(new Symbol("x"));
    RCP<const Basic> y = rcp(new Symbol("y"));
    RCP<const Basic> z = rcp(new Symbol("z"));
    RCP<const Basic> w = rcp(new Symbol("w"));
    RCP<const Basic> i4 = rcp(new Integer(2));

    RCP<const Basic> e, f1, f2, r;

    e = pow(add(add(add(x, y), z), w), i4);
    f1 = expand(e);
    f2 = expand(add(e, w));

    umap_basic_num syms;
    insert(syms, x, rcp(new Integer(0)));
    insert(syms, y, rcp(new Integer(1)));
    insert(syms, z, rcp(new Integer(2)));
    insert(syms, w, rcp(new Integer(3)));

    umap_vec_mpz P1, P2, C;

    expr2poly(f1, syms, P1);
    expr2poly(f2, syms, P2);
    std::cout << "poly_mul start" << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    poly_mul(P1, P2, C);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "poly_mul stop" << std::endl;


    /*
    std::cout << *e << std::endl;
    std::cout << *f1 << std::endl;
    std::cout << P1 << std::endl;
    std::cout << *f2 << std::endl;
    std::cout << P2 << std::endl;
    std::cout << "RESULT:" << std::endl;
    std::cout << C << std::endl;
    */
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_monomial_mul();
    test_expand();

    return 0;
}
