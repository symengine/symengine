/**
 *  \file expansion.h
 *  Series expansions of expressions using Piranha
 *
 **/

#include <symengine/symengine_config.h>

#ifdef HAVE_SYMENGINE_PIRANHA
#include <piranha/monomial.hpp>
#include <piranha/polynomial.hpp>
#include <piranha/mp_rational.hpp>
#include <piranha/mp_integer.hpp>
#endif

#include <stdexcept>
#include <memory>

#include <symengine/rational.h>
#include <symengine/number.h>
#include <symengine/symbol.h>
#include <symengine/functions.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/visitor.h>
#include <symengine/constants.h>

//!TODO:
//! - benchmark fast functions of short series (see ring_series.py)
//! - real coefficients
//! - Puiseux series (see ring_series.py)
//! - multivariate expressions (see ring_series.py)
//! - see "not implemented"
//! - use piranha::symbol for polynomial x
//! - make prec a reference and change it if necessary

namespace SymEngine {

#ifdef HAVE_SYMENGINE_PIRANHA
using pp_t = piranha::polynomial<piranha::rational, piranha::monomial<short>>;

//========== helpers ============
static inline RCP<const Number> prat2synum(const piranha::rational& p_rat)
{
    mpq_class cl_rat(p_rat.get_mpq_view());
    cl_rat.canonicalize();
    if (cl_rat.get_den() == 1)
        return make_rcp<const Integer>(cl_rat.get_num());
    return make_rcp<const Rational>(cl_rat);
}

static inline piranha::rational syrat2prat(const Rational& syrat)
{
    const piranha::integer i1(syrat.as_mpq().get_num().get_mpz_t());
    const piranha::integer i2(syrat.as_mpq().get_den().get_mpz_t());
    piranha::rational r(i1);
    r /= i2;
    return r;
}

static inline piranha::rational mpqc2prat(const mpq_class& the_mpqc)
{
    const piranha::integer i1(the_mpqc.get_num_mpz_t());
    const piranha::integer i2(the_mpqc.get_den_mpz_t());
    piranha::rational r(i1);
    r /= i2;
    return r;
}

static std::list<unsigned int> step_list(unsigned int prec)
{
    static std::list<unsigned int> steps;
    if (not steps.empty()) {
        if (*(steps.rbegin()) == prec)
            return steps;
        else
            steps.clear();
    }

    unsigned int tprec = prec;
    while (tprec > 4) {
        tprec = 2 + tprec/2;
        steps.push_front(tprec);
    }
    steps.push_front(2);
    steps.push_back(prec);
    return steps;
}

pp_t series_invert(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s == 0)
        throw std::runtime_error("series_invert: division by zero");
    if (s == 1)
        return pp_t{1};
    const short ldeg = s.ldegree();
    const piranha::rational& co = s.find_cf({ldeg});
    pp_t p(co.pow(-1)), ss = s;
    if (ldeg != 0)
        ss = s * var.pow(-ldeg);
    const auto& steps = step_list(prec);
    for (const auto step : steps) {
        pp_t::set_auto_truncate_degree(step);
        p -= p*(p*ss - 1);
    }
    if (ldeg != 0)
        return p * var.pow(-ldeg);
    else
        return p;
}

pp_t series_reverse(const pp_t& s, const pp_t& var, unsigned int prec)
{
    const piranha::rational& co = s.find_cf({0});
    if (co != 0)
        throw std::runtime_error("reversion of series with constant term");
    const piranha::rational& a = s.find_cf({1});
    if (a == 0)
        throw std::runtime_error("reversion of series with zero term of degree one");
    const std::string& varname(var.get_symbol_set()[0].get_name());
    pp_t r(var);
    r /= a;
    for (unsigned int i=2; i<prec; i++) {
        pp_t::set_auto_truncate_degree(i+1);
        const pp_t sp(s.subs(varname, r));
        r -= (var.pow(i) * sp.find_cf({i}))/a;
    }
    return r;
}

