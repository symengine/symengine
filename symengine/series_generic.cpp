#include <exception>
#include <algorithm>
#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/dict.h>
#include <symengine/series_visitor.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {

UnivariateSeries::UnivariateSeries(const s_coef sp, const std::string varname, const unsigned degree)
//UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const sp_expr &poly)
        : SeriesBase(std::move(sp), varname, degree) {
  
}
RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t, const std::string &x,
                                                         unsigned int prec) {
    // throw std::runtime_error("Not Implemented");
    SeriesVisitor<s_coef, Integer, UnivariateSeries> visitor(s_coef(std::stoi(x)), x, prec);
    return visitor.series(t);
}

SymEngine::Integer UnivariateSeries::convert(const Integer &x) {
    throw std::runtime_error("Not Implemented");
    // return RCP<Integer>(x.as_mpz());
}
SymEngine::Rational UnivariateSeries::convert(const mpq_class &x) {
    throw std::runtime_error("Not Implemented");
    // return x.from_mpq();
}

s_coef UnivariateSeries::var(const std::string &s) {
    return s_coef(std::stoi(s));
}

SymEngine::Rational UnivariateSeries::convert(const Rational &x) {
    throw std::runtime_error("Not Implemented");
    // return x;
}
SymEngine::Rational UnivariateSeries::convert(const Number &x) {
    throw std::runtime_error("Not Implemented");
    // return x.as_mpz().from_mpq();
}

RCP<const Basic> UnivariateSeries::as_basic() const {
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    throw std::runtime_error("Not Implemented");
}

RCP<const Basic> UnivariateSeries::get_coeff(int) const {
    throw std::runtime_error("Not Implemented");
}

s_coef UnivariateSeries::mul(const s_coef &s, const s_coef &r, unsigned prec) {
    throw std::runtime_error("Not Implemented");
}

s_coef UnivariateSeries::pow(const s_coef &s, int n, unsigned prec) {
    throw std::runtime_error("Not Implemented");
}

unsigned UnivariateSeries::ldegree(const s_coef &s) {
    throw std::runtime_error("Not Implemented");
}

SymEngine::Rational UnivariateSeries::find_cf(const s_coef &s, const s_coef &var, unsigned deg) {
    throw std::runtime_error("Not Implemented");
}

SymEngine::Rational UnivariateSeries::root(SymEngine::Rational &c, unsigned n) {
    throw std::runtime_error("Not Implemented");
}

s_coef UnivariateSeries::diff(const s_coef &s, const s_coef &var) {
    throw std::runtime_error("Not Implemented");
}

s_coef UnivariateSeries::integrate(const s_coef &s, const s_coef &var) {
    throw std::runtime_error("Not Implemented");
}

s_coef UnivariateSeries::subs(const s_coef &s, const s_coef &var, const s_coef &r, unsigned prec) {
    throw std::runtime_error("Not Implemented");
}



bool UnivariateSeries::is_canonical(const UnivariatePolynomial& poly, const unsigned int &prec) const
{
    return true;
}

std::size_t UnivariateSeries::__hash__() const
{
    //return poly_->hash() + std::size_t(prec_ * 84728863L);
    std::size_t seed = UNIVARIATESERIES;
    hash_combine(seed, p_);
    hash_combine(seed, var_);
    hash_combine(seed, degree_);
    return seed;
}

int UnivariateSeries::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<UnivariateSeries>(o))
    const UnivariateSeries &s = static_cast<const UnivariateSeries &>(o);
    if (var_ != s.var_)
        return (var_ < s.var_) ? -1 : 1;
    if (degree_ != s.degree_)
        return (degree_ < s.degree_) ? -1 : 1;
    if (p_ == s.p_)
        return 0;
    //return p.compare(*s.p_);
    return p_.get_basic()->__cmp__(*s.p_.get_basic()); //__cmp__()
    /*if (not is_a<UnivariateSeries>(other))
        throw std::domain_error("cannot compare with UnivariateSeries");
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return poly_->compare(*o.poly_);*/
}



} // SymEngine
