#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "integer.h"
#include "matrix.h"


using CSymPy::Basic;
using CSymPy::Integer;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::integer;
using CSymPy::DenseMatrix;

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    std::vector<RCP<const Basic>> l, m, n;
    RCP<const DenseMatrix> A, B, C;

    l = {integer(1), integer(2), integer(3), integer(4), integer(5), integer(6),
        integer(7), integer(8), integer(9), integer(10), integer(11), integer(12),
        integer(13), integer(14), integer(15), integer(16)};
    A = densematrix(4, 4, l);

    m = {integer(1), integer(2), integer(3), integer(4), integer(5), integer(6),
        integer(7), integer(8), integer(9), integer(10), integer(11), integer(12),
        integer(13), integer(14), integer(15), integer(16)};
    B = densematrix(4, 4, m);

    std::cout << "Adding Two Matrices" << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    C = add_dense_dense(*A, *B);
    auto t2 = std::chrono::high_resolution_clock::now();
    
    std::cout
        << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()
        << " microseconds" << std::endl;
    std::cout << "matrix dimensions: 4 x 4" << std::endl;

    return 0;
}
