/**
 *  \file solve.h
 *  Solvers
 *
 **/

#ifndef SYMENGINE_SOLVE_H
#define SYMENGINE_SOLVE_H

#include <symengine/sets.h>
#include <symengine/polys/uintpoly_flint.h>
#include <symengine/polys/uintpoly_piranha.h>
#include <symengine/polys/uexprpoly.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/polys/uratpoly.h>
#include <symengine/matrix.h>

namespace SymEngine
{

RCP<const Set> solve(const RCP<const Basic> &f, const RCP<const Symbol> &sym,
                     const RCP<const Set> &domain = universalset());

RCP<const Set> solve_rational(const RCP<const Basic> &f,
                              const RCP<const Symbol> &sym,
                              const RCP<const Set> &domain = universalset());

RCP<const Set> solve_poly(const RCP<const Basic> &f,
                          const RCP<const Symbol> &sym,
                          const RCP<const Set> &domain = universalset());

RCP<const Set> solve_poly_heuristics(const vec_basic &coeffs,
                                     const RCP<const Set> &domain
                                     = universalset());

RCP<const Set> solve_poly_linear(const vec_basic &coeffs,
                                 const RCP<const Set> &domain = universalset());

RCP<const Set> solve_poly_quadratic(const vec_basic &coeffs,
                                    const RCP<const Set> &domain
                                    = universalset());

RCP<const Set> solve_poly_cubic(const vec_basic &coeffs,
                                const RCP<const Set> &domain = universalset());

RCP<const Set> solve_poly_quartic(const vec_basic &coeffs,
                                  const RCP<const Set> &domain
                                  = universalset());

// Solver for System of Equations
// TODO : solve systems that have infinitely many solutions or no solution.
// Input as an Augmented Matrix. (A|b) to solve `Ax=b`.
vec_basic linsolve(const DenseMatrix &system, const vec_sym &syms);

// Input as a vector of linear equations.
vec_basic linsolve(const vec_basic &system, const vec_sym &syms);

// converts system of linear equations into Matrix form.
// first Matrix is for `A` and second one is for `b`.
std::pair<const DenseMatrix, const DenseMatrix>
linear_eqns_to_matrix(const vec_basic &equations, const vec_sym &syms);

} // namespace SymEngine

#endif // SYMENGINE_SOLVE_H
