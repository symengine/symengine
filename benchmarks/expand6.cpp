#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "mul.h"
#include "pow.h"
#include "integer.h"
#include "constants.h"

using CSymPy::Basic;
using CSymPy::RCP;
using CSymPy::symbol;
using CSymPy::zero;
using CSymPy::map_basic_basic;
using CSymPy::sqrt;
using CSymPy::integer;

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();
    int N;
    if (argc == 2) {
        N = std::atoi(argv[1]);
    } else {
        N = 100;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    RCP<const Basic> e, f, s, a0, a1;
    a0 = symbol("a0");
    a1 = symbol("a1");
    e = add(a0, a1);
    f = zero;
    for (int i = 2; i < N; i++) {
        s = symbol("a" + std::to_string(i));
        e = add(e, s);
        f = sub(f, s);
    }
    e = expand(mul(e, e));
    map_basic_basic dict;
    insert(dict, a0, f);
    e = e->subs(dict);

    e = expand(e);

    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
    std::cout << e->__str__() << std::endl;

    return 0;
}
