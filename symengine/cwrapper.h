#ifndef CWRAPPER_H
#define CWRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

#include "symengine/symengine_config.h"

#ifdef HAVE_SYMENGINE_MPFR
#include <mpfr.h>
#endif // HAVE_SYMENGINE_MPFR

#ifdef __cplusplus
extern "C" {
#endif

// Use SYMENGINE_C_ASSERT in C tests
#define SYMENGINE_C_ASSERT(cond)                                               \
    {                                                                          \
        if (0 == (cond)) {                                                     \
            printf("SYMENGINE_C_ASSERT failed: %s \nfunction %s (), line "     \
                   "number %d at\n%s\n",                                       \
                   __FILE__, __func__, __LINE__, #cond);                       \
            abort();                                                           \
        }                                                                      \
    }

#include "symengine/symengine_exception.h"

typedef symengine_exceptions_t CWRAPPER_OUTPUT_TYPE;

typedef enum {
#define SYMENGINE_INCLUDE_ALL
#define SYMENGINE_ENUM(type, Class) SYMENGINE_##type,
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
#undef SYMENGINE_INCLUDE_ALL
    SYMENGINE_TypeID_Count
} TypeID;

// The size of 'CRCPBasic_C' must be the same as CRCPBasic (which contains a
// single RCP<const Basic> member) *and* they must have the same alignment
// (because we allocate CRCPBasic into the memory occupied by this struct in
// cwrapper.cpp). We cannot use C++ in this file, so we need to use C tools to
// arrive at the correct size and alignment.  The size of the RCP object on
// most platforms (with WITH_SYMENGINE_RCP on) should be just the size of the
// 'T *ptr_' pointer that it contains (as there is no virtual function table)
// and the alignment should also be of a pointer.  So we just put 'void *data'
// as the only member of the struct, that should have the correct size and
// alignment. With WITH_SYMENGINE_RCP off (i.e. using Teuchos::RCP), we have to
// add additional members into the structure.
//
// However, this is checked at compile time in cwrapper.cpp, so if the size or
// alignment is different on some platform, the compilation will fail --- in
// that case one needs to modify the contents of this struct to adjust its size
// and/or alignment.
struct CRCPBasic_C {
    void *data;
#if !defined(WITH_SYMENGINE_RCP)
    void *teuchos_handle;
    int teuchos_strength;
#endif
};

//! 'basic' is internally implemented as a size 1 array of the type
//  CRCPBasic, which has the same size and alignment as RCP<const Basic> (see
//  the above comment for details). That is then used by the user to allocate
//  the memory needed for RCP<const Basic> on the stack. A 'basic' type should
//  be initialized using basic_new_stack(), before any function is called.
//  Assignment should be done only by using basic_assign(). Before the variable
//  goes out of scope, basic_free_stack() must be called.
//
//  For C, define a dummy struct with the right size, so that it can be
//  allocated on the stack. For C++, the CRCPBasic is declared in cwrapper.cpp.
#ifdef __cplusplus
typedef struct CRCPBasic basic_struct;
#else
typedef struct CRCPBasic_C basic_struct;
#endif

typedef basic_struct basic[1];

//! Initialize a new basic instance. 's' is allocated on stack using the
// 'basic' type, this function initializes an RCP<const Basic> on the stack
// allocated variable. The 's' variable must be freed using basic_free_stack()
void basic_new_stack(basic s);
//! Free the C++ class wrapped by s.
void basic_free_stack(basic s);

// Use these two functions to allocate and free 'basic' on a heap. The pointer
// can then be used in all the other methods below (i.e. the methods that
// accept 'basic s' work no matter if 's' is stack or heap allocated).
basic_struct *basic_new_heap();
void basic_free_heap(basic_struct *s);

//! Use these functions to get the commonly used constants as basic.

//! Assigns to s a SymEngine constant with name c
//! This function creates a new SymEngine::Constant from a copy of
//! the string in c, thus the caller is free to use c afterwards,
//! and also the caller must free c.
void basic_const_set(basic s, const char *c);

void basic_const_zero(basic s);
void basic_const_one(basic s);
void basic_const_minus_one(basic s);
void basic_const_I(basic s);

void basic_const_pi(basic s);
void basic_const_E(basic s);
void basic_const_EulerGamma(basic s);

//! Assign value of b to a.
CWRAPPER_OUTPUT_TYPE basic_assign(basic a, const basic b);

//! Parse str and assign value to b
CWRAPPER_OUTPUT_TYPE basic_parse(basic b, const char *str);

//! Returns the typeID of the basic struct
TypeID basic_get_type(const basic s);
//! Returns the typeID of the class with the name c
TypeID basic_get_class_id(const char *c);
//! Returns the class name of with the typeid `id`
char *basic_get_class_from_id(TypeID id);

//! Assign to s, a symbol with string representation c.
//! This function creates a new SymEngine::Symbol from a copy of
//! the string in c, thus the caller is free to use c afterwards.
CWRAPPER_OUTPUT_TYPE symbol_set(basic s, const char *c);

//! Assign to s, a long.
CWRAPPER_OUTPUT_TYPE integer_set_si(basic s, long i);
//! Assign to s, a ulong.
CWRAPPER_OUTPUT_TYPE integer_set_ui(basic s, unsigned long i);
//! Assign to s, a mpz_t.
CWRAPPER_OUTPUT_TYPE integer_set_mpz(basic s, const mpz_t i);
//! Assign to s, an integer that has base 10 representation c.
CWRAPPER_OUTPUT_TYPE integer_set_str(basic s, const char *c);
//! Assign to s, a real_double that has value of d.
CWRAPPER_OUTPUT_TYPE real_double_set_d(basic s, double d);
//! Returns double value of s.
double real_double_get_d(const basic s);

#ifdef HAVE_SYMENGINE_MPFR
//! Assign to s, a real mpfr that has value d with precision prec.
CWRAPPER_OUTPUT_TYPE real_mpfr_set_d(basic s, double d, int prec);
//! Assign to s, a real mpfr that has base 10 representation c with precision
//! prec.
CWRAPPER_OUTPUT_TYPE real_mpfr_set_str(basic s, const char *c, int prec);
//! Returns double value of s.
double real_mpfr_get_d(const basic s);
//! Assign to s, a real mpfr that has value pointed by m.
CWRAPPER_OUTPUT_TYPE real_mpfr_set(basic s, mpfr_srcptr m);
//! Assign to m, the mpfr_t given in s.
CWRAPPER_OUTPUT_TYPE real_mpfr_get(mpfr_ptr m, const basic s);
//! Returns the precision of the mpfr_t given by s.
mpfr_prec_t real_mpfr_get_prec(const basic s);
//! Returns 1 if s has value zero; 0 otherwise
int real_mpfr_is_zero(const basic s);
#endif // HAVE_SYMENGINE_MPFR

#ifdef HAVE_SYMENGINE_MPC
//! Returns 1 if s has value zero; 0 otherwise
int complex_mpc_is_zero(const basic s);
//! Assign to s, the real part of com
CWRAPPER_OUTPUT_TYPE complex_mpc_real_part(basic s, const basic com);
//! Assign to s, the imaginary part of com
CWRAPPER_OUTPUT_TYPE complex_mpc_imaginary_part(basic s, const basic com);
#endif // HAVE_SYMENGINE_MPC

//! Returns signed long value of s.
signed long integer_get_si(const basic s);
//! Returns unsigned long value of s.
unsigned long integer_get_ui(const basic s);
//! Returns s as a mpz_t.
CWRAPPER_OUTPUT_TYPE integer_get_mpz(mpz_t a, const basic s);

//! Assign to s, a rational i/j. Returns 0 if either i or j is not an integer.
CWRAPPER_OUTPUT_TYPE rational_set(basic s, const basic i, const basic j);
//! Assign to s, a rational i/j, where i and j are signed longs.
CWRAPPER_OUTPUT_TYPE rational_set_si(basic s, long i, long j);
//! Assign to s, a rational i/j, where i and j are unsigned longs.
CWRAPPER_OUTPUT_TYPE rational_set_ui(basic s, unsigned long i, unsigned long j);
//! Returns s as a mpq_t.
CWRAPPER_OUTPUT_TYPE rational_get_mpq(mpq_t a, const basic s);
//! Assign to s, a rational i, where is of type mpq_t.
CWRAPPER_OUTPUT_TYPE rational_set_mpq(basic s, const mpq_t i);

//! Assign to s, a complex re + i*im.
CWRAPPER_OUTPUT_TYPE complex_set(basic s, const basic re, const basic im);
//! Assign to s, a complex re + i*im, where re and im are rationals.
CWRAPPER_OUTPUT_TYPE complex_set_rat(basic s, const basic re, const basic im);
//! Assign to s, a complex re + i*im, where re and im are of type mpq.
CWRAPPER_OUTPUT_TYPE complex_set_mpq(basic s, const mpq_t re, const mpq_t im);
//! Assign to s, a real where com is a complex
CWRAPPER_OUTPUT_TYPE complex_real_part(basic s, const basic com);
//! Assign to s, an imaginary where com is a complex
CWRAPPER_OUTPUT_TYPE complex_imaginary_part(basic s, const basic com);
//! Assign to s, a real double where com is a complex double
CWRAPPER_OUTPUT_TYPE complex_double_real_part(basic s, const basic com);
//! Assign to s, an imaginary double where com is a complex double
CWRAPPER_OUTPUT_TYPE complex_double_imaginary_part(basic s, const basic com);

//! Assigns s = a + b.
CWRAPPER_OUTPUT_TYPE basic_add(basic s, const basic a, const basic b);
//! Assigns s = a - b.
CWRAPPER_OUTPUT_TYPE basic_sub(basic s, const basic a, const basic b);
//! Assigns s = a * b.
CWRAPPER_OUTPUT_TYPE basic_mul(basic s, const basic a, const basic b);
//! Assigns s = a / b.
CWRAPPER_OUTPUT_TYPE basic_div(basic s, const basic a, const basic b);
//! Assigns s = a ** b.
CWRAPPER_OUTPUT_TYPE basic_pow(basic s, const basic a, const basic b);
//! Assign to s, derivative of expr with respect to sym. Returns 0 if sym is not
//! a symbol.
CWRAPPER_OUTPUT_TYPE basic_diff(basic s, const basic expr, const basic sym);
//! Returns 1 if both basic are equal, 0 if not
int basic_eq(const basic a, const basic b);
//! Returns 1 if both basic are not equal, 0 if they are
int basic_neq(const basic a, const basic b);
//! Returns +1 if s (Number) is positive, 0 if 0, -1 if negative
int basic_number_sign(const basic s);

//! Expands the expr a and assigns to s.
CWRAPPER_OUTPUT_TYPE basic_expand(basic s, const basic a);
//! Assigns s = -a.
CWRAPPER_OUTPUT_TYPE basic_neg(basic s, const basic a);

//! Assigns s = abs(a).
CWRAPPER_OUTPUT_TYPE basic_abs(basic s, const basic a);

//! Assigns s = sin(a).
CWRAPPER_OUTPUT_TYPE basic_sin(basic s, const basic a);
//! Assigns s = cos(a).
CWRAPPER_OUTPUT_TYPE basic_cos(basic s, const basic a);
//! Assigns s = tan(a).
CWRAPPER_OUTPUT_TYPE basic_tan(basic s, const basic a);

//! Assigns s = asin(a).
CWRAPPER_OUTPUT_TYPE basic_asin(basic s, const basic a);
//! Assigns s = acos(a).
CWRAPPER_OUTPUT_TYPE basic_acos(basic s, const basic a);
//! Assigns s = atan(a).
CWRAPPER_OUTPUT_TYPE basic_atan(basic s, const basic a);

//! Assigns s = csc(a).
CWRAPPER_OUTPUT_TYPE basic_csc(basic s, const basic a);
//! Assigns s = sec(a).
CWRAPPER_OUTPUT_TYPE basic_sec(basic s, const basic a);
//! Assigns s = cot(a).
CWRAPPER_OUTPUT_TYPE basic_cot(basic s, const basic a);

//! Assigns s = acsc(a).
CWRAPPER_OUTPUT_TYPE basic_acsc(basic s, const basic a);
//! Assigns s = asec(a).
CWRAPPER_OUTPUT_TYPE basic_asec(basic s, const basic a);
//! Assigns s = acot(a).
CWRAPPER_OUTPUT_TYPE basic_acot(basic s, const basic a);

//! Assigns s = sinh(a).
CWRAPPER_OUTPUT_TYPE basic_sinh(basic s, const basic a);
//! Assigns s = cosh(a).
CWRAPPER_OUTPUT_TYPE basic_cosh(basic s, const basic a);
//! Assigns s = tanh(a).
CWRAPPER_OUTPUT_TYPE basic_tanh(basic s, const basic a);

//! Assigns s = asinh(a).
CWRAPPER_OUTPUT_TYPE basic_asinh(basic s, const basic a);
//! Assigns s = acosh(a).
CWRAPPER_OUTPUT_TYPE basic_acosh(basic s, const basic a);
//! Assigns s = atanh(a).
CWRAPPER_OUTPUT_TYPE basic_atanh(basic s, const basic a);

//! Assigns s = csch(a).
CWRAPPER_OUTPUT_TYPE basic_csch(basic s, const basic a);
//! Assigns s = sech(a).
CWRAPPER_OUTPUT_TYPE basic_sech(basic s, const basic a);
//! Assigns s = coth(a).
CWRAPPER_OUTPUT_TYPE basic_coth(basic s, const basic a);

//! Assigns s = acsch(a).
CWRAPPER_OUTPUT_TYPE basic_acsch(basic s, const basic a);
//! Assigns s = asech(a).
CWRAPPER_OUTPUT_TYPE basic_asech(basic s, const basic a);
//! Assigns s = acoth(a).
CWRAPPER_OUTPUT_TYPE basic_acoth(basic s, const basic a);

//! Assigns s = lambertw(a).
CWRAPPER_OUTPUT_TYPE basic_lambertw(basic s, const basic a);
//! Assigns s = zeta(a).
CWRAPPER_OUTPUT_TYPE basic_zeta(basic s, const basic a);
//! Assigns s = dirichlet_eta(a).
CWRAPPER_OUTPUT_TYPE basic_dirichlet_eta(basic s, const basic a);
//! Assigns s = gamma(a).
CWRAPPER_OUTPUT_TYPE basic_gamma(basic s, const basic a);

//! Returns a new char pointer to the string representation of s.
char *basic_str(const basic s);
//! Frees the string s
void basic_str_free(char *s);

//! Return 1 if s is a Number, 0 if not.
int is_a_Number(const basic s);
//! Return 1 if s is an Integer, 0 if not.
int is_a_Integer(const basic s);
//! Return 1 if s is a Rational, 0 if not.
int is_a_Rational(const basic s);
//! Return 1 if s is a Symbol, 0 if not.
int is_a_Symbol(const basic s);
//! Return 1 if s is a Complex, 0 if not.
int is_a_Complex(const basic s);
//! Return 1 if c is a RealDouble, 0 if not.
int is_a_RealDouble(const basic c);
//! Return 1 if c is a ComplexDouble, 0 if not.
int is_a_ComplexDouble(const basic c);
//! Return 1 if c is a RealMPFR, 0 if not.
int is_a_RealMPFR(const basic c);
//! Return 1 if c is a ComplexMPC, 0 if not.
int is_a_ComplexMPC(const basic c);

//! Wrapper for std::vector<int>

typedef struct CVectorInt CVectorInt;

CVectorInt *vectorint_new();

// 'data' must point to allocated memory of size 'size'. The function returns 0
// if std::vector<int> can be initialized using placement new into 'data',
// otherwise 1 if 'size' is too small or 2 if 'data' is not properly aligned.
// No memory is leaked either way. Use vectorint_placement_new_check() to check
// that the 'data' and 'size' is properly allocated and aligned. Use
// vectorint_placement_new() to do the actual allocation.
int vectorint_placement_new_check(void *data, size_t size);
CVectorInt *vectorint_placement_new(void *data);

void vectorint_placement_free(CVectorInt *self);

void vectorint_free(CVectorInt *self);
void vectorint_push_back(CVectorInt *self, int value);
int vectorint_get(CVectorInt *self, int n);

//! Wrapper for vec_basic

typedef struct CVecBasic CVecBasic;

CVecBasic *vecbasic_new();
void vecbasic_free(CVecBasic *self);
void vecbasic_push_back(CVecBasic *self, const basic value);
void vecbasic_get(CVecBasic *self, int n, basic result);
size_t vecbasic_size(CVecBasic *self);

//! Wrappers for Matrices

typedef struct CDenseMatrix CDenseMatrix;
typedef struct CSparseMatrix CSparseMatrix;

CDenseMatrix *dense_matrix_new();
CSparseMatrix *sparse_matrix_new();

void dense_matrix_free(CDenseMatrix *self);
//! Return a DenseMatrix with l's elements
CDenseMatrix *dense_matrix_new_vec(unsigned rows, unsigned cols, CVecBasic *l);
//! Return a DenseMatrix with r rows and c columns
CDenseMatrix *dense_matrix_new_rows_cols(unsigned r, unsigned c);

void sparse_matrix_free(CSparseMatrix *self);

//! Assign to s, a DenseMatrix with value d
CWRAPPER_OUTPUT_TYPE dense_matrix_set(CDenseMatrix *s, const CDenseMatrix *d);

//! Return a string representation of s
char *dense_matrix_str(const CDenseMatrix *s);
//! Resize mat to rxc
CWRAPPER_OUTPUT_TYPE dense_matrix_rows_cols(CDenseMatrix *mat, unsigned r,
                                            unsigned c);
//! Assign to s, mat[r][c]
CWRAPPER_OUTPUT_TYPE dense_matrix_get_basic(basic s, const CDenseMatrix *mat,
                                            unsigned long int r,
                                            unsigned long int c);
//! Assign s to mat[r][c]
CWRAPPER_OUTPUT_TYPE dense_matrix_set_basic(CDenseMatrix *mat,
                                            unsigned long int r,
                                            unsigned long int c, basic s);
//! Assign to s, mat[r][c]
CWRAPPER_OUTPUT_TYPE sparse_matrix_get_basic(basic s, const CSparseMatrix *mat,
                                             unsigned long int r,
                                             unsigned long int c);
//! Assign s to mat[r][c]
CWRAPPER_OUTPUT_TYPE sparse_matrix_set_basic(CSparseMatrix *mat,
                                             unsigned long int r,
                                             unsigned long int c, basic s);
//! Assign to s, determinent of mat
CWRAPPER_OUTPUT_TYPE dense_matrix_det(basic s, const CDenseMatrix *mat);
//! Assign to s, a DenseMatrix which is the inverse of mat
CWRAPPER_OUTPUT_TYPE dense_matrix_inv(CDenseMatrix *s, const CDenseMatrix *mat);
//! Assign to s, a DenseMatrix which is the transpose of mat
CWRAPPER_OUTPUT_TYPE dense_matrix_transpose(CDenseMatrix *s,
                                            const CDenseMatrix *mat);
//! Assign to s, a SubMatrix of mat, starting with [r1, r2] until [r2, c2], with
//! step sizes [r, c]
CWRAPPER_OUTPUT_TYPE
dense_matrix_submatrix(CDenseMatrix *s, const CDenseMatrix *mat,
                       unsigned long int r1, unsigned long int c1,
                       unsigned long int r2, unsigned long int c2,
                       unsigned long int r, unsigned long int c);
//! Return the number of columns of s
unsigned long int dense_matrix_cols(const CDenseMatrix *s);
//! Return the number of rows of s
unsigned long int dense_matrix_rows(const CDenseMatrix *s);
//! Assign to s, the addition of matA and matB
CWRAPPER_OUTPUT_TYPE dense_matrix_add_matrix(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const CDenseMatrix *matB);
//! Assign to s, the matrix multiplication of matA and matB
CWRAPPER_OUTPUT_TYPE dense_matrix_mul_matrix(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const CDenseMatrix *matB);
//! Assign to s, the addition of scalar b to matrix matA
CWRAPPER_OUTPUT_TYPE dense_matrix_add_scalar(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const basic b);
//! Assign to s, the multiplication of scalar b to matrix matA
CWRAPPER_OUTPUT_TYPE dense_matrix_mul_scalar(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const basic b);
//! Assign to l and u, LU factorization of mat
CWRAPPER_OUTPUT_TYPE dense_matrix_LU(CDenseMatrix *l, CDenseMatrix *u,
                                     const CDenseMatrix *mat);
//! Assign to l and d, LDL factorization of mat
CWRAPPER_OUTPUT_TYPE dense_matrix_LDL(CDenseMatrix *l, CDenseMatrix *d,
                                      const CDenseMatrix *mat);
//! Assign to lu, fraction free LU factorization of mat
CWRAPPER_OUTPUT_TYPE dense_matrix_FFLU(CDenseMatrix *lu,
                                       const CDenseMatrix *mat);
//! Assign to l, d and u, FFLDU factorization of mat
CWRAPPER_OUTPUT_TYPE dense_matrix_FFLDU(CDenseMatrix *l, CDenseMatrix *d,
                                        CDenseMatrix *u,
                                        const CDenseMatrix *mat);
//! Assign to x, solution to A x = b
CWRAPPER_OUTPUT_TYPE dense_matrix_LU_solve(CDenseMatrix *x,
                                           const CDenseMatrix *A,
                                           const CDenseMatrix *b);
//! Assign to s, a matrix of ones of size rxc
CWRAPPER_OUTPUT_TYPE dense_matrix_ones(CDenseMatrix *s, unsigned long int r,
                                       unsigned long int c);
//! Assign to s, a matrix of zeros of size rxc
CWRAPPER_OUTPUT_TYPE dense_matrix_zeros(CDenseMatrix *s, unsigned long int r,
                                        unsigned long int c);
//! Assign to s, a diagonal matrix with a diagonal at offset k, with elements in
//! d
CWRAPPER_OUTPUT_TYPE dense_matrix_diag(CDenseMatrix *s, CVecBasic *d,
                                       long int k);
//! Assign to s, a matrix of size NxM, with diagonal of 1s at offset k
CWRAPPER_OUTPUT_TYPE dense_matrix_eye(CDenseMatrix *s, unsigned long int N,
                                      unsigned long int M, int k);

//! Assign to s, a CSRMatrix
void sparse_matrix_init(CSparseMatrix *s);
//! Assign to s, a CSRMatrix with r rows and c columns
void sparse_matrix_rows_cols(CSparseMatrix *s, unsigned long int r,
                             unsigned long int c);
//! Return a string representation of s
char *sparse_matrix_str(const CSparseMatrix *s);

//! Return 1 if c is a DenseMatrix, 0 if not.
int is_a_DenseMatrix(const CDenseMatrix *c);
//! Return 1 if c is a SparseMatrix, 0 if not.
int is_a_SparseMatrix(const CSparseMatrix *c);

//! Return 1 if lhs == rhs, 0 if not
int dense_matrix_eq(CDenseMatrix *lhs, CDenseMatrix *rhs);
//! Return 1 if lhs == rhs, 0 if not
int sparse_matrix_eq(CSparseMatrix *lhs, CSparseMatrix *rhs);

//! Wrapper for set_basic

typedef struct CSetBasic CSetBasic;

CSetBasic *setbasic_new();
void setbasic_free(CSetBasic *self);
//! Returns 1 if insert is successful and 0 if set already contains the value
//! and insertion is unsuccessful
int setbasic_insert(CSetBasic *self, const basic value);
void setbasic_get(CSetBasic *self, int n, basic result);
//! Returns 1 if value is found in the set and 0 if not
int setbasic_find(CSetBasic *self, basic value);
size_t setbasic_size(CSetBasic *self);

//! Wrapper for map_basic_basic

typedef struct CMapBasicBasic CMapBasicBasic;

CMapBasicBasic *mapbasicbasic_new();
void mapbasicbasic_free(CMapBasicBasic *self);
void mapbasicbasic_insert(CMapBasicBasic *self, const basic key,
                          const basic mapped);
//! Returns 1 if such a key exists in the map and get is successful, 0 if not
int mapbasicbasic_get(CMapBasicBasic *self, const basic key, basic mapped);
size_t mapbasicbasic_size(CMapBasicBasic *self);

// -------------------------------------

//! Returns a CVecBasic of vec_basic given by get_args
CWRAPPER_OUTPUT_TYPE basic_get_args(const basic self, CVecBasic *args);
//! Returns a CSetBasic of set_basic given by free_symbols
CWRAPPER_OUTPUT_TYPE basic_free_symbols(const basic self, CSetBasic *symbols);
//! returns the hash of the Basic object
size_t basic_hash(const basic self);
//! substitutes all the keys with their mapped values
//! in the given basic `e` and returns it through basic 's'
CWRAPPER_OUTPUT_TYPE basic_subs(basic s, const basic e,
                                const CMapBasicBasic *mapbb);
//! substitutes a basic 'a' with another basic 'b',
//! in the given basic 'e' and returns it through basic 's'
CWRAPPER_OUTPUT_TYPE basic_subs2(basic s, const basic e, const basic a,
                                 const basic b);

//! Assigns to s a FunctionSymbol with name described by c, with dependent
//! symbols arg
CWRAPPER_OUTPUT_TYPE function_symbol_set(basic s, const char *c,
                                         const CVecBasic *arg);

//! Wrapper for ascii_art()

//! Returns a new char pointer to the ascii_art string
//! The caller is responsible to free the pointer using 'basic_str_free'.
char *ascii_art_str();

//! Wrapper for ntheory
//! Greatest Common Divisor
CWRAPPER_OUTPUT_TYPE ntheory_gcd(basic s, const basic a, const basic b);
//! Least Common Multiple
CWRAPPER_OUTPUT_TYPE ntheory_lcm(basic s, const basic a, const basic b);
//! \return next prime after `a`
CWRAPPER_OUTPUT_TYPE ntheory_nextprime(basic s, const basic a);
//! modulo round toward zero
CWRAPPER_OUTPUT_TYPE ntheory_mod(basic s, const basic n, const basic d);
//! \return quotient round toward zero when `n` is divided by `d`
CWRAPPER_OUTPUT_TYPE ntheory_quotient(basic s, const basic n, const basic d);
//! nth Fibonacci number //  fibonacci(0) = 0 and fibonacci(1) = 1
CWRAPPER_OUTPUT_TYPE ntheory_fibonacci(basic s, unsigned long a);
//! Lucas number
CWRAPPER_OUTPUT_TYPE ntheory_lucas(basic s, unsigned long a);
//! Binomial Coefficient
CWRAPPER_OUTPUT_TYPE ntheory_binomial(basic s, const basic a, unsigned long b);
//! Evaluate b and assign the value to s
CWRAPPER_OUTPUT_TYPE basic_evalf(basic s, const basic b, unsigned long bits,
                                 int real);

//! Print stacktrace on segfault
void symengine_print_stack_on_segfault();

#ifdef __cplusplus
}
#endif
#endif
