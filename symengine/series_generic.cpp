#include <climits>
#include <exception>
#include <algorithm>
#include <exception>
#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/dict.h>
#include <symengine/series_visitor.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine
{

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t,
                                                     const std::string &x,
                                                     unsigned int prec)
{
    UnivariateExprPolynomial p(UnivariatePolynomial::create(symbol(x), {0, 1}));
    SeriesVisitor<UnivariateExprPolynomial, Expression, UnivariateSeries>
        visitor(std::move(p), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const
{
    return p_.get_univariate_poly()->hash()
           + std::size_t(get_degree() * 84728863L);
}

int UnivariateSeries::compare(const Basic &other) const
{
    SYMENGINE_ASSERT(is_a<UnivariateSeries>(other))
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return p_.get_basic()->__cmp__(*o.p_.get_basic());
}

RCP<const Basic> UnivariateSeries::as_basic() const
{
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const
{
    umap_int_basic map;
    for (const auto &it : p_.get_univariate_poly()->get_dict())
        if (it.second != 0)
            map[it.first] = it.second.get_basic();
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int deg) const
{
    if (p_.get_univariate_poly()->get_dict().count(deg) == 0)
        return zero;
    else
        return p_.get_univariate_poly()->get_dict().at(deg).get_basic();
}

UnivariateExprPolynomial UnivariateSeries::var(const std::string &s)
{
    return UnivariateExprPolynomial(
        UnivariatePolynomial::create(symbol(s), {0, 1}));
}

Expression UnivariateSeries::convert(const Basic &x)
{
    return Expression(x.rcp_from_this());
}

int UnivariateSeries::ldegree(const UnivariateExprPolynomial &s)
{
    return s.get_univariate_poly()->get_dict().begin()->first;
}

UnivariateExprPolynomial
UnivariateSeries::mul(const UnivariateExprPolynomial &a,
                      const UnivariateExprPolynomial &b, unsigned prec)
{
    map_int_Expr p;
    for (auto &it1 : a.get_univariate_poly()->get_dict()) {
        for (auto &it2 : b.get_univariate_poly()->get_dict()) {
            int exp = it1.first + it2.first;
            if (exp < (int)prec) {
                p[exp] += it1.second * it2.second;
            } else {
                break;
            }
        }
    }
    if (a.get_univariate_poly()->get_var()->get_name() == "")
        return UnivariateExprPolynomial(UnivariatePolynomial::from_dict(
            b.get_univariate_poly()->get_var(), std::move(p)));
    else
        return UnivariateExprPolynomial(UnivariatePolynomial::from_dict(
            a.get_univariate_poly()->get_var(), std::move(p)));
}

UnivariateExprPolynomial
UnivariateSeries::pow(const UnivariateExprPolynomial &base, int exp,
                      unsigned prec)
{
    if (exp < 0) {
        SYMENGINE_ASSERT(base.get_univariate_poly()->get_dict().size() == 1)
        map_int_Expr dict;
        dict[-(base.get_univariate_poly()->get_dict().begin()->first)]
            = 1 / base.get_univariate_poly()->get_dict().begin()->second;
        return pow(UnivariateExprPolynomial(univariate_polynomial(
                       base.get_univariate_poly()->get_var(), std::move(dict))),
                   -exp, prec);
    }
    if (exp == 0) {
        if (base == 0) {
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
    if (s.get_univariate_poly()->get_dict().count(deg) == 0)
        return Expression(0);
    else
        return (s.get_univariate_poly()->get_dict()).at(deg);
}

Expression UnivariateSeries::root(Expression &c, unsigned n)
{
    return pow_ex(c, 1 / Expression(n));
}

UnivariateExprPolynomial
UnivariateSeries::diff(const UnivariateExprPolynomial &s,
                       const UnivariateExprPolynomial &var)
{
    RCP<const Basic> p
        = s.get_univariate_poly()->diff(var.get_univariate_poly()->get_var());
    if (is_a<const UnivariatePolynomial>(*p))
        return UnivariateExprPolynomial(
            rcp_static_cast<const UnivariatePolynomial>(p));
    else
        throw std::runtime_error("Not a UnivariatePolynomial");
}

UnivariateExprPolynomial
UnivariateSeries::integrate(const UnivariateExprPolynomial &s,
                            const UnivariateExprPolynomial &var)
{
    map_int_Expr dict;
    for (auto &it : s.get_univariate_poly()->get_dict()) {
        if (it.first != -1) {
            dict.insert(std::pair<int, Expression>(it.first + 1,
                                                   it.second / (it.first + 1)));
        } else {
            throw std::runtime_error("Not Implemented");
        }
    }
    return UnivariateExprPolynomial(univariate_polynomial(
        var.get_univariate_poly()->get_var(), std::move(dict)));
}

UnivariateExprPolynomial
UnivariateSeries::subs(const UnivariateExprPolynomial &s,
                       const UnivariateExprPolynomial &var,
                       const UnivariateExprPolynomial &r, unsigned prec)
{
    UnivariateExprPolynomial result(
        r.get_univariate_poly()->get_var()->get_name());
    for (auto &i : s.get_univariate_poly()->get_dict())
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
    return find_cf(p_, MultivariateSeries::var(var_), deg).get_basic();
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
    int min = INT_MAX;
    for (auto bucket : s.get_poly()->dict_) {
        for(unsigned int i = 0; i < bucket.first.size(); i++)
            if (bucket.first[i] < min)
                min = bucket.first[i];
    }
    return min;
}

MultivariateExprPolynomial
MultivariateSeries::mul(const MultivariateExprPolynomial &a,
                      const MultivariateExprPolynomial &b, unsigned prec)
{
    umap_vec_expr d;
    set_sym s;
    vec_uint translator1;
    vec_uint translator2;
    unsigned int size = reconcile(translator1, translator2, s, a.get_poly()->vars_, b.get_poly()->vars_);
    for (auto bucket1 : a.get_poly()->dict_) {
        for (auto bucket2 : b.get_poly()->dict_) {
            vec_int target = int_vec_translate_and_add(bucket1.first, bucket2.first, translator1, translator2, size );
            //uses prec tor restrict first variable of a, if it exists.  Otherwise uses the first variable of b.  If both a and b have empty variable sets, then they are both constant,
            // so prec gives no restriction, i.e. restrictor = 0.
            unsigned int restrictor = 0;
            if (translator1.size() > 0) {
                restrictor = target[translator1[0]];
            } else if (translator2.size() > 0) {
                restrictor = target[translator2[0]];
            }
            if (restrictor < prec) {
                if (d.find(target) == d.end()) {
                    d.insert(std::pair<vec_int, Expression>(target, bucket1.second * bucket2.second));
                } else {
                    d.find(target)->second += bucket1.second * bucket2.second;
                }
            }
        }
    } 


    return MultivariateExprPolynomial(MultivariatePolynomial::from_dict(s,std::move(d)));
}

MultivariateExprPolynomial
MultivariateSeries::pow(const MultivariateExprPolynomial &base, int exp,
                      unsigned prec)
{
    if (exp < 0)
        return pow(MultivariateSeries::series_invert(base, MultivariateSeries::var(base.get_var()->get_name()), prec) , -exp, prec);
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
    //going to be very inefficient with unordered map
    unsigned int i = 0;
    bool is_outside = false; // if var is outside of variable set of s 
    for (auto variable : s.get_poly()->vars_) {
        if (0 == variable->compare(*(var.get_var())))
            break;
        if (-1 == variable->compare(*(var.get_var()))) {
            is_outside = true;
        }
        i++;
    }
    vec_int v;
    v.resize(s.get_poly()->vars_.size(),0);
    if (s.get_poly()->vars_.size() == i)
        is_outside = true;
    if (is_outside) {
        if(0 == deg) {
            if (s.get_poly()->dict_.size() == 0) {
                return Expression(0);
            } else if ((s.get_poly()->dict_.size() == 1) && (s.get_poly()->dict_.begin()->first == v) ) {
                return s.get_poly()->dict_.begin()->second;
            } else {
                return Expression(s.get_basic());
            }
        } else {
            return Expression(0);
        }
    }
    umap_vec_expr d;    
    for (auto bucket : s.get_poly()->dict_) {
        if (bucket.first[i] == deg) {
            vec_int exps = bucket.first;
            exps[i] = 0;
            d.insert(std::pair<vec_int, Expression>(exps, bucket.second));
        }
    }
    if (d.size() == 0) {
        return Expression(0);
    } else if ((d.size() == 1) && (d.begin()->first == v)) {
        return d.begin()->second;
    } else {
        return Expression(MultivariatePolynomial::from_dict(s.get_vars(), std::move(d)));
    }
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
    umap_vec_expr dict;
    set_sym vars;
    vec_uint translator1;
    unsigned int translator2;
    unsigned int size = reconcile(translator1, translator2, vars, s.get_vars(), var.get_var());
    for (auto &it : s.get_dict()) {
        vec_int v = translate(it.first, translator1, size);
        v[translator2]++;
        dict.insert(std::pair<vec_int, Expression>(v, it.second / (v[translator2])));
    }
    return MultivariateExprPolynomial(MultivariatePolynomial::from_dict(vars, std::move(dict))); 
}

MultivariateExprPolynomial
MultivariateSeries::subs(const MultivariateExprPolynomial &s,
                       const MultivariateExprPolynomial &var,
                       const MultivariateExprPolynomial &r, unsigned prec)
{
    MultivariateExprPolynomial 
    result(r.get_var());
    for (auto &i : s.get_dict()) {       
        result += i.second * pow(r, i.first[0], prec);
    }
    return result;
}

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
