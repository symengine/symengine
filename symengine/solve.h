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

RCP<const Set>
solve_poly_heuristics(const vec_basic &coeffs, const RCP<const Symbol> &sym,
                      const RCP<const Set> &domain = universalset());

#if (defined(HAVE_SYMENGINE_FLINT)                                             \
     && SYMENGINE_INTEGER_CLASS != SYMENGINE_FLINT)
inline RCP<const Basic> get_coeff_basic(const fmpz_wrapper &i)
{
    integer_class x;
    fmpz_get_mpz(x.get_mpz_t(), i.get_fmpz_t());
    return integer(x);
}

inline RCP<const Basic> get_coeff_basic(const fmpq_wrapper &i)
{
    rational_class x;
    fmpq_get_mpq(x.get_mpq_t(), i.get_fmpq_t());
    return Rational::from_mpq(x);
}
#endif

#if (defined(HAVE_SYMENGINE_PIRANHA)                                           \
     && SYMENGINE_INTEGER_CLASS != SYMENGINE_PIRANHA)
inline RCP<const Basic> get_coeff_basic(const piranha::integer &i)
{
    integer_class x;
    mpz_set(x.get_mpz_t(), i.get_mpz_view());
    return integer(x);
}
inline RCP<const Basic> get_coeff_basic(const piranha::rational &i)
{
    rational_class x;
    mpq_set(x.get_mpq_t(), i.get_mpq_view());
    return Rational::from_mpq(x);
}
#endif

inline RCP<const Basic> get_coeff_basic(const integer_class &i)
{
    return integer(i);
}

inline RCP<const Basic> get_coeff_basic(const rational_class &i)
{
    return Rational::from_mpq(i);
}

inline RCP<const Basic> get_coeff_basic(const Expression &i)
{
    return i.get_basic();
}

template <typename Poly>
inline vec_basic extract_coeffs(const RCP<const Poly> &f)
{
    int degree = f->size() - 1;
    vec_basic coeffs;
    for (int i = 0; i <= degree; i++)
        coeffs.push_back(get_coeff_basic(f->get_coeff(i)));
    return coeffs;
}

template <typename Poly>
struct solve_upoly_helper {
    static RCP<const Set> solve_upoly_h(const RCP<const Poly> &f,
                                        const RCP<const Symbol> &sym,
                                        const RCP<const Set> &domain)
    {
        auto degree = f->get_poly().degree();
        if (degree <= 4) {
            return solve_poly_heuristics(extract_coeffs(f), sym, domain);
        } else {
            return conditionset(sym, logical_and({Eq(f->as_symbolic(), zero),
                                                  domain->contains(sym)}));
        }
    }
};

#ifdef HAVE_SYMENGINE_FLINT
template <>
struct solve_upoly_helper<UIntPolyFlint> {
    static RCP<const Set> solve_upoly_h(const RCP<const UIntPolyFlint> &f,
                                        const RCP<const Symbol> &sym,
                                        const RCP<const Set> &domain)
    {
#if __FLINT_RELEASE > 20502
        try {
            auto fac = factors(*f);
            set_set solns;
            for (const auto &elem : fac) {
                auto uip = UIntPoly::from_poly(*elem.first);
                auto degree = uip->get_poly().degree();
                if (degree <= 4) {
                    solns.insert(solve_poly_heuristics(extract_coeffs(uip), sym,
                                                       domain));
                } else {
                    solns.insert(conditionset(
                        sym, logical_and({Eq(uip->as_symbolic(), zero),
                                          domain->contains(sym)})));
                }
            }
            return SymEngine::set_union(solns);
        } catch (SymEngineException &x) {
            // Try next
        }
#endif
        auto degree = f->get_poly().degree();
        if (degree <= 4) {
            return solve_poly_heuristics(extract_coeffs(f), sym, domain);
        } else {
            return conditionset(sym, logical_and({Eq(f->as_symbolic(), zero),
                                                  domain->contains(sym)}));
        }
    }
};
#endif

template <typename Poly>
RCP<const Set> solve_upoly(const RCP<const Poly> &f,
                           const RCP<const Symbol> &sym,
                           const RCP<const Set> &domain = universalset())
{
    return solve_upoly_helper<Poly>::solve_upoly_h(f, sym, domain);
}

RCP<const Set> solve_poly_linear(const vec_basic &coeffs,
                                 const RCP<const Symbol> &sym,
                                 const RCP<const Set> &domain = universalset());

RCP<const Set>
solve_poly_quadratic(const vec_basic &coeffs, const RCP<const Symbol> &sym,
                     const RCP<const Set> &domain = universalset());

RCP<const Set> solve_poly_cubic(const vec_basic &coeffs,
                                const RCP<const Symbol> &sym,
                                const RCP<const Set> &domain = universalset());

RCP<const Set>
solve_poly_quartic(const vec_basic &coeffs, const RCP<const Symbol> &sym,
                   const RCP<const Set> &domain = universalset());

} // namespace SymEngine

#endif // SYMENGINE_SOLVE_H
