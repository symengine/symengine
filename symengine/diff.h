/**
 *  \file diff.h
 *  Symbolic differentiation of expressions
 *
 **/

#ifndef SYMENGINE_DIFF_H
#define SYMENGINE_DIFF_H

#include <symengine/symengine_config.h>

#include <symengine/basic.h>

namespace SymEngine {

//! Returns the derivative of self
RCP<const Basic> diff(const RCP<const Basic> &self, const RCP<const Symbol> &x);

} // SymEngine

#endif
