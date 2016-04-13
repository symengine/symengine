#include <exception>
#include <algorithm>
#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/dict.h>
#include <symengine/series_visitor.h>


using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine
{

UnivariateSeries::UnivariateSeries(
    const RCP<const Symbol> &var, const unsigned int &precision,
    const RCP<const UnivariateIntPolynomial> &poly)
    : var_{var}, poly_{std::move(poly)}, prec_{precision}
{
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var,
                                   const unsigned int &precision,
                                   const unsigned int &max, map_uint_mpz &&dict)
    : var_{var}, prec_{precision}
{

    poly_ = univariate_int_polynomial(var_, std::move(dict));
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var,
                                   const unsigned int &precision,
                                   const map_uint_mpz &dict)
    : var_{var}, prec_{precision}
{

    map_uint_mpz dict_trunc;
    unsigned int max = 0;
    std::copy_if(dict.begin(), dict.end(),
                 std::inserter(dict_trunc, dict_trunc.end()),
                 [&](const map_uint_mpz::value_type i) {
                     if (i.first < prec_) {
                         if (max < i.first)
                             max = i.first;
                         return true;
                     }
                     return false;
                 });
    poly_ = univariate_int_polynomial(var_, std::move(dict_trunc));
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var,
                                   const unsigned int &precision,
                                   const std::vector<integer_class> &v)
    : var_{var}, prec_{precision}
{

    std::vector<integer_class> vtrunc;
    std::copy_if(v.begin(), v.end(), std::back_inserter(vtrunc),
                 [&](decltype(v[0]) i) { return i < prec_; });
    poly_ = UnivariateIntPolynomial::create(var_, vtrunc);
}

bool UnivariateSeries::is_canonical(const UnivariateIntPolynomial &poly,
                                    const unsigned int &prec) const
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
    for (const auto &it : poly_->get_dict()) {
        if (it.second == 0)
            continue;
        if (first) {
            if (it.second < 0)
                o << "-";
        } else {
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

RCP<const UnivariateSeries> add_uni_series(const UnivariateSeries &a,
                                           const UnivariateSeries &b)
{
    map_uint_mpz dict;
    SYMENGINE_ASSERT(a.var_->get_name() == b.var_->get_name())
    unsigned int minprec = (a.prec_ < b.prec_) ? a.prec_ : b.prec_;
    for (const auto &it : a.poly_->get_dict()) {
        if (it.first >= minprec)
            break;
        dict[it.first] = it.second;
    }

    unsigned int max = 0;
    for (const auto &it : b.poly_->get_dict()) {
        if (it.first >= minprec)
            break;
        dict[it.first] += it.second;
        if (dict[it.first] != 0 and it.first > max)
            max = it.first;
    }
    return make_rcp<const UnivariateSeries>(a.var_, minprec, max,
                                            std::move(dict));
}

RCP<const UnivariateSeries> neg_uni_series(const UnivariateSeries &a)
{
    return make_rcp<const UnivariateSeries>(a.var_, a.prec_,
                                            neg_poly(*a.poly_));
}

RCP<const UnivariateSeries> sub_uni_series(const UnivariateSeries &a,
                                           const UnivariateSeries &b)
{
    return add_uni_series(a, *neg_uni_series(b));
}

RCP<const UnivariateSeries> mul_uni_series(const UnivariateSeries &a,
                                           const UnivariateSeries &b)
{
    map_uint_mpz dict;
    SYMENGINE_ASSERT(a.var_->get_name() == b.var_->get_name())
    const unsigned int minprec = (a.prec_ < b.prec_) ? a.prec_ : b.prec_;
    unsigned int max = 0;
    for (const auto &ait : a.poly_->get_dict()) {
        const unsigned int aexp = ait.first;
        if (aexp < minprec) {
            for (const auto &bit : b.poly_->get_dict()) {
                const unsigned int expsum = aexp + bit.first;
                if (expsum < minprec)
                    mp_addmul(dict[expsum], ait.second, bit.second);
                else
                    break;
                if (expsum > max)
                    max = expsum;
            }
        } else
            break;
    }
    return make_rcp<const UnivariateSeries>(a.var_, minprec, max,
                                            std::move(dict));
}

RCP<const MultivariateSeries> MultivariateSeries::series(const RCP<const Basic> &t,
                                                     const std::string &x,
                                                     unsigned int prec)
{
    MultivariateExprPolynomial p(MultivariatePolynomial::from_dict({symbol(x)}, { {{1}, 1} }));
    SeriesVisitor<MultivariateExprPolynomial, Expression, MultivariateSeries> visitor(std::move(p), x, prec);
    return visitor.series(t);
}

std::size_t MultivariateSeries::__hash__() const
{
    return p_.get_basic()->hash() + std::size_t(get_degree() * 84728863L);
}

int MultivariateSeries::compare(const Basic &other) const
{
    SYMENGINE_ASSERT(is_a<MultivariateSeries>(other))
    const MultivariateSeries &o = static_cast<const MultivariateSeries &>(other);
    return p_.get_basic()->__cmp__(*o.p_.get_basic());
}

RCP<const Basic> MultivariateSeries::as_basic() const
{
    return p_.get_basic();
}

umap_int_basic MultivariateSeries::as_dict() const
{
    umap_int_basic map;
    // for (int i = 0; i <= get_degree(); i++)
       // map[i] = p_.get_basic()->get_dict().at(i).get_basic();
    return map;
}

RCP<const Basic> MultivariateSeries::get_coeff(int deg) const
{
    // if (p_.get_basic()->get_dict().count(deg) == 0)
        return zero;
    // else
        // return p_.get_basic()->get_dict().at(deg).get_basic();
}

MultivariateExprPolynomial MultivariateSeries::var(const std::string &s)
{
    return MultivariateExprPolynomial(MultivariatePolynomial::from_dict({symbol(s)}, { {{1}, 1} }));
}

Expression MultivariateSeries::convert(const Basic &x)
{
    return Expression(x.rcp_from_this());
}

int MultivariateSeries::ldegree(const MultivariateExprPolynomial &s)
{
    //temp
    return 0;
    // return s.get_basic()->get_dict().begin()->first[];
}

MultivariateExprPolynomial
MultivariateSeries::mul(const MultivariateExprPolynomial &a,
                      const MultivariateExprPolynomial &b, unsigned prec)
{
    return a*b;
/*
    map_int_Expr p;

    for (auto &it1 : a.get_dict()) {

        for (auto &it2 : b.get_dict()) {
            int exp = it1.first + it2.first;
            if (exp < (int)prec) {
                p[exp] += it1.second * it2.second;
            } else {
                break;
            }
        }
    }
    if (a.get_basic()->var_->get_name() == "")
        return MultivariateExprPolynomial(MultivariatePolynomial::from_dict(b.get_basic()->var_, std::move(p)));
    else
        return MultivariateExprPolynomial(MultivariatePolynomial::from_dict(a.get_basic()->var_, std::move(p)));
*/}

MultivariateExprPolynomial
MultivariateSeries::pow(const MultivariateExprPolynomial &base, int exp,
                      unsigned prec)
{
    //can't do negative exp at this time
    if (exp < 0)
            throw std::runtime_error("Error: Exp is negative (Not implemented yet).");

    //     return MultivariateSeries::pow(MultivariateExprPolynomial::inverse(base), -exp, prec);
    if (exp == 0) {
        if (base == 0) {
            throw std::runtime_error("Error: 0**0 is undefined.");
        } else {
            return MultivariateExprPolynomial(1);
        }
    }

    MultivariateExprPolynomial x(base);
    MultivariateExprPolynomial y(1);
    while (exp > 1) {
        if (exp % 2 == 0) { 
            x = mul(x, x, prec);
            exp /= 2;
        } 
        else {
            y = mul(x, y, prec);
            x = mul(x, x, prec);
            exp = (exp - 1) / 2;
        }
    }
    return mul(x, y, prec);
}

Expression MultivariateSeries::find_cf(const MultivariateExprPolynomial &s,
                                     const MultivariateExprPolynomial &var,
                                     int deg)
{
    return Expression(integer(0));

    /*
    if (s.get_dict().count(deg) == 0)
        return Expression(0);
    else

        return (s.get_dict()).at(deg);*/
}

Expression MultivariateSeries::root(Expression &c, unsigned n)
{
    return pow_ex(c, 1/Expression(n));
}

MultivariateExprPolynomial
MultivariateSeries::diff(const MultivariateExprPolynomial &s,
                       const MultivariateExprPolynomial &var)
{
    RCP<const Basic> p = s.get_basic()->diff(var.get_var());
    if (is_a<const MultivariatePolynomial>(*p))
        return MultivariateExprPolynomial(rcp_static_cast<const MultivariatePolynomial>(p));
    else
        throw std::runtime_error("Not a MultivariatePolynomial");
}

MultivariateExprPolynomial
MultivariateSeries::integrate(const MultivariateExprPolynomial &s,
                            const MultivariateExprPolynomial &var)
{
    umap_uvec_expr dict;
    set_sym vars;
    vec_uint translator1;
    unsigned int translator2;
    unsigned int size = reconcile(translator1, translator2, vars, s.get_vars(), var.get_var());
    for (auto &it : s.get_dict()) {
        vec_uint v = translate(it.first, translator1, size);
        v[translator2]++;
        dict.insert(std::pair<vec_uint, Expression>(v, it.second / (v[translator2])));
    }
    return MultivariateExprPolynomial(MultivariatePolynomial::from_dict(vars, std::move(dict))); 
}
/*
MultivariateExprPolynomial
MultivariateSeries::subs(const MultivariateExprPolynomial &s,
                       const MultivariateExprPolynomial &var,
                       const MultivariateExprPolynomial &r, unsigned prec)
{
    MultivariateExprPolynomial 
    result(r.get_var());
    for (auto &i : s.get_dict()) {       
        result += i.second * pow(r, i.first, prec);
    }
    return result;
}
*/
Expression MultivariateSeries::sin(const Expression& c)
{
    return SymEngine::sin(c.get_basic());
}

Expression MultivariateSeries::cos(const Expression& c)
{
    return SymEngine::cos(c.get_basic());
}

Expression MultivariateSeries::tan(const Expression& c)
{
    return SymEngine::tan(c.get_basic());
}

Expression MultivariateSeries::asin(const Expression& c)
{
    return SymEngine::asin(c.get_basic());
}

Expression MultivariateSeries::acos(const Expression& c)
{
    return SymEngine::acos(c.get_basic());
}

Expression MultivariateSeries::atan(const Expression& c)
{
    return SymEngine::atan(c.get_basic());
}

Expression MultivariateSeries::sinh(const Expression& c)
{
    return SymEngine::sinh(c.get_basic());
}

Expression MultivariateSeries::cosh(const Expression& c)
{
    return SymEngine::cosh(c.get_basic());
}

Expression MultivariateSeries::tanh(const Expression& c)
{
    return SymEngine::tanh(c.get_basic());
}

Expression MultivariateSeries::asinh(const Expression& c)
{
    return SymEngine::asinh(c.get_basic());
}

Expression MultivariateSeries::atanh(const Expression& c)
{
    return SymEngine::atanh(c.get_basic());
}

Expression MultivariateSeries::exp(const Expression& c)
{
    return SymEngine::exp(c.get_basic());
}

Expression MultivariateSeries::log(const Expression& c)
{
    return SymEngine::log(c.get_basic());
}

} // SymEngine
