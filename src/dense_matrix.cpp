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

} // CSymPy
