#include <algorithm>
#include <exception>
#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine
{

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t,
                                                     const std::string &x,
                                                     unsigned int prec)
{
    UnivariateExprPolynomial p({{1, Expression(1)}});
    SeriesVisitor<UnivariateExprPolynomial, Expression, UnivariateSeries>
        visitor(std::move(p), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const
{
    return p_.__hash__() + std::size_t(get_degree() * 84728863L);
}

int UnivariateSeries::compare(const Basic &other) const
{
    SYMENGINE_ASSERT(is_a<UnivariateSeries>(other))
    const UnivariateSeries &o_ = static_cast<const UnivariateSeries &>(other);
    if (p_.size() != o_.p_.size())
        return (p_.size() < o_.p_.size()) ? -1 : 1;
    auto p = p_.get_dict().begin();
    auto o = o_.p_.get_dict().begin();
    for (; p != p_.get_dict().end(); ++p, ++o) {
        if (p->first != o->first)
            return (p->first < o->first) ? -1 : 1;
        if (p->second != o->second)
            return (p->second.get_basic()->__cmp__(*o->second.get_basic())) ? -1
                                                                            : 1;
    }
    return 0;
}

RCP<const Basic> UnivariateSeries::as_basic() const
{
    RCP<const Symbol> x = symbol(var_);
    umap_basic_num dict_;
    RCP<const Number> coeff;
    for (const auto &it : p_.get_dict()) {
        if (it.first != 0) {
            auto term = SymEngine::mul(
                it.second.get_basic(),
                pow_ex(Expression(x), Expression(it.first)).get_basic());
            RCP<const Number> coef;
            coef = zero;
            Add::coef_dict_add_term(outArg((coef)), dict_, one, term);
        } else
            coeff = rcp_static_cast<const Number>(it.second.get_basic());
    }
    return Add::from_dict(coeff, std::move(dict_));
}

umap_int_basic UnivariateSeries::as_dict() const
{
    umap_int_basic map;
    for (const auto &it : p_.get_dict())
        if (it.second != 0)
            map[it.first] = it.second.get_basic();
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int deg) const
{
    if (p_.get_dict().count(deg) == 0)
        return zero;
    else
        return p_.get_dict().at(deg).get_basic();
}

UnivariateExprPolynomial UnivariateSeries::var(const std::string &s)
{
    return UnivariateExprPolynomial({{1, Expression(1)}});
}

Expression UnivariateSeries::convert(const Basic &x)
{
    return Expression(x.rcp_from_this());
}

int UnivariateSeries::ldegree(const UnivariateExprPolynomial &s)
{
    return s.get_dict().begin()->first;
}

UnivariateExprPolynomial
UnivariateSeries::mul(const UnivariateExprPolynomial &a,
                      const UnivariateExprPolynomial &b, unsigned prec)
{
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
    return UnivariateExprPolynomial(p);
}

UnivariateExprPolynomial
UnivariateSeries::pow(const UnivariateExprPolynomial &base, int exp,
                      unsigned prec)
{
    if (exp < 0) {
        SYMENGINE_ASSERT(base.size() == 1)
        map_int_Expr dict;
        dict[-(base.get_dict().begin()->first)]
            = 1 / base.get_dict().begin()->second;
        return pow(UnivariateExprPolynomial(dict), -exp, prec);
    }
    if (exp == 0) {
        if (base == 0 or base.get_dict().size() == 0) {
            throw std::runtime_error("Error: 0**0 is undefined.");
        } else {
            return UnivariateExprPolynomial(1);
        }
    }

    UnivariateExprPolynomial x(base);
    UnivariateExprPolynomial y(1);
    while (exp > 1) {
        if (exp % 2 == 0) {
            x = mul(x, x, prec);
            exp /= 2;
        } else {
            y = mul(x, y, prec);
            x = mul(x, x, prec);
            exp = (exp - 1) / 2;
        }
    }
    return mul(x, y, prec);
}

Expression UnivariateSeries::find_cf(const UnivariateExprPolynomial &s,
                                     const UnivariateExprPolynomial &var,
                                     int deg)
{
    if (s.get_dict().count(deg) == 0)
        return Expression(0);
    else
        return (s.get_dict()).at(deg);
}

Expression UnivariateSeries::root(Expression &c, unsigned n)
{
    return pow_ex(c, 1 / Expression(n));
}

UnivariateExprPolynomial
UnivariateSeries::diff(const UnivariateExprPolynomial &s,
                       const UnivariateExprPolynomial &var)
{
    if (var.get_dict().size() == 1 and var.get_dict().at(1) == Expression(1)) {
        map_int_Expr d;
        for (const auto &p : s.get_dict()) {
            if (p.first != 0)
                d[p.first - 1] = p.second * p.first;
        }
        return UnivariateExprPolynomial(d);
    } else {
        return UnivariateExprPolynomial({{0, Expression(0)}});
    }
}

UnivariateExprPolynomial
UnivariateSeries::integrate(const UnivariateExprPolynomial &s,
                            const UnivariateExprPolynomial &var)
{
    map_int_Expr dict;
    for (auto &it : s.get_dict()) {
        if (it.first != -1) {
            dict.insert(std::pair<int, Expression>(it.first + 1,
                                                   it.second / (it.first + 1)));
        } else {
            throw std::runtime_error("Not Implemented");
        }
    }

    return UnivariateExprPolynomial(dict);
}

UnivariateExprPolynomial
UnivariateSeries::subs(const UnivariateExprPolynomial &s,
                       const UnivariateExprPolynomial &var,
                       const UnivariateExprPolynomial &r, unsigned prec)
{
    UnivariateExprPolynomial result({{1, Expression(1)}});

    for (auto &i : s.get_dict())
        result += i.second * pow(r, i.first, prec);

    return result;
}

Expression UnivariateSeries::sin(const Expression &c)
{
    return SymEngine::sin(c.get_basic());
}

Expression UnivariateSeries::cos(const Expression &c)
{
    return SymEngine::cos(c.get_basic());
}

Expression UnivariateSeries::tan(const Expression &c)
{
    return SymEngine::tan(c.get_basic());
}

Expression UnivariateSeries::asin(const Expression &c)
{
    return SymEngine::asin(c.get_basic());
}

Expression UnivariateSeries::acos(const Expression &c)
{
    return SymEngine::acos(c.get_basic());
}

Expression UnivariateSeries::atan(const Expression &c)
{
    return SymEngine::atan(c.get_basic());
}

Expression UnivariateSeries::sinh(const Expression &c)
{
    return SymEngine::sinh(c.get_basic());
}

Expression UnivariateSeries::cosh(const Expression &c)
{
    return SymEngine::cosh(c.get_basic());
}

Expression UnivariateSeries::tanh(const Expression &c)
{
    return SymEngine::tanh(c.get_basic());
}

Expression UnivariateSeries::asinh(const Expression &c)
{
    return SymEngine::asinh(c.get_basic());
}

Expression UnivariateSeries::atanh(const Expression &c)
{
    return SymEngine::atanh(c.get_basic());
}

Expression UnivariateSeries::exp(const Expression &c)
{
    return SymEngine::exp(c.get_basic());
}

Expression UnivariateSeries::log(const Expression &c)
{
    return SymEngine::log(c.get_basic());
}

} // SymEngine
