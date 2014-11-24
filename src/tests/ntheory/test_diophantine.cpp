#include <algorithm>

#include "diophantine.h"
#include "matrix.h"
#include "integer.h"

using CSymPy::print_stack_on_segfault;
using CSymPy::DenseMatrix;
using CSymPy::integer;
using CSymPy::homogeneous_lde;

void test_homogeneous_lde()
{
    DenseMatrix A = DenseMatrix(2, 4, {
        integer(-1), integer(1), integer(2), integer(-3),
        integer(-1), integer(3), integer(-2), integer(-1)});

    std::vector<DenseMatrix> basis;
    homogeneous_lde(basis, A);

    assert(basis.size() == 2);
    assert(std::find(basis.begin(), basis.end(),
        DenseMatrix(1, 4, {integer(0), integer(1), integer(1), integer(1)})) != basis.end());
    assert(std::find(basis.begin(), basis.end(),
        DenseMatrix(1, 4, {integer(4), integer(2), integer(1), integer(0)})) != basis.end());

    basis.clear();
    A = DenseMatrix(1, 4, {integer(-1), integer(1), integer(2), integer(-3)});
    homogeneous_lde(basis, A);

    assert(basis.size() == 6);
    assert(std::find(basis.begin(), basis.end(),
        DenseMatrix(1, 4, {integer(0), integer(0), integer(3), integer(2)})) != basis.end());
    assert(std::find(basis.begin(), basis.end(),
        DenseMatrix(1, 4, {integer(0), integer(1), integer(1), integer(1)})) != basis.end());
    assert(std::find(basis.begin(), basis.end(),
        DenseMatrix(1, 4, {integer(0), integer(3), integer(0), integer(1)})) != basis.end());
    assert(std::find(basis.begin(), basis.end(),
        DenseMatrix(1, 4, {integer(1), integer(0), integer(2), integer(1)})) != basis.end());
    assert(std::find(basis.begin(), basis.end(),
        DenseMatrix(1, 4, {integer(2), integer(0), integer(1), integer(0)})) != basis.end());
    assert(std::find(basis.begin(), basis.end(),
        DenseMatrix(1, 4, {integer(1), integer(1), integer(0), integer(0)})) != basis.end());
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_homogeneous_lde();

    return 0;
}
