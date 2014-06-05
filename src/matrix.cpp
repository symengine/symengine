#include "matrix.h"
#include "add.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {

// ----------------------------- Dense Matrix --------------------------------//

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

// Matrix addition
//RCP<const MatrixBase> DenseMatrix::add_matrix(const MatrixBase &other) const
//{
//    if (is_a<DenseMatrix>(other))
//        return add_dense_dense(*this, static_cast<const DenseMatrix &>(other));
//}

//// Matrix multiplication
//RCP<const MatrixBase> DenseMatrix::mul_matrix(const MatrixBase &other) const
//{
//    if (is_a<DenseMatrix>(other))
//        return mul_dense_dense(*this, static_cast<const DenseMatrix &>(other));
//}

// ----------------------------- Sparse Matrix -------------------------------//

// Virtual functions inherited from Basic class
std::size_t SparseMatrix::__hash__() const
{
    throw std::runtime_error("Not implemented.");
}

bool SparseMatrix::__eq__(const Basic &o) const
{
    throw std::runtime_error("Not implemented.");
}

int SparseMatrix::compare(const Basic &o) const
{
    throw std::runtime_error("Not implemented.");
}

//Constructors
SparseMatrix::SparseMatrix(unsigned row, unsigned col)
        : MatrixBase(row, col) {}

SparseMatrix::SparseMatrix(unsigned row, unsigned col,
        std::map<int, RCP<Basic>> &l): MatrixBase(row, col), m_{l} {}

// Get and set elements
RCP<const Basic> SparseMatrix::get(unsigned i) const
{
    throw std::runtime_error("Not implemented.");
}

void SparseMatrix::set(unsigned i, RCP<const Basic> &e)
{
    throw std::runtime_error("Not implemented.");
}

unsigned SparseMatrix::rank() const
{
    throw std::runtime_error("Not implemented.");
}

RCP<const Basic> SparseMatrix::det() const
{
    throw std::runtime_error("Not implemented.");
}

RCP<const MatrixBase> SparseMatrix::inv() const
{
    throw std::runtime_error("Not implemented.");
}

// Matrix addition
RCP<const MatrixBase> SparseMatrix::add_matrix(const MatrixBase &other) const
{
    throw std::runtime_error("Not implemented.");
}

// Matrix multiplication
RCP<const MatrixBase> SparseMatrix::mul_matrix(const MatrixBase &other) const
{
    throw std::runtime_error("Not implemented.");
}

// ------------------------------- Matrix Addition ---------------------------//
void add_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C)
{
    CSYMPY_ASSERT(A.row_ == B.row_ && A.col_ == B.col_ && A.row_ == C.row_ && A.col- == C.col_);

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

    std::vector<RCP<const Basic>> t = A.m_;

    for (i = 0; i < col; i++) {
        if (pivots == row)
            break;

        if (eq(t[pivots*col + i], zero)) {

            for (k = pivots; k < row; k++)
                if (!eq(t[k*col + i], zero)) {
                    for (l = 0; l < col; l++) {
                        tmp = t[k*col + l];
                        t[k*col + l] = t[pivots*col + l];
                        t[pivots*col + l] = tmp;
                    }
                    break;
                }

            if (k == row)
                continue;
        }

        scale = t[pivots*col + i];
        for (l = 0; l < col; l++)
            t[pivots*col + l] = div(t[pivots*col + l], scale);


        for (unsigned j = 0; j < row; j++) {
            if (j == pivots)
                continue;

            scale = t[j*col + i];
            for(l = 0; l < col; l++)
                t[j*col + l] = sub(t[j*col + l], mul(scale, t[pivots*col + l]));
        }

        pivots++;
    }
}

} // CSymPy
