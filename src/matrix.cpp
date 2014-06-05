#include "matrix.h"
#include "add.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {

// ----------------------------- Dense Matrix --------------------------------//

// Virtual functions inherited from Basic class
std::size_t DenseMatrix::__hash__() const
{
    std::size_t seed = 0;
    for (auto &p: m_) {
        hash_combine<Basic>(seed, *p);
    }
    return seed;
}

bool DenseMatrix::__eq__(const Basic &o) const
{
    const DenseMatrix &o_ = static_cast<const DenseMatrix &>(o);

    if (is_a<DenseMatrix>(o)) {
        if (row_ != o_.row_ || col_ != o_.col_)
            return false;

        std::vector<RCP<const Basic>>::const_iterator oit = o_.m_.begin();
        std::vector<RCP<const Basic>>::const_iterator it = m_.begin();

        for (unsigned i = 0; i < row_*col_; i++) {
            if (neq(*oit, *it))
                return false;
            it++;
            oit++;
        }
        return true;
    }
    return false;
}

int DenseMatrix::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<DenseMatrix>(o))
    const DenseMatrix &o_ = static_cast<const DenseMatrix &>(o);
    // # of elements
    if (m_.size() != o_.m_.size())
        return (m_.size() < o_.m_.size()) ? -1 : 1;

    return 0;
}

// Constructors
DenseMatrix::DenseMatrix(unsigned row, unsigned col)
        : MatrixBase(row, col) {}

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
RCP<const MatrixBase> DenseMatrix::add_matrix(const MatrixBase &other) const
{
    if (is_a<DenseMatrix>(other))
        return add_dense_dense(*this, static_cast<const DenseMatrix &>(other));
}

// Matrix multiplication
RCP<const MatrixBase> DenseMatrix::mul_matrix(const MatrixBase &other) const
{
    if (is_a<DenseMatrix>(other))
        return mul_dense_dense(*this, static_cast<const DenseMatrix &>(other));
}

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

void SparseMatrix::set(unsigned i, RCP<Basic> &e)
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
RCP<const DenseMatrix> add_dense_dense(const DenseMatrix &A,
        const DenseMatrix &B)
{
    unsigned row = A.row_;
    unsigned col = A.col_;

    CSYMPY_ASSERT(row == B.row_ && col == B.col_)

    std::vector<RCP<const Basic>> sum(row*col);

    std::vector<RCP<const Basic>>::const_iterator ait = A.m_.begin();
    std::vector<RCP<const Basic>>::const_iterator bit = B.m_.begin();

    for(auto &it: sum) {
        it = add(*ait, *bit);
        ait++;
        bit++;
    }

    return rcp(new DenseMatrix(row, col, sum));
}

RCP<const DenseMatrix> add_dense_scalar(const DenseMatrix &A,
    RCP<const Basic> &k)
{
    std::vector<RCP<const Basic>> scalar_add(A.row_*A.col_);

    for (unsigned i = 0; i < A.row_*A.col_; i++)
        scalar_add[i] = add(A.m_[i], k);

    return rcp(new DenseMatrix(A.row_, A.col_, scalar_add));
}

// ------------------------------- Matrix Multiplication ---------------------//
RCP<const DenseMatrix> mul_dense_dense(const DenseMatrix &A,
        const DenseMatrix &B)
{
    unsigned row = A.row_;
    unsigned col = A.col_;

    CSYMPY_ASSERT(col == B.row_)

    std::vector<RCP<const Basic>> prod(row*B.col_);

    for (unsigned r = 0; r<row; r++) {
        for (unsigned c = 0; c<B.col_; c++) {
            prod[r*B.col_ + c] = zero; // Integer Zero
            for (unsigned k = 0; k<col; k++)
                prod[r*B.col_ + c] = add(prod[r*B.col_ + c],
                        mul(A.m_[r*col + k], B.m_[k*B.col_ + c]));
        }
    }

    return rcp(new DenseMatrix(row, B.col_, prod));
}

RCP<const DenseMatrix> mul_dense_scalar(const DenseMatrix &A,
        RCP<const Basic> &k)
{
    std::vector<RCP<const Basic>> scalar_mul(A.row_*A.col_);

    for (unsigned i = 0; i < A.row_*A.col_; i++)
        scalar_mul[i] = mul(A.m_[i], k);

    return rcp(new DenseMatrix(A.row_, A.col_, scalar_mul));
}

// ------------------------------ Gaussian Elimination -----------------------//
RCP<const DenseMatrix> gaussian_elimination(const DenseMatrix &A)
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

    return densematrix(row, col, t);
}

//--------------------------------- Diagonal solve ---------------------------//
RCP<const DenseMatrix> diagonal_solve(const DenseMatrix &A,
    const DenseMatrix &b)
{
    CSYMPY_ASSERT(b.col_ == 1);
    CSYMPY_ASSERT(A.row_ == b.row_);

    unsigned row = A.row_;
    unsigned col = A.col_;

    std::vector<RCP<const Basic>> t(row*col + row);
    std::vector<RCP<const Basic>> solutions(row);

    // Create the augmented matrix combinning A and b
    for (unsigned i = 0; i < row; i++) {
        for (unsigned j = 0; j < col; j++)
            t[i*col + i + j] = A.m_[i*col + j];
        t[i*col + i + col] = b.m_[i];
    }

    RCP<const DenseMatrix> B = gaussian_elimination(*densematrix(row, col + 1, t));

    // No checks are done to see if the diagonal entries are zero
    for (unsigned i = 0; i < col; i++)
        solutions[i] = div(B->get(i*col + i + col), B->get(i*(col+1) + i));

    return densematrix(row, 1, solutions);
}

} // CSymPy
