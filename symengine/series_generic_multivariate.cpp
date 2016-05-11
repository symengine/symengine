#include <climits>
#include <exception>
#include <algorithm>
#include <exception>
#include <iterator>
#include <symengine/series_generic_multivariate.h>
#include <symengine/dict.h>
#include <symengine/series_visitor.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine
{

MultivariateSeries::MultivariateSeries(const MultivariateExprPolynomial &sp,
                                       const std::string varname,
                                       const unsigned degree)
    : SeriesBase(std::move(sp), varname, degree)
{
    precs_.insert(
        std::pair<RCP<const Symbol>, unsigned int>(symbol(varname), degree));
    SYMENGINE_ASSERT(is_canonical(p_, var_, degree_, precs_))
}

MultivariateSeries::MultivariateSeries(const MultivariateExprPolynomial &sp,
                                       const std::string varname,
                                       const unsigned degree,
                                       const map_sym_uint &precs)
    : SeriesBase(std::move(sp), varname, degree), precs_(precs)
{
    SYMENGINE_ASSERT(is_canonical(p_, var_, degree_, precs));
}

bool MultivariateSeries::is_canonical(const MultivariateExprPolynomial p,
                                      const std::string var, const long degree,
                                      const map_sym_uint precs)
{
    // If the series only has a constant term, then degree = 0, var is irrelvant
    if (p.get_poly()->vars_.empty()) {
        /*  if (degree != 0)
              //return false;
          if (var != "")
              //return false;*/
    } else {
        // check that p is a polynomial in terms of symbol(var)
        if (p.get_poly()->vars_.find(symbol(var))
            == p.get_poly()->vars_.end()) {
            return false;
        }

        // check that degree is the precision of "var" in precs.
        if (precs.find(symbol(var)) != precs.end()) {
            if (precs.find(symbol(var))->second
                != static_cast<unsigned int>(degree))
                return false;
        } else {
            return false;
        }
    }

    // check that the precision in precs define precisions for all of the
    // symbols of p and that these precisions are less than the degree of those
    // variables in p
    for (RCP<const Symbol> s : p.get_poly()->vars_) {
        if (precs.find(s) == precs.end())
            return false;
        if (p.get_poly()->degrees_.find(s)->second > 0
            && precs.find(s)->second
                   < static_cast<unsigned int>(
                         p.get_poly()->degrees_.find(s)->second))
            return false;
    }

    return true;
}

RCP<const MultivariateSeries>
MultivariateSeries::series(const RCP<const Basic> &t, const std::string &x,
                           unsigned int prec)
{
    MultivariateExprPolynomial p(
        MultivariatePolynomial::from_dict({symbol(x)}, {{{1}, 1}}));
    SeriesVisitor<MultivariateExprPolynomial, Expression, MultivariateSeries>
        visitor(std::move(p), x, prec);
    return visitor.series(t);
}

std::size_t MultivariateSeries::__hash__() const
{
    std::size_t seed = MULTIVARIATESERIES;
    seed ^= p_.get_basic()->hash() + 0x9e3779b + (seed << 6) + (seed >> 2);
    for (auto bucket : precs_) {
        seed ^= bucket.second + 0x9e3779b + (seed << 6) + (seed >> 2);
    }

    return seed;
}

int MultivariateSeries::compare(const Basic &other) const
{
    SYMENGINE_ASSERT(is_a<MultivariateSeries>(other))
    const MultivariateSeries &o
        = static_cast<const MultivariateSeries &>(other);
    int cmp = p_.get_basic()->__cmp__(*o.p_.get_basic());
    if (cmp != 0)
        return cmp;

    return map_sym_uint_compare(precs_, o.precs_);
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

bool MultivariateSeries::__eq__(const Basic &o) const
{
    // ignore var_ and degree_, since thier information should be contained in
    // p_.vars_ and precs_
    return (is_a<MultivariateSeries>(o)
            and p_ == static_cast<const MultivariateSeries &>(o).p_
            and map_sym_uint_eq(precs_, static_cast<const MultivariateSeries &>(o).precs_));
}

RCP<const Number> MultivariateSeries::add(const Number &other) const
{
    if (is_a<MultivariateSeries>(other)) {
        const MultivariateSeries &o
            = static_cast<const MultivariateSeries &>(other);

        map_sym_uint precs = precs_;
        // minimise degrees in precs

        for (auto prec : o.precs_) {
            if (precs.find(prec.first) != precs.end()) {
                if (precs.find(prec.first)->second > prec.second)
                    precs.find(prec.first)->second = prec.second;
            } else {
                precs.insert(prec);
            }
        }

        unsigned int deg = precs.find(symbol(var_))->second;

        return make_rcp<MultivariateSeries>(
            MultivariateSeries::add(p_, o.p_, precs), var_, deg, precs);
    } else if (other.get_type_code() < MultivariateSeries::type_code_id) {
        MultivariateExprPolynomial p
            = MultivariateSeries::series(other.rcp_from_this(), var_, degree_)
                  ->p_;
        MultivariateSeries s(p, var_, degree_);
        return this->add(s);
        // return this->add(*MultivariateSeries::series(other.rcp_from_this(),
        // var_, degree_));
    } else {
        return other.add(*this);
    }
}

RCP<const Number> MultivariateSeries::mul(const Number &other) const
{
    if (is_a<MultivariateSeries>(other)) {
        const MultivariateSeries &o
            = static_cast<const MultivariateSeries &>(other);

        map_sym_uint precs = precs_;
        // minimise degrees in precs

        for (auto prec : o.precs_) {
            if (precs.find(prec.first) != precs.end()) {
                if (precs.find(prec.first)->second > prec.second)
                    precs.find(prec.first)->second = prec.second;
            } else {
                precs.insert(prec);
            }
        }

        unsigned int deg = precs.find(symbol(var_))->second;

        return make_rcp<MultivariateSeries>(
            MultivariateSeries::mul(p_, o.p_, precs), var_, deg, precs);
    } else if (other.get_type_code() < MultivariateSeries::type_code_id) {
        MultivariateExprPolynomial p
            = MultivariateSeries::series(other.rcp_from_this(), var_, degree_)
                  ->p_;
        MultivariateSeries s(p, var_, degree_);
        return this->mul(s);
        // return this->add(*MultivariateSeries::series(other.rcp_from_this(),
        // var_, degree_));
    } else {
        return other.mul(*this);
    }
}

RCP<const Basic> MultivariateSeries::get_coeff(int deg) const
{
    return find_cf(p_, MultivariateSeries::var(var_), deg).get_basic();
}

MultivariateExprPolynomial
MultivariateSeries::add(const MultivariateExprPolynomial &s,
                        const MultivariateExprPolynomial &r, map_sym_uint prec)
{
    return (s + r).truncate(prec);
}

MultivariateExprPolynomial
MultivariateSeries::mul(const MultivariateExprPolynomial &s,
                        const MultivariateExprPolynomial &r, map_sym_uint prec)
{
    return (s * r).truncate(prec);
}

MultivariateExprPolynomial MultivariateSeries::var(const std::string &s)
{
    return MultivariateExprPolynomial(
        MultivariatePolynomial::from_dict({symbol(s)}, {{{1}, 1}}));
}

Expression MultivariateSeries::convert(const Basic &x)
{
    return Expression(x.rcp_from_this());
}

int MultivariateSeries::ldegree(const MultivariateExprPolynomial &s)
{
    // if is a constant polynomial, should have ldegree 0
    if (s.get_poly()->vars_.size() == 0)
        return 0;
    int min = INT_MAX;
    for (auto bucket : s.get_poly()->dict_) {
        for (unsigned int i = 0; i < bucket.first.size(); i++)
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
    unsigned int size = reconcile(translator1, translator2, s,
                                  a.get_poly()->vars_, b.get_poly()->vars_);
    for (auto bucket1 : a.get_poly()->dict_) {
        for (auto bucket2 : b.get_poly()->dict_) {
            vec_int target = int_vec_translate_and_add(
                bucket1.first, bucket2.first, translator1, translator2, size);
            // uses prec tor restrict first variable of a, if it exists.
            // Otherwise uses the first variable of b.  If both a and b have
            // empty variable sets, then they are both constant,
            // so prec gives no restriction, i.e. restrictor = 0.
            unsigned int restrictor = 0;
            if (translator1.size() > 0 && target[translator1[0]] > 0) {
                restrictor = target[translator1[0]];
            } else if (translator2.size() > 0 && target[translator2[0]] > 0) {
                restrictor = target[translator2[0]];
            }
            if (restrictor < prec) {
                if (d.find(target) == d.end()) {
                    d.insert(std::pair<vec_int, Expression>(
                        target, bucket1.second * bucket2.second));
                } else {
                    d.find(target)->second += bucket1.second * bucket2.second;
                }
            }
        }
    }

    return MultivariateExprPolynomial(
        MultivariatePolynomial::from_dict(s, std::move(d)));
}

MultivariateExprPolynomial
MultivariateSeries::pow(const MultivariateExprPolynomial &base, int exp,
                        unsigned prec)
{
    if (base.get_dict().size() == 1) {
        vec_int v = base.get_dict().begin()->first;
        for (unsigned int i = 0; i < v.size(); i++)
            v[i] *= exp;

        if (!v.empty() && (v[0] > 0 && static_cast<unsigned int>(v[0]) >= prec))
            return MultivariateExprPolynomial(0);

        return MultivariateExprPolynomial(MultivariatePolynomial::from_dict(
            base.get_vars(),
            {{v, pow_ex(base.get_dict().begin()->second, exp)}}));
    }
    if (exp < 0) {
        if (base.get_vars().empty()) {
            if (base.get_dict().empty())
                throw std::runtime_error("Error: Division by zero");
            return MultivariateExprPolynomial(
                pow_ex(base.get_dict().begin()->second, exp));
        }
        return pow(MultivariateSeries::series_invert(
                       base,
                       MultivariateSeries::var(base.get_var()->get_name()),
                       prec),
                   -exp, prec);
    }
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
        } else {
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
    // going to be very inefficient with unordered map
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
    v.resize(s.get_poly()->vars_.size(), 0);
    if (s.get_poly()->vars_.size() == i)
        is_outside = true;
    if (is_outside) {
        if (0 == deg) {
            if (s.get_poly()->dict_.size() == 0) {
                return Expression(0);
            } else if ((s.get_poly()->dict_.size() == 1)
                       && (s.get_poly()->dict_.begin()->first == v)) {
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
        return Expression(
            MultivariatePolynomial::from_dict(s.get_vars(), std::move(d)));
    }
}

Expression MultivariateSeries::root(Expression &c, unsigned n)
{
    return pow_ex(c, 1 / Expression(n));
}

MultivariateExprPolynomial
MultivariateSeries::diff(const MultivariateExprPolynomial &s,
                         const MultivariateExprPolynomial &var)
{
    RCP<const Basic> p = s.get_basic()->diff(var.get_var());
    if (is_a<const MultivariatePolynomial>(*p))
        return MultivariateExprPolynomial(
            rcp_static_cast<const MultivariatePolynomial>(p));
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
    unsigned int size = reconcile(translator1, translator2, vars, s.get_vars(),
                                  var.get_var());
    for (auto &it : s.get_dict()) {
        vec_int v = translate(it.first, translator1, size);
        v[translator2]++;
        dict.insert(
            std::pair<vec_int, Expression>(v, it.second / (v[translator2])));
    }
    return MultivariateExprPolynomial(
        MultivariatePolynomial::from_dict(vars, std::move(dict)));
}

MultivariateExprPolynomial
MultivariateSeries::subs(const MultivariateExprPolynomial &s,
                         const MultivariateExprPolynomial &var,
                         const MultivariateExprPolynomial &r, unsigned prec)
{
    MultivariateExprPolynomial result(r.get_var());
    for (auto &i : s.get_dict()) {
        result += i.second * pow(r, i.first[0], prec);
    }
    return result;
}

Expression MultivariateSeries::sin(const Expression &c)
{
    return SymEngine::sin(c.get_basic());
}

Expression MultivariateSeries::cos(const Expression &c)
{
    return SymEngine::cos(c.get_basic());
}

Expression MultivariateSeries::tan(const Expression &c)
{
    return SymEngine::tan(c.get_basic());
}

Expression MultivariateSeries::asin(const Expression &c)
{
    return SymEngine::asin(c.get_basic());
}

Expression MultivariateSeries::acos(const Expression &c)
{
    return SymEngine::acos(c.get_basic());
}

Expression MultivariateSeries::atan(const Expression &c)
{
    return SymEngine::atan(c.get_basic());
}

Expression MultivariateSeries::sinh(const Expression &c)
{
    return SymEngine::sinh(c.get_basic());
}

Expression MultivariateSeries::cosh(const Expression &c)
{
    return SymEngine::cosh(c.get_basic());
}

Expression MultivariateSeries::tanh(const Expression &c)
{
    return SymEngine::tanh(c.get_basic());
}

Expression MultivariateSeries::asinh(const Expression &c)
{
    return SymEngine::asinh(c.get_basic());
}

Expression MultivariateSeries::atanh(const Expression &c)
{
    return SymEngine::atanh(c.get_basic());
}

Expression MultivariateSeries::exp(const Expression &c)
{
    return SymEngine::exp(c.get_basic());
}

Expression MultivariateSeries::log(const Expression &c)
{
    return SymEngine::log(c.get_basic());
}

} // SymEngine
