#ifndef SYMENGINE_MATRIX_H
#define SYMENGINE_MATRIX_H

#include <symengine/basic.h>

namespace SymEngine {

// Base class for matrices
class MatrixBase {
public:
    MatrixBase()
        : row_{0}, col_{0} {};
    MatrixBase(unsigned row, unsigned col)
        : row_{row}, col_{col} {};
    virtual ~MatrixBase() {};

    // Below methods should be implemented by the derived classes. If not
    // applicable, raise an exception

    // Get the # of rows and # of columns
    unsigned nrows() const { return row_; }
    unsigned ncols() const { return col_; }
    virtual bool eq(const MatrixBase &other) const;

    // Get and set elements
    virtual RCP<const Basic> get(unsigned i, unsigned j) const = 0;
    virtual void set(unsigned i, unsigned j, const RCP<const Basic> &e) = 0;

    // Print Matrix, very mundane version, should be overriden derived
    // class if better printing is available
    virtual std::string __str__() const;

    virtual unsigned rank() const = 0;
    virtual RCP<const Basic> det() const = 0;
    virtual void inv(MatrixBase &result) const = 0;

    // Matrix addition
    virtual void add_matrix(const MatrixBase &other, MatrixBase &result) const = 0;

    // Matrix Multiplication
    virtual void mul_matrix(const MatrixBase &other, MatrixBase &result) const = 0;

    // Add a scalar
    virtual void add_scalar(const RCP<const Basic> &k, MatrixBase &result) const = 0;

    // Multiply by a scalar
    virtual void mul_scalar(const RCP<const Basic> &k, MatrixBase &result) const = 0;

    // Matrix transpose
    virtual void transpose(MatrixBase &result) const = 0;

    // Extract out a submatrix
    virtual void submatrix( unsigned row_start,
                            unsigned row_end,
                            unsigned col_start,
                            unsigned col_end,
                            MatrixBase &result) const = 0;
    // LU factorization
    virtual void LU(MatrixBase &L, MatrixBase &U) const = 0;

    // LDL factorization
    virtual void LDL(MatrixBase &L, MatrixBase &D) const = 0;

    // Fraction free LU factorization
    virtual void FFLU(MatrixBase &LU) const = 0;

    // Fraction free LDU factorization
    virtual void FFLDU(MatrixBase&L, MatrixBase &D, MatrixBase &U) const = 0;

    // Solve Ax = b using LU factorization
    virtual void LU_solve(const MatrixBase &b, MatrixBase &x) const = 0;

protected:
    // Stores the dimension of the Matrix
    unsigned row_;
    unsigned col_;
};

// ----------------------------- Dense Matrix --------------------------------//
class DenseMatrix: public MatrixBase {
public:
    // Constructors
    DenseMatrix();
    DenseMatrix(unsigned row, unsigned col);
    DenseMatrix(unsigned row, unsigned col, const vec_basic &l);

    // Should implement all the virtual methods from MatrixBase
    // and throw an exception if a method is not applicable.

    // Get and set elements
    virtual RCP<const Basic> get(unsigned i, unsigned j) const;
    virtual void set(unsigned i, unsigned j, const RCP<const Basic> &e);

    virtual unsigned rank() const;
    virtual RCP<const Basic> det() const;
    virtual void inv(MatrixBase &result) const;

    // Matrix addition
    virtual void add_matrix(const MatrixBase &other, MatrixBase &result) const;

    // Matrix multiplication
    virtual void mul_matrix(const MatrixBase &other, MatrixBase &result) const;

    // Add a scalar
    virtual void add_scalar(const RCP<const Basic> &k, MatrixBase &result) const;

    // Multiply by a scalar
    virtual void mul_scalar(const RCP<const Basic> &k, MatrixBase &result) const;

    // Matrix transpose
    virtual void transpose(MatrixBase &result) const;

    // Extract out a submatrix
    virtual void submatrix( unsigned row_start,
                            unsigned row_end,
                            unsigned col_start,
                            unsigned col_end,
                            MatrixBase &result) const;

    // LU factorization
    virtual void LU(MatrixBase &L, MatrixBase &U) const;

    // LDL factorization
    virtual void LDL(MatrixBase &L, MatrixBase &D) const;

    // Solve Ax = b using LU factorization
    virtual void LU_solve(const MatrixBase &b, MatrixBase &x) const;

    // Fraction free LU factorization
    virtual void FFLU(MatrixBase &LU) const;

