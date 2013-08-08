#ifndef CSYMPY_RINGS_H
#define CSYMPY_RINGS_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

// Multiply two polynomials
void poly_mul(const map_vec_mpz &p1, const map_vec_mpz &p2, map_vec_mpz &r);

} // CSymPy

#endif