pp_t series_nthroot(const pp_t& s, int n, const pp_t& var, unsigned int prec)
{
    if (n == 0)
        return pp_t{1};
    if (n == 1)
        return s;
    if (n == -1)
        return series_invert(s, var, prec);

    const piranha::rational ct(s.find_cf({0}));
    if (ct == 0)
        throw std::runtime_error("series_nthroot needs a constant term");
    const mpq_class cl_rat(ct.get_mpq_view());
    mpq_class cl_root(cl_rat);
    bool res;
    bool do_inv = false;
    if (n < 0) {
        n = -n;
        do_inv = true;
    }
    if (mpz_cmp_ui(cl_rat.get_den_mpz_t(), 1) == 0) {
        // integer constant
        res = mpz_root(cl_root.get_num_mpz_t(), cl_rat.get_num_mpz_t(), n) != 0;
    }
    else {
        cl_root.canonicalize();
        RCP<const Rational> cterm = make_rcp<const Rational>(cl_root);
        RCP<const Number> cout;
        res = cterm->nth_root(outArg(cout), n);
        cl_root = dynamic_cast<const Rational&>(*cout).i;
    }
    if (not res)
        throw std::runtime_error("constant term is not an nth power");

    pp_t res_p{1};
    const pp_t sn(s/ct), ctroot(mpqc2prat(cl_root));
    const auto& steps = step_list(prec);
    for (const auto step : steps) {
        pp_t::set_auto_truncate_degree(step);
        res_p += (res_p - (res_p.pow(n+1) * sn)) / n;
    }
    if (do_inv)
        return res_p * ctroot;
    else
        return series_invert(res_p, var, prec) * ctroot;
}

//========= elementary functions ==========

pp_t series_atan(const pp_t& s, const pp_t& var, unsigned int prec)
{
    pp_t res_p{0};
    if (s == 0)
        return res_p;
    if (s.find_cf({0}) != 0)
        throw std::logic_error("atan(const) not Implemented");
    const std::string& varname(var.get_symbol_set()[0].get_name());
    pp_t var_p(varname);
    if (s == var_p) {
        //! fast atan(x)
        short sign = 1;
        for (unsigned int i=1; i<prec; i+=2, sign*=-1) {
            res_p += piranha::rational(sign, i) * var.pow(i);
        }
        return res_p;
    }
    const pp_t& p = s.pow(2) + 1;
    pp_t::set_auto_truncate_degree(prec-1);
    res_p = s.partial(varname) * series_invert(p, var, prec-1);
    pp_t::set_auto_truncate_degree(prec);
    return res_p.integrate(varname);
}

pp_t series_tan(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s.find_cf({0}) != 0)
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

    pp_t res_p{0};
    const auto& steps = step_list(prec);
    for (const auto step : steps) {
        pp_t::set_auto_truncate_degree(step);
        res_p += (s - series_atan(res_p, var, step)) * (res_p.pow(2) + 1);
    }
    return res_p;
}

pp_t series_cot(const pp_t& s, const pp_t& var, unsigned int prec)
{
    return series_invert(series_tan(s, var, prec), var, prec);
}

pp_t series_sin(const pp_t& s, const pp_t& var, unsigned int prec)
{
    pp_t res_p{0};

    if (s == var) {
        //! fast sin(x)
        piranha::integer prod{1};
        for (unsigned int i=0; i<prec/2; i++) {
            const short j = 2*i + 1;
            if (i != 0)
                prod *= 1-j;
            prod *= j;
            res_p += piranha::rational{1, prod} * var.pow(j);
        }
        return res_p;
    }

    if (s.find_cf({0}) != 0)
        throw std::logic_error("sin(const) not Implemented");

    const pp_t t(series_tan(s/2, var, prec));
    return series_invert(t.pow(2) + 1, var, prec) * t * 2;
}

pp_t series_csc(const pp_t& s, const pp_t& var, unsigned int prec)
{
    return series_invert(series_sin(s, var, prec), var, prec);
}

pp_t series_asin(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s.find_cf({0}) != 0)
        throw std::logic_error("asin(const) not Implemented");

    const std::string& varname(var.get_symbol_set()[0].get_name());
    const pp_t t((s.pow(2) - 1)*(-1));
    return (s.partial(varname) * series_nthroot(t, -2, var, prec-1)).integrate(varname);
}

