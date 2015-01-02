#ifndef __CWRAPPER_H
#define __CWRAPPER_H

#include <gmp.h>

#ifdef __cplusplus
#include "basic.h"
namespace CSymPy {
    class CWrapper {
        public:
            RCP<const Basic> ptr;
            CWrapper() {
                ptr = RCP<const Basic>();
            }
    };
}
using CSymPy::CWrapper;

extern "C" {
#endif

typedef struct CWrapper CWrapper;

typedef CWrapper* basic;

basic basic_new();
void basic_assign(basic a, basic b);
void basic_free(basic s);
int basic_is_null(basic s);

void basic_symbol(basic s, char* c);

void basic_integer_si(basic s, long i);
void basic_integer_ui(basic s, unsigned long i);
void basic_integer_mpz(basic s, mpz_t i);
void basic_integer_str(basic s, char* c);

int basic_rational(basic s, basic i, basic j);
void basic_rational_si(basic s, long i, long j);
void basic_rational_ui(basic s, unsigned long i, unsigned long j);
void basic_rational_mpq(basic s, mpq_t i);

void basic_mul(basic s, basic a, basic b);
void basic_pow(basic s, basic a, basic b);
void basic_add(basic s, basic a, basic b);
void basic_div(basic s, basic a, basic b);
int basic_diff(basic s, basic expr, basic symbol);

char* basic_str(basic c);

int is_a_Integer(basic c);
int is_a_Rational(basic c);
int is_a_Symbol(basic c);

#ifdef __cplusplus
}
#endif
#endif
