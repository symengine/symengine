/**
 *  \file series.h
 *  Class for univariate series.
 *
 **/
#ifndef SYMENGINE_SERIES_H
#define SYMENGINE_SERIES_H

#include <list>

#include <symengine/polynomial.h>

namespace SymEngine {
//! UnivariateSeries Class
class UnivariateSeries : public Basic {
public:
    //! `var_` : Variable of the UnivariateSeries
    //! `poly_` : holds the UnivariatePolynomial of the series
    //! `prec_` : precision of the UnivariateSeries, i.e. self = poly + O(var^prec)
    // UnivariateSeries 1 + 2*x + x**2 + O(x**5) has dict_ = {{0, 1}, {1, 2}, {2, 1}} with var_ = "x" and prec_ = 5
    RCP<const Symbol> var_;
    RCP<const UnivariatePolynomial> poly_;
    unsigned int prec_;
public:
    IMPLEMENT_TYPEID(UNIVARIATESERIES)
    //! Constructor of UnivariateSeries class
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const RCP<const UnivariatePolynomial> &poly);
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int& precision, const unsigned int& max_exp, map_uint_mpz&& dict);
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_uint_mpz &dict);
    //! Constructor using a dense vector of mpz_class coefficients
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<mpz_class> &v);

    static RCP<const UnivariateSeries> create(const RCP<const Symbol> &var,
            const unsigned int &prec, const std::vector<mpz_class> &v) {
        return make_rcp<const UnivariateSeries>(var, prec, v);
    }

    //! \return true if canonical
    bool is_canonical(const UnivariatePolynomial&, const unsigned int &) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */

    bool operator==(const UnivariateSeries &o) const {
        return (eq(*var_, *o.var_) and poly_->__eq__(*o.poly_) and prec_ == o.prec_);
    }
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    std::string __str__() const;
    virtual vec_basic get_args() const { return {}; }
};


inline RCP<const UnivariateSeries> univariate_series(RCP<const Symbol> i,       unsigned int prec, const map_uint_mpz& dict)
{
    return make_rcp<const UnivariateSeries>(i, prec, dict);
}

RCP<const UnivariateSeries> add_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);
RCP<const UnivariateSeries> neg_uni_series (const UnivariateSeries& a);
RCP<const UnivariateSeries> sub_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);
RCP<const UnivariateSeries> mul_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);

template <typename Poly, typename Coeff, typename Series>
class SeriesBase : public Number {
public:
    Poly p_;
    std::string var_;
    long degree_;
public:
    inline SeriesBase(Poly p, std::string var, long degree) : p_(std::move(p)), var_(var), degree_(degree) {

    }
    inline virtual bool is_zero() const { return false; }

    inline virtual bool is_one() const { return false; }

    inline virtual bool is_minus_one() const { return false; }

    inline virtual bool is_negative() const { return false; }

    inline virtual bool is_positive() const { return false; }

    inline virtual bool __eq__(const Basic &o) const {
        return (is_a<Series>(o) and var_ == static_cast<const Series &>(o).var_ and
            p_ == static_cast<const Series &>(o).p_ and
            degree_ == static_cast<const Series &>(o).degree_);
    }

    virtual RCP<const Number> add(const Number &other) const {
        if (is_a<Series>(other)) {
            const Series &o = static_cast<const Series &>(other);
            long deg = std::min(degree_, o.degree_);
            if (var_ != o.var_) {
                throw std::runtime_error("Multivariate Series not implemented");
            }
            return make_rcp<Series>(p_ + o.p_, var_, deg);
        } else if (other.get_type_code() < Series::type_code_id){
            Poly p = Series::series(other.rcp_from_this(), var_, degree_)->p_;
            return make_rcp<Series>(p_ + p, var_, degree_);
        } else {
            return other.add(*this);
        }
    }

