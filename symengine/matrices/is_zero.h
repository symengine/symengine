#ifndef SYMENGINE_MATRICES_IS_ZERO_H
#define SYMENGINE_MATRICES_IS_ZERO_H

#include <symengine/basic.h>
#include <symengine/assumptions.h>
#include <symengine/matrices/matrix_expr.h>

namespace SymEngine
{

tribool is_zero(const MatrixExpr &m, const Assumptions *assumptions = nullptr);

}

#endif
