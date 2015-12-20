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
// Univariate Rational Coefficient Power Series using Piranha
class URatPSeriesPiranha : public Series<pp_t, piranha::rational, URatPSeriesPiranha, false> {
public:
    IMPLEMENT_TYPEID(URATPSERIESPIRANHA)
    virtual void accept(Visitor &v) const;
    static RCP<const URatPSeriesPiranha> series(const RCP<const Basic> &t, const RCP<const Symbol> &x,
                                                unsigned int prec);
    static inline piranha::integer convert(const Integer &x) {
        return piranha::integer(x.as_mpz().get_mpz_t());
    }
    static inline piranha::rational convert(const mpq_class &x) {
        piranha::integer i1(x.get_num_mpz_t());
        piranha::integer i2(x.get_den_mpz_t());
        piranha::rational r(i1);
        r /= i2;
        return r;
    }
    static inline piranha::rational convert(const Rational &x) {
        return convert(x.as_mpq());
    }
    static inline piranha::rational convert(const Number &x) {
        throw std::runtime_error("Not Implemented");
    }
    static inline pp_t mul(const pp_t &s, const pp_t &r, unsigned prec) {
        pp_t::set_auto_truncate_degree(prec);
        return s * r;
    }
    static inline pp_t pow(const pp_t &s, unsigned n, unsigned prec) {
        pp_t::set_auto_truncate_degree(prec);
        return s.pow(n);
    }
    static inline unsigned ldegree(const pp_t &s) {
        return s.ldegree();
    }
    static inline piranha::rational find_cf(const pp_t &s, const pp_t &var, unsigned deg) {
        return s.find_cf({deg});
    }
    static inline piranha::rational root(piranha::rational &c, unsigned n) {
        mpq_class cl_rat(c.get_mpq_view());
        mpq_class cl_root;
        bool res;
        if (mpz_cmp_ui(cl_rat.get_den_mpz_t(), 1) == 0) {
            // integer constant
            res = mpz_root(cl_root.get_num_mpz_t(), cl_rat.get_num_mpz_t(), n) != 0;
        }
        else {
            cl_root.canonicalize();
            RCP<const Rational> cterm = make_rcp<const Rational>(cl_root);
            RCP<const Number> cout;
            res = cterm->nth_root(outArg(cout), n);
            cl_root = static_cast<const Rational&>(*cout).i;
        }
        if (not res)
            throw std::runtime_error("constant term is not an nth power");
        return convert(cl_root);
    }
    static inline pp_t diff(const pp_t &s, const pp_t &var) {
        return s.partial(var.get_symbol_set()[0].get_name());
    }
    static inline pp_t integrate(const pp_t &s, const pp_t &var) {
        return s.integrate(var.get_symbol_set()[0].get_name());
    }
};

} // SymEngine

#endif // HAVE_SYMENGINE_PIRANHA

#endif //SYMENGINE_SERIES_PIRANHA_H
