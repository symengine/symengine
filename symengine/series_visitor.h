#ifndef SYMENGINE_SERIES_VISITOR_H
#define SYMENGINE_SERIES_VISITOR_H

#include <symengine/series.h>
#include <symengine/visitor.h>

namespace SymEngine {

template<typename Poly, typename Coeff, typename Series>
class SeriesVisitor : public BaseVisitor<SeriesVisitor<Poly, Coeff, Series>> {
private:
    Poly p;
    const Poly var;
    const std::string varname;
    const unsigned prec;
public:
    inline SeriesVisitor(const Poly var_, const std::string varname_, const unsigned prec_)
            : var(var_), varname(varname_), prec(prec_) {

    }
    RCP<const Series> series(const RCP<const Basic> &x) {
        return make_rcp<Series>(apply(x), varname, prec);
    }

    Poly apply(const RCP<const Basic> &x) {
        x->accept(*this);
        Poly temp(std::move(p));
        return temp;
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
            temp = Series::mul(temp, apply(pow(term.first, term.second)), prec);
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
                p = Series::pow(p, sh, prec);
            } else if (sh == -1) {
                p = Series::series_invert(p, var, prec);
            } else {
                p = Series::series_invert(Series::pow(p, -sh, prec), var, prec);
            }

        } else if (is_a<Rational>(*exp)) {
            const Rational &rat = (static_cast<const Rational &>(*exp));
            const mpz_class &expnumz = rat.i.get_num();
            const mpz_class &expdenz = rat.i.get_den();
            if (not expnumz.fits_sint_p() or not expdenz.fits_sint_p())
                throw std::runtime_error("series rational power exponent size");
            const int num = expnumz.get_si();
            const int den = expdenz.get_si();
            base->accept(*this);
            const Poly proot(Series::series_nthroot(apply(base), den, var, prec));
            if (num == 1) {
                p = proot;
            } else if (num > 0) {
                p = Series::pow(proot, num, prec);
            } else if (num == -1) {
                p = Series::series_invert(proot, var, prec);
            } else {
                p = Series::series_invert(Series::pow(proot, -num, prec), var, prec);
            }
        } else if (eq(*E, *base)) {
            p = Series::series_exp(apply(exp), var, prec);
        } else {
            p = Series::series_exp(apply(exp) * Series::series_log(apply(base), var, prec), var, prec);
        }
    }

    void bvisit(const Series &x) {
        if (x.var_ != varname) {
            throw std::runtime_error("Multivariate Series not implemented");
        }
        if (x.degree_ < prec) {
            throw std::runtime_error("Series with lesser prec found");
        }
        p = x.p_;
    }
    void bvisit(const Integer &x) {
        p = Series::convert(x);
    }
    void bvisit(const Rational &x) {
        p = Series::convert(x);
    }
    void bvisit(const Complex &x) {
        p = Series::convert(x);
    }
    void bvisit(const RealDouble &x) {
        p = Series::convert(x);
    }
    void bvisit(const ComplexDouble &x) {
        p = Series::convert(x);
    }
#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x) {
        p = Series::convert(x);
    }
#endif
#ifdef HAVE_SYMENGINE_MPC
    void bvisit(const ComplexMPC &x) {
        p = Series::convert(x);
    }
#endif
    void bvisit(const Sin &x) {
        x.get_arg()->accept(*this);
        p = Series::series_sin(p, var, prec);
    }
    void bvisit(const Cos &x) {
        x.get_arg()->accept(*this);
        p = Series::series_cos(p, var, prec);
    }
    void bvisit(const Tan &x) {
        x.get_arg()->accept(*this);
        p = Series::series_tan(p, var, prec);
    }
    void bvisit(const Cot &x) {
        x.get_arg()->accept(*this);
        p = Series::series_cot(p, var, prec);
    }
    void bvisit(const Csc &x) {
        x.get_arg()->accept(*this);
        p = Series::series_csc(p, var, prec);
    }
    void bvisit(const Sec &x) {
        x.get_arg()->accept(*this);
        p = Series::series_sec(p, var, prec);
    }
    void bvisit(const Log &x) {
        x.get_arg()->accept(*this);
        p = Series::series_log(p, var, prec);
    }
    void bvisit(const ASin &x) {
        x.get_arg()->accept(*this);
        p = Series::series_asin(p, var, prec);
    }
    void bvisit(const ACos &x) {
        x.get_arg()->accept(*this);
        p = Series::series_acos(p, var, prec);
    }
    void bvisit(const ATan &x) {
        x.get_arg()->accept(*this);
        p = Series::series_atan(p, var, prec);
    }
    void bvisit(const Sinh &x) {
        x.get_arg()->accept(*this);
        p = Series::series_sinh(p, var, prec);
    }
    void bvisit(const Cosh &x) {
        x.get_arg()->accept(*this);
        p = Series::series_cosh(p, var, prec);
    }
    void bvisit(const Tanh &x) {
        x.get_arg()->accept(*this);
        p = Series::series_tanh(p, var, prec);
    }
    void bvisit(const ASinh &x) {
        x.get_arg()->accept(*this);
        p = Series::series_asinh(p, var, prec);
    }
    void bvisit(const ATanh &x) {
        x.get_arg()->accept(*this);
        p = Series::series_atanh(p, var, prec);
    }
    void bvisit(const LambertW &x) {
        x.get_arg()->accept(*this);
        p = Series::series_lambertw(p, var, prec);
    }
    void bvisit(const Symbol &x) {
        p = Series::var(x.get_name());
    }
    void bvisit(const Basic &x) {
        throw std::runtime_error("Not Implemented");
    }
};

} //SymEngine
#endif //SYMENGINE_SERIES_VISITOR_H
