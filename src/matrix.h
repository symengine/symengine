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
    friend void transpose_dense(const DenseMatrix &A, DenseMatrix &B);
    friend void submatrix_dense(const DenseMatrix &A, unsigned row_start,
        unsigned row_end, unsigned col_start, unsigned col_end, DenseMatrix &B);

    // Row operations
    friend void row_exchange_dense(DenseMatrix &A , unsigned i, unsigned j);
    friend void row_mul_scalar_dense(DenseMatrix &A, unsigned i, RCP<const Basic> &c);
    friend void row_add_row_dense(DenseMatrix &A, unsigned i, unsigned j,
        RCP<const Basic> &c);

    // Gaussian elimination
    friend void pivoted_gaussian_elimination(const DenseMatrix &A,
        DenseMatrix &B, std::vector<unsigned> &pivotlist);
    friend void fraction_free_gaussian_elimination(const DenseMatrix &A,
        DenseMatrix &B);
    friend void pivoted_fraction_free_gaussian_elimination(const DenseMatrix &A,
        DenseMatrix &B, std::vector<unsigned> &pivotlist);
    friend void pivoted_gauss_jordan_elimination(const DenseMatrix &A,
        DenseMatrix &B, std::vector<unsigned> &pivotlist);
    friend void fraction_free_gauss_jordan_elimination(const DenseMatrix &A,
        DenseMatrix &B);
    friend void pivoted_fraction_free_gauss_jordan_elimination(const DenseMatrix &A,
        DenseMatrix &B, std::vector<unsigned> &pivotlist);
    friend unsigned pivot(DenseMatrix &B, unsigned r, unsigned c);

    // Ax = b
    friend void augment_dense(const DenseMatrix &A, const DenseMatrix &b,
        DenseMatrix &C);
    friend void diagonal_solve(const DenseMatrix &A, const DenseMatrix &b,
        DenseMatrix &x);
    friend void back_substitution(const DenseMatrix &U, const DenseMatrix &b,
        DenseMatrix &x);
    friend void fraction_free_gaussian_elimination_solve(const DenseMatrix &A,
        const DenseMatrix &b, DenseMatrix &x);
    friend void fraction_free_gauss_jordan_solve(const DenseMatrix &A,
        const DenseMatrix &b, DenseMatrix &x);

    // Matrix Decomposition
    friend void fraction_free_LU(const DenseMatrix &A, DenseMatrix &L,
        DenseMatrix &U);
    friend void LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &U);
    friend void fraction_free_LU(const DenseMatrix &A, DenseMatrix &L,
        DenseMatrix &D, DenseMatrix &U);
    friend void QR(const DenseMatrix &A, DenseMatrix &Q, DenseMatrix &R);
    friend void LDL(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &D);
    friend void cholesky(const DenseMatrix &A, DenseMatrix &L);

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

// ------------------ DenseMatrix related functions --------------------------//

// Matrix operations
void add_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C);

void add_dense_scalar(const DenseMatrix &A, RCP<const Basic> &k, DenseMatrix &B);

void mul_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C);

void mul_dense_scalar(const DenseMatrix &A, RCP<const Basic> &k, DenseMatrix &C);

// Common functions
void transpose_dense(const DenseMatrix &A, DenseMatrix &B);

void submatrix_dense(const DenseMatrix &A, unsigned row_start, unsigned row_end,
        unsigned col_start, unsigned col_end, DenseMatrix &B);

void augment_dense(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &C);

// Row operations
void row_exchange_dense(DenseMatrix &A , unsigned i, unsigned j);

void row_mul_scalar_dense(DenseMatrix &A, unsigned i, RCP<const Basic> &c);

void row_add_row_dense(DenseMatrix &A, unsigned i, unsigned j,
    RCP<const Basic> &c);

// Gaussian elimination
void pivoted_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B,
    std::vector<unsigned> &pivotlist);

void fraction_free_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B);

void pivoted_fraction_free_gaussian_elimination(const DenseMatrix &A,
    DenseMatrix &B, std::vector<unsigned> &pivotlist);

void pivoted_gauss_jordan_elimination(const DenseMatrix &A, DenseMatrix &B,
    std::vector<unsigned> &pivotlist);

void fraction_free_gauss_jordan_elimination(const DenseMatrix &A, DenseMatrix &B);

void pivoted_fraction_free_gauss_jordan_elimination(const DenseMatrix &A,
    DenseMatrix &B, std::vector<unsigned> &pivotlist);

// Matrix Decomposition
void fraction_free_LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &U);

void LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &U);

void fraction_free_LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &D,
    DenseMatrix &U);

void QR(const DenseMatrix &A, DenseMatrix &Q, DenseMatrix &R);

void LDL(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &D);

void cholesky(const DenseMatrix &A, DenseMatrix &L);

// Solve Ax = b
void diagonal_solve(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &x);

void back_substitution(const DenseMatrix &U, const DenseMatrix &b,
    DenseMatrix &x);

void fraction_free_gaussian_elimination_solve(const DenseMatrix &A,
    const DenseMatrix &b, DenseMatrix &x);

void fraction_free_gauss_jordan_solve(const DenseMatrix &A, const DenseMatrix &b,
    DenseMatrix &x);

// ------------------------ Common functions ---------------------------------//

// Test two matrices for equality
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
