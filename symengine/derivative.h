/**
 *  \file derivative.h
 *  Includes differentation functions
 *
 **/

#ifndef SYMENGINE_DERIVATIVE_H
#define SYMENGINE_DERIVATIVE_H

#include <symengine/basic.h>

namespace SymEngine
{

//! Differentiation w.r.t symbols
RCP<const Basic> diff(const RCP<const Basic> &arg, const RCP<const Symbol> &x,
                      bool cache = true);

//! SymPy style differentiation w.r.t non-symbols and symbols
RCP<const Basic> sdiff(const RCP<const Basic> &arg, const RCP<const Basic> &x,
                       bool cache = true);

} // namespace SymEngine

#endif // SYMENGINE_DERIVATIVE_H