pp_t series_cos(const pp_t& s, const pp_t& var, unsigned int prec)
{
    pp_t res_p{1};

    if (s == var) {
        //! fast cos(x)
        piranha::integer prod{1};
        for (unsigned int i=1; i<=prec/2; i++) {
            const short j = 2*i;
            prod *= 1-j;
            prod *= j;
            res_p += piranha::rational{1, prod} * var.pow(j);
        }
        return res_p;
    }

    if (s.find_cf({0}) != 0)
        throw std::logic_error("cos(const) not Implemented");

    const pp_t t(series_tan(s/2, var, prec).pow(2));
    return series_invert(t + 1, var, prec) * (-(t - 1));
}

pp_t series_sec(const pp_t& s, const pp_t& var, unsigned int prec)
{
    return series_invert(series_cos(s, var, prec), var, prec);
}

pp_t series_log(const pp_t& s, const pp_t& var, unsigned int prec)
{
    const std::string& varname(var.get_symbol_set()[0].get_name());
    pp_t res_p{0};
    if (s==1)
        return res_p;
    if (s-1 == var) {
        //! fast log(1+x)
        for (unsigned int i=1; i<prec; i++) {
            res_p += piranha::rational{((i%2)==0)?-1:1, i} * var.pow(i);
        }
        return res_p;
    }

    if (s.find_cf({0}) != 1)
        throw std::logic_error("log(const!=0) not Implemented");
    res_p = s.partial(varname);
    res_p *= series_invert(s, var, prec);
    return res_p.integrate(varname);
}

pp_t series_exp(const pp_t& s, const pp_t& var, unsigned int prec)
{
    pp_t res_p{1};
    if (s == 0)
        return res_p;

    if (s == var) {
        //! fast exp(x)
        piranha::rational coef{1, 1};
        for (unsigned int i=1; i<prec; i++) {
            coef /= i;
            res_p += coef * var.pow(i);
        }
        return res_p;
    }

    if (s.find_cf({0}) != 0)
        throw std::logic_error("exp(const) not Implemented");

    const auto& steps = step_list(prec);
    for (const auto step : steps) {
        pp_t::set_auto_truncate_degree(step);
        res_p += res_p*(s - series_log(res_p, var, step));
    }
    return res_p;
}

pp_t series_lambertw(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s.find_cf({0}) != 0)
        throw std::logic_error("lambertw(const) not Implemented");

    pp_t p1{0};

    const auto& steps = step_list(prec);
    for (const auto step : steps) {
        pp_t::set_auto_truncate_degree(step);
        const pp_t e(series_exp(p1, var, step));
        const pp_t p2(e * p1 - s);
        const pp_t p3(series_invert(e * (p1 + 1), var, step));
        p1 -= p2 * p3;
    }
    return p1;
}

pp_t series_sinh(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s.find_cf({0}) != 0)
        throw std::logic_error("sinh(const) not Implemented");

    const pp_t p1(series_exp(s, var, prec));
    const pp_t p2(series_invert(p1, var, prec));
    return (p1 - p2)/2;
}

pp_t series_cosh(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s.find_cf({0}) != 0)
        throw std::logic_error("cosh(const) not Implemented");

    const pp_t p1(series_exp(s, var, prec));
    const pp_t p2(series_invert(p1, var, prec));
    return (p1 + p2)/2;
}

pp_t series_atanh(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s.find_cf({0}) != 0)
        throw std::logic_error("atanh(const) not Implemented");

    const std::string& varname(var.get_symbol_set()[0].get_name());
    const pp_t& p(-(s.pow(2)) + 1);
    pp_t::set_auto_truncate_degree(prec-1);
    const pp_t res_p(s.partial(varname) * series_invert(p, var, prec-1));
    pp_t::set_auto_truncate_degree(prec);
    return res_p.integrate(varname);
}