    // Fraction free LDU factorization
    virtual void FFLDU(MatrixBase&L, MatrixBase &D, MatrixBase &U) const;

    // Return the Jacobian of the matrix
    friend void jacobian(const DenseMatrix &A, const DenseMatrix &x,
            DenseMatrix &result);

    // Friend functions related to Matrix Operations
    friend void add_dense_dense(const DenseMatrix &A, const DenseMatrix &B,
        DenseMatrix &C);
    friend void add_dense_scalar(const DenseMatrix &A, const RCP<const Basic> &k,
        DenseMatrix &B );
    friend void mul_dense_dense(const DenseMatrix &A, const DenseMatrix &B,
        DenseMatrix &C);
    friend void mul_dense_scalar(const DenseMatrix &A, const RCP<const Basic> &k,
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
    friend void diagonal_solve(const DenseMatrix &A, const DenseMatrix &b,
        DenseMatrix &x);
    friend void back_substitution(const DenseMatrix &U, const DenseMatrix &b,
        DenseMatrix &x);
    friend void forward_substitution(const DenseMatrix &A,
        const DenseMatrix &b, DenseMatrix &x);
    friend void fraction_free_gaussian_elimination_solve(const DenseMatrix &A,
        const DenseMatrix &b, DenseMatrix &x);
    friend void fraction_free_gauss_jordan_solve(const DenseMatrix &A,
        const DenseMatrix &b, DenseMatrix &x);

    // Matrix Decomposition
    friend void fraction_free_LU(const DenseMatrix &A, DenseMatrix &LU);
    friend void LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &U);
    friend void fraction_free_LDU(const DenseMatrix &A, DenseMatrix &L,
        DenseMatrix &D, DenseMatrix &U);
    friend void QR(const DenseMatrix &A, DenseMatrix &Q, DenseMatrix &R);
    friend void LDL(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &D);
    friend void cholesky(const DenseMatrix &A, DenseMatrix &L);

    // Matrix queries
    friend bool is_symmetric_dense(const DenseMatrix &A);

    // Determinant
    friend RCP<const Basic> det_bareis(const DenseMatrix &A);
    friend void berkowitz(const DenseMatrix &A, std::vector<DenseMatrix> &polys);

    // Inverse
    friend void inverse_fraction_free_LU(const DenseMatrix &A, DenseMatrix &B);
    friend void inverse_LU(const DenseMatrix &A, DenseMatrix&B);
    friend void inverse_gauss_jordan(const DenseMatrix &A, DenseMatrix &B);

    // NumPy-like functions
    friend void eye(DenseMatrix &A, unsigned N, unsigned M, int k);
    friend void diag(DenseMatrix &A, vec_basic &v, int k);
    friend void ones(DenseMatrix &A, unsigned rows, unsigned cols);
    friend void zeros(DenseMatrix &A, unsigned rows, unsigned cols);

protected:
    // Matrix elements are stored in row-major order
    vec_basic m_;
};

// ----------------------------- Sparse Matrices -----------------------------//
class CSRMatrix: public MatrixBase {
public:
    CSRMatrix();
    CSRMatrix(unsigned row, unsigned col);
    CSRMatrix(unsigned row, unsigned col, std::vector<unsigned>&& p,
        std::vector<unsigned>&& j, vec_basic&& x);

    bool is_canonical() const;

    virtual bool eq(const MatrixBase &other) const;

    // Get and set elements
    virtual RCP<const Basic> get(unsigned i, unsigned j) const;
    virtual void set(unsigned i, unsigned j, const RCP<const Basic> &e);

    virtual unsigned rank() const;
    virtual RCP<const Basic> det() const;
    virtual void inv(MatrixBase &result) const;

    // Matrix addition
    virtual void add_matrix(const MatrixBase &other, MatrixBase &result) const;

    // Matrix Multiplication
    virtual void mul_matrix(const MatrixBase &other, MatrixBase &result) const;

    // Add a scalar
    virtual void add_scalar(const RCP<const Basic> &k, MatrixBase &result) const;

    // Multiply by a scalar
    virtual void mul_scalar(const RCP<const Basic> &k, MatrixBase &result) const;

    // Matrix transpose
    virtual void transpose(MatrixBase &result) const;

    // Extract out a submatrix
    virtual void submatrix( unsigned row_start,
                            unsigned row_end,
                            unsigned col_start,
                            unsigned col_end,
                            MatrixBase &result) const;

    // LU factorization
    virtual void LU(MatrixBase &L, MatrixBase &U) const;

