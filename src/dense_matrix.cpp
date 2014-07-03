#include "matrix.h"
#include "add.h"
#include "mul.h"
#include "integer.h"
#include "pow.h"

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

// ----------------------------- Matrix Transpose ----------------------------//
void transpose_dense(const DenseMatrix &A, DenseMatrix &B)
{
    CSYMPY_ASSERT(B.row_ == A.row_ && B.col_ == A.col_);

    unsigned row = A.row_, col = A.col_;

    for (unsigned i = 0; i < row; i++)
        for (unsigned j = 0; j < col; j++)
            B.m_[j*col + i] = A.m_[i*col + j];
}

// ------------------------------- Submatrix ---------------------------------//
void submatrix_dense(const DenseMatrix &A, unsigned row_start, unsigned row_end,
        unsigned col_start, unsigned col_end, DenseMatrix &B)
{
    CSYMPY_ASSERT(row_end > row_start && col_end > col_start);
    CSYMPY_ASSERT(B.row_ == row_end - row_start + 1 &&
            B.col_ == col_end - col_start + 1);

    unsigned row = B.row_, col = B.col_;

    for (unsigned i = 0; i < row; i++)
        for (unsigned j = 0; j < col; j++)
            B.m_[i*col + j] =
                A.m_[(row_start + i - 1)*A.col_ + col_start - 1 + j];
}

// ------------------------------- Matrix Addition ---------------------------//
void add_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_ &&
        A.row_ == C.row_ && A.col_ == C.col_);

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
    CSYMPY_ASSERT(A.col_ == B.row_ && C.row_ == A.row_ && C.col_ == B.col_);

    unsigned row = A.row_, col = B.col_;

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

    while (ait != A.m_.end()) {
         *bit = mul(*ait, k);
         ait++;
         bit++;
    }
}

// -------------------------------- Row Operations ---------------------------//
void row_exchange_dense(DenseMatrix &A , unsigned i, unsigned j)
{
    CSYMPY_ASSERT(i != j && i < A.row_ && j < A.row_);

    unsigned col = A.col_;

    for (unsigned k = 0; k < A.col_; k++)
        std::swap(A.m_[i*col + k], A.m_[j*col + k]);
}

void row_mul_scalar_dense(DenseMatrix &A, unsigned i, RCP<const Basic> &c)
{
    CSYMPY_ASSERT(i < A.row_);

    unsigned col = A.col_;

    for (unsigned j = 0; j < A.col_; j++)
        A.m_[i*col + j] = mul(c, A.m_[i*col + j]);
}

void row_add_row_dense(DenseMatrix &A, unsigned i, unsigned j,
    RCP<const Basic> &c)
{
    CSYMPY_ASSERT(i != j && i < A.row_ && j < A.row_);

    unsigned col = A.col_;

    for (unsigned k = 0; k < A.col_; k++)
        A.m_[i*col + k] = add(A.m_[i*col + k], mul(c, A.m_[j*col + k]));
}

// ------------------------------ Gaussian Elimination -----------------------//
void pivoted_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B,
    std::vector<unsigned> &pivotlist)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);
    CSYMPY_ASSERT(pivotlist.size() == A.row_);

    unsigned row = A.row_, col = A.col_;
    unsigned index = 0, i, j, k;
    B.m_ = A.m_;

    for (i = 0; i < row; i++)
        pivotlist[i] = i;

    RCP<const Basic> scale;

    for (i = 0; i < col - 1; i++) {
        if (index == row)
            break;

        k = pivot(B, index, i);
        if (k == row)
            continue;
        if (k != index) {
            row_exchange_dense(B, k, index);
            std::swap(pivotlist[k], pivotlist[index]);
        }

        scale = div(one, B.m_[index*col + i]);
        row_mul_scalar_dense(B, index, scale);

        for (j = i + 1; j < row; j++) {
            for (k = i + 1; k < col; k++)
                B.m_[j*col + k] = sub(B.m_[j*col + k],
                    mul(B.m_[j*col + i], B.m_[i*col + k]));
            B.m_[j*col + i] = zero;
        }

        index++;
    }
}

