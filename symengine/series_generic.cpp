#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>
#include <symengine/dict.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const RCP<const UnivariatePolynomial> &poly) :
        var_{var}, poly_{std::move(poly)} , prec_{precision} {
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const unsigned int &max, map_uint_mpz&& dict) :
        var_{var}, prec_{precision} {

    poly_ = univariate_polynomial(var_, max, std::move(dict));
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_uint_mpz& dict) :
        var_{var}, prec_{precision} {

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
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<mpz_class> &v) :
        var_{var}, prec_{precision} {

    std::vector<mpz_class> vtrunc;
    std::copy_if(v.begin(), v.end(), std::back_inserter(vtrunc),
        [&](decltype(v[0]) i) { return i < prec_; } );
    poly_ = UnivariatePolynomial::create(var_, vtrunc);
}

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t, const std::string &x, unsigned int prec) {
    SeriesVisitor<UnivariateExprPolynomial, Expression, UnivariateSeries> visitor(UnivariateExprPolynomial(std::stoi(x)), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const {
    return p_->hash() + std::size_t(prec_ * 84728863L);
}

int UnivariateSeries::compare(const Basic &other) const {
    if (not is_a<UnivariateSeries>(other))
        throw std::domain_error("cannot compare with UnivariateSeries");
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return p_->compare(*(o.p_));
}

RCP<const Basic> UnivariateSeries::as_basic() const {
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    umap_int_basic map;
    for (int i = 0; i <= degree_; i++) {
       map[i] = p_.get_univariate_poly()->dict_.at(i);
    } 
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int i) const {
    mpq_class cl_rat(p_.find_cf({i}).get_mpq_view());
    cl_rat.canonicalize();
    RCP<const Basic> basic;
    if (cl_rat.get_den() == 1)
        basic = make_rcp<const Integer>(cl_rat.get_num());
    else
        basic = make_rcp<const Rational>(cl_rat);
    return std::move(basic);
}

UnivariateExprPolynomial UnivariateSeries::var(const std::string &s) {
    return UnivariateExprPolynomial(std::stoi(s));
}

Expression UnivariateSeries::convert(const Integer &x) {
    return UnivariateExprPolynomial(x.as_int());
}
Expression UnivariateSeries::convert(const mpq_class &x) {
    Expression i1(mpz_get_si(x.get_num_mpz_t()));
    Expression i2(mpz_get_si(x.get_den_mpz_t()));
    return i1/i2;
}

Expression UnivariateSeries::convert(const Rational &x) {
    UnivariateExprPolynomial i1(x.get_num());
    UnivariateExprPolynomial i2(x.get_den());
    i1 /= i2;
    return i1;
}

Expression UnivariateSeries::convert(const Number &x) {
    throw std::runtime_error("Not Implemented");
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
    return pow_ex(s, SymEngine::UnivariateExprPolynomial(n));
}

Expression UnivariateSeries::find_cf(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, unsigned deg) {
    return coeff(s, var, Expression(deg));
}

Expression UnivariateSeries::root(UnivariateExprPolynomial &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

UnivariateExprPolynomial UnivariateSeries::diff(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    return diff(s.get_basic(), var.get_basic());
}

UnivariateExprPolynomial UnivariateSeries::integrate(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    throw std::runtime_error("Not Implemented");
}

UnivariateExprPolynomial UnivariateSeries::subs(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, const UnivariateExprPolynomial &r, unsigned prec) {
    map_basic_basic x{{r.get_basic(), var.get_basic()}};
    SymEngine::UnivariateExprPolynomial sb = s.get_basic()->subs(x);
    return sb;
}

bool UnivariateSeries::is_canonical(const UnivariatePolynomial& poly, const unsigned int &prec) const
{
    return true;
}

} // SymEngine
