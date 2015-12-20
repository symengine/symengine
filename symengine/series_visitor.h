#ifndef SYMENGINE_SERIES_VISITOR_H
#define SYMENGINE_SERIES_VISITOR_H

#include <symengine/series.h>
#include <symengine/visitor.h>

namespace SymEngine {

template<typename Poly, typename Coeff, typename SeriesClass, bool symbolic = false>
class SeriesVisitor : public BaseVisitor<SeriesVisitor<Poly, Coeff, SeriesClass, symbolic>> {
private:
    Poly p;
    const Poly &var;
    const std::string &varname;
    const unsigned prec;
public:
    inline SeriesVisitor(const Poly &var_, const std::string &varname_, const unsigned prec_)
            : var(var_), varname(varname_), prec(prec_) {

    }
    RCP<const SeriesClass> series(const RCP<const Basic> &x) {
        return make_rcp<SeriesClass>(apply(x), varname, prec);
    }

    static inline const std::list<unsigned int> &step_list(unsigned int prec) {
        static std::list<unsigned int> steps;
        if (not steps.empty()) {
            if (*(steps.rbegin()) == prec)
                return steps;
            else
                steps.clear();
        }

        unsigned int tprec = prec;
        while (tprec > 4) {
            tprec = 2 + tprec / 2;
            steps.push_front(tprec);
        }
        steps.push_front(2);
        steps.push_back(prec);
        return steps;
    }

    static inline Poly series_invert(const Poly &s, const Poly& var, unsigned int prec) {
        if (s == 0)
            throw std::runtime_error("series_invert: division by zero");
        if (s == 1)
            return Poly(1);
        const short ldeg = SeriesClass::ldegree(s);
        const Coeff co = SeriesClass::find_cf(s, var, ldeg);
        Poly p(1/co), ss = s;
        if (ldeg != 0)
            ss = s * SeriesClass::pow(var, -ldeg, -ldeg);
        auto steps = step_list(prec);
        for (const auto step : steps) {
            p = SeriesClass::mul(2 - SeriesClass::mul(p, ss, step), p, step);
        }
        if (ldeg != 0)
            return p * SeriesClass::pow(var, -ldeg, prec);
        else
            return p;
    }

    static inline Poly series_reverse(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff co = SeriesClass::find_cf(s, var, 0);
        if (co != 0)
            throw std::runtime_error("reversion of series with constant term");
        const Coeff a = SeriesClass::find_cf(s, var, 1);
        if (a == 0)
            throw std::runtime_error("reversion of series with zero term of degree one");
        Poly r(var);
        r /= a;
        for (unsigned int i = 2; i < prec; i++) {
            Poly sp = SeriesClass::subs(s, var, r, i + 1);
            r -= SeriesClass::pow(var, i, i) * SeriesClass::find_cf(sp, var, i) / a;
        }
        return r;
    }

    static inline Poly series_nthroot(const Poly &s, int n, const Poly &var, unsigned int prec) {
        if (n == 0)
            return Poly(1);
        if (n == 1)
            return s;
        if (n == -1)
            return series_invert(s, var, prec);

        Coeff ct = SeriesClass::find_cf(s, var, 0);
        if (ct == 0)
            throw std::runtime_error("series_nthroot needs a constant term");
        bool do_inv = false;
        if (n < 0) {
            n = -n;
            do_inv = true;
        }

        Coeff ctroot = SeriesClass::root(ct, n);
        Poly res_p(1), sn = s / ct;
        auto steps = step_list(prec);
        for (const auto step : steps) {
            Poly t = SeriesClass::mul(SeriesClass::pow(res_p, n + 1, step), sn, step);
            res_p += (res_p - t) / n;
        }
        if (do_inv)
            return res_p * ctroot;
        else
            return series_invert(res_p, var, prec) * ctroot;
    }

    static inline Poly series_atan(const Poly &s, const Poly& var, unsigned int prec) {
        Poly res_p(0);
        if (s == 0)
            return res_p;
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("atan(const) not Implemented");

        if (s == var) {
            //! fast atan(x)
            short sign = 1;
            for (unsigned int i = 1; i < prec; i += 2, sign *= -1) {
                res_p += SeriesClass::pow(var, i, prec) * (Coeff(sign) / Coeff(i));
            }
            return res_p;
        }
        const Poly &p = SeriesClass::pow(s, 2, prec - 1) + 1;
        res_p = SeriesClass::mul(SeriesClass::diff(s, var), series_invert(p, var, prec - 1), prec - 1);
        return SeriesClass::integrate(res_p, var);
    }

