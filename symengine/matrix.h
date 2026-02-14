#ifndef SYMENGINE_MATRIX_H
#define SYMENGINE_MATRIX_H

#include <symengine/basic.h>
#include <symengine/sets.h>

namespace SymEngine
{

enum MatrixTypeID {
    SYMENGINE_DENSE_MATRIX,
    SYMENGINE_CSR_MATRIX,
};

// Base class for matrices
class SYMENGINE_EXPORT MatrixBase
{
public:
    virtual ~MatrixBase(){};

    virtual MatrixTypeID get_type_code() const = 0;

    bool is_square() const
    {
        return ncols() == nrows();
    }

    // Below methods should be implemented by the derived classes. If not
    // applicable, raise an exception

    // Get the # of rows and # of columns
    virtual unsigned nrows() const = 0;
    virtual unsigned ncols() const = 0;
    virtual bool eq(const MatrixBase &other) const;

    virtual tribool is_real(const Assumptions *assumptions = nullptr) const = 0;

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
    virtual void add_matrix(const MatrixBase &other,
                            MatrixBase &result) const = 0;

    // Matrix Multiplication
    virtual void mul_matrix(const MatrixBase &other,
                            MatrixBase &result) const = 0;

    // Matrix elementwise Multiplication
    virtual void elementwise_mul_matrix(const MatrixBase &other,
                                        MatrixBase &result) const = 0;

    // Add a scalar
    virtual void add_scalar(const RCP<const Basic> &k,
                            MatrixBase &result) const = 0;

    // Multiply by a scalar
    virtual void mul_scalar(const RCP<const Basic> &k,
                            MatrixBase &result) const = 0;

    // Matrix conjugate
    virtual void conjugate(MatrixBase &result) const = 0;

    // Matrix transpose
    virtual void transpose(MatrixBase &result) const = 0;

    // Matrix conjugate transpose
    virtual void conjugate_transpose(MatrixBase &result) const = 0;

    // Extract out a submatrix
    virtual void submatrix(MatrixBase &result, unsigned row_start,
                           unsigned col_start, unsigned row_end,
                           unsigned col_end, unsigned row_step = 1,
                           unsigned col_step = 1) const = 0;
    // LU factorization
    virtual void LU(MatrixBase &L, MatrixBase &U) const = 0;

    // LDL factorization
    virtual void LDL(MatrixBase &L, MatrixBase &D) const = 0;

    // Fraction free LU factorization
    virtual void FFLU(MatrixBase &LU) const = 0;

    // Fraction free LDU factorization
    virtual void FFLDU(MatrixBase &L, MatrixBase &D, MatrixBase &U) const = 0;

    // QR factorization
    virtual void QR(MatrixBase &Q, MatrixBase &R) const = 0;

    // Cholesky decomposition
    virtual void cholesky(MatrixBase &L) const = 0;

    // Solve Ax = b using LU factorization
    virtual void LU_solve(const MatrixBase &b, MatrixBase &x) const = 0;
};

typedef std::vector<std::pair<int, int>> permutelist;

class CSRMatrix;

// ----------------------------- Dense Matrix --------------------------------//
class SYMENGINE_EXPORT DenseMatrix : public MatrixBase
{
public:
    // Constructors
    DenseMatrix();
    DenseMatrix(const DenseMatrix &) = default;
    DenseMatrix(unsigned row, unsigned col);
    DenseMatrix(unsigned row, unsigned col, const vec_basic &l);
    DenseMatrix(const vec_basic &column_elements);
    DenseMatrix &operator=(const DenseMatrix &other) = default;
    // type_code
    const static MatrixTypeID type_code_id = SYMENGINE_DENSE_MATRIX;
    virtual MatrixTypeID get_type_code() const override
    {
        return SYMENGINE_DENSE_MATRIX;
    }
    // Resize
    void resize(unsigned i, unsigned j);

