#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>


using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Pow;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::umap_basic_num;
using SymEngine::map_vec_int;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::multinomial_coefficients;
using SymEngine::RCP;
using SymEngine::rcp_dynamic_cast;

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i60 = integer(60);

    RCP<const Basic> e, r;

    e = pow(add(add(add(x, y), z), w), i60);

    std::cout << "Expanding: " << *e << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    r = expand(e);
    auto t2 = std::chrono::high_resolution_clock::now();
    //std::cout << *r << std::endl;
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
    std::cout << "number of terms: "
        << rcp_dynamic_cast<const Add>(r)->dict_.size() << std::endl;

    return 0;
}