// Algorithm 1, page 12, Nakos, G. C., Turner, P. R., Williams, R. M. (1997).
// Fraction-free algorithms for linear and polynomial equations.
// ACM SIGSAM Bulletin, 31(3), 11–19. doi:10.1145/271130.271133.
void fraction_free_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);

    unsigned col = A.col_;
    B.m_ = A.m_;

    for (unsigned i = 0; i < col - 1; i++)
        for (unsigned j = i + 1; j < A.row_; j++) {
            for (unsigned k = i + 1; k < col; k++) {
                B.m_[j*col + k] = sub(mul(B.m_[i*col + i], B.m_[j*col + k]),
                    mul(B.m_[j*col + i], B.m_[i*col + k]));
                if (i > 0)
                    B.m_[j*col + k] = div(B.m_[j*col + k],
                        B.m_[i*col - col + i - 1]);
            }
            B.m_[j*col + i] = zero;
        }
}

// Pivoted version of `fraction_free_gaussian_elimination`
void pivoted_fraction_free_gaussian_elimination(const DenseMatrix &A,
    DenseMatrix &B, std::vector<unsigned> &pivotlist)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);
    CSYMPY_ASSERT(pivotlist.size() == A.row_);

    unsigned col = A.col_, row = A.row_;
    unsigned index = 0, i, k, j;
    B.m_ = A.m_;

    for (i = 0; i < row; i++)
        pivotlist[i] = i;

    for (i = 0; i < col - 1; i++) {
        if (index == row)
            break;

        k = pivot(B, index, i);
        if (k == row)
            continue;
        if (k != index) {
            row_exchange_dense(B, k, index);
            std::swap(pivotlist[k], pivotlist[index]);
        }

        for (j = i + 1; j < row; j++) {
            for (k = i + 1; k < col; k++) {
                B.m_[j*col + k] = sub(mul(B.m_[i*col + i], B.m_[j*col + k]),
                    mul(B.m_[j*col + i], B.m_[i*col + k]));
                if (i > 0)
                    B.m_[j*col + k] = div(B.m_[j*col + k], B.m_[i*col - col + i - 1]);
            }
            B.m_[j*col + i] = zero;
        }

        index++;
    }
}

void pivoted_gauss_jordan_elimination(const DenseMatrix &A, DenseMatrix &B,
    std::vector<unsigned> &pivotlist)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);
    CSYMPY_ASSERT(pivotlist.size() == A.row_);

    unsigned row = A.row_, col = A.col_;
    unsigned index = 0, i, j, k;
    RCP<const Basic> scale;
    B.m_ = A.m_;

    for (i = 0; i < row; i++)
        pivotlist[i] = i;

    for (i = 0; i < col; i++) {
        if (index == row)
            break;

        k = pivot(B, index, i);
        if (k == row)
            continue;
        if (k != index) {
            row_exchange_dense(B, k, index);
            std::swap(pivotlist[k], pivotlist[index]);
        }

        scale = div(one, B.m_[index*col + i]);
        row_mul_scalar_dense(B, index, scale);

        for (j = 0; j < row; j++) {
            if (j == index)
                continue;

            scale = mul(minus_one, B.m_[j*col + i]);
            row_add_row_dense(B, j, index, scale);
        }

        index++;
    }
}

// Algorithm 2, page 13, Nakos, G. C., Turner, P. R., Williams, R. M. (1997).
// Fraction-free algorithms for linear and polynomial equations.
// ACM SIGSAM Bulletin, 31(3), 11–19. doi:10.1145/271130.271133.
void fraction_free_gauss_jordan_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);

    unsigned row = A.row_, col = A.col_;
    unsigned i, j, k;
    RCP<const Basic> d;

    B.m_ = A.m_;

    for (i = 0; i < col; i++) {
        if (i > 0)
            d = B.m_[i*col - col + i - 1];
        for (j = 0; j < row; j++)
            if (j != i)
                for (k = 0; k < col; k++) {
                    if (k != i) {
                        B.m_[j*col + k] = sub(mul(B.m_[i*col + i], B.m_[j*col + k]),
                            mul(B.m_[j*col + i], B.m_[i*col + k]));
                        if (i > 0)
                            B.m_[j*col + k] = div(B.m_[j*col + k], d);
                    }
                }
        for (j = 0; j < row; j++)
            if (j != i)
                B.m_[j*col + i] = zero;
    }
}

