#include "matrix.h"
#include "add.h"

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

DenseMatrix::DenseMatrix(unsigned row, unsigned col, std::vector<RCP<const Basic>> &l)
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
//    else if(is_a<SparseMatrix>(other))
//        return add_sparse_dense(static_cast<const SparseMatrix &>(other), *this);
}

// ----------------------------- Sparse Matrix -------------------------------//

//Constructors
//SparseMatrix::SparseMatrix(unsigned row, unsigned col)
//        : MatrixBase(row, col) {}

//SparseMatrix::SparseMatrix(unsigned row, unsigned col,
//        std::map<int, RCP<Basic>> &l): MatrixBase(row, col), m_{l} {}

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

    for(auto &it : sum) {
        it = add(*ait, *bit);
        ait++;
        bit++;
    }

    return rcp(new DenseMatrix(row, col, sum));
}

RCP<const DenseMatrix> add_sparse_dense(const SparseMatrix &A,
        const DenseMatrix &B)
{
    throw std::runtime_error("Not implemented.");
}

RCP<const SparseMatrix> add_sparse_sparse(const SparseMatrix &A,
        const SparseMatrix &B)
{
    throw std::runtime_error("Not implemented.");
}

} // CSymPy