    // Should implement all the virtual methods from MatrixBase
    // and throw an exception if a method is not applicable.

    // Get and set elements
    RCP<const Basic> get(unsigned i, unsigned j) const override;
    void set(unsigned i, unsigned j, const RCP<const Basic> &e) override;
    virtual vec_basic as_vec_basic() const;

    unsigned nrows() const override
    {
        return row_;
    }
    unsigned ncols() const override
    {
        return col_;
    }

    //! Returns a string of the instance serialized.
    std::string dumps() const;

    //! Creates an instance of a serialized string.
    static DenseMatrix loads(const std::string &);

    virtual bool is_lower() const;
    virtual bool is_upper() const;
    virtual tribool is_zero() const;
    virtual tribool is_diagonal() const;
    tribool is_real(const Assumptions *assumptions = nullptr) const override;
    virtual tribool is_symmetric() const;
    virtual tribool is_hermitian() const;
    virtual tribool is_weakly_diagonally_dominant() const;
    virtual tribool is_strictly_diagonally_dominant() const;
    virtual tribool is_positive_definite() const;
    virtual tribool is_negative_definite() const;

    RCP<const Basic> trace() const;
    unsigned rank() const override;
    RCP<const Basic> det() const override;
    void inv(MatrixBase &result) const override;

    // Matrix addition
    void add_matrix(const MatrixBase &other, MatrixBase &result) const override;

    // Matrix multiplication
    void mul_matrix(const MatrixBase &other, MatrixBase &result) const override;

    // Matrix elementwise Multiplication
    void elementwise_mul_matrix(const MatrixBase &other,
                                MatrixBase &result) const override;

    // Add a scalar
    void add_scalar(const RCP<const Basic> &k,
                    MatrixBase &result) const override;

    // Multiply by a scalar
    void mul_scalar(const RCP<const Basic> &k,
                    MatrixBase &result) const override;

    // Matrix conjugate
    void conjugate(MatrixBase &result) const override;

    // Matrix transpose
    void transpose(MatrixBase &result) const override;

    // Matrix conjugate transpose
    void conjugate_transpose(MatrixBase &result) const override;

    // Extract out a submatrix
    void submatrix(MatrixBase &result, unsigned row_start, unsigned col_start,
                   unsigned row_end, unsigned col_end, unsigned row_step = 1,
                   unsigned col_step = 1) const override;

    // LU factorization
    void LU(MatrixBase &L, MatrixBase &U) const override;

    // LDL factorization
    void LDL(MatrixBase &L, MatrixBase &D) const override;

    // Solve Ax = b using LU factorization
    void LU_solve(const MatrixBase &b, MatrixBase &x) const override;

    // Fraction free LU factorization
    void FFLU(MatrixBase &LU) const override;

    // Fraction free LDU factorization
    void FFLDU(MatrixBase &L, MatrixBase &D, MatrixBase &U) const override;

    // QR factorization
    void QR(MatrixBase &Q, MatrixBase &R) const override;

    // Cholesky decomposition
    void cholesky(MatrixBase &L) const override;

    // Return the Jacobian of the matrix
    SYMENGINE_EXPORT friend void jacobian(const DenseMatrix &A,
                                          const DenseMatrix &x,
                                          DenseMatrix &result, bool diff_cache);
    // Return the Jacobian of the matrix using sdiff
    SYMENGINE_EXPORT friend void sjacobian(const DenseMatrix &A,
                                           const DenseMatrix &x,
                                           DenseMatrix &result,
                                           bool diff_cache);

    // Differentiate the matrix element-wise
    SYMENGINE_EXPORT friend void diff(const DenseMatrix &A,
                                      const RCP<const Symbol> &x,
                                      DenseMatrix &result, bool diff_cache);
    // Differentiate the matrix element-wise using SymPy compatible diff
    SYMENGINE_EXPORT friend void sdiff(const DenseMatrix &A,
                                       const RCP<const Basic> &x,
                                       DenseMatrix &result, bool diff_cache);