void pivoted_fraction_free_gauss_jordan_elimination(const DenseMatrix &A,
        DenseMatrix &B, std::vector<unsigned> &pivotlist)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);
    CSYMPY_ASSERT(pivotlist.size() == A.row_);

    unsigned row = A.row_, col = A.col_;
    unsigned index = 0, i, k, j;
    RCP<const Basic> d;

    B.m_ = A.m_;

    for (i = 0; i < row; i++)
        pivotlist[i] = i;

    for (i = 0; i < col; i++) {
        if (index == row)
            break;

        k = pivot(B, index, i);
        if (k == row)
            continue;
        if (k != index) {
            row_exchange_dense(B, k, index);
            std::swap(pivotlist[k], pivotlist[index]);
        }

        if (i > 0)
            d = B.m_[i*col - col + i - 1];
        for (j = 0; j < row; j++) {
            if (j != i)
                for (k = 0; k < col; k++) {
                    if (k != i) {
                        B.m_[j*col + k] = sub(mul(B.m_[i*col + i], B.m_[j*col + k]),
                            mul(B.m_[j*col + i], B.m_[i*col + k]));
                        if (i > 0)
                            B.m_[j*col + k] = div(B.m_[j*col + k], d);
                    }
                }
        }

        for (j = 0; j < row; j++)
            if (j != i)
                B.m_[j*col + i] = zero;

        index++;
    }
}

unsigned pivot(DenseMatrix &B, unsigned r, unsigned c)
{
    unsigned k = r;

    if (eq(B.m_[r*B.col_ + c], zero))
        for (k = r; k < B.row_; k++)
            if (!eq(B.m_[k*B.col_ + c], zero))
                break;
    return k;
}

// --------------------------- Solve Ax = b  ---------------------------------//
void augment_dense(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &C)
{
    CSYMPY_ASSERT(A.row_ == b.row_ && A.row_ == C.row_);
    CSYMPY_ASSERT(C.col_ == A.col_ + b.col_);

    unsigned col = A.col_ + b.col_;

    for (unsigned i = 0; i < A.row_; i++) {
        for (unsigned j = 0; j < A.col_; j++)
            C.m_[i*col + j] = A.m_[i*A.col_ + j];
        for (unsigned j = 0; j < b.col_; j++)
            C.m_[i*col + A.col_ + j] = b.m_[i*b.col_ + j];
    }
}

// Assuming A is a diagonal square matrix
void diagonal_solve(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &x)
{
    CSYMPY_ASSERT(A.row_ == A.col_);
    CSYMPY_ASSERT(b.row_ == A.row_ && b.col_ == 1);
    CSYMPY_ASSERT(x.row_ == A.col_ && x.col_ == 1);

    // No checks are done to see if the diagonal entries are zero
    for (unsigned i = 0; i < A.col_; i++)
        x.m_[i] = div(b.m_[i], A.m_[i]);
}

// Assuming U is an Upper square matrix
void back_substitution(const DenseMatrix &U, const DenseMatrix &b,
    DenseMatrix &x)
{
    CSYMPY_ASSERT(U.row_ == U.col_);
    CSYMPY_ASSERT(b.row_ == U.row_ && b.col_ == 1);
    CSYMPY_ASSERT(x.row_ == U.col_ && x.col_ == 1);

    int i, j, col = U.col_;

    DenseMatrix b_ = DenseMatrix(b.row_, 1, b.m_);

    for (i = 0; i < col; i++)
        x.m_[i] = zero; // Integer zero;

    for (i = col - 1; i >= 0; i--) {
        for (j = i + 1; j < col; j++)
            b_.m_[i] = sub(b_.m_[i], mul(U.m_[i*col + j], x.m_[j]));
        x.m_[i] = div(b_.m_[i], U.m_[i*col + i]);
    }
}