    static inline Poly series_tan(const Poly &s, const Poly& var, unsigned int prec) {
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("tan(const) not Implemented");

        // IDEA: use this to get tan(x) coefficients:
        //    # n -> [0, a(1), a(2), ..., a(n)] for n > 0.
        //    def A000182_list(n):
        //    ....T = [0 for i in range(1, n+2)]
        //    ....T[1] = 1
        //    ....for k in range(2, n+1):
        //    ........T[k] = (k-1)*T[k-1]
        //    ....for k in range(2, n+1):
        //    ........for j in range(k, n+1):
        //    ............T[j] = (j-k)*T[j-1]+(j-k+2)*T[j]
        //    ....return T
        //  Ref.: https://oeis.org/A000182

        Poly res_p(0);
        auto steps = step_list(prec);
        for (const auto step : steps) {
            Poly t = SeriesClass::pow(res_p, 2, step) + 1;
            res_p += SeriesClass::mul(s - series_atan(res_p, var, step), t, step);
        }
        return res_p;
    }

    static inline Poly series_cot(const Poly &s, const Poly& var, unsigned int prec) {
        return series_invert(series_tan(s, var, prec), var, prec);
    }

    static inline Poly series_sin(const Poly &s, const Poly& var, unsigned int prec) {
        Poly res_p(0);

        if (s == var) {
            //! fast sin(x)
            Coeff prod(1);
            for (unsigned int i = 0; i < prec / 2; i++) {
                const short j = 2 * i + 1;
                if (i != 0)
                    prod /= 1 - j;
                prod /= j;
                res_p += SeriesClass::pow(var, j, j) * prod;
            }
            return res_p;
        }

        if (SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("sin(const) not Implemented");

        const Poly t(series_tan(s / 2, var, prec));
        return series_invert(SeriesClass::pow(t, 2, prec) + 1, var, prec) * t * 2;
    }

    static inline Poly series_csc(const Poly &s, const Poly& var, unsigned int prec) {
        return series_invert(series_sin(s, var, prec), var, prec);
    }

    static inline Poly series_asin(const Poly &s, const Poly& var, unsigned int prec) {
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("asin(const) not Implemented");

        const Poly t(-SeriesClass::pow(s, 2, prec) + 1);
        return SeriesClass::diff(s, var) * SeriesClass::integrate(series_nthroot(t, -2, var, prec - 1), var);
    }

    static inline Poly series_cos(const Poly &s, const Poly& var, unsigned int prec) {
        Poly res_p(1);

        if (s == var) {
            //! fast cos(x)
            Coeff prod(1);
            for (unsigned int i = 1; i <= prec / 2; i++) {
                const short j = 2 * i;
                prod /= 1 - j;
                prod /= j;
                res_p += SeriesClass::pow(var, j, j) * prod;
            }
            return res_p;
        }

        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("cos(const) not Implemented");

        Poly t(SeriesClass::pow(series_tan(s / 2, var, prec), 2, prec));
        return series_invert(t + 1, var, prec) * (-(t - 1));
    }

    static inline Poly series_sec(const Poly &s, const Poly& var, unsigned int prec) {
        return series_invert(series_cos(s, var, prec), var, prec);
    }

    static inline Poly series_log(const Poly &s, const Poly& var, unsigned int prec) {
        Poly res_p(0);
        if (s == 1)
            return res_p;
        if (s == var + 1) {
            //! fast log(1+x)
            for (unsigned int i = 1; i < prec; i++) {
                res_p += SeriesClass::pow(var, i, i) * Coeff(((i % 2) == 0) ? -1 : 1) / Coeff(i);
            }
            return res_p;
        }

        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 1)
            throw std::logic_error("log(const!=0) not Implemented");
        res_p = SeriesClass::mul(SeriesClass::diff(s, var), series_invert(s, var, prec), prec - 1);
        return SeriesClass::integrate(res_p, var);
    }

