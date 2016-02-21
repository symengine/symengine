#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>
#include <symengine/dict.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {

UnivariateSeries::UnivariateSeries(const SymEngine::UnivariateExprPolynomial sp, const std::string varname, const unsigned degree) :
    SeriesBase(std::move(sp), varname, degree) {
}

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t, const std::string &x,
                                                         unsigned int prec) {
    SeriesVisitor<SymEngine::UnivariateExprPolynomial, Integer, UnivariateSeries> visitor(SymEngine::UnivariateExprPolynomial(std::stoi(x)), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const {
    std::size_t seed = UNIVARIATESERIES;
    hash_combine(seed, &p_.get_basic());
    hash_combine(seed, var_);
    hash_combine(seed, degree_);
    return seed;
}

int UnivariateSeries::compare(const Basic &o) const {
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

SymEngine::UnivariateExprPolynomial UnivariateSeries::convert(const Integer &x) {
    return SymEngine::UnivariateExprPolynomial(x.as_int());
}
SymEngine::UnivariateExprPolynomial UnivariateSeries::convert(const mpq_class &x) {
    SymEngine::UnivariateExprPolynomial i1(mpz_get_si(x.get_num_mpz_t()));
    SymEngine::UnivariateExprPolynomial i2(mpz_get_si(x.get_den_mpz_t()));
    return i1/i2;
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::var(const std::string &s) {
    return SymEngine::UnivariateExprPolynomial(std::stoi(s));
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::convert(const Rational &x) {
    SymEngine::UnivariateExprPolynomial i1(x.get_num());
    SymEngine::UnivariateExprPolynomial i2(x.get_den());
    i1 /= i2;
    return i1;
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::convert(const Number &x) {
    throw std::runtime_error("Not Implemented");
}

RCP<const Basic> UnivariateSeries::as_basic() const {
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    umap_int_basic map;
    mpq_class gc;
    for (int n = 0; n < degree_; n++) {
        const SymEngine::Integer fc(n);//get_coeff(n));
        if (not fc.is_zero()) {
            mpq_class cl_rat(fc.first.get_mpq_view());
            //fmpq_get_mpq(gc.get_mpq_t(), fc._data().inner);
            gc.canonicalize();
            RCP<const Number> basic;
            if (gc.get_den() == 1)
                basic = integer(gc.get_num());
            else
                basic = Rational::from_mpq(gc);
            map[n] = basic;
        }
    }
    return map;
    /*
    umap_int_basic map;
    for (const auto &it : p_) {
        if (it.first != 0) {
            mpq_class cl_rat(it.first.get_mpq_view());
            cl_rat.canonicalize();
            RCP<const Basic> basic;
            if (cl_rat.get_den() == 1)
                basic = make_rcp<const Integer>(cl_rat.get_num());
            else
                basic = make_rcp<const Rational>(cl_rat);
            map[it.second.degree()] = basic;
        }   
    } 
    return map;*/
}

RCP<const Basic> UnivariateSeries::get_coeff(int i) const {
/*    mpq_class cl_rat(p_.find_cf({i}).get_mpq_view());
    cl_rat.canonicalize();
    RCP<const Basic> basic;
    if (cl_rat.get_den() == 1)
        basic = make_rcp<const Integer>(cl_rat.get_num());
    else
        basic = make_rcp<const Rational>(cl_rat);
    return std::move(basic);*/
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::mul(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &r, unsigned prec) {
    // No prec mul
    return s * r;
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::pow(const SymEngine::UnivariateExprPolynomial &s, int n, unsigned prec) {
    // No prec mul
    return pow_ex(s, SymEngine::UnivariateExprPolynomial(n));
}

unsigned UnivariateSeries::ldegree(const SymEngine::UnivariateExprPolynomial &s) {
    throw std::runtime_error("Not Implemented");
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::find_cf(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &var, unsigned deg) {
    return coeff(s, var, Expression(deg));
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::root(SymEngine::UnivariateExprPolynomial &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::diff(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &var) {
    return diff(s.get_basic(), var.get_basic());
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::integrate(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &var) {
    throw std::runtime_error("Not Implemented");
}

SymEngine::UnivariateExprPolynomial UnivariateSeries::subs(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &var, const SymEngine::UnivariateExprPolynomial &r, unsigned prec) {
    map_basic_basic x{{r.get_basic(), var.get_basic()}};
    SymEngine::UnivariateExprPolynomial sb = s.get_basic()->subs(x);
    return sb;
}

bool UnivariateSeries::is_canonical(const UnivariatePolynomial& poly, const unsigned int &prec) const
{
    return true;
}

} // SymEngine