    virtual RCP<const Number> mul(const Number &other) const {
        if (is_a<Series>(other)) {
            const Series &o = static_cast<const Series &>(other);
            long deg = std::min(degree_, o.degree_);
            if (var_ != o.var_) {
                throw std::runtime_error("Multivariate Series not implemented");
            }
            return make_rcp<Series>(Series::mul(p_, o.p_, deg), var_, deg);
        } else if (other.get_type_code() < Series::type_code_id){
            Poly p = Series::series(other.rcp_from_this(), var_, degree_)->p_;
            return make_rcp<Series>(Series::mul(p_, p, degree_), var_, degree_);
        } else {
            return other.mul(*this);
        }
    }

    virtual RCP<const Number> pow(const Number &other) const {
        long deg = degree_;
        Poly p;
        if (is_a<Series>(other)) {
            const Series &o = static_cast<const Series &>(other);
            deg = std::min(deg, o.degree_);
            if (var_ != o.var_) {
                throw std::runtime_error("Multivariate Series not implemented");
            }
            p = o.p_;
        } else if (is_a<Integer>(other)) {
            return make_rcp<Series>(Series::pow(p_, var_, deg), var_, deg);
        } else if (other.get_type_code() < Series::type_code_id) {
            p = Series::series(other.rcp_from_this(), var_, degree_)->p_;
        } else {
            return other.rpow(*this);
        }
        p = Series::series_exp(p * Series::series_log(p_, Series::var(var_), deg),
                               Series::var(var_), deg);
        return make_rcp<Series>(p, var_, deg);
    }

    virtual RCP<const Number> rpow(const Number &other) const {
        if (other.get_type_code() < Series::type_code_id){
            Poly p = Series::series(other.rcp_from_this(), var_, degree_)->p_;
            p = Series::series_exp(p_ * Series::series_log(p, Series::var(var_), degree_),
                                   Series::var(var_), degree_);
            return make_rcp<Series>(p, var_, degree_);
        } else {
            throw std::runtime_error("Unknown type");
        }
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
            throw std::runtime_error("Series::series_invert: division by zero");
        if (s == 1)
            return Poly(1);
        const short ldeg = Series::ldegree(s);
        const Coeff co = Series::find_cf(s, var, ldeg);
        Poly p(1/co), ss = s;
        if (ldeg != 0) {
            ss = s * Series::pow(var, -ldeg, prec);
        }
        auto steps = step_list(prec);
        for (const auto step : steps) {
            p = Series::mul(2 - Series::mul(p, ss, step), p, step);
        }
        if (ldeg != 0) {
            return p * Series::pow(var, -ldeg, prec);
        } else {
            return p;
        }
    }

    static inline Poly series_reverse(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff co = Series::find_cf(s, var, 0);
        if (co != 0)
            throw std::runtime_error("reversion of series with constant term");
        const Coeff a = Series::find_cf(s, var, 1);
        if (a == 0)
            throw std::runtime_error("reversion of series with zero term of degree one");
        Poly r(var);
        r /= a;
        for (unsigned int i = 2; i < prec; i++) {
            Poly sp = Series::subs(s, var, r, i + 1);
            r -= Series::pow(var, i, i + 1) * Series::find_cf(sp, var, i) / a;
        }
        return r;
    }

    static inline Poly series_nthroot(const Poly &s, int n, const Poly &var, unsigned int prec) {
        if (n == 0)
            return Poly(1);
        if (n == 1)
            return s;
        if (n == -1)
            return Series::series_invert(s, var, prec);

        const short ldeg = Series::ldegree(s);
        if (ldeg % n != 0) {
            throw std::runtime_error("Puiseux series not implemented.");
        }
        Poly ss = s;
        if (ldeg != 0) {
            ss = s * Series::pow(var, -ldeg, prec);
        }
        Coeff ct = Series::find_cf(ss, var, 0);
        bool do_inv = false;
        if (n < 0) {
            n = -n;
            do_inv = true;
        }

        Coeff ctroot = Series::root(ct, n);
        Poly res_p(1), sn = ss / ct;
        auto steps = step_list(prec);
        for (const auto step : steps) {
            Poly t = Series::mul(Series::pow(res_p, n + 1, step), sn, step);
            res_p += (res_p - t) / n;
        }
        if (ldeg != 0) {
            res_p *= Series::pow(var, ldeg / n, prec);
        }
        if (do_inv)
            return res_p * ctroot;
        else
            return Series::series_invert(res_p, var, prec) * ctroot;
    }

