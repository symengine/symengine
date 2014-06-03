#ifndef CSYMPY_MATRIX_H
#define CSYMPY_MATRIX_H

#include "basic.h"

namespace CSymPy {

// Forward declaration
class SparseMatrix;

// Base class for matrices
class MatrixBase: public Basic {
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

    // These functions create a new instance of either DenseMatrix or
    // SparseMatrix and return a reference to the result
    virtual RCP<const MatrixBase> add_matrix(const MatrixBase &other) const = 0;
    virtual RCP<const MatrixBase> mul_matrix(const MatrixBase &other) const = 0;

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

    // Virtual functions inherited from Basic class
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;

    // Should implement all the virtual methods from MatrixBase
    // and throw an exception if a method is not applicable.
    // add_matrix, mul_matrix will have to find the correct function
    // to call depending on the `other` argument.

    // Get and set elements
    virtual RCP<const Basic> get(unsigned i) const;
    virtual void set(unsigned i, RCP<const Basic> &e);

    virtual unsigned rank() const;
    virtual RCP<const Basic> det() const;
    virtual RCP<const MatrixBase> inv() const;

    // Matrix addition
    virtual RCP<const MatrixBase> add_matrix(const MatrixBase &other) const;
    friend RCP<const DenseMatrix> add_dense_dense(const DenseMatrix &A,
            const DenseMatrix &B);
    friend RCP<const DenseMatrix> add_dense_scalar(const DenseMatrix &A,
            RCP<const Basic> &k);

    // Matrix multiplication
    virtual RCP<const MatrixBase> mul_matrix(const MatrixBase &other) const;
    friend RCP<const DenseMatrix> mul_dense_dense(const DenseMatrix &A,
            const DenseMatrix &B);
    friend RCP<const DenseMatrix> mul_dense_scalar(const DenseMatrix &A,
            RCP<const Basic> &k);

    // Gaussian elimination
    friend RCP<const DenseMatrix> gaussian_elimination(const DenseMatrix &A);

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
    // add_matrix and mul_matrix will have to find the correct function
    // to call depending on the `other` argument.

    // Get and Set elements
    virtual RCP<const Basic> get(unsigned i) const;
    virtual void set(unsigned i, RCP<Basic> &e);

    virtual unsigned rank() const;
    virtual RCP<const Basic> det() const;
    virtual RCP<const MatrixBase> inv() const;

    // Matrix addition
    virtual RCP<const MatrixBase> add_matrix(const MatrixBase &other) const;

    // Matrix Multiplication
    virtual RCP<const MatrixBase> mul_matrix(const MatrixBase &other) const;

protected:
    std::map<int, RCP<Basic>> m_;
};

inline RCP<const DenseMatrix> densematrix(unsigned row, unsigned col,
        const std::vector<RCP<const Basic>> &l)
{
    return rcp(new DenseMatrix(row, col, l));
}

} // CSymPy

#endif
