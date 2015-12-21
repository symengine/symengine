#ifndef SYMENGINE_SERIES_PIRANHA_H
#define SYMENGINE_SERIES_PIRANHA_H

#include <symengine/series.h>
#include <symengine/rational.h>

#ifdef HAVE_SYMENGINE_PIRANHA
#include <piranha/monomial.hpp>
#include <piranha/polynomial.hpp>
#include <piranha/mp_rational.hpp>
#include <piranha/mp_integer.hpp>

namespace SymEngine {

using pp_t = piranha::polynomial<piranha::rational,piranha::monomial<short>>;
// Univariate Rational Coefficient Power SeriesBase using Piranha
class URatPSeriesPiranha : public SeriesBase<pp_t, piranha::rational, URatPSeriesPiranha, false> {
public:
    URatPSeriesPiranha(const pp_t p, const std::string varname, const unsigned degree);
    IMPLEMENT_TYPEID(URATPSERIESPIRANHA)
    virtual void accept(Visitor &v) const;
    virtual int compare(const Basic &o) const;
    virtual std::size_t __hash__() const;
    static RCP<const URatPSeriesPiranha> series(const RCP<const Basic> &t, const std::string &x,
                                                unsigned int prec);
    static piranha::integer convert(const Integer &x);
    static piranha::rational convert(const mpq_class &x);
    static pp_t var(const std::string &s);
    static piranha::rational convert(const Rational &x);
    static piranha::rational convert(const Number &x);
    static pp_t mul(const pp_t &s, const pp_t &r, unsigned prec);
    static pp_t pow(const pp_t &s, int n, unsigned prec);
    static unsigned ldegree(const pp_t &s);
    static piranha::rational find_cf(const pp_t &s, const pp_t &var, unsigned deg);
    static piranha::rational root(piranha::rational &c, unsigned n);
    static pp_t diff(const pp_t &s, const pp_t &var);
    static pp_t integrate(const pp_t &s, const pp_t &var);
    static pp_t subs(const pp_t &s, const pp_t &var, const pp_t &r, unsigned prec);
};

} // SymEngine

#endif // HAVE_SYMENGINE_PIRANHA

#endif //SYMENGINE_SERIES_PIRANHA_H