void fraction_free_gaussian_elimination_solve(const DenseMatrix &A,
    const DenseMatrix &b, DenseMatrix &x)
{
    CSYMPY_ASSERT(A.row_ == A.col_);
    CSYMPY_ASSERT(b.row_ == A.row_ && b.col_ == 1);
    CSYMPY_ASSERT(x.row_ == A.col_ && x.col_ == 1);

    int i, j, col = A.col_;
    DenseMatrix A_ = DenseMatrix(A.row_, A.col_, A.m_);
    DenseMatrix b_ = DenseMatrix(b.row_, 1, b.m_);

    for (i = 0; i < col - 1; i++)
        for (j = i + 1; j < col; j++) {
            b_.m_[j] = sub(mul(A_.m_[i*col + i], b_.m_[j]),
                mul(A_.m_[j*col + i], b_.m_[i]));
            if(i > 0)
                b_.m_[j] = div(b_.m_[j], A_.m_[i*col - col + i - 1]);

            for (int k = i + 1; k < col; k++) {
                A_.m_[j*col + k] = sub(mul(A_.m_[i*col + i], A_.m_[j*col + k]),
                    mul(A_.m_[j*col + i], A_.m_[i*col + k]));
                if (i> 0)
                    A_.m_[j*col + k] = div(A_.m_[j*col + k], A_.m_[i*col - col + i - 1]);
            }
            A_.m_[j*col + i] = zero;
        }

    for (i = 0; i < col; i++)
        x.m_[i] = zero; // Integer zero;

    for (i = col - 1; i >= 0; i--) {
        for (j = i + 1; j < col; j++)
            b_.m_[i] = sub(b_.m_[i], mul(A_.m_[i*col + j], x.m_[j]));
        x.m_[i] = div(b_.m_[i], A_.m_[i*col + i]);
    }
}

