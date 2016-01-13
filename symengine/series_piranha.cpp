
#include <symengine/series_piranha.h>
#include <symengine/series_visitor.h>

namespace SymEngine {

URatPSeriesPiranha::URatPSeriesPiranha(pp_t p, const std::string varname, const unsigned degree)
        : SeriesBase(std::move(p), varname, degree) {

}
RCP<const URatPSeriesPiranha> URatPSeriesPiranha::series(const RCP<const Basic> &t, const std::string &x,
                                                         unsigned int prec) {
    SeriesVisitor<pp_t, piranha::rational, URatPSeriesPiranha> visitor(pp_t(x), x, prec);
    return visitor.series(t);
}

std::size_t URatPSeriesPiranha::__hash__() const {
    std::size_t seed = URATPSERIESPIRANHA;
    hash_combine(seed, p_.hash());
    hash_combine(seed, var_);
    hash_combine(seed, degree_);
    return seed;
}

int URatPSeriesPiranha::compare(const Basic &o) const {
    SYMENGINE_ASSERT(is_a<URatPSeriesPiranha>(o))
    const URatPSeriesPiranha &s = static_cast<const URatPSeriesPiranha &>(o);
    if (var_ != s.var_)
        return (var_ < s.var_) ? -1 : 1;
    if (degree_ != s.degree_)
        return (degree_ < s.degree_) ? -1 : 1;
    if (p_ == s.p_)
        return 0;
    return (p_.hash() < s.p_.hash()) ? -1 : 1;
}

piranha::integer URatPSeriesPiranha::convert(const Integer &x) {
    return piranha::integer(x.as_mpz().get_mpz_t());
}

piranha::rational URatPSeriesPiranha::convert(const mpq_class &x) {
    piranha::integer i1(x.get_num_mpz_t());
    piranha::integer i2(x.get_den_mpz_t());
    piranha::rational r(i1);
    r /= i2;
    return r;
}

pp_t URatPSeriesPiranha::var(const std::string &s) {
    return pp_t(s);
}

piranha::rational URatPSeriesPiranha::convert(const Rational &x) {
    return convert(x.as_mpq());
}

piranha::rational URatPSeriesPiranha::convert(const Number &x) {
    throw std::runtime_error("Not Implemented");
}

pp_t URatPSeriesPiranha::mul(const pp_t &s, const pp_t &r, unsigned prec) {
    pp_t::set_auto_truncate_degree(prec - 1);
    pp_t ret = s * r;
    pp_t::unset_auto_truncate_degree();
    return ret;
}

pp_t URatPSeriesPiranha::pow(const pp_t &s, int n, unsigned prec) {
    pp_t::set_auto_truncate_degree(prec - 1);
    pp_t::clear_pow_cache();
    pp_t ret = s.pow(n);
    pp_t::unset_auto_truncate_degree();
    return ret;
}

unsigned URatPSeriesPiranha::ldegree(const pp_t &s) {
    return s.ldegree();
}

piranha::rational URatPSeriesPiranha::find_cf(const pp_t &s, const pp_t &var, unsigned deg) {
    return s.find_cf({deg});
}

piranha::rational URatPSeriesPiranha::root(piranha::rational &c, unsigned n) {
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

pp_t URatPSeriesPiranha::diff(const pp_t &s, const pp_t &var) {
    return s.partial(var.get_symbol_set()[0].get_name());
}

pp_t URatPSeriesPiranha::integrate(const pp_t &s, const pp_t &var) {
    return s.integrate(var.get_symbol_set()[0].get_name());
}

pp_t URatPSeriesPiranha::subs(const pp_t &s, const pp_t &var, const pp_t &r, unsigned prec) {
    pp_t::set_auto_truncate_degree(prec - 1);
    pp_t ret = s.subs(var.get_symbol_set()[0].get_name(), r);
    pp_t::unset_auto_truncate_degree();
    return ret;
}

UPSeriesPiranha::UPSeriesPiranha(p_expr p, const std::string varname, const unsigned degree)
        : SeriesBase(std::move(p), varname, degree) {

}

RCP<const UPSeriesPiranha> UPSeriesPiranha::series(const RCP<const Basic> &t, const std::string &x,
                                                         unsigned int prec) {
    SeriesVisitor<p_expr, Expression, UPSeriesPiranha> visitor(p_expr(x), x, prec);
    return visitor.series(t);
}

std::size_t UPSeriesPiranha::__hash__() const {
    std::size_t seed = URATPSERIESPIRANHA;
    hash_combine(seed, p_.hash());
    hash_combine(seed, var_);
    hash_combine(seed, degree_);
    return seed;
}

int UPSeriesPiranha::compare(const Basic &o) const {
    SYMENGINE_ASSERT(is_a<UPSeriesPiranha>(o))
    const UPSeriesPiranha &s = static_cast<const UPSeriesPiranha &>(o);
    if (var_ != s.var_)
        return (var_ < s.var_) ? -1 : 1;
    if (degree_ != s.degree_)
        return (degree_ < s.degree_) ? -1 : 1;
    if (p_ == s.p_)
        return 0;
    return (p_.hash() < s.p_.hash()) ? -1 : 1;
}

p_expr UPSeriesPiranha::var(const std::string &s) {
    return p_expr(s);
}

Expression UPSeriesPiranha::convert(const Number &x) {
    return Expression(x.rcp_from_this());
}

p_expr UPSeriesPiranha::mul(const p_expr &s, const p_expr &r, unsigned prec) {
    p_expr::set_auto_truncate_degree(prec - 1);
    p_expr ret = s * r;
    p_expr::unset_auto_truncate_degree();
    return ret;
}

p_expr UPSeriesPiranha::pow(const p_expr &s, int n, unsigned prec) {
    p_expr::set_auto_truncate_degree(prec - 1);
    p_expr::clear_pow_cache();
    p_expr ret = s.pow(n);
    p_expr::unset_auto_truncate_degree();
    return ret;
}

unsigned UPSeriesPiranha::ldegree(const p_expr &s) {
    return s.ldegree();
}

Expression UPSeriesPiranha::find_cf(const p_expr &s, const p_expr &var, unsigned deg) {
    return s.find_cf({deg});
}

Expression UPSeriesPiranha::root(Expression &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

p_expr UPSeriesPiranha::diff(const p_expr &s, const p_expr &var) {
    return s.partial(var.get_symbol_set()[0].get_name());
}

p_expr UPSeriesPiranha::integrate(const p_expr &s, const p_expr &var) {
    return s.integrate(var.get_symbol_set()[0].get_name());
}

p_expr UPSeriesPiranha::subs(const p_expr &s, const p_expr &var, const p_expr &r, unsigned prec) {
    p_expr::set_auto_truncate_degree(prec - 1);
    p_expr ret = s.subs(var.get_symbol_set()[0].get_name(), r);
    p_expr::unset_auto_truncate_degree();
    return ret;
}

Expression UPSeriesPiranha::sin(const Expression& c) {
    return SymEngine::sin(c.get_basic());
}

Expression UPSeriesPiranha::cos(const Expression& c) {
    return SymEngine::cos(c.get_basic());
}

Expression UPSeriesPiranha::tan(const Expression& c) {
    return SymEngine::tan(c.get_basic());
}

Expression UPSeriesPiranha::asin(const Expression& c) {
    return SymEngine::asin(c.get_basic());
}

Expression UPSeriesPiranha::acos(const Expression& c) {
    return SymEngine::acos(c.get_basic());
}

Expression UPSeriesPiranha::atan(const Expression& c) {
    return SymEngine::atan(c.get_basic());
}

Expression UPSeriesPiranha::sinh(const Expression& c) {
    return SymEngine::sinh(c.get_basic());
}

Expression UPSeriesPiranha::cosh(const Expression& c) {
    return SymEngine::cosh(c.get_basic());
}

Expression UPSeriesPiranha::tanh(const Expression& c) {
    return SymEngine::tanh(c.get_basic());
}

Expression UPSeriesPiranha::asinh(const Expression& c) {
    return SymEngine::asinh(c.get_basic());
}

Expression UPSeriesPiranha::atanh(const Expression& c) {
    return SymEngine::atanh(c.get_basic());
}

Expression UPSeriesPiranha::exp(const Expression& c) {
    return SymEngine::exp(c.get_basic());
}

Expression UPSeriesPiranha::log(const Expression& c) {
    return SymEngine::log(c.get_basic());
}
}
