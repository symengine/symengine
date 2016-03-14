#include <exception>
#include <algorithm>
#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const UnivariateExprPolynomial &poly) :
        SeriesBase(std::move(poly), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const unsigned int &max, map_int_Expr &&dict) :
        SeriesBase(UnivariateExprPolynomial(univariate_polynomial(var, (int)max, std::move(dict))), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<integer_class> &v) :
        SeriesBase(UnivariateExprPolynomial(convert_vector(v)), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_int_Expr &dict) :
        SeriesBase(UnivariateExprPolynomial(convert_poly(var, std::move(dict), precision)), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<Expression> &v) :
        SeriesBase(UnivariateExprPolynomial(UnivariatePolynomial::create(var, v)), var->get_name(), precision) {}

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t, const std::string &x, unsigned int prec) {
    SeriesVisitor<UnivariateExprPolynomial, Expression, UnivariateSeries> visitor(UnivariateExprPolynomial(UnivariatePolynomial::create(symbol(x), {})), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const {
    return p_.__hash__() + std::size_t(get_degree() * 84728863L);
}

int UnivariateSeries::compare(const Basic &other) const {
    if (not is_a<UnivariateSeries>(other))
        throw std::domain_error("cannot compare with UnivariateSeries");
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return p_.get_basic()->__cmp__(*o.p_.get_basic());
}

bool UnivariateSeries::operator==(const UnivariateSeries &other) const {
    return p_ == (other.p_);
}

RCP<const Basic> UnivariateSeries::as_basic() const {
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    umap_int_basic map;
    for (int i = 0; i <= get_degree(); i++)
       map[i] = p_.get_univariate_poly()->get_dict().at(i).get_basic();
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int deg) const {
    if(p_.get_univariate_poly()->get_dict().count(deg) == 0)
        return zero;
    else
        return p_.get_univariate_poly()->get_dict().at(deg).get_basic();
}

UnivariateExprPolynomial UnivariateSeries::var(const std::string &s) {
    return UnivariateExprPolynomial(std::stoi(s));
}

Expression UnivariateSeries::convert(const Number &x) {
    return Expression(x.rcp_from_this());
}

RCP<const UnivariatePolynomial> UnivariateSeries::convert_poly(const RCP<const Symbol> &var, const map_int_Expr &d, unsigned pr) {
    map_int_Expr dict_trunc;
    unsigned int max = 0;
    for (const auto &it : d) {
        if ((unsigned)it.first < pr) {
            if (max < (unsigned)it.first)
                max = (unsigned)it.first;
            dict_trunc[it.first] = it.second;
        }
    }
    return univariate_polynomial(var, max, std::move(dict_trunc));
}

RCP<const UnivariatePolynomial> UnivariateSeries::convert_vector(const std::vector<integer_class> &v) {
    std::vector<Expression> vtrunc;
    for (unsigned int i = 0; i < v.size(); i++)
        if (i < get_degree())
            vtrunc.push_back(Expression(mp_get_si(v[i])));
    return UnivariatePolynomial::create(symbol(var_), vtrunc);
}

unsigned UnivariateSeries::ldegree(const UnivariateExprPolynomial &s) {
    return s.get_univariate_poly()->get_dict().begin()->first;
}

UnivariateExprPolynomial UnivariateSeries::mul(const UnivariateExprPolynomial &a, const UnivariateExprPolynomial &b, unsigned prec) {
    unsigned int exp;
    map_int_Expr p;

    for(auto &it1 : a.get_univariate_poly()->get_dict()) {
        for(auto &it2 : b.get_univariate_poly()->get_dict()) {
            exp = it1.first + it2.first;
            if(exp < prec) {
                p[exp] = it1.second * it2.second;
            } else {
                break;
            }
        }
    }
    return UnivariateExprPolynomial(UnivariatePolynomial::from_dict(a.get_univariate_poly()->get_var(), std::move(p)));
}

UnivariateExprPolynomial UnivariateSeries::pow(const UnivariateExprPolynomial &s, int n, unsigned prec) {
    if(is_a<const Constant>(*s.get_basic())) {
        throw std::runtime_error("Polynomial cannot be a constant");
    }
    if(s.get_univariate_poly()->get_var() != symbol("x")) {
        throw std::runtime_error("Polynomial is not x");
    }
    if(n < 0) {
        throw std::runtime_error("Not implemented");
        // UnivariateExprPolynomial p1 = UnivariateSeries::pow(s, -n, prec);
        // return UnivariateExprPolynomial(1 /p1.get_univariate_poly());
    } else if(n == 0) {
        return UnivariateExprPolynomial(1);
    } else if(n == 1) {
        return UnivariateSeries::convert_poly(s.get_univariate_poly()->get_var(), s.get_univariate_poly()->get_dict(), prec);
    } else {
        UnivariateExprPolynomial p = s;
        for(int i = 0; i < n; i++) {
            p = UnivariateSeries::mul(s, p, prec);
        }
        return p;
    }
}

Expression UnivariateSeries::find_cf(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, unsigned deg) {
    return (s.get_univariate_poly()->get_dict()).at(deg);
}

Expression UnivariateSeries::root(Expression &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

UnivariateExprPolynomial UnivariateSeries::diff(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    RCP<const Basic> p = s.get_univariate_poly()->diff(var.get_univariate_poly()->get_var());
    if(is_a<const UnivariatePolynomial>(*p))
        return UnivariateExprPolynomial(rcp_static_cast<const UnivariatePolynomial>(p));
    else
        throw std::runtime_error("Not a UnivariatePolynomial");
}

UnivariateExprPolynomial UnivariateSeries::integrate(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    map_int_Expr dict;
    for (auto &it : s.get_univariate_poly()->get_dict())
        dict.insert(std::pair<unsigned, Expression>(it.first + 1, it.second / (it.first + 1)));
    return UnivariateExprPolynomial(univariate_polynomial(var.get_univariate_poly()->get_var(), (dict.end()--)->first, std::move(dict))); 
}

UnivariateExprPolynomial UnivariateSeries::subs(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, const UnivariateExprPolynomial &r, unsigned prec) {
    map_basic_basic x{{r.get_basic(), var.get_basic()}};
    return UnivariateExprPolynomial(Expression(s.get_basic()->subs(x)));
}

Expression UnivariateSeries::sin(const Expression& c) {
    return sin(c.get_basic());
}

Expression UnivariateSeries::cos(const Expression& c) {
    return cos(c.get_basic());
}

Expression UnivariateSeries::tan(const Expression& c) {
    return tan(c.get_basic());
}

Expression UnivariateSeries::asin(const Expression& c) {
    return asin(c.get_basic());
}

Expression UnivariateSeries::acos(const Expression& c) {
    return acos(c.get_basic());
}

Expression UnivariateSeries::atan(const Expression& c) {
    return atan(c.get_basic());
}

Expression UnivariateSeries::sinh(const Expression& c) {
    return sinh(c.get_basic());
}

Expression UnivariateSeries::cosh(const Expression& c) {
    return cosh(c.get_basic());
}

Expression UnivariateSeries::tanh(const Expression& c) {
    return tanh(c.get_basic());
}

Expression UnivariateSeries::asinh(const Expression& c) {
    return asinh(c.get_basic());
}

Expression UnivariateSeries::atanh(const Expression& c) {
    return atanh(c.get_basic());
}

Expression UnivariateSeries::exp(const Expression& c) {
    return exp(c.get_basic());
}

Expression UnivariateSeries::log(const Expression& c) {
    return log(c.get_basic());
}

RCP<const UnivariateSeries> add_uni_series(const UnivariateSeries& a, const UnivariateSeries &b) {
    // UnivariateExprPolynomial c = a.get_poly() + b.get_poly();
    // std::cout << c.get_univariate_poly()->__str__() << std::endl;
    // int minprec = (a.prec_ < b.prec_)? a.prec_ : b.prec_;
    // return make_rcp<const UnivariateSeries>(a.get_poly().get_univariate_poly()->get_var(), minprec, c);
   map_int_Expr dict;
   SYMENGINE_ASSERT(a.get_var() == b.get_var())
   int minprec = (a.get_degree() < b.get_degree())? a.get_degree() : b.get_degree();
   for (const auto &it : a.get_poly().get_univariate_poly()->get_dict()) {
       if (it.first >= minprec)
           break;
       dict[it.first] = it.second;
   }

   int max = 0;
   for (const auto &it : b.get_poly().get_univariate_poly()->get_dict()) {
       if (it.first >= minprec)
           break;
       dict[it.first] += it.second;
       if (dict[it.first] != 0 and it.first > max)
           max = it.first;
   }
   return make_rcp<const UnivariateSeries>(symbol(a.get_var()), minprec, max, std::move(dict));
}

RCP<const UnivariateSeries> neg_uni_series (const UnivariateSeries& a) {
    return make_rcp<const UnivariateSeries>(symbol(a.get_var()), a.get_degree(), std::move(UnivariateExprPolynomial(neg_uni_poly(*(a.get_poly().get_univariate_poly())))));
}
    
RCP<const UnivariateSeries> sub_uni_series (const UnivariateSeries& a, const UnivariateSeries &b) {
    return add_uni_series(a, *neg_uni_series(b));
}

RCP<const UnivariateSeries> mul_uni_series (const UnivariateSeries& a, const UnivariateSeries &b) {
    map_int_Expr dict;
    SYMENGINE_ASSERT(a.get_var() == b.get_var())
    const unsigned int minprec = (a.get_degree() < b.get_degree())? a.get_degree() : b.get_degree();
    unsigned int max = 0;
    for (const auto &ait : a.get_poly().get_univariate_poly()->get_dict()) {
        const unsigned int aexp = ait.first;
        if (aexp < minprec) {
            for (const auto &bit : b.get_poly().get_univariate_poly()->get_dict()) {
                const unsigned int expsum = aexp + bit.first;
                if (expsum < minprec)
                    dict[expsum] += ait.second * bit.second;
                else
                    break;
                if (expsum > max)
                     max = expsum;
            }
        }
        else
            break;
    }
    return make_rcp<const UnivariateSeries>(symbol(a.get_var()), minprec, max, std::move(dict));
}

} // SymEngine