void fraction_free_gauss_jordan_solve(const DenseMatrix &A, const DenseMatrix &b,
    DenseMatrix &x)
{
    CSYMPY_ASSERT(A.row_ == A.col_);
    CSYMPY_ASSERT(b.row_ == A.row_ && b.col_ == 1);
    CSYMPY_ASSERT(x.row_ == A.col_ && x.col_ == 1);

    unsigned i, j, col = A.col_;
    RCP<const Basic> d;
    DenseMatrix A_ = DenseMatrix(A.row_, A.col_, A.m_);
    DenseMatrix b_ = DenseMatrix(b.row_, 1, b.m_);

    for (i = 0; i < col; i++) {
        if (i > 0)
            d = A_.m_[i*col - col + i - 1];
        for (j = 0; j < col; j++)
            if (j != i) {
                b_.m_[j] = sub(mul(A_.m_[i*col + i], b_.m_[j]),
                    mul(A_.m_[j*col + i], b_.m_[i]));
                if (i > 0)
                    b_.m_[j] = div(b_.m_[j], d);

                for (unsigned k = 0; k < col; k++) {
                    if (k != i) {
                        A_.m_[j*col + k] = sub(mul(A_.m_[i*col + i], A_.m_[j*col + k]),
                            mul(A_.m_[j*col + i], A_.m_[i*col + k]));
                        if (i > 0)
                            A_.m_[j*col + k] = div(A_.m_[j*col + k], d);
                    }
                }
            }

        for (j = 0; j < col; j++)
            if (j != i)
                A_.m_[j*col + i] = zero;
    }

    // No checks are done to see if the diagonal entries are zero
    for (i = 0; i < col; i++)
        x.m_[i] = div(b_.m_[i], A_.m_[i*col + i]);
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
    for (i = 0; i < row*row; i++)
        D.m_[i] = zero; // Integer zero

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

// SymPy's QRecomposition in sympy.matrices.matrices.MatrixBase.QRdecomposition
// Rank check is not performed
void QR(const DenseMatrix &A, DenseMatrix &Q, DenseMatrix &R)
{
    unsigned row = A.row_;
    unsigned col = A.col_;

    CSYMPY_ASSERT(Q.row_ == row && Q.col_ == col && R.row_ == col && R.col_ == col);

    unsigned i, j, k;
    RCP<const Basic> t;
    std::vector<RCP<const Basic>> tmp (row);

    // Initialize Q
    for (i = 0; i < row*col; i++)
        Q.m_[i] = zero;

    // Initialize R
    for (i = 0; i < col*col; i++)
        R.m_[i] = zero;

    for (j = 0; j < col; j++) {
        // Use submatrix for this
        for (k = 0; k < row; k++)
            tmp[k] = A.m_[k*col + j];

        for (i = 0; i < j; i++) {
            t = zero;
            for (k = 0; k < row; k++)
                t = add(t, mul(A.m_[k*col + j], Q.m_[k*col + i]));
            std::cout << "(" << j << "," << i << ") " << *t << std::endl;
            for (k = 0; k < row; k++)
                tmp[k] = expand(sub(tmp[k], mul(Q.m_[k*col + i], t)));
        }

        // calculate norm
        t = zero;
        for (k = 0; k < row; k++)
            t = add(t, pow(tmp[k], integer(2)));

        t = pow(t, div(one, integer(2)));

        R.m_[j*col + j] = t;
        for (k = 0; k < row; k++)
            Q.m_[k*col + j] = div(tmp[k], t);

        for (i = 0; i < j; i++) {
            t = zero;
            for (k = 0; k < row; k++)
                t = add(t, mul(Q.m_[k*col + i], A.m_[k*col + j]));
            R.m_[i*col + j] = t;
        }
    }
}

// SymPy's LDL decomposition, Assuming A is a symmetric, square, positive
// definite non singular matrix
void LDL(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &D)
{
    CSYMPY_ASSERT(A.row_ == A.col_);
    CSYMPY_ASSERT(L.row_ == A.row_ && L.col_ == A.row_);
    CSYMPY_ASSERT(D.row_ == A.row_ && D.col_ == A.row_);

    unsigned col = A.col_;
    unsigned i, k, j;
    RCP<const Basic> sum;
    RCP<const Basic> i2 = integer(2);

    // Initialize D
    for (i = 0; i < col; i++)
        for (j = 0; j < col; j++)
            D.m_[i*col + j] = zero; // Integer zero

    // Initialize L
    for (i = 0; i < col; i++)
        for (j = 0; j < col; j++)
            L.m_[i*col + j] = (i != j) ? zero : one;

    for (i = 0; i < col; i++) {
        for (j = 0; j < i; j++) {
            sum = zero;
            for (k = 0; k < j; k++)
                sum = add(sum, mul(mul(L.m_[i*col + k], L.m_[j*col + k]),
                    D.m_[k*col + k]));
            L.m_[i*col + j] = mul(div(one, D.m_[j*col + j]),
                sub(A.m_[i*col + j], sum));
        }
        sum = zero;
        for (k = 0; k < i; k++)
            sum = add(sum, mul(pow(L.m_[i*col + k], i2), D.m_[k*col + k]));
        D.m_[i*col + i] = sub(A.m_[i*col + i], sum);
    }
}

// SymPy's cholesky decomposition
void cholesky(const DenseMatrix &A, DenseMatrix &L)
{
    CSYMPY_ASSERT(A.row_ == A.col_);
    CSYMPY_ASSERT(L.row_ == A.row_ && L.col_ == A.row_);

    unsigned col = A.col_;
    unsigned i, j, k;
    RCP<const Basic> sum;
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> half = div(one, i2);

    // Initialize L
    for (i = 0; i < col; i++)
        for(j = 0; j < col; j++)
            L.m_[i*col + j] = zero;

    for (i = 0; i < col; i++) {
        for (j = 0; j < i; j++) {
            sum = zero;
            for(k = 0; k < j; k++)
                sum = add(sum, mul(L.m_[i*col + k], L.m_[j*col + k]));

            L.m_[i*col + j] = mul(div(one, L.m_[j*col + j]),
                sub(A.m_[i*col + j], sum));
        }
        sum = zero;
        for (k = 0; k < i; k++)
            sum = add(sum, pow(L.m_[i*col + k], i2));

        L.m_[i*col + i] = pow(sub(A.m_[i*col + i], sum), half);
    }
}

} // CSymPy
