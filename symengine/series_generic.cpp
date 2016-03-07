#include <exception>
#include <algorithm>
#include <iterator>*/
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>
// #include <symengine/dict.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {
  /*
UnivariateSeries::UnivariateSeries(const s_coef sp, const std::string varname, const unsigned degree)
        : SeriesBase(std::move(sp), varname, degree) {
  
}
RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t, const std::string &x,
                                                         unsigned int prec) {
    // throw std::runtime_error("Not Implemented");
    SeriesVisitor<s_coef, Integer, UnivariateSeries> visitor(s_coef(std::stoi(x)), x, prec);
    return visitor.series(t);
}


UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const RCP<const UnivariateIntPolynomial> &poly) :
        var_{var}, poly_{std::move(poly)} , prec_{precision} {
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
    return p_.get_basic()->__cmp__(*s.p_.get_basic()); //__cmp__()*/
    /*if (not is_a<UnivariateSeries>(other))
        throw std::domain_error("cannot compare with UnivariateSeries");
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return poly_->compare(*o.poly_);*//*
}


    poly_ = univariate_int_polynomial(var_, std::move(dict));
}

s_coef UnivariateSeries::var(const std::string &s) {
    return s_coef(std::stoi(s));
}

    map_uint_mpz dict_trunc;
    unsigned int max = 0;
    std::copy_if(dict.begin(), dict.end(), std::inserter(dict_trunc, dict_trunc.end()),
        [&](const map_uint_mpz::value_type i)
        {
            if (i.first < prec_) {
                if (max < i.first)
                    max = i.first;
                return true;
            }
            return false;
        } );
    poly_ = univariate_int_polynomial(var_, std::move(dict_trunc));
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<integer_class> &v) :
        var_{var}, prec_{precision} {

    std::vector<integer_class> vtrunc;
    std::copy_if(v.begin(), v.end(), std::back_inserter(vtrunc),
        [&](decltype(v[0]) i) { return i < prec_; } );
    poly_ = UnivariateIntPolynomial::create(var_, vtrunc);
}


bool UnivariateSeries::is_canonical(const UnivariateIntPolynomial& poly, const unsigned int &prec) const
{
    return true;
}

RCP<const Basic> UnivariateSeries::get_coeff(int i) const {
    throw std::runtime_error("Not Implemented");*/
    /*mpq_class cl_rat(p_.find_cf({i}).get_mpq_view());
    cl_rat.canonicalize();
    RCP<const Basic> basic;
    if (cl_rat.get_den() == 1)
        basic = make_rcp<const Integer>(cl_rat.get_num());
    else
        basic = make_rcp<const Rational>(cl_rat);
    return std::move(basic);*//*
}

s_coef UnivariateSeries::mul(const s_coef &s, const s_coef &r, unsigned prec) {
    // No prec mul
    return s * r;
}

s_coef UnivariateSeries::pow(const s_coef &s, int n, unsigned prec) {
    // No prec mul
    return pow_ex(s, s_coef(n));
}

std::string UnivariateSeries::__str__() const
{
    std::ostringstream o;
    bool first = true;
    for (const auto& it : poly_->dict_) {
        if (it.second == 0)
            continue;
        if (first) {
            if (it.second < 0)
                o << "-";
        }
        else {
            if (it.second < 0)
                o << " - ";
            else
                o << " + ";
        }
        first = false;
        if (it.first == 0) {
            o << mp_abs(it.second);
            continue;
        }
        if (mp_abs(it.second) == 1)
            o << var_->get_name();
        else
            o << mp_abs(it.second) << "*" << var_->get_name();
        if (it.first > 1)
            o << "**" << it.first;
    }
    if (o.str() != "0")
        o << " + O(" << var_->get_name() << "**" << prec_ << ")";
    return o.str();
}

s_coef UnivariateSeries::find_cf(const s_coef &s, const s_coef &var, unsigned deg) {
    return coeff(s, var, Expression(deg));
}

s_coef UnivariateSeries::root(s_coef &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

RCP<const UnivariateSeries> neg_uni_series (const UnivariateSeries& a)
{
    return make_rcp<const UnivariateSeries>(a.var_, a.prec_, std::move(neg_poly(*a.poly_)));
}

s_coef UnivariateSeries::integrate(const s_coef &s, const s_coef &var) {
    throw std::runtime_error("Not Implemented");
}

RCP<const UnivariateSeries> mul_uni_series (const UnivariateSeries& a, const UnivariateSeries& b)
{
    map_uint_mpz dict;
    SYMENGINE_ASSERT(a.var_->get_name() == b.var_->get_name())
    const unsigned int minprec = (a.prec_ < b.prec_)? a.prec_ : b.prec_;
    unsigned int max = 0;
    for (const auto &ait : a.poly_->dict_) {
        const unsigned int aexp = ait.first;
        if (aexp < minprec) {
            for (const auto &bit : b.poly_->dict_) {
                const unsigned int expsum = aexp + bit.first;
                if (expsum < minprec)
                    mp_addmul(dict[expsum], ait.second, bit.second);
                else
                    break;
                if (expsum > max)
                     max = expsum;
            }
        }
        else
            break;
    }
    return make_rcp<const UnivariateSeries>(a.var_, minprec, max, std::move(dict));
}
 */
/*bool UnivariateSeries::is_canonical(const UnivariatePolynomial& poly, const unsigned int &prec) const
{
    return true;
}*/


} // SymEngine
