#ifndef CSYMPY_RINGS_H
#define CSYMPY_RINGS_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

// Multiply two polynomials
void poly_mul(const map_vec_mpz &A, const map_vec_mpz &B, map_vec_mpz &C);

} // CSymPy

#endif
