/**
 *  \file solve.h
 *  Solvers
 *
 **/

#ifndef SYMENGINE_SOLVE_H
#define SYMENGINE_SOLVE_H

#include <symengine/sets.h>
#include <symengine/polys/uintpoly_flint.h>
#include <symengine/polys/uintpoly.h>
#ifdef HAVE_SYMENGINE_FLINT

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
RCP<const Set> solve_poly_heuristics(const RCP<const UIntPoly> &f,
                                     const RCP<const Symbol> &sym,
                                     const RCP<const Set> &domain
                                     = universalset());
RCP<const Set> solve_poly_linear(const RCP<const UIntPoly> &f,
                                 const RCP<const Symbol> &sym,
                                 const RCP<const Set> &domain = universalset());

RCP<const Set>
solve_poly_quadratic(const RCP<const UIntPoly> &f, const RCP<const Symbol> &sym,
                     const RCP<const Set> &domain = universalset());

RCP<const Set> solve_poly_cubic(const RCP<const UIntPoly> &f,
                                const RCP<const Symbol> &sym,
                                const RCP<const Set> &domain = universalset());

RCP<const Set>
solve_poly_quartic(const RCP<const UIntPoly> &f, const RCP<const Symbol> &sym,
                   const RCP<const Set> &domain = universalset());

} // namespace SymEngine
#endif // HAVE_SYMENGINE_FLINT

#endif // SYMENGINE_SOLVE_H
