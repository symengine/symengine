#ifndef CSYMPY_RINGS_H
#define CSYMPY_RINGS_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

// Converts expression "p" into a polynomial P, with symbols 'sym'
void expr2poly(const RCP<const Basic> &p, umap_basic_int &syms,
        umap_vec_mpz &P);

// Multiply two polynomials: C = A*B
void poly_mul(const umap_vec_mpz &A, const umap_vec_mpz &B, umap_vec_mpz &C);

} // CSymPy

#endif
