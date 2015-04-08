#ifndef CWRAPPER_H
#define CWRAPPER_H

#include <gmp.h>

#ifdef __cplusplus
extern "C" {
#endif

// The size of 'basic_struct' must be the same as RCP<const Basic> *and* they
// must have the same alignment (because we allocate RCP<const Basic> into the
// memory occupied by this struct in cwrapper.cpp). We cannot use C++ in this
// file, so we need to use C tools to arrive at the correct size and alignment.
// The size of the RCP object on most platforms should be just the size of the
// 'T *ptr_' pointer that it contains (as there is no virtual function table)
// and the alignment should also be of a pointer. So we just put 'void *data'
// as the only member of the struct, that should have the correct size and
// alignment.  However, this is checked at compile time in cwrapper.cpp, so if
// the size or alignment is different on some platform, the compilation will
// fail --- in that case one needs to modify the contents of this struct to
// adjust its size and/or alignment.
typedef struct
{
    void *data;
} basic_struct;

//! 'basic' is internally implemented as a size 1 array of the type
//   basic_struct, which has the same size and alignment as RCP<const Basic>
//   (see the above comment for details). That is then used by the user to
//   allocate the memory needed for RCP<const Basic> on the stack. A 'basic'
//   type should be initialized using basic_init(), before any function is
//   called.  Assignment should be done only by using basic_assign(). Before
//   the variable goes out of scope, basic_free() must be called.
typedef basic_struct basic[1];


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
//! Assigns s = a ^ b.
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

#ifdef __cplusplus
}
#endif
#endif
