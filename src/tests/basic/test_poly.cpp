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
#include "rings.h"
#include "monomials.h"

using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Pow;
using CSymPy::Symbol;
using CSymPy::Dict_int;
using CSymPy::map_vec_int;
using CSymPy::Integer;
using CSymPy::multinomial_coefficients;
using CSymPy::map_vec_mpz;
using CSymPy::expr2poly;
using CSymPy::vec_int;
using CSymPy::monomial_mul;
using CSymPy::poly_mul;
using CSymPy::umap_vec_mpz;

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
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i4 = rcp(new Integer(2));

    RCP<Basic> e, f1, f2, r;

    e = pow(add(add(add(x, y), z), w), i4);
    f1 = expand(e);
    f2 = expand(add(e, w));

    Dict_int syms;
    syms[x] = rcp(new Integer(0));
    syms[y] = rcp(new Integer(1));
    syms[z] = rcp(new Integer(2));
    syms[w] = rcp(new Integer(3));

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
    Teuchos::print_stack_on_segfault();

    test_monomial_mul();
    test_expand();

    return 0;
}
