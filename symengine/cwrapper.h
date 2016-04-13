#ifndef CWRAPPER_H
#define CWRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

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
void basic_assign(basic a, const basic b);

//! Returns the typeID of the basic struct
TypeID basic_get_type(const basic s);
//! Returns the typeID of the class with the name c
TypeID basic_get_class_id(const char *c);
//! Returns the class name of with the typeid `id`
char *basic_get_class_from_id(TypeID id);

//! Assign to s, a symbol with string representation c.
//! This function creates a new SymEngine::Symbol from a copy of
//! the string in c, thus the caller is free to use c afterwards.
void symbol_set(basic s, const char *c);

//! Assign to s, a long.
void integer_set_si(basic s, long i);
//! Assign to s, a ulong.
void integer_set_ui(basic s, unsigned long i);
//! Assign to s, a mpz_t.
void integer_set_mpz(basic s, const mpz_t i);
//! Assign to s, an integer that has base 10 representation c.
void integer_set_str(basic s, char *c);

//! Returns signed long value of s.
signed long integer_get_si(const basic s);
//! Returns unsigned long value of s.
unsigned long integer_get_ui(const basic s);
//! Returns s as a mpz_t.
void integer_get_mpz(mpz_t a, const basic s);

//! Assign to s, a rational i/j. Returns 0 if either i or j is not an integer.
int rational_set(basic s, const basic i, const basic j);
//! Assign to s, a rational i/j, where i and j are signed longs.
void rational_set_si(basic s, long i, long j);
//! Assign to s, a rational i/j, where i and j are unsigned longs.
void rational_set_ui(basic s, unsigned long i, unsigned long j);
//! Assign to s, a rational i, where is of type mpq_t.
void rational_set_mpq(basic s, const mpq_t i);

//! Assign to s, a complex re + i*im.
void complex_set(basic s, const basic re, const basic im);
//! Assign to s, a complex re + i*im, where re and im are rationals.
void complex_set_rat(basic s, const basic re, const basic im);
//! Assign to s, a complex re + i*im, where re and im are of type mpq.
void complex_set_mpq(basic s, const mpq_t re, const mpq_t im);

//! Assigns s = a + b.
void basic_add(basic s, const basic a, const basic b);
//! Assigns s = a - b.
void basic_sub(basic s, const basic a, const basic b);
//! Assigns s = a * b.
void basic_mul(basic s, const basic a, const basic b);
//! Assigns s = a / b.
void basic_div(basic s, const basic a, const basic b);
//! Assigns s = a ** b.
void basic_pow(basic s, const basic a, const basic b);
//! Assign to s, derivative of expr with respect to sym. Returns 0 if sym is not
//! a symbol.
int basic_diff(basic s, const basic expr, const basic sym);
//! Returns 1 if both basic are equal, 0 if not
int basic_eq(const basic a, const basic b);
//! Returns 1 if both basic are not equal, 0 if they are
int basic_neq(const basic a, const basic b);

//! Expands the expr a and assigns to s.
void basic_expand(basic s, const basic a);
//! Assigns s = -a.
void basic_neg(basic s, const basic a);

//! Assigns s = abs(a).
void basic_abs(basic s, const basic a);

//! Assigns s = sin(a).
void basic_sin(basic s, const basic a);
//! Assigns s = cos(a).
void basic_cos(basic s, const basic a);
//! Assigns s = tan(a).
void basic_tan(basic s, const basic a);

//! Assigns s = asin(a).
void basic_asin(basic s, const basic a);
//! Assigns s = acos(a).
void basic_acos(basic s, const basic a);
//! Assigns s = atan(a).
void basic_atan(basic s, const basic a);

//! Assigns s = csc(a).
void basic_csc(basic s, const basic a);
//! Assigns s = sec(a).
void basic_sec(basic s, const basic a);
//! Assigns s = cot(a).
void basic_cot(basic s, const basic a);

