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
    unsigned row = A.row_;
    unsigned col = A.col_;

    CSYMPY_ASSERT(B.row_ == row && B.col_ == col);

    for (unsigned i = 0; i < row; i++)
        for (unsigned j = 0; j < col; j++)
            B.m_[j*col + i] = A.m_[i*col + j];
}

// ------------------------------- Submatrix ---------------------------------//
void submatrix_dense(const DenseMatrix &A, unsigned row_start, unsigned row_end,
        unsigned col_start, unsigned col_end, DenseMatrix &B)
{
    unsigned row = B.row_;
    unsigned col = B.col_;

    CSYMPY_ASSERT(row_end > row_start && col_end > col_start);
    CSYMPY_ASSERT(row == row_end - row_start + 1 && col == col_end - col_start + 1);

    for (unsigned i = 0; i < row; i++)
        for (unsigned j = 0; j < col; j++)
            B.m_[i*col + j] = A.m_[(row_start + i - 1)*A.col_ + col_start - 1 + j];
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
void pivoted_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    unsigned row = A.row_;
    unsigned col = A.col_;

    CSYMPY_ASSERT(row == B.row_ && col == B.col_);

    unsigned index = 0, i, j, k;
    B.m_ = A.m_;

    RCP<const Basic> scale;

    for (i = 0; i < col - 1; i++) {
        if (index == row)
            break;

        k = pivot(B, index, i);
        if (k == row)
            continue;
        if (k != index)
            row_exchange_dense(B, k, index);

        scale = div(one, B.m_[index*col + i]);
        row_mul_scalar_dense(B, index, scale);

        for (j = i + 1; j < row; j++) {
            for (k = i + 1; k < col; k++)
                B.m_[j*col + k] = sub(B.m_[j*col + k],
                    mul(B.m_[j*col + i], B.m_[index*col + k]));
            B.m_[j*col + i] = zero;
        }

        index++;
    }
}


void fraction_free_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    unsigned col = A.col_;

    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);

    B.m_ = A.m_;

    for (unsigned i = 0; i < col - 1; i++)
        for (unsigned j = i + 1; j < A.row_; j++) {
            for (unsigned k = i + 1; k < col; k++) {
                B.m_[j*col + k] = sub(mul(B.m_[i*col + i], B.m_[j*col + k]),
                    mul(B.m_[j*col + i], B.m_[i*col + k]));
                if (i)
                    B.m_[j*col + k] = div(B.m_[j*col + k], B.m_[i*col - col + i - 1]);
            }
            B.m_[j*col + i] = zero;
        }
}

void pivoted_fraction_free_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    unsigned col = A.col_;
    unsigned row = A.row_;

    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_);

    unsigned index = 0, i, k, j;
    B.m_ = A.m_;

    for (i = 0; i < col - 1; i++) {
        if (index == row)
            break;

        k = pivot(B, index, i);
        if (k == row)
            continue;
        if (k != index)
            row_exchange_dense(B, k, index);

        for (j = i + 1; j < row; j++) {
            for (k = i + 1; k < col; k++) {
                B.m_[j*col + k] = sub(mul(B.m_[i*col + i], B.m_[j*col + k]),
                    mul(B.m_[j*col + i], B.m_[i*col + k]));
                if (i)
                    B.m_[j*col + k] = div(B.m_[j*col + k], B.m_[i*col - col + i - 1]);
            }
            B.m_[j*col + i] = zero;
        }

        index++;
    }
}

void pivoted_gauss_jordan_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    unsigned row = A.row_;
    unsigned col = A.col_;

    CSYMPY_ASSERT(row == B.row_ && col == B.col_);

    unsigned index = 0, i, j, k;
    RCP<const Basic> scale;

    B.m_ = A.m_;

    for (i = 0; i < col; i++) {
        if (index == row)
            break;

        k = pivot(B, index, i);
        if (k == row)
            continue;
        if (k != index)
            row_exchange_dense(B, k, index);

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

void fraction_free_gauss_jordan_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    unsigned row = A.row_;
    unsigned col = A.col_;

    CSYMPY_ASSERT(row == B.row_ && col == B.col_);

    unsigned i, j, k;
    RCP<const Basic> d;

    B.m_ = A.m_;

    for (i = 0; i < col; i++) {
        if (i)
            d = B.m_[i*col - col + i - 1];
        for (j = 0; j < row; j++)
            if (j != i)
                for (k = 0; k < col; k++) {
                    B.m_[j*col + k] = sub(mul(B.m_[i*col + i], B.m_[j*col + k]),
                        mul(B.m_[j*col + i], B.m_[i*col + k]));
                    if (i)
                        B.m_[j*col + k] = div(B.m_[j*col + k], d);
                }
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

// --------------------------- Matrix Decomposition --------------------------//
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

// --------------------------- Solve Ax = b  ---------------------------------//
void augment_dense(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &C)
{
    CSYMPY_ASSERT(A.row_ == b.row_ && A.row_ == C.row_);
    unsigned col = A.col_ + b.col_;
    CSYMPY_ASSERT(C.col_ == col);

    for (unsigned i = 0; i < A.row_; i++) {
        for (unsigned j = 0; j < A.col_; j++)
            C.m_[i*col + j] = A.m_[i*A.col_ + j];
        for (unsigned j = 0; j < b.col_; j++)
            C.m_[i*col + A.col_ + j] = b.m_[i*b.col_ + j];
    }
}

void diagonal_solve(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &C)
{
    CSYMPY_ASSERT(b.col_ == 1);
    CSYMPY_ASSERT(A.row_ == b.row_);
    CSYMPY_ASSERT(C.row_ == A.col_ && C.col_ == 1);

    DenseMatrix B = DenseMatrix(A.row_, A.col_ + 1);
    DenseMatrix D = DenseMatrix(A.row_, A.col_ + 1);

    augment_dense(A, b, B);
    pivoted_gauss_jordan_elimination(B, D);

    // No checks are done to see if the diagonal entries are zero
    for (unsigned i = 0; i < A.col_; i++)
        C.m_[i] = div(D.get(i*A.col_ + i + A.col_), D.get(i*A.col_ + 2*i));
}

} // CSymPy