    static inline Poly series_exp(const Poly &s, const Poly& var, unsigned int prec) {
        Poly res_p(1);
        if (s == 0)
            return res_p;

        if (s == var) {
            //! fast exp(x)
            Coeff coef(1);
            for (unsigned int i = 1; i < prec; i++) {
                coef /= i;
                res_p += SeriesClass::pow(var, i, prec) * coef;
            }
            return res_p;
        }

        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("exp(const) not Implemented");

        auto steps = step_list(prec);
        Poly t = s + 1;
        for (const auto step : steps) {
            res_p = SeriesClass::mul(res_p, t - series_log(res_p, var, step), step);
        }
        return res_p;
    }

    static inline Poly series_lambertw(const Poly &s, const Poly& var, unsigned int prec) {
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("lambertw(const) not Implemented");

        Poly p1(0);

        auto steps = step_list(prec);
        for (const auto step : steps) {
            const Poly e(series_exp(p1, var, step));
            const Poly p2(SeriesClass::mul(e, p1, step) - s);
            const Poly p3(series_invert(SeriesClass::mul(e, (p1 + 1), step), var, step));
            p1 -= SeriesClass::mul(p2, p3, step);
        }
        return p1;
    }

    static inline Poly series_sinh(const Poly &s, const Poly& var, unsigned int prec) {
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("sinh(const) not Implemented");

        const Poly p1(series_exp(s, var, prec));
        const Poly p2(series_invert(p1, var, prec));
        return (p1 - p2) / 2;
    }

    static inline Poly series_cosh(const Poly &s, const Poly& var, unsigned int prec) {
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("cosh(const) not Implemented");

        const Poly p1(series_exp(s, var, prec));
        const Poly p2(series_invert(p1, var, prec));
        return (p1 + p2) / 2;
    }

    static inline Poly series_atanh(const Poly &s, const Poly& var, unsigned int prec) {
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("atanh(const) not Implemented");

        const Poly &p(-(SeriesClass::pow(s, 2, prec - 1)) + 1);
        const Poly res_p(SeriesClass::mul(SeriesClass::diff(s, var), series_invert(p, var, prec - 1), prec - 1));
        return SeriesClass::integrate(res_p, var);
    }

    static inline Poly series_asinh(const Poly &s, const Poly& var, unsigned int prec) {
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("asinh(const) not Implemented");

        const Poly &p(series_nthroot(SeriesClass::pow(s, 2, prec - 1) + 1, 2, var, prec - 1));
        const Poly res_p(SeriesClass::diff(s, var) * series_invert(p, var, prec - 1));
        return SeriesClass::integrate(res_p, var);
    }

    static inline Poly series_tanh(const Poly &s, const Poly& var, unsigned int prec) {
        if (not symbolic and SeriesClass::find_cf(s, var, 0) != 0)
            throw std::logic_error("tanh(const) not Implemented");

        Poly res_p(s);
        auto steps = step_list(prec);
        for (const auto step : steps) {
            const Poly p(s - series_atanh(res_p, var, step));
            res_p += SeriesClass::mul(-p, SeriesClass::pow(res_p, 2, step) - 1, step);
        }
        return res_p;
    }

    Poly apply(const RCP<const Basic> &x) {
        x->accept(*this);
        Poly temp(std::move(p));
        return std::move(p);
    }