pp_t series_asinh(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s.find_cf({0}) != 0)
        throw std::logic_error("asinh(const) not Implemented");

    const std::string& varname(var.get_symbol_set()[0].get_name());
    const pp_t& p(series_nthroot(s.pow(2) + 1, 2, var, prec));
    pp_t::set_auto_truncate_degree(prec-1);
    const pp_t res_p(s.partial(varname) * series_invert(p, var, prec-1));
    pp_t::set_auto_truncate_degree(prec);
    return res_p.integrate(varname);
}

pp_t series_tanh(const pp_t& s, const pp_t& var, unsigned int prec)
{
    if (s.find_cf({0}) != 0)
        throw std::logic_error("tanh(const) not Implemented");

    pp_t res_p{s};
    const auto& steps = step_list(prec);
    for (const auto step : steps) {
        pp_t::set_auto_truncate_degree(step);
        const pp_t p(s - series_atanh(res_p, var, step));
        res_p += -p * (res_p.pow(2) - 1);
    }
    return res_p;
}

using series_func_t = decltype(&series_tan);

static std::map<TypeID, series_func_t> funcmap;
void build_funcmap() {
    RCP<const Symbol> x = symbol("x");
    funcmap[tan(x)->get_type_code()] = &series_tan;
    funcmap[cot(x)->get_type_code()] = &series_cot;
    funcmap[csc(x)->get_type_code()] = &series_csc;
    funcmap[sec(x)->get_type_code()] = &series_sec;
    funcmap[sin(x)->get_type_code()] = &series_sin;
    funcmap[cos(x)->get_type_code()] = &series_cos;
    funcmap[log(x)->get_type_code()] = &series_log;
    funcmap[atan(x)->get_type_code()] = &series_atan;
    funcmap[asin(x)->get_type_code()] = &series_asin;
    funcmap[sinh(x)->get_type_code()] = &series_sinh;
    funcmap[cosh(x)->get_type_code()] = &series_cosh;
    funcmap[atanh(x)->get_type_code()] = &series_atanh;
    funcmap[asinh(x)->get_type_code()] = &series_asinh;
    funcmap[tanh(x)->get_type_code()] = &series_tanh;
    funcmap[lambertw(x)->get_type_code()] = &series_lambertw;
}

pp_t num2poly(const Number &num)
{
    if (is_a<Integer>(num)) {
        const Integer& ii = static_cast<const Integer&>(num);
        const piranha::integer bigint(ii.i.get_mpz_t());
        return pp_t(bigint);
    }
    throw std::runtime_error("Unhandled type in num2poly");
}

