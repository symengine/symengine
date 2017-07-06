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

} // namespace SymEngine

#endif // SYMENGINE_SOLVE_H
