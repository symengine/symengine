#include <iostream>
#include <chrono>

#include "polynomial.h"

using SymEngine::Polynomial;
using SymEngine::polynomial;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;

void polynomial_constructor()
{   
    std::string x = "x";
    RCP<const Polynomial> P = polynomial(x, {{0, 1}, {1, 2}, {2, 1}});
    assert(P->degree == 2);
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    polynomial_constructor();
    return 0;
}
