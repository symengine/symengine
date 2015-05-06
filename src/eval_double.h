/**
 *  \file eval_double.h
 *
 **/
#ifndef SYMENGINE_EVAL_DOUBLE_H
#define SYMENGINE_EVAL_DOUBLE_H

#include "basic.h"

namespace SymEngine {

double eval_double(const Basic &b);

double eval_double_single_dispatch(const Basic &b);

} // SymEngine

#endif