    static inline Poly series_atan(const Poly &s, const Poly& var, unsigned int prec) {
        Poly res_p(0);
        if (s == 0)
            return res_p;

        if (s == var) {
            //! fast atan(x)
            short sign = 1;
            for (unsigned int i = 1; i < prec; i += 2, sign *= -1) {
                res_p += Series::pow(var, i, prec) * (Coeff(sign) / Coeff(i));
            }
            return res_p;
        }
        const Coeff c(Series::find_cf(s, var, 0));
        const Poly p(Series::pow(s, 2, prec - 1) + 1);
        res_p = Series::mul(Series::diff(s, var), Series::series_invert(p, var, prec - 1), prec - 1);

        if (c == 0) {
            // atan(s) = integrate(diff(s)*(1+s**2))
            return Series::integrate(res_p, var);
        } else {
            return Series::integrate(res_p, var) + Series::atan(c);
        }
    }

    static inline Poly series_tan(const Poly &s, const Poly& var, unsigned int prec) {
        Poly res_p(0), ss = s;
        const Coeff c(Series::find_cf(s, var, 0));
        if (c != 0) {
            ss = s - c;
        }

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

        auto steps = step_list(prec);
        for (const auto step : steps) {
            Poly t = Series::pow(res_p, 2, step) + 1;
            res_p += Series::mul(ss - Series::series_atan(res_p, var, step), t, step);
        }

        if (c == 0) {
            return res_p;
        } else {
            return Series::mul(res_p + Series::tan(c), Series::series_invert(1 + res_p * (-Series::tan(c)), var, prec), prec);
        }
    }

    static inline Poly series_cot(const Poly &s, const Poly& var, unsigned int prec) {
        return Series::series_invert(Series::series_tan(s, var, prec), var, prec);
    }

    static inline Poly series_sin(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));

        if (c != 0) {
            const Poly t = s - c;
            return Series::sin(c)*Series::series_cos(t, var, prec) + Series::cos(c)*Series::series_sin(t, var, prec);
        }
        //! fast sin(x)
        Poly res_p(0);
        Coeff prod(1);
        for (unsigned int i = 0; i < prec / 2; i++) {
            const short j = 2 * i + 1;
            if (i != 0)
                prod /= 1 - j;
            prod /= j;
            res_p += Series::pow(s, j, prec) * prod;
        }
        return res_p;

//        if (c == 0) {
//            // return 2*t/(1+t**2)
//            const Poly t(Series::series_tan(s / 2, var, prec));     // t = tan(s/2);
//            const Poly t2(Series::pow(t, 2, prec));
//            return Series::series_invert(t2 + 1, var, prec) * t * 2;
//        } else {
//            const Poly t(Series::series_tan((s - c) / 2, var, prec));     // t = tan(s/2);
//            const Poly t2(Series::pow(t, 2, prec));
//            // return sin(c)*cos(s) + cos(c)*sin(s)
//            return (-Series::sin(c)) * (t2 - 1) * Series::series_invert(t2 + 1, var, prec)
//                + (Series::cos(c) * 2) * t * Series::series_invert(t2 + 1, var, prec);
//        }
    }

    static inline Poly series_csc(const Poly &s, const Poly& var, unsigned int prec) {
        return Series::series_invert(Series::series_sin(s, var, prec), var, prec);
    }

    static inline Poly series_asin(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));

        // asin(s) = integrate(sqrt(1/(1-s**2))*diff(s))
        const Poly t(1 - Series::pow(s, 2, prec - 1));
        const Poly res_p(Series::integrate(Series::diff(s, var) * Series::series_nthroot(t, -2, var, prec - 1), var));

        if (c != 0) {
            return res_p + Series::asin(c);
        } else {
            return res_p;
        }
    }

    static inline Poly series_acos(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));
        return Series::acos(c) - series_asin(s - c, var, prec);
    }

    static inline Poly series_cos(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));
        if (c != 0) {
            const Poly t = s - c;
            return Series::cos(c)*Series::series_cos(t, var, prec) - Series::sin(c)*Series::series_sin(t, var, prec);
        }
        Poly res_p(1);
        Coeff prod(1);
        for (unsigned int i = 1; i <= prec / 2; i++) {
            const short j = 2 * i;
            if (i != 0)
                prod /= 1 - j;
            prod /= j;
            res_p += Series::pow(s, j, prec) * prod;
        }
        return res_p;
