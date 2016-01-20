#include <exception>
#include <algorithm>
#include <iterator>
#include <symengine/series_generic.h>
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
    poly_ = univariate_polynomial(var_, max, std::move(dict_trunc));
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<mpz_class> &v) :
        var_{var}, prec_{precision} {

    std::vector<mpz_class> vtrunc;
    std::copy_if(v.begin(), v.end(), std::back_inserter(vtrunc),
        [&](decltype(v[0]) i) { return i < prec_; } );
    poly_ = UnivariatePolynomial::create(var_, vtrunc);
}

bool UnivariateSeries::is_canonical(const UnivariatePolynomial& poly, const unsigned int &prec) const
{
    return true;
}

std::size_t UnivariateSeries::__hash__() const
{
    return poly_->hash() + std::size_t(prec_ * 84728863L);
}

bool UnivariateSeries::__eq__(const Basic &other) const
{
    if (not is_a<UnivariateSeries>(other))
        return false;
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return operator==(o);
}

int UnivariateSeries::compare(const Basic &other) const
{
    if (not is_a<UnivariateSeries>(other))
        throw std::domain_error("cannot compare with UnivariateSeries");
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return poly_->compare(*o.poly_);
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
            o << abs(it.second);
            continue;
        }
        if (abs(it.second) == 1)
            o << var_->get_name();
        else
            o << abs(it.second) << "*" << var_->get_name();
        if (it.first > 1)
            o << "**" << it.first;
    }
    if (o.str() != "0")
        o << " + O(" << var_->get_name() << "**" << prec_ << ")";
    return o.str();
}

RCP<const UnivariateSeries> add_uni_series (const UnivariateSeries& a, const UnivariateSeries& b)
{
    map_uint_mpz dict;
    SYMENGINE_ASSERT(a.var_->get_name() == b.var_->get_name())
    unsigned int minprec = (a.prec_ < b.prec_)? a.prec_ : b.prec_;
    for (const auto &it : a.poly_->dict_) {
        if (it.first >= minprec)
            break;
        dict[it.first] = it.second;
    }

    unsigned int max = 0;
    for (const auto &it : b.poly_->dict_) {
        if (it.first >= minprec)
            break;
        dict[it.first] += it.second;
        if (dict[it.first] != 0 and it.first > max)
            max = it.first;
    }
    return make_rcp<const UnivariateSeries>(a.var_, minprec, max, std::move(dict));
}

RCP<const UnivariateSeries> neg_uni_series (const UnivariateSeries& a)
{
    return make_rcp<const UnivariateSeries>(a.var_, a.prec_, std::move(neg_uni_poly(*a.poly_)));
}

RCP<const UnivariateSeries> sub_uni_series (const UnivariateSeries& a, const UnivariateSeries& b)
{
    return add_uni_series(a, *neg_uni_series(b));
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
                    mpz_addmul(dict[expsum].get_mpz_t(), ait.second.get_mpz_t(), bit.second.get_mpz_t());
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

} // SymEngine
