#ifndef CWRAPPER_H
#define CWRAPPER_H

#include <stdlib.h>
#include <gmp.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CRCPBasic_C
{
    void *data;
#if !defined(WITH_SYMENGINE_RCP)
    void *teuchos_handle;
    int teuchos_strength;
#endif
};

// For C, define a dummy struct with the right size, so that it can be
// allocated on the stack. For C++, the CRCPBasic is declared in cwrapper.cpp.
#ifdef __cplusplus
typedef struct CRCPBasic basic[1];
#else
typedef struct CRCPBasic_C basic[1];
#endif


//! Initialize a new basic instance.
void basic_init(basic s);
//! Assign value of b to a.
void basic_assign(basic a, const basic b);
//! Free the C++ class wrapped by s.
void basic_free(basic s);

//! Assign to s, a symbol with string representation c.
void symbol_set(basic s, char* c);

//! Assign to s, a long.
void integer_set_si(basic s, long i);
//! Assign to s, a ulong.
void integer_set_ui(basic s, unsigned long i);
//! Assign to s, a mpz_t.
void integer_set_mpz(basic s, const mpz_t i);
//! Assign to s, an integer that has base 10 representation c.
void integer_set_str(basic s, char* c);

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
//! Assign to s, derivative of expr with respect to sym. Returns 0 if sym is not a symbol.
int basic_diff(basic s, const basic expr, const basic sym);
//! Assigns s = -a.
void basic_neg(basic s, const basic a);
//! Assigns s = abs(a).
void basic_abs(basic s, const basic a);
//! Expands the expr a and assigns to s.
void basic_expand(basic s, const basic a);

//! Returns a new char pointer to the string representation of s.
char* basic_str(const basic s);
//! Frees the string s
void basic_str_free(char* s);

//! Return 1 if s is an Integer, 0 if not.
int is_a_Integer(const basic s);
//! Return 1 if s is an Rational, 0 if not.
int is_a_Rational(const basic s);
//! Return 1 if s is an Symbol, 0 if not.
int is_a_Symbol(const basic s);


//! Wrapper for std::vector<int>

typedef struct CVectorInt CVectorInt;

CVectorInt* vectorint_new();

// 'self' must point to allocated memory of size 'size'. The function returns 0
// if std::vector<int> can be initialized using placement new into 'self',
// otherwise 1 if 'size' is too small or 2 if 'self' is not properly aligned.
// No memory is leaked either way.
int vectorint_placement_new(CVectorInt *self, size_t size);

void vectorint_placement_free(CVectorInt *self);

void vectorint_free(CVectorInt *self);
void vectorint_push_back(CVectorInt *self, int value);
int vectorint_get(CVectorInt *self, int n);

//! Wrapper for vec_basic

typedef struct CVecBasic CVecBasic;

CVecBasic* vecbasic_new();
void vecbasic_free(CVecBasic *self);
void vecbasic_push_back(CVecBasic *self, const basic value);
void vecbasic_get(CVecBasic *self, int n, basic result);
size_t vecbasic_size(CVecBasic *self);

// -------------------------------------

void basic_get_args(const basic self, CVecBasic *args);

#ifdef __cplusplus
}
#endif
#endif
