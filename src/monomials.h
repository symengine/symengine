/**
 *  \file monomials.h
 *  Monomial Multiplication
 *
 **/
 
#ifndef CSYMPY_MONOMIALS_H
#define CSYMPY_MONOMIALS_H

#include "basic.h"

namespace SymEngine {
//! Monomial multiplication
void monomial_mul(const vec_int &A, const vec_int &B, vec_int &C);

} // SymEngine

#endif