    // Friend functions related to Matrix Operations
    SYMENGINE_EXPORT friend void
    add_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C);
    SYMENGINE_EXPORT friend void add_dense_scalar(const DenseMatrix &A,
                                                  const RCP<const Basic> &k,
                                                  DenseMatrix &B);
    SYMENGINE_EXPORT friend void
    mul_dense_dense(const DenseMatrix &A, const DenseMatrix &B, DenseMatrix &C);
    SYMENGINE_EXPORT friend void
    elementwise_mul_dense_dense(const DenseMatrix &A, const DenseMatrix &B,
                                DenseMatrix &C);
    SYMENGINE_EXPORT friend void mul_dense_scalar(const DenseMatrix &A,
                                                  const RCP<const Basic> &k,
                                                  DenseMatrix &C);
    SYMENGINE_EXPORT friend void conjugate_dense(const DenseMatrix &A,
                                                 DenseMatrix &B);
    SYMENGINE_EXPORT friend void transpose_dense(const DenseMatrix &A,
                                                 DenseMatrix &B);
    SYMENGINE_EXPORT friend void conjugate_transpose_dense(const DenseMatrix &A,
                                                           DenseMatrix &B);
    SYMENGINE_EXPORT friend void
    submatrix_dense(const DenseMatrix &A, DenseMatrix &B, unsigned row_start,
                    unsigned col_start, unsigned row_end, unsigned col_end,
                    unsigned row_step, unsigned col_step);
    void row_join(const DenseMatrix &B);
    void col_join(const DenseMatrix &B);
    void row_insert(const DenseMatrix &B, unsigned pos);
    void col_insert(const DenseMatrix &B, unsigned pos);
    void row_del(unsigned k);
    void col_del(unsigned k);

    // Row operations
    SYMENGINE_EXPORT friend void row_exchange_dense(DenseMatrix &A, unsigned i,
                                                    unsigned j);
    SYMENGINE_EXPORT friend void
    row_mul_scalar_dense(DenseMatrix &A, unsigned i, RCP<const Basic> &c);
    SYMENGINE_EXPORT friend void row_add_row_dense(DenseMatrix &A, unsigned i,
                                                   unsigned j,
                                                   RCP<const Basic> &c);
    SYMENGINE_EXPORT friend void permuteFwd(DenseMatrix &A, permutelist &pl);

    // Column operations
    SYMENGINE_EXPORT friend void column_exchange_dense(DenseMatrix &A,
                                                       unsigned i, unsigned j);

