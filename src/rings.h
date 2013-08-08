#ifndef CSYMPY_RINGS_H
#define CSYMPY_RINGS_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

// Converts expression "p" into a polynomial P, with symbols 'sym'
void expr2poly(const Teuchos::RCP<Basic> &p, Dict_int &syms,
        map_vec_mpz &P);

// Multiply two polynomials: C = A*B
void poly_mul(const map_vec_mpz &A, const map_vec_mpz &B, map_vec_mpz &C);

} // CSymPy

#endif
