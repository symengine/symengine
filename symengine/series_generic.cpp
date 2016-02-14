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

s_coef UnivariateSeries::convert(const Integer &x) {
    // throw std::runtime_error("Not Implemented");
    return s_coef(x.as_int());
    //return RCP<Integer>(x.as_mpz());
}
s_coef UnivariateSeries::convert(const mpq_class &x) {
    throw std::runtime_error("Not Implemented");
    // s_coef i1(x.get_num_mpz_t());
    // return i1;
    // return x.from_mpq();
}

s_coef UnivariateSeries::var(const std::string &s) {
    return s_coef(std::stoi(s));
}

s_coef UnivariateSeries::convert(const Rational &x) {
    // throw std::runtime_error("Not Implemented");
    s_coef i1(x.get_num());
    s_coef i2(x.get_den());
    i1 /= i2;
    return i1;
    // return x;
}

s_coef UnivariateSeries::convert(const Number &x) {
    throw std::runtime_error("Not Implemented");
}

RCP<const Basic> UnivariateSeries::as_basic() const {
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    throw std::runtime_error("Not Implemented");
    /*umap_int_basic map;
    mpq_class gc;
    for (int n=0; n<degree_; n++) {
        const SymEngine::Integer fc(p_.get_coeff(n));
        if (not fc.is_zero()) {
            fmpq_get_mpq(gc.get_mpq_t(), fc._data().inner);
            gc.canonicalize();
            RCP<const Number> basic;
            if (gc.get_den() == 1)
                basic = integer(gc.get_num());
            else
                basic = Rational::from_mpq(gc);
            map[n] = basic;
        }
    }
    return map;*/
}

RCP<const Basic> UnivariateSeries::get_coeff(int i) const {
    throw std::runtime_error("Not Implemented");
    /*mpq_class cl_rat(p_.find_cf({i}).get_mpq_view());
    cl_rat.canonicalize();
    RCP<const Basic> basic;
    if (cl_rat.get_den() == 1)
        basic = make_rcp<const Integer>(cl_rat.get_num());
    else
        basic = make_rcp<const Rational>(cl_rat);
    return std::move(basic);*/
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