    // Gaussian elimination
    SYMENGINE_EXPORT friend void
    pivoted_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B,
                                 permutelist &pivotlist);
    SYMENGINE_EXPORT friend void
    fraction_free_gaussian_elimination(const DenseMatrix &A, DenseMatrix &B);
    SYMENGINE_EXPORT friend void pivoted_fraction_free_gaussian_elimination(
        const DenseMatrix &A, DenseMatrix &B, permutelist &pivotlist);
    SYMENGINE_EXPORT friend void
    pivoted_gauss_jordan_elimination(const DenseMatrix &A, DenseMatrix &B,
                                     permutelist &pivotlist);
    SYMENGINE_EXPORT friend void
    fraction_free_gauss_jordan_elimination(const DenseMatrix &A,
                                           DenseMatrix &B);
    SYMENGINE_EXPORT friend void pivoted_fraction_free_gauss_jordan_elimination(
        const DenseMatrix &A, DenseMatrix &B, permutelist &pivotlist);
    friend unsigned pivot(DenseMatrix &B, unsigned r, unsigned c);

    SYMENGINE_EXPORT friend void reduced_row_echelon_form(const DenseMatrix &A,
                                                          DenseMatrix &B,
                                                          vec_uint &pivot_cols,
                                                          bool normalize_last);

    // Ax = b
    SYMENGINE_EXPORT friend void
    diagonal_solve(const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &x);
    SYMENGINE_EXPORT friend void back_substitution(const DenseMatrix &U,
                                                   const DenseMatrix &b,
                                                   DenseMatrix &x);
    SYMENGINE_EXPORT friend void forward_substitution(const DenseMatrix &A,
                                                      const DenseMatrix &b,
                                                      DenseMatrix &x);
    SYMENGINE_EXPORT friend void fraction_free_gaussian_elimination_solve(
        const DenseMatrix &A, const DenseMatrix &b, DenseMatrix &x);
    SYMENGINE_EXPORT friend void
    fraction_free_gauss_jordan_solve(const DenseMatrix &A, const DenseMatrix &b,
                                     DenseMatrix &x, bool pivot);

    // Matrix Decomposition
    SYMENGINE_EXPORT friend void fraction_free_LU(const DenseMatrix &A,
                                                  DenseMatrix &LU);
    SYMENGINE_EXPORT friend void LU(const DenseMatrix &A, DenseMatrix &L,
                                    DenseMatrix &U);
    SYMENGINE_EXPORT friend void pivoted_LU(const DenseMatrix &A,
                                            DenseMatrix &LU, permutelist &pl);
    SYMENGINE_EXPORT friend void pivoted_LU(const DenseMatrix &A,
                                            DenseMatrix &L, DenseMatrix &U,
                                            permutelist &pl);
    SYMENGINE_EXPORT friend void fraction_free_LDU(const DenseMatrix &A,
                                                   DenseMatrix &L,
                                                   DenseMatrix &D,
                                                   DenseMatrix &U);
    SYMENGINE_EXPORT friend void QR(const DenseMatrix &A, DenseMatrix &Q,
                                    DenseMatrix &R);
    SYMENGINE_EXPORT friend void LDL(const DenseMatrix &A, DenseMatrix &L,
                                     DenseMatrix &D);
    SYMENGINE_EXPORT friend void cholesky(const DenseMatrix &A, DenseMatrix &L);

    // Matrix queries
    SYMENGINE_EXPORT friend bool is_symmetric_dense(const DenseMatrix &A);

    // Determinant
    SYMENGINE_EXPORT friend RCP<const Basic> det_bareis(const DenseMatrix &A);
    SYMENGINE_EXPORT friend void berkowitz(const DenseMatrix &A,
                                           std::vector<DenseMatrix> &polys);

    // Inverse
    SYMENGINE_EXPORT friend void inverse_fraction_free_LU(const DenseMatrix &A,
                                                          DenseMatrix &B);
    SYMENGINE_EXPORT friend void inverse_LU(const DenseMatrix &A,
                                            DenseMatrix &B);
    SYMENGINE_EXPORT friend void inverse_pivoted_LU(const DenseMatrix &A,
                                                    DenseMatrix &B);
    SYMENGINE_EXPORT friend void inverse_gauss_jordan(const DenseMatrix &A,
                                                      DenseMatrix &B);

    // Vector-specific methods
    SYMENGINE_EXPORT friend void dot(const DenseMatrix &A, const DenseMatrix &B,
                                     DenseMatrix &C);
    SYMENGINE_EXPORT friend void cross(const DenseMatrix &A,
                                       const DenseMatrix &B, DenseMatrix &C);

    // NumPy-like functions
    SYMENGINE_EXPORT friend void eye(DenseMatrix &A, int k);
    SYMENGINE_EXPORT friend void diag(DenseMatrix &A, vec_basic &v, int k);
    SYMENGINE_EXPORT friend void ones(DenseMatrix &A);
    SYMENGINE_EXPORT friend void zeros(DenseMatrix &A);

    friend CSRMatrix;

private:
    // Matrix elements are stored in row-major order
    vec_basic m_;
    // Stores the dimension of the Matrix
    unsigned row_;
    unsigned col_;

    tribool shortcut_to_posdef() const;
    tribool is_positive_definite_GE();
};

