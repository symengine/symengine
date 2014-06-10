#ifndef CSYMPY_MATRIX_H
#define CSYMPY_MATRIX_H

#include "basic.h"

namespace CSymPy {

// Forward declaration
class SparseMatrix;

// Base class for matrices
class MatrixBase {
public:
    MatrixBase(unsigned row, unsigned col)
        : row_{row}, col_{col} {};

    // Below methods should be implemented by the derived classes. If not
    // applicable, raise an exception

    // Get the # of rows and # of columns
    unsigned nrows() const { return row_; }
    unsigned ncols() const { return col_; }

    // Get and set elements
    virtual RCP<const Basic>get(unsigned i) const = 0;
    virtual void set(unsigned i, RCP<const Basic> &e) = 0;

    virtual unsigned rank() const = 0;
    virtual RCP<const Basic> det() const = 0;
    virtual RCP<const MatrixBase> inv() const = 0;

    // Matrix addition
    virtual MatrixBase& add_matrix(const MatrixBase &other) const = 0;

    // Matrix Multiplication
    virtual MatrixBase& mul_matrix(const MatrixBase &other) const = 0;

protected:
    // Stores the dimension of the Matrix
    unsigned row_;
    unsigned col_;
};

// ----------------------------- Dense Matrix --------------------------------//

class DenseMatrix: public MatrixBase {
public:
    // Constructors
    DenseMatrix(unsigned row, unsigned col);
    DenseMatrix(unsigned row, unsigned col, const std::vector<RCP<const Basic>> &l);

    // Should implement all the virtual methods from MatrixBase
    // and throw an exception if a method is not applicable.

    // Get and set elements
    virtual RCP<const Basic> get(unsigned i) const;
    virtual void set(unsigned i, RCP<const Basic> &e);

    virtual unsigned rank() const;
    virtual RCP<const Basic> det() const;
    virtual RCP<const MatrixBase> inv() const;

    // Matrix addition
    virtual MatrixBase& add_matrix(const MatrixBase &other) const;
    friend void add_dense_dense(const DenseMatrix &A, const DenseMatrix &B,
        DenseMatrix &C);
    friend void add_dense_scalar(const DenseMatrix &A, RCP<const Basic> &k,
        DenseMatrix &B );

    // Matrix multiplication
    virtual MatrixBase& mul_matrix(const MatrixBase &other) const;
    friend void mul_dense_dense(const DenseMatrix &A, const DenseMatrix &B,
        DenseMatrix &C);
    friend void mul_dense_scalar(const DenseMatrix &A, RCP<const Basic> &k,
        DenseMatrix &C);

    // Gaussian elimination
    friend void gaussian_elimination(const DenseMatrix &A, DenseMatrix &B);

    // Ax = b
    friend void augment_dense(const DenseMatrix &A, const DenseMatrix &b,
        DenseMatrix &C);
    friend void diagonal_solve(const DenseMatrix &A, const DenseMatrix &b,
        DenseMatrix &C);

protected:
    // Matrix elements are stored in row-major order
    std::vector<RCP<const Basic>> m_;
};

// ----------------------------- Sparse Matrix -------------------------------//

class SparseMatrix: public MatrixBase {
public:
    // Constructors
    SparseMatrix(unsigned row, unsigned col);
    SparseMatrix(unsigned row, unsigned col,
            std::map<int, RCP<Basic>> &l);

    // Virtual functions inherited from Basic class
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;

    // Should implement all the virtual methods from MatrixBase
    // and throw an exception if a method is not applicable.

    // Get and Set elements
    virtual RCP<const Basic> get(unsigned i) const;
    virtual void set(unsigned i, RCP<const Basic> &e);

    virtual unsigned rank() const;
    virtual RCP<const Basic> det() const;
    virtual RCP<const MatrixBase> inv() const;

    // Matrix addition
    virtual MatrixBase& add_matrix(const MatrixBase &other) const;

    // Matrix Multiplication
    virtual MatrixBase& mul_matrix(const MatrixBase &other) const;

protected:
    std::map<int, RCP<Basic>> m_;
};

void add_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C);
void add_dense_scalar(const DenseMatrix &A, RCP<const Basic> &k, DenseMatrix &B);
void mul_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C);
void mul_dense_scalar(const DenseMatrix &A, RCP<const Basic> &k, DenseMatrix &C);
void gaussian_elimination(const DenseMatrix &A, DenseMatrix &B);
void augment_dense(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &C);
void diagonal_solve(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &C);

inline bool operator==(const MatrixBase &lhs, const MatrixBase &rhs)
{
    if (lhs.nrows() != rhs.nrows() || lhs.ncols() != rhs.ncols())
        return false;

    for (unsigned i = 0; i < lhs.nrows()*lhs.ncols(); i++)
        if(neq(lhs.get(i), rhs.get(i)))
            return false;

    return true;
}

} // CSymPy

#endif
