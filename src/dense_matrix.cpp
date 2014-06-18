#include "matrix.h"
#include "add.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {

// Constructors
DenseMatrix::DenseMatrix(unsigned row, unsigned col)
        : MatrixBase(row, col)
{
    m_ = std::vector<RCP<const Basic>>(row*col);
}

DenseMatrix::DenseMatrix(unsigned row, unsigned col, const std::vector<RCP<const Basic>> &l)
        : MatrixBase(row, col), m_{l}
{
    CSYMPY_ASSERT(m_.size() == row*col)
}

// Get and set elements
RCP<const Basic> DenseMatrix::get(unsigned i) const
{
    return m_[i];
}

void DenseMatrix::set(unsigned i, RCP<const Basic> &e)
{
    m_[i] = e;
}

unsigned DenseMatrix::rank() const
{
    throw std::runtime_error("Not implemented.");
}

RCP<const Basic> DenseMatrix::det() const
{
    throw std::runtime_error("Not implemented.");
}

RCP<const MatrixBase> DenseMatrix::inv() const
{
    throw std::runtime_error("Not implemented.");
}

MatrixBase& DenseMatrix::add_matrix(const MatrixBase &other) const
{
    throw std::runtime_error("Not implemented.");
}

MatrixBase& DenseMatrix::mul_matrix(const MatrixBase &other) const
{
    throw std::runtime_error("Not implemented.");
}

// ------------------------------- Matrix Addition ---------------------------//
void add_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_ && A.row_ == C.row_ && A.col_ == C.col_);

    std::vector<RCP<const Basic>>::const_iterator ait = A.m_.begin();
    std::vector<RCP<const Basic>>::const_iterator bit = B.m_.begin();
    std::vector<RCP<const Basic>>::iterator cit = C.m_.begin();

    while(ait != A.m_.end()) {
        *cit = add(*ait, *bit);
        ait++;
        bit++;
        cit++;
    }
}

void add_dense_scalar(const DenseMatrix &A, RCP<const Basic> &k, DenseMatrix &B)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);

    std::vector<RCP<const Basic>>::const_iterator ait = A.m_.begin();
    std::vector<RCP<const Basic>>::iterator bit = B.m_.begin();

    while (ait != A.m_.end()) {
        *bit = add(*ait, k);
        ait++;
        bit++;
    }
}

// ------------------------------- Matrix Multiplication ---------------------//
void mul_dense_dense(const DenseMatrix &A, const DenseMatrix &B,
        DenseMatrix &C)
{
    unsigned row = A.row_;
    unsigned col = B.col_;

    CSYMPY_ASSERT(A.col_ == B.row_ && C.row_ == row && C.col_ == col);

    for (unsigned r = 0; r<row; r++) {
        for (unsigned c = 0; c<col; c++) {
            C.m_[r*col + c] = zero; // Integer Zero
            for (unsigned k = 0; k<A.col_; k++)
                C.m_[r*col + c] = add(C.m_[r*col + c],
                        mul(A.m_[r*A.col_ + k], B.m_[k*col + c]));
        }
    }
}

void mul_dense_scalar(const DenseMatrix &A, RCP<const Basic> &k, DenseMatrix& B)
{
    CSYMPY_ASSERT(A.col_ == B.col_ && A.row_ == B.row_);

    std::vector<RCP<const Basic>>::const_iterator ait = A.m_.begin();
    std::vector<RCP<const Basic>>::iterator bit = B.m_.begin();

    while (ait != A.m_.end()){
         *bit = mul(*ait, k);
         ait++;
         bit++;
    }
}

// ------------------------------ Gaussian Elimination -----------------------//
void gaussian_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    unsigned row = A.row_;
    unsigned col = A.col_;
    unsigned pivots = 0, i, k, l;

    RCP<const Basic> tmp, scale;

    B.m_ = A.m_;

    for (i = 0; i < col; i++) {
        if (pivots == row)
            break;

        if (eq(B.m_[pivots*col + i], zero)) {

            for (k = pivots; k < row; k++)
                if (!eq(B.m_[k*col + i], zero)) {
                    for (l = 0; l < col; l++) {
                        tmp = B.m_[k*col + l];
                        B.m_[k*col + l] = B.m_[pivots*col + l];
                        B.m_[pivots*col + l] = tmp;
                    }
                    break;
                }

            if (k == row)
                continue;
        }

        scale = B.m_[pivots*col + i];
        for (l = 0; l < col; l++)
            B.m_[pivots*col + l] = div(B.m_[pivots*col + l], scale);


        for (unsigned j = 0; j < row; j++) {
            if (j == pivots)
                continue;

            scale = B.m_[j*col + i];
            for(l = 0; l < col; l++)
                B.m_[j*col + l] = sub(B.m_[j*col + l], mul(scale, B.m_[pivots*col + l]));
        }

        pivots++;
    }
}

// --------------------------- Matrix Decomposition --------------------------//