//
//        if (c == 0) {
//            // return (1-t**2)/(1+t**2)
//            const Poly t(Series::series_tan(s / 2, var, prec));     // t = tan(s/2);
//            const Poly t2(Series::pow(t, 2, prec));
//            return Series::series_invert(t2 + 1, var, prec) * ((t2 - 1) * -1);
//        } else {
//            const Poly t(Series::series_tan((s - c)/ 2, var, prec));     // t = tan(s/2);
//            const Poly t2(Series::pow(t, 2, prec));
//            // return cos(c)*cos(s) - sin(c)*sin(s)
//            return (-Series::cos(c)) * (t2 - 1) * Series::series_invert(t2 + 1, var, prec)
//                - Series::sin(c) * 2 * t * Series::series_invert(t2 + 1, var, prec);
//        }
    }

    static inline Poly series_sec(const Poly &s, const Poly& var, unsigned int prec) {
        return Series::series_invert(Series::series_cos(s, var, prec), var, prec);
    }

    static inline Poly series_log(const Poly &s, const Poly& var, unsigned int prec) {
        Poly res_p(0);
        if (s == 1)
            return res_p;
        if (s == var + 1) {
            //! fast log(1+x)
            for (unsigned int i = 1; i < prec; i++) {
                res_p += Series::pow(var, i, i + 1) * Coeff(((i % 2) == 0) ? -1 : 1) / Coeff(i);
            }
            return res_p;
        }

        const Coeff c(Series::find_cf(s, var, 0));
        res_p = Series::mul(Series::diff(s, var), Series::series_invert(s, var, prec), prec - 1);
        res_p = Series::integrate(res_p, var);

        if (c != 1) {
            res_p += Series::log(c);
        }
        return res_p;
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
                res_p += Series::pow(var, i, prec) * coef;
            }
            return res_p;
        }

        const Coeff c(Series::find_cf(s, var, 0));
        Poly t = s + 1;
        if (c != 0) {
            // exp(s) = exp(c)*exp(s-c)
            t = s - c + 1;
        }
        auto steps = step_list(prec);
        for (const auto step : steps) {
            res_p = Series::mul(res_p, t - Series::series_log(res_p, var, step), step);
        }
        if (c != 0) {
            return res_p * Series::exp(c);
        } else {
            return res_p;
        }
    }

    static inline Poly series_lambertw(const Poly &s, const Poly& var, unsigned int prec) {
        if (Series::find_cf(s, var, 0) != 0)
            throw std::logic_error("lambertw(const) not Implemented");

        Poly p1(0);

        auto steps = step_list(prec);
        for (const auto step : steps) {
            const Poly e(Series::series_exp(p1, var, step));
            const Poly p2(Series::mul(e, p1, step) - s);
            const Poly p3(Series::series_invert(Series::mul(e, (p1 + 1), step), var, step));
            p1 -= Series::mul(p2, p3, step);
        }
        return p1;
    }

    static inline Poly series_sinh(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));
        const Poly p1(Series::series_exp(s-c, var, prec));
        const Poly p2(Series::series_invert(p1, var, prec));

        if (c == 0) {
            return (p1 - p2) / 2;
        } else {
            return Series::cosh(c) * (p1 - p2) / 2 + Series::sinh(c) * (p1 + p2) / 2;
        }
    }

    static inline Poly series_cosh(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));
        const Poly p1(Series::series_exp(s-c, var, prec));
        const Poly p2(Series::series_invert(p1, var, prec));

        if (c == 0) {
            return (p1 + p2) / 2;
        } else {
            return Series::cosh(c) * (p1 + p2) / 2 + Series::sinh(c) * (p1 - p2) / 2;
        }
    }

    static inline Poly series_atanh(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));
        const Poly p(1 - Series::pow(s, 2, prec - 1));
        const Poly res_p(Series::mul(Series::diff(s, var), Series::series_invert(p, var, prec - 1), prec - 1));

        if (c == 0) {
            return Series::integrate(res_p, var);
        } else {
            return Series::integrate(res_p, var) + Series::atanh(c);
        }
    }

    static inline Poly series_asinh(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));

        const Poly p(Series::series_nthroot(Series::pow(s, 2, prec - 1) + 1, 2, var, prec - 1));
        const Poly res_p(Series::diff(s, var) * Series::series_invert(p, var, prec - 1));

        if (c == 0) {
            return Series::integrate(res_p, var);
        } else {
            return Series::integrate(res_p, var) + Series::asinh(c);
        }
    }

    static inline Poly series_tanh(const Poly &s, const Poly& var, unsigned int prec) {
        const Coeff c(Series::find_cf(s, var, 0));
        Poly res_p(s);
        if (c != 0) {
            res_p -= c;
        }
        auto steps = step_list(prec);
        for (const auto step : steps) {
            const Poly p(s - Series::series_atanh(res_p, var, step));
            res_p += Series::mul(-p, Series::pow(res_p, 2, step) - 1, step);
        }
        if (c != 0) {
            return (res_p + Series::tanh(c)) * Series::series_invert(1 + Series::tanh(c) * res_p, var, prec);
        } else {
            return res_p;
        }
    }

    static inline Coeff sin(const Coeff& c) {
        throw std::runtime_error("sin(const) not implemented");
    }
    static inline Coeff cos(const Coeff& c) {
        throw std::runtime_error("cos(const) not implemented");
    }
    static inline Coeff tan(const Coeff& c) {
        throw std::runtime_error("tan(const) not implemented");
    }
    static inline Coeff asin(const Coeff& c) {
        throw std::runtime_error("asin(const) not implemented");
    }
    static inline Coeff acos(const Coeff& c) {
        throw std::runtime_error("acos(const) not implemented");
    }
    static inline Coeff atan(const Coeff& c) {
        throw std::runtime_error("atan(const) not implemented");
    }
    static inline Coeff sinh(const Coeff& c) {
        throw std::runtime_error("sinh(const) not implemented");
    }
    static inline Coeff cosh(const Coeff& c) {
        throw std::runtime_error("cosh(const) not implemented");
    }
    static inline Coeff tanh(const Coeff& c) {
        throw std::runtime_error("tanh(const) not implemented");
    }
    static inline Coeff asinh(const Coeff& c) {
        throw std::runtime_error("asinh(const) not implemented");
    }
    static inline Coeff atanh(const Coeff& c) {
        throw std::runtime_error("atanh(const) not implemented");
    }
    static inline Coeff exp(const Coeff& c) {
        throw std::runtime_error("exp(const) not implemented");
    }
    static inline Coeff log(const Coeff& c) {
        throw std::runtime_error("log(const) not implemented");
    }

    /*
     * int ldegree(Poly &s);
     * Coeff& find_cf(Poly &s, int n);
     * Poly var(const std::string &s);
     * Poly pow(Poly &s, int n);
     * Poly mul(Poly &a, Poly &b, unsigned prec);
     * Poly diff(Poly &a, Poly &var);
     * Poly integrate(Poly &a, Poly &var);
     * Coeff root(Coeff &c, unsigned n);
     * Poly subs(Poly &s, Poly &var, Poly& new, unsigned prec);
     * Poly/Coeff convert(Number &n)
     */
};

}  //SymEngine
#endif