//! Assigns s = acsc(a).
void basic_acsc(basic s, const basic a);
//! Assigns s = asec(a).
void basic_asec(basic s, const basic a);
//! Assigns s = acot(a).
void basic_acot(basic s, const basic a);

//! Assigns s = sinh(a).
void basic_sinh(basic s, const basic a);
//! Assigns s = cosh(a).
void basic_cosh(basic s, const basic a);
//! Assigns s = tanh(a).
void basic_tanh(basic s, const basic a);

//! Assigns s = asinh(a).
void basic_asinh(basic s, const basic a);
//! Assigns s = acosh(a).
void basic_acosh(basic s, const basic a);
//! Assigns s = atanh(a).
void basic_atanh(basic s, const basic a);

//! Assigns s = csch(a).
void basic_csch(basic s, const basic a);
//! Assigns s = sech(a).
void basic_sech(basic s, const basic a);
//! Assigns s = coth(a).
void basic_coth(basic s, const basic a);

//! Assigns s = acsch(a).
void basic_acsch(basic s, const basic a);
//! Assigns s = asech(a).
void basic_asech(basic s, const basic a);
//! Assigns s = acoth(a).
void basic_acoth(basic s, const basic a);

//! Assigns s = lambertw(a).
void basic_lambertw(basic s, const basic a);
//! Assigns s = zeta(a).
void basic_zeta(basic s, const basic a);
//! Assigns s = dirichlet_eta(a).
void basic_dirichlet_eta(basic s, const basic a);
//! Assigns s = gamma(a).
void basic_gamma(basic s, const basic a);

//! Returns a new char pointer to the string representation of s.
char *basic_str(const basic s);
//! Frees the string s
void basic_str_free(char *s);

//! Return 1 if s is an Integer, 0 if not.
int is_a_Integer(const basic s);
//! Return 1 if s is a Rational, 0 if not.
int is_a_Rational(const basic s);
//! Return 1 if s is a Symbol, 0 if not.
int is_a_Symbol(const basic s);
//! Return 1 if s is a Complex, 0 if not.
int is_a_Complex(const basic s);

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
void basic_get_args(const basic self, CVecBasic *args);
//! Returns a CSetBasic of set_basic given by free_symbols
void basic_free_symbols(const basic self, CSetBasic *symbols);
//! returns the hash of the Basic object
size_t basic_hash(const basic self);
//! substitutes all the keys with their mapped values
//! in the given basic `e` and returns it through basic 's'
void basic_subs(basic s, const basic e, const CMapBasicBasic *mapbb);
//! substitutes a basic 'a' with another basic 'b',
//! in the given basic 'e' and returns it through basic 's'
void basic_subs2(basic s, const basic e, const basic a, const basic b);

//! Wrapper for ascii_art()

//! Returns a new char pointer to the ascii_art string
//! The caller is responsible to free the pointer using 'basic_str_free'.
char *ascii_art_str();

//! Wrapper for ntheory
//! Greatest Common Divisor
void ntheory_gcd(basic s, const basic a, const basic b);
//! Least Common Multiple
void ntheory_lcm(basic s, const basic a, const basic b);
//! \return next prime after `a`
void ntheory_nextprime(basic s, const basic a);
//! modulo round toward zero
void ntheory_mod(basic s, const basic n, const basic d);
//! \return quotient round toward zero when `n` is divided by `d`
void ntheory_quotient(basic s, const basic n, const basic d);
//! nth Fibonacci number //  fibonacci(0) = 0 and fibonacci(1) = 1
void ntheory_fibonacci(basic s, unsigned long a);
//! Lucas number
void ntheory_lucas(basic s, unsigned long a);
//! Binomial Coefficient
void ntheory_binomial(basic s, const basic a, unsigned long b);

//! Print stacktrace on segfault
void symengine_print_stack_on_segfault();

#ifdef __cplusplus
}
#endif
#endif