//=================================================================
//! Transform the Basic expression and return the series polynomial
pp_t _series(const RCP<const Basic> &ex, const RCP<const Symbol> &var, unsigned int prec)
{
    if (is_a<Integer>(*ex)) {
        const Integer& ii = static_cast<const Integer&>(*ex);
        piranha::integer i(ii.as_mpz().get_mpz_t());
        return pp_t(i);
    }

    if (is_a<Rational>(*ex)) {
        const Rational& rr = static_cast<const Rational&>(*ex);
        return pp_t(syrat2prat(rr));
    }

    if (is_a<Symbol>(*ex))
        return pp_t(dynamic_cast<const Symbol&>(*ex).get_name());

    const pp_t var_p(var->get_name());
    if (is_a_sub<Function>(*ex)) {
        const Function& fun = static_cast<const Function&>(*ex);
        const auto& args = fun.get_args();
        if (args.size() > 1)
            throw std::logic_error("Expansion of multivar functions not Implemented");
        if (funcmap.size() == 0)
            build_funcmap();
        const series_func_t& func = funcmap[fun.get_type_code()];
        if (func == nullptr)
            throw std::logic_error(std::string("No expansion for this function: ")
                                    + fun.__str__());
        const auto& series_inner = _series(args[0], var, prec);
        return func(std::move(series_inner), var_p, prec);
    }

    if (is_a<Add>(*ex)) {
        const Add& A = static_cast<const Add&>(*ex);
        pp_t p(num2poly(*(A.coef_)));
        for (const auto& term : A.dict_) {
            const pp_t t2nd(num2poly(*(term.second)));
            const pp_t t1st(_series(term.first, var, prec));
            p += t1st * t2nd;
        }
        return p;
    }

    if (is_a<Mul>(*ex)) {
        const Mul& M = static_cast<const Mul&>(*ex);
        pp_t p(num2poly(*(M.coef_)));
        for (const auto& term : M.dict_) {
            p *= _series(pow(term.first, term.second), var, prec);
        }
        return p;
    }

    if (is_a<Pow>(*ex)) {
        const Pow& P = static_cast<const Pow&>(*ex);
        const RCP<const Basic>& base = P.get_base(), exp = P.get_exp();
        if (is_a<Integer>(*exp)) {
            const Integer& ii = (static_cast<const Integer&>(*exp));
            if (not ii.i.fits_sint_p())
                throw std::runtime_error("series power exponent size");
            const int sh = ii.as_int();
            const pp_t pbase(_series(base, var, prec));
            if (sh == 1)
                return pbase;
            if (sh > 0)
                return pbase.pow(sh);
            else {
                if (sh == -1)
                    return series_invert(pbase, var_p, prec);
                else
                    return series_invert(pbase.pow(-sh), var_p, prec);
            }
        }
        if (is_a<Rational>(*exp)) {
            const Rational& rat = (static_cast<const Rational&>(*exp));
            const mpz_class& expnumz = rat.i.get_num();
            const mpz_class& expdenz = rat.i.get_den();
            if (not expnumz.fits_sint_p() or not expdenz.fits_sint_p())
                throw std::runtime_error("series rational power exponent size");
            const int num = expnumz.get_si();
            const int den = expdenz.get_si();
            const pp_t proot(series_nthroot(
                        _series(base, var, prec),
                        den, var_p, prec));
            if (num == 1)
                return proot;
            if (num > 0)
                return proot.pow(num);
            else {
                if (num == -1)
                    return series_invert(proot, var_p, prec);
                else
                    return series_invert(proot.pow(-num), var_p, prec);
            }
        }
        if (is_a<Constant>(*base) and E->__eq__(*base)) {
            const pp_t pexp(_series(exp, var, prec));
            return series_exp(pexp, var_p, prec);
        }
        const pp_t pexp(_series(exp, var, prec));
        const pp_t pbase(_series(base, var, prec));
        return series_exp(pexp*series_log(pbase, var_p, prec), var_p, prec);
    }

    std::string str("Unknown Basic type: ");
    str += ex->__str__();
    throw std::runtime_error(str);
}

umap_short_basic pp2vec(const pp_t &p, unsigned int prec)
{
    umap_short_basic map;
    for (const auto& it : p) {
        if (it.first != 0)
            map[it.second.degree()] = std::move(prat2synum(it.first));
    }
    return map;
}
#endif // HAVE_SYMENGINE_PIRANHA

umap_short_basic series(const RCP<const Basic> &ex, const RCP<const Symbol> &var, unsigned int prec)
{
#ifdef HAVE_SYMENGINE_PIRANHA
    if (prec == 0)
        return {{0, zero}};
    if (not has_symbol(*ex, var))
        return {{0, ex}};
    if (is_a<Symbol>(*ex))
        return {{0, ex}};

    pp_t::set_auto_truncate_degree(prec);
    return pp2vec(_series(ex, var, prec), prec);
#else
    throw std::runtime_error("Series expansion is supported only with Piranha");
#endif
}

umap_short_basic series_invfunc(const RCP<const Basic> &ex, const RCP<const Symbol> &var, unsigned int prec)
{
#ifdef HAVE_SYMENGINE_PIRANHA
    if (prec == 0)
        return {{0, zero}};
    if (is_a<Symbol>(*ex))
        return {{0, ex}};

    pp_t::set_auto_truncate_degree(prec);
    return pp2vec(series_reverse(_series(ex, var, prec), pp_t(var->get_name()), prec), prec);
#else
    throw std::runtime_error("Series expansion is supported only with Piranha");
#endif
}

} //SymEngine
