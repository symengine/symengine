
/**
 *  \file eval.h
 *  Evaluation of numeric expressions using Arb
 *
 **/

#ifndef CSYMPY_EVAL_H
#define CSYMPY_EVAL_H

#include "basic.h"

#ifdef HAVE_CSYMPY_ARB
#  include "arb.h"
#endif // HAVE_CSYMPY_ARB

namespace CSymPy {

//! Create an arb_t instance from a gmp integer
inline void mpz_to_arb(arb_t &a, const mpz_t z);

//! Create an arb_t instance from a gmp rational
inline void mpq_to_arb(arb_t &a, const mpq_t q, long prec);

//! Evaluate a CSymPy power using Arb
RCP<const Basic> eval_pow(const RCP<const Basic> &x, long prec);

//! Main eval function
RCP<const Basic> eval(const RCP<const Basic> &x, long prec);

} // CSymPy

#endif
