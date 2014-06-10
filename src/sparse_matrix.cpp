#include "matrix.h"
#include "add.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {

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
MatrixBase& SparseMatrix::add_matrix(const MatrixBase &other) const
{
    throw std::runtime_error("Not implemented.");
}

// Matrix multiplication
MatrixBase& SparseMatrix::mul_matrix(const MatrixBase &other) const
{
    throw std::runtime_error("Not implemented.");
}

} // CSymPy
