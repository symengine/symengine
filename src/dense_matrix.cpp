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

// ------------------------------ Gaussian Elimination -----------------------//
void fraction_free_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B)
{
    unsigned n = A.row_;

    CSYMPY_ASSERT(A.row_ == A.col_)
    CSYMPY_ASSERT(n == B.row_ && n == B.col_);

    B.m_ = A.m_;

    for (unsigned i = 0; i < n - 1; i++)
        for (unsigned j = i + 1; j < n; j++) {
            for (unsigned k = i + 1; k < n; k++)
                B.m_[j*n + k] = sub(mul(B.m_[i*n + i], B.m_[j*n + k]),
                    mul(B.m_[j*n + i], B.m_[i*n + k]));
            B.m_[j*n + i] = zero;
        }
}

void gauss_jordan_elimination(const DenseMatrix &A, DenseMatrix &B)
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
    gauss_jordan_elimination(B, D);

    // No checks are done to see if the diagonal entries are zero
    for (unsigned i = 0; i < A.col_; i++)
        C.m_[i] = div(D.get(i*A.col_ + i + A.col_), D.get(i*(A.col_ + 1) + i));
}

} // CSymPy
