#include <iostream>
#include <chrono>

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/polys.h>
#include <symengine/rings.h>
#include <symengine/monomials.h>

using SymEngine::Basic;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::umap_basic_num;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::expr2poly;
using SymEngine::poly_mul;
using SymEngine::umap_vec_mpz;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;
using SymEngine::Polynomial;
using SymEngine::make_rcp;
using SymEngine::mul_poly;

int main(int argc, char* argv[])
{
    print_stack_on_segfault();
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i15 = integer(15);

    RCP<const Basic> e, f1, f2, r;

    e = pow(add(add(add(x, y), z), w), i15);
    f1 = expand(e);
    f2 = expand(add(e, w));

    umap_basic_num syms;
    insert(syms, x, integer(0));
    insert(syms, y, integer(1));
    insert(syms, z, integer(2));
    insert(syms, w, integer(3));

    RCP<const Polynomial> P1 = make_rcp<const Polynomial>(f1, syms);
    RCP<const Polynomial> P2 = make_rcp<const Polynomial>(f2, syms);

    std::cout << "poly_mul start" << std::endl;
    // auto t1 = std::chrono::high_resolution_clock::now();
    RCP<const Polynomial> P3 = mul_poly(*P1, *P2);
    // auto t2 = std::chrono::high_resolution_clock::now();
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
    // std::cout
    //     << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
    //     << "ms" << std::endl;
    std::cout << "number of terms: "
        << P3->polys_set_.size() << std::endl;



    return 0;
}
