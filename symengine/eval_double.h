/**
 *  \file eval_double.h
 *
 **/
#ifndef SYMENGINE_EVAL_DOUBLE_H
#define SYMENGINE_EVAL_DOUBLE_H

#include <complex>
#include "basic.h"

namespace SymEngine {

/*
 * We have two implementations, the visitor pattern (eval_double) and
 * single dispatch (eval_double_single_dispatch).
 */

double eval_double(const Basic &b);

double eval_double_single_dispatch(const Basic &b);

std::complex<double> eval_complex_double(const Basic &b);

} // SymEngine

#endif