// ----------------------------- Sparse Matrices -----------------------------//
class SYMENGINE_EXPORT CSRMatrix : public MatrixBase
{
public:
    CSRMatrix();
    CSRMatrix(unsigned row, unsigned col);
    CSRMatrix(unsigned row, unsigned col, const std::vector<unsigned> &p,
              const std::vector<unsigned> &j, const vec_basic &x);
    CSRMatrix(unsigned row, unsigned col, std::vector<unsigned> &&p,
              std::vector<unsigned> &&j, vec_basic &&x);
    CSRMatrix &operator=(CSRMatrix &&other);
    CSRMatrix(const CSRMatrix &) = default;
    const static MatrixTypeID type_code_id = SYMENGINE_CSR_MATRIX;
    virtual MatrixTypeID get_type_code() const override
    {
        return SYMENGINE_CSR_MATRIX;
    }
    std::tuple<std::vector<unsigned>, std::vector<unsigned>, vec_basic>
    as_vectors() const;

    bool is_canonical() const;

    bool eq(const MatrixBase &other) const override;

    // Get and set elements
    RCP<const Basic> get(unsigned i, unsigned j) const override;
    void set(unsigned i, unsigned j, const RCP<const Basic> &e) override;

    unsigned nrows() const override
    {
        return row_;
    }
    unsigned ncols() const override
    {
        return col_;
    }

    tribool is_real(const Assumptions *assumptions = nullptr) const override;
    unsigned rank() const override;
    RCP<const Basic> det() const override;
    void inv(MatrixBase &result) const override;

    // Matrix addition
    void add_matrix(const MatrixBase &other, MatrixBase &result) const override;

    // Matrix Multiplication
    void mul_matrix(const MatrixBase &other, MatrixBase &result) const override;

    // Matrix elementwise Multiplication
    void elementwise_mul_matrix(const MatrixBase &other,
                                MatrixBase &result) const override;

    // Add a scalar
    void add_scalar(const RCP<const Basic> &k,
                    MatrixBase &result) const override;

    // Multiply by a scalar
    void mul_scalar(const RCP<const Basic> &k,
                    MatrixBase &result) const override;

    // Matrix conjugate
    void conjugate(MatrixBase &result) const override;

    // Matrix transpose
    void transpose(MatrixBase &result) const override;
    CSRMatrix transpose(bool conjugate = false) const;

    // Matrix conjugate transpose
    void conjugate_transpose(MatrixBase &result) const override;

    // Extract out a submatrix
    void submatrix(MatrixBase &result, unsigned row_start, unsigned col_start,
                   unsigned row_end, unsigned col_end, unsigned row_step = 1,
                   unsigned col_step = 1) const override;

    // LU factorization
    void LU(MatrixBase &L, MatrixBase &U) const override;

    // LDL factorization
    void LDL(MatrixBase &L, MatrixBase &D) const override;

    // Solve Ax = b using LU factorization
    void LU_solve(const MatrixBase &b, MatrixBase &x) const override;

    // Fraction free LU factorization
    void FFLU(MatrixBase &LU) const override;

    // Fraction free LDU factorization
    void FFLDU(MatrixBase &L, MatrixBase &D, MatrixBase &U) const override;

    // QR factorization
    void QR(MatrixBase &Q, MatrixBase &R) const override;

    // Cholesky decomposition
    void cholesky(MatrixBase &L) const override;

    static void csr_sum_duplicates(std::vector<unsigned> &p_,
                                   std::vector<unsigned> &j_, vec_basic &x_,
                                   unsigned row_);

    static void csr_sort_indices(std::vector<unsigned> &p_,
                                 std::vector<unsigned> &j_, vec_basic &x_,
                                 unsigned row_);

