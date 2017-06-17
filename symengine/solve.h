/**
 *  \file solve.h
 *  Solvers
 *
 **/

#ifndef SYMENGINE_SOLVE_H
#define SYMENGINE_SOLVE_H

#include <symengine/sets.h>

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
RCP<const Set> solve_poly_linear(const RCP<const Basic> &f,
                                 const RCP<const Symbol> &sym,
                                 const RCP<const Set> &domain = universalset());

RCP<const Set>
solve_poly_quadratic(const RCP<const Basic> &f, const RCP<const Symbol> &sym,
                     const RCP<const Set> &domain = universalset());

RCP<const Set> solve_poly_cubic(const RCP<const Basic> &f,
                                const RCP<const Symbol> &sym,
                                const RCP<const Set> &domain = universalset());

} // namespace SymEngine

#endif // SYMENGINE_SOLVE_H
