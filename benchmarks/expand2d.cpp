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
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::umap_basic_num;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::expr2poly;
using SymEngine::poly_mul;
using SymEngine::poly_mul2;
using SymEngine::poly_mul3;
using SymEngine::packed2poly;
using SymEngine::poly2packed;
using SymEngine::packed2hashset;
using SymEngine::hashset2packed;
using SymEngine::umap_vec_mpz;
using SymEngine::umap_ull_mpz;
using SymEngine::hash_set;
using SymEngine::RCP;
using SymEngine::rcp;
using SymEngine::print_stack_on_segfault;

int main(int argc, char* argv[])
{
    print_stack_on_segfault();
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i15 = integer(15);

    RCP<const Basic> e, f1, f2;

    e = pow(add(add(add(x, y), z), w), i15);
    f1 = expand(e);
    f2 = expand(add(e, w));

    umap_basic_num syms;
    insert(syms, x, integer(0));
    insert(syms, y, integer(1));
    insert(syms, z, integer(2));
    insert(syms, w, integer(3));

    umap_vec_mpz P1, P2, C;

    expr2poly(f1, syms, P1);
    expr2poly(f2, syms, P2);

    umap_ull_mpz p, q, r;

    poly2packed(P1, p);
    poly2packed(P2, q);

    hash_set l, m, n;

    packed2hashset(p, l);
    packed2hashset(q, m);

    std::cout << "poly_mul start" << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    poly_mul3(l, m, n);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "poly_mul stop" << std::endl;

    hashset2packed(n, r);
    packed2poly(r, syms, C);

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
    std::cout << "number of terms: "
        << C.size() << std::endl;

    return 0;
}