    static bool csr_has_sorted_indices(const std::vector<unsigned> &p_,
                                       const std::vector<unsigned> &j_,
                                       unsigned row_);

    static bool csr_has_duplicates(const std::vector<unsigned> &p_,
                                   const std::vector<unsigned> &j_,
                                   unsigned row_);

    static bool csr_has_canonical_format(const std::vector<unsigned> &p_,
                                         const std::vector<unsigned> &j_,
                                         unsigned row_);

    static CSRMatrix from_coo(unsigned row, unsigned col,
                              const std::vector<unsigned> &i,
                              const std::vector<unsigned> &j,
                              const vec_basic &x);
    static CSRMatrix jacobian(const vec_basic &exprs, const vec_sym &x,
                              bool diff_cache = true);
    static CSRMatrix jacobian(const DenseMatrix &A, const DenseMatrix &x,
                              bool diff_cache = true);

    SYMENGINE_EXPORT friend void
    csr_matmat_pass1(const CSRMatrix &A, const CSRMatrix &B, CSRMatrix &C);
    SYMENGINE_EXPORT friend void
    csr_matmat_pass2(const CSRMatrix &A, const CSRMatrix &B, CSRMatrix &C);
    SYMENGINE_EXPORT friend void csr_diagonal(const CSRMatrix &A,
                                              DenseMatrix &D);
    SYMENGINE_EXPORT friend void csr_scale_rows(CSRMatrix &A,
                                                const DenseMatrix &X);
    SYMENGINE_EXPORT friend void csr_scale_columns(CSRMatrix &A,
                                                   const DenseMatrix &X);

    SYMENGINE_EXPORT friend void csr_binop_csr_canonical(
        const CSRMatrix &A, const CSRMatrix &B, CSRMatrix &C,
        RCP<const Basic> (&bin_op)(const RCP<const Basic> &,
                                   const RCP<const Basic> &));

private:
    std::vector<unsigned> p_;
    std::vector<unsigned> j_;
    vec_basic x_;
    // Stores the dimension of the Matrix
    unsigned row_;
    unsigned col_;
};

// Return the Jacobian of the matrix
SYMENGINE_EXPORT
void jacobian(const DenseMatrix &A, const DenseMatrix &x, DenseMatrix &result,
              bool diff_cache = true);
// Return the Jacobian of the matrix using sdiff
SYMENGINE_EXPORT
void sjacobian(const DenseMatrix &A, const DenseMatrix &x, DenseMatrix &result,
               bool diff_cache = true);

// Differentiate all the elements
SYMENGINE_EXPORT
void diff(const DenseMatrix &A, const RCP<const Symbol> &x, DenseMatrix &result,
          bool diff_cache = true);
// Differentiate all the elements using SymPy compatible diff
SYMENGINE_EXPORT
void sdiff(const DenseMatrix &A, const RCP<const Basic> &x, DenseMatrix &result,
           bool diff_cache = true);

// Get submatrix from a DenseMatrix
SYMENGINE_EXPORT
void submatrix_dense(const DenseMatrix &A, DenseMatrix &B, unsigned row_start,
                     unsigned col_start, unsigned row_end, unsigned col_end,
                     unsigned row_step = 1, unsigned col_step = 1);

// Row operations
SYMENGINE_EXPORT void row_exchange_dense(DenseMatrix &A, unsigned i,
                                         unsigned j);
SYMENGINE_EXPORT void row_mul_scalar_dense(DenseMatrix &A, unsigned i,
                                           RCP<const Basic> &c);

SYMENGINE_EXPORT
void row_add_row_dense(DenseMatrix &A, unsigned i, unsigned j,
                       RCP<const Basic> &c);

// Column operations
SYMENGINE_EXPORT void column_exchange_dense(DenseMatrix &A, unsigned i,
                                            unsigned j);

// Vector-specific methods
SYMENGINE_EXPORT void dot(const DenseMatrix &A, const DenseMatrix &B,
                          DenseMatrix &C);
