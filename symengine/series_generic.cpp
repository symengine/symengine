#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>
#include <symengine/derivative.cpp>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const UnivariateExprPolynomial poly) :
        SeriesBase(std::move(poly), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const unsigned int &max, map_uint_mpz&& dict) :
        SeriesBase(UnivariateExprPolynomial(univariate_polynomial(var, max, convert_map(dict))), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_uint_mpz& dict) :
        SeriesBase(convert_poly(std::move(dict)), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<mpz_class> &v) :
        SeriesBase(convert_vector(v), var->get_name(), precision) {}

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t, const std::string &x, unsigned int prec) {
    SeriesVisitor<UnivariateExprPolynomial, Expression, UnivariateSeries> visitor(UnivariateExprPolynomial(std::stoi(x)), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const {
    return p_.__hash__() + std::size_t(prec_ * 84728863L);
}

int UnivariateSeries::compare(const Basic &other) const {
    if (not is_a<UnivariateSeries>(other))
        throw std::domain_error("cannot compare with UnivariateSeries");
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return p_ == (o.p_);
}

RCP<const Basic> UnivariateSeries::as_basic() const {
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    umap_int_basic map;
    for (int i = 0; i <= degree_; i++) 
       map[i] = p_.get_univariate_poly()->dict_.at(i).get_basic();
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int deg) const {
    return p_.get_univariate_poly()->get_dict().at(deg).get_basic();
}

UnivariateExprPolynomial UnivariateSeries::var(const std::string &s) {
    return UnivariateExprPolynomial(std::stoi(s));
}

Expression UnivariateSeries::convert(const Number &x) {
    // throw std::runtime_error("Not Implemented");
    return Expression(x.rcp_from_this());
}

map_uint_Expr UnivariateSeries::convert_map(const map_uint_mpz &d) {
    map_uint_Expr dict;
    for (const auto &it : d)
        dict[it.first] = Expression(it.second.get_ui());
    return dict;
}

RCP<const UnivariatePolynomial> UnivariateSeries::convert_poly(const map_uint_mpz &d) {
    map_uint_Expr dict_trunc;
    unsigned int max = 0;
    for (const auto &it : d) {
        if (it.first < prec_) {
            if (max < it.first)
                max = it.first;
            dict_trunc[it.first] = it.second.get_ui();
        }
    }
    return univariate_polynomial(symbol(var_), max, std::move(dict_trunc));
}

RCP<const UnivariatePolynomial> UnivariateSeries::convert_vector(const std::vector<mpz_class> &v) {
    std::vector<Expression> vtrunc;
    for (const auto &it : v)
        if (it.get_si() < prec_) 
            vtrunc.push_back(it.get_si());
    return UnivariatePolynomial::create(symbol(var_), vtrunc);
}

unsigned UnivariateSeries::ldegree(const UnivariateExprPolynomial &s) {
    return s.get_univariate_poly()->get_dict().begin()->first;
}

UnivariateExprPolynomial UnivariateSeries::mul(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &r, unsigned prec) {
    // No prec mul
    return s * r;
}

UnivariateExprPolynomial UnivariateSeries::pow(const UnivariateExprPolynomial &s, int n, unsigned prec) {
    // No prec mul
    return pow_ex(Expression(s.get_basic()), Expression(n));
}

Expression UnivariateSeries::find_cf(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, unsigned deg) {
    return (s.get_univariate_poly()->get_dict()).at(deg);
}

Expression UnivariateSeries::root(Expression &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

UnivariateExprPolynomial UnivariateSeries::diff(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    return UnivariateExprPolynomial(Expression(DiffImplementation::diff(*(s.get_univariate_poly()), var.get_univariate_poly()->get_var())));
}

UnivariateExprPolynomial UnivariateSeries::integrate(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    throw std::runtime_error("Not Implemented");
}

UnivariateExprPolynomial UnivariateSeries::subs(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, const UnivariateExprPolynomial &r, unsigned prec) {
    map_basic_basic x{{r.get_basic(), var.get_basic()}};
    return UnivariateExprPolynomial(Expression(s.get_basic()->subs(x)));
}

Expression UnivariateSeries::sin(const Expression& c) {
    return sin(c.get_basic());
}

Expression UnivariateSeries::cos(const Expression& c) {
    return cos(c.get_basic());
}

Expression UnivariateSeries::tan(const Expression& c) {
    return tan(c.get_basic());
}

Expression UnivariateSeries::asin(const Expression& c) {
    return asin(c.get_basic());
}

Expression UnivariateSeries::acos(const Expression& c) {
    return acos(c.get_basic());
}

Expression UnivariateSeries::atan(const Expression& c) {
    return atan(c.get_basic());
}

Expression UnivariateSeries::sinh(const Expression& c) {
    return sinh(c.get_basic());
}

Expression UnivariateSeries::cosh(const Expression& c) {
    return cosh(c.get_basic());
}

Expression UnivariateSeries::tanh(const Expression& c) {
    return tanh(c.get_basic());
}

Expression UnivariateSeries::asinh(const Expression& c) {
    return asinh(c.get_basic());
}

Expression UnivariateSeries::atanh(const Expression& c) {
    return atanh(c.get_basic());
}

Expression UnivariateSeries::exp(const Expression& c) {
    return exp(c.get_basic());
}

Expression UnivariateSeries::log(const Expression& c) {
    return log(c.get_basic());
}

} // SymEngine
