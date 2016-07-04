#ifndef SYMENGINE_SERIES_FLINT_H
#define SYMENGINE_SERIES_FLINT_H

#include <symengine/series.h>
#include <symengine/expression.h>

#ifdef HAVE_SYMENGINE_FLINT
#include <symengine/flint_wrapper.h>

namespace SymEngine
{

using fp_t = fmpq_poly_wrapper;
// Univariate Rational Coefficient Power SeriesBase using Flint
class URatPSeriesFlint : public SeriesBase<fp_t, fmpq_wrapper, URatPSeriesFlint>
{
public:
    URatPSeriesFlint(const fp_t p, const std::string varname,
                     const unsigned degree);
    IMPLEMENT_TYPEID(URATPSERIESFLINT)
    virtual int compare(const Basic &o) const;
    virtual std::size_t __hash__() const;
    virtual RCP<const Basic> as_basic() const;
    virtual umap_int_basic as_dict() const;
    virtual RCP<const Basic> get_coeff(int) const;

    static RCP<const URatPSeriesFlint>
    series(const RCP<const Basic> &t, const std::string &x, unsigned int prec);
    static fp_t var(const std::string &s);
    static fp_t convert(const integer_class &x);
    static fp_t convert(const rational_class &x);
    static fp_t convert(const Rational &x);
    static fp_t convert(const Integer &x);
    static fp_t convert(const Basic &x);
    static inline fp_t mul(const fp_t &s, const fp_t &r, unsigned prec)
    {
        return s.mullow(r, prec);
    }
    static fp_t pow(const fp_t &s, int n, unsigned prec);
    static unsigned ldegree(const fp_t &s);
    static inline fmpq_wrapper find_cf(const fp_t &s, const fp_t &var,
                                       unsigned deg)
    {
        return s.get_coeff(deg);
    }
    static fmpq_wrapper root(fmpq_wrapper &c, unsigned n);
    static fp_t diff(const fp_t &s, const fp_t &var);
    static fp_t integrate(const fp_t &s, const fp_t &var);
    static fp_t subs(const fp_t &s, const fp_t &var, const fp_t &r,
                     unsigned prec);

    static inline fp_t series_invert(const fp_t &s, const fp_t &var,
                                     unsigned int prec)
    {
        SYMENGINE_ASSERT(not s.get_coeff(0).is_zero());
        return s.inv_series(prec);
    }
    static inline fp_t series_reverse(const fp_t &s, const fp_t &var,
                                      unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero()
                         and not s.get_coeff(1).is_zero());
        return s.revert_series(prec);
    }
    static inline fp_t series_log(const fp_t &s, const fp_t &var,
                                  unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_one());
        return s.log_series(prec);
    }
    static inline fp_t series_exp(const fp_t &s, const fp_t &var,
                                  unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.exp_series(prec);
    }
    static inline fp_t series_sin(const fp_t &s, const fp_t &var,
                                  unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.sin_series(prec);
    }

    static inline fp_t series_cos(const fp_t &s, const fp_t &var,
                                  unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.cos_series(prec);
    }

    static inline fp_t series_tan(const fp_t &s, const fp_t &var,
                                  unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.tan_series(prec);
    }
    static inline fp_t series_atan(const fp_t &s, const fp_t &var,
                                   unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.atan_series(prec);
    }
    static inline fp_t series_atanh(const fp_t &s, const fp_t &var,
                                    unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.atanh_series(prec);
    }
    static inline fp_t series_asin(const fp_t &s, const fp_t &var,
                                   unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.asin_series(prec);
    }
    static inline fp_t series_asinh(const fp_t &s, const fp_t &var,
                                    unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.asinh_series(prec);
    }
    static inline fp_t series_acos(const fp_t &s, const fp_t &var,
                                   unsigned int prec)
    {
        throw std::runtime_error("acos() not implemented");
    }
    static inline fp_t series_sinh(const fp_t &s, const fp_t &var,
                                   unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.sinh_series(prec);
    }
    static inline fp_t series_cosh(const fp_t &s, const fp_t &var,
                                   unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.cosh_series(prec);
    }
    static inline fp_t series_tanh(const fp_t &s, const fp_t &var,
                                   unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());
        return s.tanh_series(prec);
    }
    static inline fp_t series_lambertw(const fp_t &s, const fp_t &var,
                                       unsigned int prec)
    {
        SYMENGINE_ASSERT(s.get_coeff(0).is_zero());

        fp_t p1;
        p1.set_zero();

        auto steps = step_list(prec);
        for (const auto step : steps) {
            const fp_t e(series_exp(p1, var, step));
            const fp_t p2(mul(e, p1, step) - s);
            const fp_t p3(
                series_invert(mul(e, fp_t(p1 + fp_t(1)), step), var, step));
            p1 -= mul(p2, p3, step);
        }
        return p1;
    }

    static inline fp_t series_nthroot(const fp_t &s, int n, const fp_t &var,
                                      unsigned int prec)
    {
        fp_t one;
        one.set_one();
        if (n == 0)
            return one;
        if (n == 1)
            return s;
        if (n == -1)
            return series_invert(s, var, prec);

        const short ldeg = ldegree(s);
        if (ldeg % n != 0) {
            throw std::runtime_error("Puiseux series not implemented.");
        }
        fp_t ss = s;
        if (ldeg != 0) {
            ss = s * pow(var, -ldeg, prec);
        }
        fmpq_wrapper ct = find_cf(ss, var, 0);
        bool do_inv = false;
        if (n < 0) {
            n = -n;
            do_inv = true;
        }

        fmpq_wrapper ctroot = root(ct, n);
        fp_t res_p = one, sn = fp_t(ss / ct);
        auto steps = step_list(prec);
        for (const auto step : steps) {
            fp_t t = mul(pow(res_p, n + 1, step), sn, step);
            res_p += (res_p - t) / n;
        }
        if (ldeg != 0) {
            res_p *= pow(var, ldeg / n, prec);
        }
        if (do_inv)
            return fp_t(res_p * ctroot);
        else
            return fp_t(series_invert(res_p, var, prec) * ctroot);
    }
};
} // SymEngine

#endif // HAVE_SYMENGINE_FLINT

#endif // SYMENGINE_SERIES_FLINT_H