// Algorithm 3, page 14, Nakos, G. C., Turner, P. R., Williams, R. M. (1997).
// Fraction-free algorithms for linear and polynomial equations.
// ACM SIGSAM Bulletin, 31(3), 11–19. doi:10.1145/271130.271133.
// This algorithms is not a true factorization of the matrix A(i.e. A != LU))
// but can be used to solve linear systems by forward/backward substitution.
void fraction_free_LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &U)
{
    CSYMPY_ASSERT(A.row_ == A.col_ && L.row_ == L.col_ && U.row_ == U.col_);
    CSYMPY_ASSERT(A.row_ == L.row_ && A.row_ == U.row_);

    unsigned n = A.row_;
    unsigned i, j, k;

    U.m_ = A.m_;

    for (i = 0; i < n - 1; i++)
        for (j = i + 1; j < n; j++)
            for (k = i + 1; k < n; k++) {
                U.m_[j*n + k] = sub(mul(U.m_[i*n + i], U.m_[j*n + k]),
                    mul(U.m_[j*n + i], U.m_[i*n + k]));
                if (i)
                    U.m_[j*n + k] = div(U.m_[j*n + k], U.m_[i*n - n + i - 1]);
            }

    for(i = 0; i < n; i++) {
        for(j = 0; j < i; j++) {
            L.m_[i*n + j] = U.m_[i*n + j];
            U.m_[i*n + j] = zero;
        }
        L.m_[i*n + i] = U.m_[i*n + i];
        for (j = i + 1; j < n; j++)
            L.m_[i*n + j] = zero; // Integer Zero
    }
}

// SymPy LUDecomposition algorithm, in sympy.matrices.matrices.Matrix.LUdecomposition
// with no pivoting
void LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &U)
{
    CSYMPY_ASSERT(A.row_ == A.col_ && L.row_ == L.col_ && U.row_ == U.col_);
    CSYMPY_ASSERT(A.row_ == L.row_ && A.row_ == U.row_);

    unsigned n = A.row_;
    unsigned i, j, k;
    RCP<const Basic> scale;

    U.m_ = A.m_;

    for (j = 0; j < n; j++) {
        for (i = 0; i < j; i++)
            for (k = 0; k < i; k++)
                U.m_[i*n + j] = sub(U.m_[i*n + j],
                    mul(U.m_[i*n + k], U.m_[k*n + j]));

        for (i = j; i < n; i++) {
            for (k = 0; k < j; k++)
                U.m_[i*n + j] = sub(U.m_[i*n + j],
                    mul(U.m_[i*n + k], U.m_[k*n + j]));
        }

        scale = div(one, U.m_[j*n + j]);

        for (i = j + 1; i < n; i++)
            U.m_[i*n + j] = mul(U.m_[i*n + j], scale);
    }

    for(i = 0; i < n; i++) {
        for(j = 0; j < i; j++) {
            L.m_[i*n + j] = U.m_[i*n + j];
            U.m_[i*n + j] = zero; // Integer zero
        }
        L.m_[i*n + i] = one; // Integer one
        for (j = i + 1; j < n; j++)
            L.m_[i*n + j] = zero; // Integer zero
    }
}

// SymPy's fraction free LU decomposition, without pivoting
// sympy.matrices.matrices.MatrixBase.LUdecompositionFF
// W. Zhou & D.J. Jeffrey, "Fraction-free matrix factors: new forms for LU and QR factors".
// Frontiers in Computer Science in China, Vol 2, no. 1, pp. 67-80, 2008.
void fraction_free_LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &D,
        DenseMatrix &U)
{
    CSYMPY_ASSERT(A.row_ == L.row_ && A.row_ == U.row_);
    CSYMPY_ASSERT(A.col_ == L.col_ && A.col_ == U.col_);

    unsigned row = A.row_, col = A.col_;
    unsigned i, j, k;
    RCP<const Basic> old = integer(1);

    U.m_ = A.m_;

    // Initialize L
    for (i = 0; i < row; i++)
        for (j = 0; j < row; j++)
            if (i != j)
                L.m_[i*col + j] = zero;
            else
                L.m_[i*col + i] = one;

    // Initialize D
    for (i = 0; i < row; i++)
        for (j = 0; j < row; j++)
            D.m_[i*col + j] = zero; // Integer zero

    for (k = 0; k < row - 1; k++) {
        L.m_[k*col + k] = U.m_[k*col + k];
        D.m_[k*col + k] = mul(old, U.m_[k*col + k]);

        for (i = k + 1; i < row; i++) {
            L.m_[i*col + k] = U.m_[i*col + k];
            for (j = k + 1; j < col; j++)
                U.m_[i*col + j] = div(sub(mul(U.m_[k*col + k], U.m_[i*col + j]),
                    mul(U.m_[k*col + j], U.m_[i*col + k])), old);
            U.m_[i*col + k] = zero; // Integer zero
        }

        old = U.m_[k*col + k];
    }

    D.m_[row*col - col + row - 1] = old;
}

} // CSymPy
