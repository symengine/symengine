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

MultivariateSeries::MultivariateSeries(const MultivariatePolynomialExpr &sp,
                                       const std::string varname,
                                       const unsigned degree)
    : SeriesBase(std::move(sp), varname, degree)
{
    precs_.insert(
        std::pair<RCP<const Basic>, unsigned int>(symbol(varname), degree));
    SYMENGINE_ASSERT(is_canonical(p_, var_, degree_, precs_))
}

MultivariateSeries::MultivariateSeries(const MultivariatePolynomialExpr &sp,
                                       const std::string varname,
                                       const unsigned degree,
                                       const map_basic_uint &precs)
    : SeriesBase(std::move(sp), varname, degree), precs_(precs)
{
    SYMENGINE_ASSERT(is_canonical(p_, var_, degree_, precs));
}

bool MultivariateSeries::is_canonical(const MultivariatePolynomialExpr p,
                                      const std::string var, const long degree,
                                      const map_basic_uint precs)
{
    // If the series only has a constant term, then degree = 0, var is irrelvant
    if (p.vars_.empty()) {
        /*  if (degree != 0)
              //return false;
          if (var != "")
              //return false;*/
    } else {
        // check that p is a polynomial in terms of symbol(var)
        if (p.vars_.find(symbol(var)) == p.vars_.end()) {
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
    for (RCP<const Basic> s : p.vars_) {
        if (precs.find(s) == precs.end())
            return false;
        if (p.degrees_.find(s)->second > 0
            && precs.find(s)->second
                   < static_cast<unsigned int>(p.degrees_.find(s)->second))
            return false;
    }

    return true;
}

RCP<const MultivariateSeries>
MultivariateSeries::series(const RCP<const Basic> &t, const std::string &x,
                           unsigned int prec)
{
    MultivariatePolynomialExpr p
        = MultivariatePolynomialExpr::create({symbol(x)}, {{{1}, 1}});
    SeriesVisitor<MultivariatePolynomialExpr, Expression, MultivariateSeries>
        visitor(std::move(p), x, prec);
    return visitor.series(t);
}

std::size_t MultivariateSeries::__hash__() const
{
    std::size_t seed = MULTIVARIATESERIES;
    seed ^= p_.__hash__() + 0x9e3779b + (seed << 6) + (seed >> 2);
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
    int cmp = p_.compare(o.p_);
    if (cmp != 0)
        return cmp;

    return unified_compare(precs_, o.precs_);
}

RCP<const Basic> MultivariateSeries::as_basic() const
{
    return p_.get_basic();
}

umap_int_basic MultivariateSeries::as_dict() const
{
    umap_int_basic map;
    // for (int i = 0; i <= get_degree(); i++)
    // map[i] = p_.get_basic()->dict_.at(i).get_basic();
    return map;
}

bool MultivariateSeries::__eq__(const Basic &o) const
{
    // ignore var_ and degree_, since thier information should be contained in
    // p_.vars_ and precs_
    return (is_a<MultivariateSeries>(o)
            and p_ == static_cast<const MultivariateSeries &>(o).p_
            and unified_eq(precs_, static_cast<const MultivariateSeries &>(o).precs_));
}

RCP<const Number> MultivariateSeries::add(const Number &other) const
{
    if (is_a<MultivariateSeries>(other)) {
        const MultivariateSeries &o
            = static_cast<const MultivariateSeries &>(other);

        map_basic_uint precs = precs_;
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
        MultivariatePolynomialExpr p
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

        map_basic_uint precs = precs_;
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
        MultivariatePolynomialExpr p
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

MultivariatePolynomialExpr
MultivariateSeries::add(const MultivariatePolynomialExpr &s,
                        const MultivariatePolynomialExpr &r,
                        map_basic_uint prec)
{
    return (s + r).truncate(prec);
}

MultivariatePolynomialExpr
MultivariateSeries::mul(const MultivariatePolynomialExpr &s,
                        const MultivariatePolynomialExpr &r,
                        map_basic_uint prec)
{
    return (s * r).truncate(prec);
}

MultivariatePolynomialExpr MultivariateSeries::var(const std::string &s)
{
    return MultivariatePolynomialExpr::create({symbol(s)}, {{{1}, 1}});
}

Expression MultivariateSeries::convert(const Basic &x)
{
    return Expression(x.rcp_from_this());
}

int MultivariateSeries::ldegree(const MultivariatePolynomialExpr &s)
{
    // if is a constant polynomial, should have ldegree 0
    if (s.vars_.size() == 0 || s.dict_.size() == 0)
        return 0;
    int min = INT_MAX;
    for (auto bucket : s.dict_) {
        for (unsigned int i = 0; i < bucket.first.size(); i++)
            if (bucket.first[i] < min)
                min = bucket.first[i];
    }
    return min;
}

MultivariatePolynomialExpr
MultivariateSeries::mul(const MultivariatePolynomialExpr &a,
                        const MultivariatePolynomialExpr &b, unsigned prec)
{
    umap_vec_expr d;
    set_basic s;
    vec_uint translator1;
    vec_uint translator2;
    unsigned int size
        = reconcile(translator1, translator2, s, a.vars_, b.vars_);
    for (auto bucket1 : a.dict_) {
        for (auto bucket2 : b.dict_) {
            vec_int target = translate_and_add<vec_int>(
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

    vec_basic vb;
    vb.insert(vb.begin(), s.begin(), s.end());
    return MultivariatePolynomialExpr::create(vb, std::move(d));
}

MultivariatePolynomialExpr
MultivariateSeries::pow(const MultivariatePolynomialExpr &base, int exp,
                        unsigned prec)
{
    if (base.dict_.size() == 1) {
        vec_int v = base.dict_.begin()->first;
        for (unsigned int i = 0; i < v.size(); i++)
            v[i] *= exp;

        if (!v.empty() && (v[0] > 0 && static_cast<unsigned int>(v[0]) >= prec))
            return MultivariatePolynomialExpr(0);

        vec_basic vb;
        vb.insert(vb.begin(), base.vars_.begin(), base.vars_.end());
        return MultivariatePolynomialExpr::create(
            vb, {{v, pow_ex(base.dict_.begin()->second, exp)}});
    }
    if (exp < 0) {
        if (base.vars_.empty()) {
            if (base.dict_.empty())
                throw std::runtime_error("Error: Division by zero");
            return MultivariatePolynomialExpr(
                pow_ex(base.dict_.begin()->second, exp));
        }
        return pow(
            MultivariateSeries::series_invert(
                base, MultivariateSeries::var((*base.vars_.begin())->__str__()),
                prec),
            -exp, prec);
    }
    if (exp == 0) {
        if (base == 0) {
            throw std::runtime_error("Error: 0**0 is undefined.");
        } else {
            return MultivariatePolynomialExpr(1);
        }
    }

    MultivariatePolynomialExpr x(base);
    MultivariatePolynomialExpr y(1);
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

Expression MultivariateSeries::find_cf(const MultivariatePolynomialExpr &s,
                                       const MultivariatePolynomialExpr &var,
                                       int deg)
{
    if (s.vars_.size() == 1 && eq(**s.vars_.begin(), **var.vars_.begin())) {
        // most common case in series_visitor.h, so handle quickly.
        auto ptr = s.dict_.find({deg});
        if (ptr == s.dict_.end()) {
            return Expression(0);
        } else {
            return ptr->second;
        }

    } else {
        // going to be very inefficient with unordered map
        unsigned int i = 0;
        bool is_outside = false; // if var is outside of variable set of s
        for (auto variable : s.vars_) {
            if (0 == variable->compare(*(*var.vars_.begin())))
                break;
            if (-1 == variable->compare(*(*var.vars_.begin()))) {
                is_outside = true;
            }
            i++;
        }
        vec_int v;
        v.resize(s.vars_.size(), 0);
        if (s.vars_.size() == i)
            is_outside = true;
        if (is_outside) {
            if (0 == deg) {
                if (s.dict_.size() == 0) {
                    return Expression(0);
                } else if ((s.dict_.size() == 1)
                           && (s.dict_.begin()->first == v)) {
                    return s.dict_.begin()->second;
                } else {
                    return Expression(s.get_basic());
                }
            } else {
                return Expression(0);
            }
        }
        Expression ans(0);
        for (auto bucket : s.dict_) {
            if (bucket.first[i] == deg) {
                vec_int exps = bucket.first;
                exps[i] = 0;
                Expression term(1);
                for (auto var : s.vars_) {
                    term *= Expression(SymEngine::pow(var, integer(exps[i])));
                }
                ans += term * bucket.second;
            }
        }
        return ans;
    }
}

Expression MultivariateSeries::root(Expression &c, unsigned n)
{
    return pow_ex(c, 1 / Expression(n));
}

MultivariatePolynomialExpr
MultivariateSeries::diff(const MultivariatePolynomialExpr &s,
                         const MultivariatePolynomialExpr &var)
{
    return MultivariatePolynomialExpr::diff(s, *var.vars_.begin());
}

MultivariatePolynomialExpr
MultivariateSeries::integrate(const MultivariatePolynomialExpr &s,
                              const MultivariatePolynomialExpr &var)
{
    umap_vec_expr dict;
    set_basic vars;
    vec_uint translator1;
    vec_uint translator2;
    unsigned int size
        = reconcile(translator1, translator2, vars, s.vars_, var.vars_);
    for (auto &it : s.dict_) {
        vec_int v = translate(it.first, translator1, size);
        v[translator2[0]]++;
        dict.insert(
            std::pair<vec_int, Expression>(v, it.second / (v[translator2[0]])));
    }

    vec_basic vb;
    vb.insert(vb.begin(), vars.begin(), vars.end());

    return MultivariatePolynomialExpr::create(vb, std::move(dict));
}

MultivariatePolynomialExpr
MultivariateSeries::subs(const MultivariatePolynomialExpr &s,
                         const MultivariatePolynomialExpr &var,
                         const MultivariatePolynomialExpr &r, unsigned prec)
{
    MultivariatePolynomialExpr result(*r.vars_.begin());
    for (auto &i : s.dict_) {
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

RCP<const MultivariateSeries> mult_series1(RCP<const Basic> func,
                                           const map_sym_uint &&precs)
{
    vec_basic vb;
    vec_sym vars;
    map_basic_uint precs2;
    umap_vec_expr dict;
    map_basic_basic b;
    RCP<const Integer> zero = integer(0);
    for (auto bucket : precs) {
        vars.push_back(bucket.first);
        vb.push_back(bucket.first);
        b.insert(
            std::pair<RCP<const Basic>, RCP<const Basic>>(bucket.first, zero));
        precs2.insert(std::pair<RCP<const Basic>, unsigned int>(bucket.first,
                                                                bucket.second));
    }
    vec_int v;
    bool done = false;
    RCP<const Basic> deriv = func->subs(b);
    v.resize(precs.size(), 0);
    while (!done) {
        dict.insert(std::pair<vec_int, Expression>(v, Expression(deriv)));
        auto iter = precs.begin();
        unsigned int whichvar = 0;
        v[whichvar]++;

        // Handle rollover
        while (iter != precs.end() && v[whichvar] > 0
               && static_cast<unsigned int>(v[whichvar]) >= iter->second) {
            v[whichvar] = 0;
            iter++;
            whichvar++;
            if (iter != precs.end())
                v[whichvar]++;
        }
        if (iter == precs.end()) {
            done = true;

            // Take the next derivative.
        } else {
            deriv = func;
            whichvar = 0;
            for (RCP<const Symbol> var : vars) {
                for (int i = 0; i < v[whichvar]; i++)
                    deriv = div(deriv->diff(var), integer(i + 1));
                whichvar++;
            }
        }
        deriv = deriv->subs(b);
    }

    return make_rcp<const MultivariateSeries>(
        MultivariatePolynomialExpr::create(vb, std::move(dict)),
        (*vars.begin())->__str__(), precs.begin()->second, std::move(precs2));
};

} // SymEngine
