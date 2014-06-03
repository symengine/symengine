#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "integer.h"
#include "matrix.h"
#include "symbol.h"

using CSymPy::Basic;
using CSymPy::Integer;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::integer;
using CSymPy::DenseMatrix;
using CSymPy::densematrix;
using CSymPy::symbol;

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    RCP<const DenseMatrix> A, B, C;

    A = densematrix(3, 3, {symbol("a"), symbol("b"), symbol("c"), symbol("d"), 
        symbol("e"), symbol("f"), symbol("g"), symbol("h"), symbol("i")});

    B = densematrix(3, 3, {symbol("x"), symbol("y"), symbol("z"), symbol("p"), 
        symbol("q"), symbol("r"), symbol("u"), symbol("v"), symbol("w")});

    std::cout << "Adding Two Matrices; matrix dimensions: 4 x 4" << std::endl;

    // We are taking an average time since time for a single addition varied in
    // a range of 40-50 microseconds
    unsigned N = 10000;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (unsigned i = 0; i < N; i++)
        C = add_dense_dense(*A, *B);
    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout
        << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()/N
        << " microseconds" << std::endl;

    return 0;
}
