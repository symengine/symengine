#include "diophantine.h"
#include "constants.h"
#include "basic.h"
#include "add.h"
#include "mul.h"

namespace CSymPy {

bool order(const DenseMatrix &t, const std::vector<DenseMatrix> &basis, unsigned k)
{
    bool eq = true;

    for (unsigned j = 0; j < t.ncols(); j++) {
        const Integer &t_ = static_cast<const Integer &>(*t.get(0, j));
        const Integer &b_ = static_cast<const Integer &>(*basis[k].get(0, j));

        if (t_.as_mpz() < b_.as_mpz()) {
            return false;
        }
        if (t_.as_mpz() > b_.as_mpz()) {
            eq = false;
        }
    }

    return !eq;
}

bool is_minimum(const DenseMatrix &t, const std::vector<DenseMatrix> &basis, unsigned n)
{
    if (n == 0) {
        return true;
    } else {
        return  !order(t, basis, n - 1) && is_minimum(t, basis, n - 1);
    }
}

// Solve the diophantine system Ax = 0 and return a basis set for solutions
// Reference:
// Evelyne Contejean, Herve Devie. An Efficient Incremental Algorithm for Solving
// Systems of Linear Diophantine Equations. Information and computation, 113(1):143-172,
// August 1994.
void homogeneous_lde(std::vector<DenseMatrix> &basis, const DenseMatrix &A)
{
    unsigned p = A.nrows();
    unsigned q = A.ncols();
    unsigned n;

    CSYMPY_ASSERT(p > 0 && q > 1);

    DenseMatrix row_zero = DenseMatrix(1, q);
    for (unsigned j = 0; j < q; j++) {
        row_zero.set(0, j, zero);
    }

    DenseMatrix col_zero = DenseMatrix(p, 1);
    for (unsigned i = 0; i < p; i++) {
        col_zero.set(i, 0, zero);
    }

    std::vector<DenseMatrix> P;
    P.push_back(row_zero);

    bool Frozen[q][q];
    for (unsigned j = 0; j < q; j++) {
        Frozen[0][j] = false;
    }
    for (unsigned i = 1; i < q; i++) {
        for (unsigned j = 0; j < q; j++) {
            Frozen[i][j] = true;
        }
    }

    bool F[q];
    for (unsigned i = 0; i < q; i++) {
        F[i] = false;
    }

    DenseMatrix t, transpose, product, T;
    RCP<const Integer> dot;

    product = DenseMatrix(p, 1);
    transpose = DenseMatrix(q, 1);

    while (P.size() > 0) {
        n = P.size() - 1;
        t = P[n];
        P.pop_back();

        t.transpose(transpose);
        A.mul_matrix(transpose, product);

        if (product.eq(col_zero) && !t.eq(row_zero)) {
            basis.push_back(t);
        } else {
            for (unsigned i = 0; i < q; i++) {
                F[i] = Frozen[n][i];
            }

            T = t;
            for (unsigned i = 0; i < q; i++) {
                T.set(0, i,
                    rcp_static_cast<const Integer>(T.get(0, i))->addint(*one));
                if (i > 0) {
                    T.set(0, i - 1,
                        rcp_static_cast<const Integer>(T.get(0, i - 1))->subint(*one));
                }

                dot = zero;
                for (unsigned j = 0; j < p; j++) {
                    const Integer &p_j0 = static_cast<const Integer &>(*product.get(j, 0));
                    const Integer &A_ji = static_cast<const Integer &>(*A.get(j, i));
                    dot = dot->addint(*p_j0.mulint(A_ji));
                }

                if (F[i] == false && ((dot->is_negative() && is_minimum(T, basis, basis.size()))
                        || t.eq(row_zero))) {
                    P.push_back(T);
                    n = n + 1;

                    for (unsigned j = 0; j < q; j++) {
                        Frozen[n - 1][j] = F[j];
                    }

                    F[i] = true;
                }
            }
        }
    }
}

}
