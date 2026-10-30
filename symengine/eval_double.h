/**
 *  \file eval_double.h
 *
 **/
#ifndef SYMENGINE_EVAL_DOUBLE_H
#define SYMENGINE_EVAL_DOUBLE_H

#include <symengine/basic.h>

#include <unordered_map>
namespace SymEngine
{
typedef std::unordered_map<RCP<const Basic>, double, RCPBasicHash,
                           RCPBasicKeyEq>
    umap_basic_double;
/*
 * We have two implementations, the visitor pattern (eval_double) and
 * single dispatch (eval_double_single_dispatch).
 */

SYMENGINE_EXPORT double eval_double(const Basic &b);

SYMENGINE_EXPORT double eval_double_single_dispatch(const Basic &b);

SYMENGINE_EXPORT double eval_double(const Basic &b,
                                    const umap_basic_double &mpp);

SYMENGINE_EXPORT double eval_double_visitor_pattern(const Basic &b);

SYMENGINE_EXPORT std::complex<double> eval_complex_double(const Basic &b);

} // namespace SymEngine

#endif
