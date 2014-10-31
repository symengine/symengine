
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

void eval_arb(arb_t &result, const Basic &b, long precision = 15);

} // CSymPy

#endif // HAVE_CSYMPY_ARB

#endif
