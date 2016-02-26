#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>
#include <symengine/dict.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const UnivariateExprPolynomial poly) :
        SeriesBase(std::move(poly), var->get_name(), precision) {
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const unsigned int &max, map_uint_mpz&& dict) :
        SeriesBase(univariate_polynomial(var, max, convert_map(std::move(dict))), var->get_name(), precision) {
}

/*UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_uint_mpz& dict) : 
        SeriesBase(univariate_polynomial(var, var_{var}, prec_{precision} 
        {
    throw std::runtime_error("Not Implemented");
    map_uint_mpz dict_trunc;
    unsigned int max = 0;
    std::copy_if(dict.begin(), dict.end(), std::inserter(dict_trunc, dict_trunc.end()), [&](const map_uint_mpz::value_type i)
        {
            if (i.first < prec_) {
                if (max < i.first)
                    max = i.first;
                return true;
            }
            return false;
        } );
    poly_ = univariate_polynomial(var_, max, std::move(dict_trunc));
}*/

/*UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<mpz_class> &v) //:
        //var_{var}, prec_{precision} 
        {
    throw std::runtime_error("Not Implemented");
    std::vector<mpz_class> vtrunc;
    std::copy_if(v.begin(), v.end(), std::back_inserter(vtrunc),
        [&](decltype(v[0]) i) { return i < prec_; } );
    poly_ = UnivariatePolynomial::create(var_, vtrunc);
}*/

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
    throw std::runtime_error("Not Implemented");
    // return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    umap_int_basic map;
    for (int i = 0; i <= degree_; i++) {
       map[i] = p_.get_univariate_poly()->dict_.at(i).get_basic();
    } 
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int i) const {
    //mpq_class cl_rat(p_.find_cf({i}).get_mpq_view());
    //cl_rat.canonicalize();
    RCP<const Basic> basic;
    /*if (cl_rat.get_den() == 1)
        basic = make_rcp<const Integer>(cl_rat.get_num());
    else
        basic = make_rcp<const Rational>(cl_rat);
    */return std::move(basic);
}

UnivariateExprPolynomial UnivariateSeries::var(const std::string &s) {
    return UnivariateExprPolynomial(std::stoi(s));
}

Expression UnivariateSeries::convert(const Integer &x) {
    return Expression(x.as_int());
}

Expression UnivariateSeries::convert(const mpq_class &x) {
    Expression i1(mpz_get_si(x.get_num_mpz_t()));
    Expression i2(mpz_get_si(x.get_den_mpz_t()));
    return i1/i2;
}

Expression UnivariateSeries::convert(const Rational &x) {
    Expression i1(x.get_num());
    Expression i2(x.get_den());
    i1 /= i2;
    return i1;
}

Expression UnivariateSeries::convert(const Number &x) {
    throw std::runtime_error("Not Implemented");
}

map_uint_Expr UnivariateSeries::convert_map(const map_uint_mpz &&d) {
    map_uint_Expr dict;
    for (const auto &it : d) {
        Expression term = it.second()->get_ui();
        dict[it.first()] = term;
    }
    return dict;
}

unsigned UnivariateSeries::ldegree(const UnivariateExprPolynomial &s) {
    throw std::runtime_error("Not Implemented");
}

UnivariateExprPolynomial UnivariateSeries::mul(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &r, unsigned prec) {
    // No prec mul
    return s * r;
}

UnivariateExprPolynomial UnivariateSeries::pow(const UnivariateExprPolynomial &s, int n, unsigned prec) {
    // No prec mul
    throw std::runtime_error("Not Implemented");
    // return pow_ex(s, SymEngine::UnivariateExprPolynomial(n));
}

Expression UnivariateSeries::find_cf(const UnivariateExprPolynomial &s, const Expression &var, unsigned deg) {
    return coeff(Expression(s), var, Expression(deg));
}

Expression UnivariateSeries::root(UnivariateExprPolynomial &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

UnivariateExprPolynomial UnivariateSeries::diff(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    return s.get_univariate_poly()->diff(var.get_univariate_poly()->get_var());
}

UnivariateExprPolynomial UnivariateSeries::integrate(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    throw std::runtime_error("Not Implemented");
}

UnivariateExprPolynomial UnivariateSeries::subs(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, const UnivariateExprPolynomial &r, unsigned prec) {
    map_basic_basic x{{r.get_basic(), var.get_basic()}};
    SymEngine::UnivariateExprPolynomial sb = s.get_basic()->subs(x);
    return sb;
}

Expression UnivariateSeries::sin(const Expression& c) {
    return SymEngine::sin(c.get_basic());
}

Expression UnivariateSeries::cos(const Expression& c) {
    return SymEngine::cos(c.get_basic());
}

Expression UnivariateSeries::tan(const Expression& c) {
    return SymEngine::tan(c.get_basic());
}

Expression UnivariateSeries::asin(const Expression& c) {
    return SymEngine::asin(c.get_basic());
}

Expression UnivariateSeries::acos(const Expression& c) {
    return SymEngine::acos(c.get_basic());
}

Expression UnivariateSeries::atan(const Expression& c) {
    return SymEngine::atan(c.get_basic());
}

Expression UnivariateSeries::sinh(const Expression& c) {
    return SymEngine::sinh(c.get_basic());
}

Expression UnivariateSeries::cosh(const Expression& c) {
    return SymEngine::cosh(c.get_basic());
}

Expression UnivariateSeries::tanh(const Expression& c) {
    return SymEngine::tanh(c.get_basic());
}

Expression UnivariateSeries::asinh(const Expression& c) {
    return SymEngine::asinh(c.get_basic());
}

Expression UnivariateSeries::atanh(const Expression& c) {
    return SymEngine::atanh(c.get_basic());
}

Expression UnivariateSeries::exp(const Expression& c) {
    return SymEngine::exp(c.get_basic());
}

Expression UnivariateSeries::log(const Expression& c) {
    return SymEngine::log(c.get_basic());
}

} // SymEngine
