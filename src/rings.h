/**
 *  \file rings.h
 *  Polynomial Manipulation
 *
 **/
#ifndef SYMENGINE_RINGS_H
#define SYMENGINE_RINGS_H

#include "basic.h"
#include "dict.h"

#include <piranha/piranha.hpp>

namespace SymEngine {

//! Converts expression `p` into a polynomial `P`, with symbols `sym`
void expr2poly(const RCP<const Basic> &p, umap_basic_num &syms,
        umap_vec_mpz &P);

//! Multiply two polynomials: `C = A*B`
void poly_mul(const umap_vec_mpz &A, const umap_vec_mpz &B, umap_vec_mpz &C);

//! Convert the tuple representation to packed
void poly2packed(const umap_vec_mpz &A, umap_ull_mpz &B);

//! Convert packed to hash_set
void packed2hashset(const umap_ull_mpz &A, hash_set &B);

//! new multiplication routine for hash_set
void poly_mul3(const hash_set &A, const hash_set &B, hash_set &C);

//! Convert hash_set to packed
void hashset2packed(const hash_set &A, umap_ull_mpz &B);

//! Multiply in packed structure
void poly_mul2(const umap_ull_mpz &A, const umap_ull_mpz &B, umap_ull_mpz &C);

//! Convert packed to tuple representation
void packed2poly(const umap_ull_mpz &A, const umap_basic_num &syms, umap_vec_mpz &P);

} // SymEngine

#endif
