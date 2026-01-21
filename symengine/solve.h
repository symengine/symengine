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
/*
 * Solves the given equation `f` and returns all possible values of `sym` as a
 * Set, given
 * they satisfy the `domain` constraint.
 */
SYMENGINE_EXPORT
RCP<const Set> solve(const RCP<const Basic> &f, const RCP<const Symbol> &sym,
                     const RCP<const Set> &domain = universalset());

// Solves rational equations.
SYMENGINE_EXPORT
RCP<const Set> solve_rational(const RCP<const Basic> &f,
                              const RCP<const Symbol> &sym,
                              const RCP<const Set> &domain = universalset());

// Solves Trigonometric equations.
SYMENGINE_EXPORT
RCP<const Set> solve_trig(const RCP<const Basic> &f,
                          const RCP<const Symbol> &sym,
                          const RCP<const Set> &domain = universalset());

// Solves Polynomial equations.
// Use this method, If you know for sure that `f` is a Polynomial.
SYMENGINE_EXPORT
RCP<const Set> solve_poly(const RCP<const Basic> &f,
                          const RCP<const Symbol> &sym,
                          const RCP<const Set> &domain = universalset());

// Helper method for solving lower order polynomials using known formulae.
SYMENGINE_EXPORT
RCP<const Set> solve_poly_heuristics(const vec_basic &coeffs,
                                     const RCP<const Set> &domain
                                     = universalset());

// Helper method for solving linear equations.
SYMENGINE_EXPORT
RCP<const Set> solve_poly_linear(const vec_basic &coeffs,
                                 const RCP<const Set> &domain = universalset());

// Helper method for solving quadratic equations.
SYMENGINE_EXPORT
RCP<const Set> solve_poly_quadratic(const vec_basic &coeffs,
                                    const RCP<const Set> &domain
                                    = universalset());

// Helper method for solving cubic equations.
SYMENGINE_EXPORT
RCP<const Set> solve_poly_cubic(const vec_basic &coeffs,
                                const RCP<const Set> &domain = universalset());

// Helper method for solving quartic(degree-4) equations.
SYMENGINE_EXPORT
RCP<const Set> solve_poly_quartic(const vec_basic &coeffs,
                                  const RCP<const Set> &domain
                                  = universalset());

/*
 * Helper method to decide if solve_trig can solve a particular equation.
 * Checks the argument of Trigonometric part, and returns false if it is
 * non-linear;
 * true otherwise.
 */
SYMENGINE_EXPORT bool is_a_LinearArgTrigEquation(const Basic &b,
                                                 const Symbol &x);

/* returns Inverse of a complex equation `fX = gY` wrt symbol `sym`.
 * It is like a solver developed specifically to solve equations of the
 * form `exp(f(x)) = gY`(required for trig solvers).
 * For example : invertComplex(exp(x), {1}, x) would give you
 * `{2*I*pi*n | n in (-oo, oo)}` aka values of `x` when `exp(x) = 1`.
 * Dummy `nD` is used as the symbol for `ImageSet` while returning the solution
 * set.
 */
SYMENGINE_EXPORT
RCP<const Set> invertComplex(const RCP<const Basic> &fX,
                             const RCP<const Set> &gY,
                             const RCP<const Symbol> &sym,
                             const RCP<const Dummy> &nD = dummy("n"),
                             const RCP<const Set> &domain = universalset());

// Solver for System of Equations
// TODO : solve systems that have infinitely many solutions or no solution.
// Input as an Augmented Matrix. (A|b) to solve `Ax=b`.
SYMENGINE_EXPORT vec_basic linsolve(const DenseMatrix &system,
                                    const vec_sym &syms);

// Input as a vector of linear equations.
SYMENGINE_EXPORT vec_basic linsolve(const vec_basic &system,
                                    const vec_sym &syms);

// converts system of linear equations into Matrix form.
// first Matrix is for `A` and second one is for `b`.
SYMENGINE_EXPORT
std::pair<DenseMatrix, DenseMatrix>
linear_eqns_to_matrix(const vec_basic &equations, const vec_sym &syms);
} // namespace SymEngine

#endif // SYMENGINE_SOLVE_H
