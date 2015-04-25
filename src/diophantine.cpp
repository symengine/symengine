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
        CSYMPY_ASSERT(is_a<Integer>(*t.get(0, j)));
        mpz_class t_ = rcp_static_cast<const Integer>(t.get(0, j))->as_mpz();

        CSYMPY_ASSERT(is_a<Integer>(*basis[k].get(0, j)));
        mpz_class b_ = rcp_static_cast<const Integer>(basis[k].get(0, j))->as_mpz();

        if (t_ < b_) {
            return false;
        }
        if (t_ > b_) {
            eq = false;
        }
    }

    return !eq;
}

bool is_minimum(const DenseMatrix &t, const std::vector<DenseMatrix> &basis, unsigned n)
{
    if (n == 0) {
        return true;
    }

    return  !order(t, basis, n - 1) && is_minimum(t, basis, n - 1);
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

    DenseMatrix row_zero;
    zeros(row_zero, 1, q);

    DenseMatrix col_zero;
    zeros(col_zero, p, 1);

    std::vector<DenseMatrix> P;
    P.push_back(row_zero);

    std::vector<std::vector<bool>> Frozen(q, std::vector<bool>(q, true));
    for (unsigned j = 0; j < q; j++) {
        Frozen[0][j] = false;
    }

    std::vector<bool> F(q, false);

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
                CSYMPY_ASSERT(is_a<Integer>(*T.get(0, i)));
                T.set(0, i,
                    rcp_static_cast<const Integer>(T.get(0, i))->addint(*rcp_static_cast<const Integer>(one)));

                if (i > 0) {
                    CSYMPY_ASSERT(is_a<Integer>(*T.get(0, i - 1)));
                    T.set(0, i - 1,
                        rcp_static_cast<const Integer>(T.get(0, i - 1))->subint(*rcp_static_cast<const Integer>(one)));
                }

                dot = rcp_static_cast<const Integer>(zero);
                for (unsigned j = 0; j < p; j++) {
                    CSYMPY_ASSERT(is_a<Integer>(*product.get(j, 0)));
                    RCP<const Integer> p_j0 = rcp_static_cast<const Integer>(product.get(j, 0));

                    CSYMPY_ASSERT(is_a<Integer>(*A.get(j, i)));
                    RCP<const Integer> A_ji = rcp_static_cast<const Integer>(A.get(j, i));

                    dot = dot->addint(*p_j0->mulint(*A_ji));
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
