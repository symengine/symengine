#ifndef CSYMPY_INTEGER_H
#define CSYMPY_INTEGER_H

#include "basic.h"

namespace CSymPy {

// Base class for matrices
class MatrixBase {
public:
    MatrixBase(unsigned row, unsigned col)
        : row_{row}, col_{col} {};

    // These functions create a new instance of either DenseMatrix or
    // SparseMatrix and return a reference to the result
    virtual RCP<const MatrixBase> add_matrix(const MatrixBase &other) const = 0;
    virtual RCP<const MatrixBase> mul_matrix(const MatrixBase &other) const = 0;
    virtual RCP<const MatrixBase> add_basic(const Basic &other) const = 0;
    virtual RCP<const MatrixBase> mul_basic(const Basic &other) const = 0;

    // To get the values of row and col
    unsigned nrows() {
        return row_;
    };

    unsigned ncols() {
        return col_;
    };

    // These should be implemented by the derived classes. If not applicable,
    // raise an exception
    virtual RCP<const Basic>get(unsigned i) const = 0;
    virtual void set(unsigned i, RCP<Basic> &e) const = 0;
    virtual unsigned rank() const = 0;
    virtual RCP<const Basic> det() const = 0;
    virtual RCP<const MatrixBase> inv() const = 0;

public:
    // Stores the dimension of the Matrix
    unsigned row_;
    unsigned col_;
};

class DenseMatrix: public MatrixBase {
public:
    // Constructors
    DenseMatrix(unsigned row, unsigned column);
    DenseMatrix(unsigned row, unsigned column, std::vector<RCP<Basic>> &l);

    // Should implement all the virtual methods from MatrixBase
    // and throw an exception if a method is not applicable.
    // add_matrix, mul_matrix will have to find the correct function
    // to call depending on the `other` argument.

    // Get and Set elements
    RCP<const Basic> get(unsigned i) const {
        return m_[i];
    }
    void set(unsigned i, RCP<Basic> &e) {
        m_[i] = e;
    }

    unsigned rank() const;
    RCP<const Basic> det() const;
    RCP<const MatrixBase> inv() const;

private:
    // Matrix elements are sotred in row-major order
    std::vector<RCP<Basic>> m_;
};

class SparseMatrix: public MatrixBase {
public:
    // Constructors
    SparseMatrix(unsigned row, unsigned column);
    SparseMatrix(unsigned row, unsigned column,
        std::map<int, RCP<Basic>> &l);

    // Should implement all the virtual methods from MatrixBase
    // and throw an exception if a method is not applicable.
    // add_matrix and mul_matrix will have to find the correct function
    // to call depending on the `other` argument.

    // Get and Set elements
    RCP<const Basic> get(unsigned i) const;
    void set(unsigned i, RCP<Basic> &e);

    unsigned rank() const;
    RCP<const Basic> det() const;
    RCP<const MatrixBase> inv() const;

private:
    std::map<int, RCP<Basic>> m_;
};

RCP<const DenseMatrix> mul_dense_dense(const DenseMatrix &A, const DenseMatrix &B);
RCP<const DenseMatrix> add_dense_dense(const DenseMatrix &A, const DenseMatrix &B);
RCP<const SparseMatrix> mul_sparse_sparse(const SparseMatrix &A, const SparseMatrix &B);
RCP<const SparseMatrix> add_sparse_sparse(const SparseMatrix &A, const SparseMatrix &B);
RCP<const DenseMatrix> mul_sparse_dense(const SparseMatrix &A, const DenseMatrix &B);
RCP<const DenseMatrix> add_sparse_dense(const SparseMatrix &A, const DenseMatrix &B);

} // CSymPy

#endif
