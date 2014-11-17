
/**
 *  \file eval_arb.h
 *  Evaluation of numeric expressions using Arb
 *
 **/

#ifndef CSYMPY_EVAL_ARB_H
#define CSYMPY_EVAL_ARB_H

#ifdef HAVE_CSYMPY_ARB

#include "basic.h"
#include "arb.h"

namespace CSymPy {

// `result` is returned by value since `arb_t` is defined as an array in `arb.h`.
// This design will not change in `arb` and hence will not change in `CSymPy`
// also.
void eval_arb(arb_t result, const Basic &b, long precision = 53);

} // CSymPy

#endif // HAVE_CSYMPY_ARB

#endif
