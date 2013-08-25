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

using CSymPy::Basic;
using CSymPy::Symbol;
using CSymPy::umap_basic_int;
using CSymPy::Integer;
using CSymPy::expr2poly;
using CSymPy::poly_mul;
using CSymPy::umap_vec_mpz;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::print_stack_on_segfault;

int main(int argc, char* argv[])
{
    print_stack_on_segfault();
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> w = rcp(new Symbol("w"));
    RCP<Basic> i15 = rcp(new Integer(15));

    RCP<Basic> e, f1, f2, r;

    e = pow(add(add(add(x, y), z), w), i15);
    f1 = expand(e);
    f2 = expand(add(e, w));

    umap_basic_int syms;
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
    std::cout << "number of terms: "
        << C.size() << std::endl;



    return 0;
}
