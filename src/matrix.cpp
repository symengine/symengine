#include "matrix.h"

namespace CSymPy {

// ----------------------------- Dense Matrix --------------------------------//

DenseMatrix::DenseMatrix(unsigned row, unsigned col)
    : MatrixBase(row, col) {}

DenseMatrix::DenseMatrix(unsigned row, unsigned col, std::vector<RCP<Basic>> &l)
    : MatrixBase(row, col), m_{l}
{
    CSYMPY_ASSERT(m_.size() == row*col)
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

// ----------------------------- Sparse Matrix -------------------------------//

SparseMatrix::SparseMatrix(unsigned row, unsigned col)
    : MatrixBase(row, col) {}

SparseMatrix::SparseMatrix(unsigned row, unsigned col,
    std::map<int, RCP<Basic>> &l)
    : MatrixBase(row, col), m_{l} {}

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

} // CSymPy