SYMENGINE_EXPORT void cross(const DenseMatrix &A, const DenseMatrix &B,
                            DenseMatrix &C);

// Matrix Factorization
SYMENGINE_EXPORT void LU(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &U);
SYMENGINE_EXPORT void LDL(const DenseMatrix &A, DenseMatrix &L, DenseMatrix &D);
SYMENGINE_EXPORT void QR(const DenseMatrix &A, DenseMatrix &Q, DenseMatrix &R);
SYMENGINE_EXPORT void cholesky(const DenseMatrix &A, DenseMatrix &L);

// Inverse
SYMENGINE_EXPORT void inverse_fraction_free_LU(const DenseMatrix &A,
                                               DenseMatrix &B);

SYMENGINE_EXPORT void inverse_gauss_jordan(const DenseMatrix &A,
                                           DenseMatrix &B);

// Solving Ax = b
SYMENGINE_EXPORT
void fraction_free_LU_solve(const DenseMatrix &A, const DenseMatrix &b,
                            DenseMatrix &x);

SYMENGINE_EXPORT
void fraction_free_gauss_jordan_solve(const DenseMatrix &A,
                                      const DenseMatrix &b, DenseMatrix &x,
                                      bool pivot = true);

SYMENGINE_EXPORT void LU_solve(const DenseMatrix &A, const DenseMatrix &b,
                               DenseMatrix &x);

SYMENGINE_EXPORT
void pivoted_LU_solve(const DenseMatrix &A, const DenseMatrix &b,
                      DenseMatrix &x);

SYMENGINE_EXPORT void LDL_solve(const DenseMatrix &A, const DenseMatrix &b,
                                DenseMatrix &x);

// Determinant
SYMENGINE_EXPORT RCP<const Basic> det_berkowitz(const DenseMatrix &A);

// Characteristic polynomial: Only the coefficients of monomials in decreasing
// order of monomial powers is returned, i.e. if `B = transpose([1, -2, 3])`
// then the corresponding polynomial is `x**2 - 2x + 3`.
SYMENGINE_EXPORT void char_poly(const DenseMatrix &A, DenseMatrix &B);

// returns a finiteset of eigenvalues of a matrix
SYMENGINE_EXPORT RCP<const Set> eigen_values(const DenseMatrix &A);

// Mimic `eye` function in NumPy
SYMENGINE_EXPORT void eye(DenseMatrix &A, int k = 0);

// Create diagonal matrices directly
SYMENGINE_EXPORT void diag(DenseMatrix &A, vec_basic &v, int k = 0);

// Create a matrix filled with ones
SYMENGINE_EXPORT void ones(DenseMatrix &A);

// Create a matrix filled with zeros
SYMENGINE_EXPORT void zeros(DenseMatrix &A);

// Reduced row echelon form and returns the cols with pivots
SYMENGINE_EXPORT
void reduced_row_echelon_form(const DenseMatrix &A, DenseMatrix &B,
                              vec_uint &pivot_cols,
                              bool normalize_last = false);

// Returns true if `b` is exactly the type T.
// Here T can be a DenseMatrix, CSRMatrix, etc.
template <class T>
inline bool is_a(const MatrixBase &b)
{
    return T::type_code_id == b.get_type_code();
}

// Test two matrices for equality
inline bool operator==(const SymEngine::MatrixBase &lhs,
                       const SymEngine::MatrixBase &rhs)
{
    return lhs.eq(rhs);
}

// Test two matrices for equality
inline bool operator!=(const SymEngine::MatrixBase &lhs,
                       const SymEngine::MatrixBase &rhs)
{
    return not lhs.eq(rhs);
}

} // namespace SymEngine

// Print Matrix
inline std::ostream &operator<<(std::ostream &out,
                                const SymEngine::MatrixBase &A)
{
    return out << A.__str__();
}

#endif