    // LDL factorization
    virtual void LDL(MatrixBase &L, MatrixBase &D) const;

    // Solve Ax = b using LU factorization
    virtual void LU_solve(const MatrixBase &b, MatrixBase &x) const;

    // Fraction free LU factorization
    virtual void FFLU(MatrixBase &LU) const;

    // Fraction free LDU factorization
    virtual void FFLDU(MatrixBase&L, MatrixBase &D, MatrixBase &U) const;

    static void csr_sum_duplicates(std::vector<unsigned>& p_,
        std::vector<unsigned>& j_,
        vec_basic& x_,
        unsigned row_);

    static void csr_sort_indices(std::vector<unsigned>& p_,
        std::vector<unsigned>& j_,
        vec_basic& x_,
        unsigned row_);

    static bool csr_has_sorted_indices(const std::vector<unsigned>& p_,
        const std::vector<unsigned>& j_,
        unsigned row_);

    static bool csr_has_duplicates(const std::vector<unsigned>& p_,
        const std::vector<unsigned>& j_,
        unsigned row_);

    static bool csr_has_canonical_format(const std::vector<unsigned>& p_,
        const std::vector<unsigned>& j_,
        unsigned row_);

    static CSRMatrix from_coo(unsigned row, unsigned col,
        const std::vector<unsigned>& i, const std::vector<unsigned>& j,
        const vec_basic& x);

    friend void csr_matmat_pass1(const CSRMatrix &A, const CSRMatrix &B,
        CSRMatrix &C);
    friend void csr_matmat_pass2(const CSRMatrix &A, const CSRMatrix &B,
        CSRMatrix &C);
    friend void csr_diagonal(const CSRMatrix& A, DenseMatrix& D);
    friend void csr_scale_rows(CSRMatrix& A, const DenseMatrix& X);
    friend void csr_scale_columns(CSRMatrix& A, const DenseMatrix& X);

    friend void csr_binop_csr_canonical(const CSRMatrix& A, const CSRMatrix& B,
        CSRMatrix& C,
        RCP<const Basic> (&bin_op)(const RCP<const Basic>&, const RCP<const Basic>&));

protected:
    std::vector<unsigned> p_;
    std::vector<unsigned> j_;
    vec_basic x_;
};

// Return the Jacobian of the matrix
void jacobian(const DenseMatrix &A, const DenseMatrix &x,
        DenseMatrix &result);

// Matrix Factorization
void LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &U);

void LDL(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &D);

// Inverse
void inverse_fraction_free_LU(const DenseMatrix &A, DenseMatrix &B);

void inverse_gauss_jordan(const DenseMatrix &A, DenseMatrix &B);

// Solving Ax = b
void fraction_free_LU_solve(const DenseMatrix &A, const DenseMatrix &b,
    DenseMatrix &x);

void fraction_free_gauss_jordan_solve(const DenseMatrix &A,
    const DenseMatrix &b, DenseMatrix &x);

void LU_solve(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &x);

void LDL_solve(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &x);

// Determinant
RCP<const Basic> det_berkowitz(const DenseMatrix &A);

// Characteristic polynomial: Only the coefficients of monomials in decreasing
// order of monomial powers is returned, i.e. if `B = transpose([1, -2, 3])`
// then the corresponding polynomial is `x**2 - 2x + 3`.
void char_poly(const DenseMatrix &A, DenseMatrix &B);

// Mimic `eye` function in NumPy
void eye(DenseMatrix &A, unsigned N, unsigned M = 0, int k = 0);

// Create diagonal matrices directly
void diag(DenseMatrix &A, vec_basic &v, int k = 0);

// Create a matrix filled with ones
void ones(DenseMatrix &A, unsigned rows, unsigned cols);

// Create a matrix filled with zeros
void zeros(DenseMatrix &A, unsigned rows, unsigned cols);

// Returns true if `b` is exactly the type T.
// Here T can be a DenseMatrix, CSRMatrix, etc.
template <class T>
inline bool is_a(const MatrixBase &b)
{
    return typeid(T) == typeid(b);
}

// Test two matrices for equality
inline bool operator==(const SymEngine::MatrixBase &lhs,
    const SymEngine::MatrixBase &rhs)
{
    return lhs.eq(rhs);
}

} // SymEngine

// Print Matrix
inline std::ostream& operator<<(std::ostream& out, const SymEngine::MatrixBase& A)
{
    return out << A.__str__();
}

#endif
