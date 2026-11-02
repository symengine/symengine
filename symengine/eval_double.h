/**
 *  \file eval_double.h
 *
 **/
#ifndef SYMENGINE_EVAL_DOUBLE_H
#define SYMENGINE_EVAL_DOUBLE_H

#include <symengine/basic.h>
#include <symengine/dict.h>

namespace SymEngine
{

/*
 * There are three implementations of real evaluation: eval_double (visitor
 * with a dedicated accept method, the fastest), eval_double_visitor_pattern
 * (generic visitor) and eval_double_single_dispatch (function table). Only
 * eval_double and eval_complex_double accept values for symbols.
 */

//! Evaluate `b` to a double. Symbols in `b` are replaced by their values in
//! `mpp`; only Symbol keys are used, other keys are ignored. A symbol missing
//! from `mpp` raises SymEngineException.
SYMENGINE_EXPORT double eval_double(const Basic &b,
                                    const umap_basic_double &mpp = {});

SYMENGINE_EXPORT double eval_double_single_dispatch(const Basic &b);

SYMENGINE_EXPORT double eval_double_visitor_pattern(const Basic &b);

//! Evaluate `b` to a complex double, with symbols replaced by their values in
//! `mpp` as in eval_double.
SYMENGINE_EXPORT std::complex<double>
eval_complex_double(const Basic &b, const umap_basic_double &mpp = {});

} // namespace SymEngine

#endif
