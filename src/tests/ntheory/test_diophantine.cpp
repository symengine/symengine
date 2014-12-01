#include <algorithm>

#include "diophantine.h"
#include "matrix.h"
#include "integer.h"

using CSymPy::print_stack_on_segfault;
using CSymPy::DenseMatrix;
using CSymPy::integer;
using CSymPy::homogeneous_lde;

bool vec_dense_matrix_eq_perm(const std::vector<DenseMatrix> &a,
    const std::vector<DenseMatrix> &b) {

    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over elements in "a"
    for (size_t i = 0; i < a.size(); i++) {
        // Find the element a[i] in "b"
        bool found = false;
        for (size_t j = 0; j < a.size(); j++) {
            if (a[i] == b[j]) {
                found = true;
                break;
            }
        }
        // If not found, then a != b
        if (!found) return false;
    }
    // If all elements were found, then a == b
    return true;
}

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