    void bvisit(const Add &x) {
        Poly temp(apply(x.coef_));
        for (const auto& term : x.dict_) {
            temp += apply(term.first) * apply(term.second);
        }
        p = temp;
    }
    void bvisit(const Mul &x) {
        Poly temp(apply(x.coef_));
        for (const auto& term : x.dict_) {
            temp = SeriesClass::mul(temp, apply(pow(term.first, term.second)), prec);
        }
        p = temp;
    }
    void bvisit(const Pow &x) {
        const RCP<const Basic>& base = x.get_base(), exp = x.get_exp();
        if (is_a<Integer>(*exp)) {
            const Integer& ii = (static_cast<const Integer&>(*exp));
            if (not ii.i.fits_sint_p())
                throw std::runtime_error("series power exponent size");
            const int sh = ii.as_int();
            base->accept(*this);
            if (sh == 1) {
                return;
            } else if (sh > 0) {
                p = SeriesClass::pow(p, sh, prec);
            } else if (sh == -1) {
                p = series_invert(p, var, prec);
            } else {
                p = series_invert(SeriesClass::pow(p, -sh, prec), var, prec);
            }

        } else if (is_a<Rational>(*exp)) {
            const Rational& rat = (static_cast<const Rational&>(*exp));
            const mpz_class& expnumz = rat.i.get_num();
            const mpz_class& expdenz = rat.i.get_den();
            if (not expnumz.fits_sint_p() or not expdenz.fits_sint_p())
                throw std::runtime_error("series rational power exponent size");
            const int num = expnumz.get_si();
            const int den = expdenz.get_si();
            base->accept(*this);
            const Poly proot(series_nthroot(apply(base), den, var, prec));
            if (num == 1)
                 p = proot;
            if (num > 0)
                 p = SeriesClass::pow(proot, num, prec);
            else {
                if (num == -1)
                    p = series_invert(proot, var, prec);
                else
                    p = series_invert(SeriesClass::pow(proot, -num, prec), var, prec);
            }
        } else if (eq(*E, *base)) {
             p = series_exp(apply(exp), var, prec);
        } else {
            p = series_exp(apply(exp) * series_log(apply(base), var, prec), var, prec);
        }
    }

    void bvisit(const Integer &x) {
        p = SeriesClass::convert(x);
    }
    void bvisit(const Rational &x) {
        p = SeriesClass::convert(x);
    }
    void bvisit(const Complex &x) {
        p = SeriesClass::convert(x);
    }
    void bvisit(const RealDouble &x) {
        p = SeriesClass::convert(x);
    }
    void bvisit(const ComplexDouble &x) {
        p = SeriesClass::convert(x);
    }
#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x) {
        p = SeriesClass::convert(x);
    }
#endif
#ifdef HAVE_SYMENGINE_MPC
    void bvisit(const ComplexMPC &x) {
        p = SeriesClass::convert(x);
    }
#endif
    void bvisit(const Sin &x) {
        x.get_arg()->accept(*this);
        p = series_sin(p, var, prec);
    }
    void bvisit(const Cos &x) {
        x.get_arg()->accept(*this);
        p = series_cos(p, var, prec);
    }
    void bvisit(const Tan &x) {
        x.get_arg()->accept(*this);
        p = series_tan(p, var, prec);
    }
    void bvisit(const Cot &x) {
        x.get_arg()->accept(*this);
        p = series_cot(p, var, prec);
    }
    void bvisit(const Csc &x) {
        x.get_arg()->accept(*this);
        p = series_csc(p, var, prec);
    }
    void bvisit(const Sec &x) {
        x.get_arg()->accept(*this);
        p = series_sec(p, var, prec);
    }
    void bvisit(const Log &x) {
        x.get_arg()->accept(*this);
        p = series_log(p, var, prec);
    }
    void bvisit(const ASin &x) {
        x.get_arg()->accept(*this);
        p = series_asin(p, var, prec);
    }
    void bvisit(const ATan &x) {
        x.get_arg()->accept(*this);
        p = series_atan(p, var, prec);
    }
    void bvisit(const Sinh &x) {
        x.get_arg()->accept(*this);
        p = series_sinh(p, var, prec);
    }
    void bvisit(const Cosh &x) {
        x.get_arg()->accept(*this);
        p = series_cosh(p, var, prec);
    }
    void bvisit(const Tanh &x) {
        x.get_arg()->accept(*this);
        p = series_tanh(p, var, prec);
    }
    void bvisit(const ASinh &x) {
        x.get_arg()->accept(*this);
        p = series_asinh(p, var, prec);
    }
    void bvisit(const ATanh &x) {
        x.get_arg()->accept(*this);
        p = series_atanh(p, var, prec);
    }
    void bvisit(const LambertW &x) {
        x.get_arg()->accept(*this);
        p = series_lambertw(p, var, prec);
    }
    void bvisit(const Basic &x) {
        throw std::runtime_error("Not Implemented");
    }
};

} //SymEngine
#endif //SYMENGINE_SERIES_VISITOR_H
