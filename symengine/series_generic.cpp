// #include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine {

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const UnivariateExprPolynomial poly) :
        SeriesBase(std::move(poly), var->get_name(), precision) {
}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const unsigned int &max, map_uint_Expr &&dict) :
        SeriesBase(UnivariateExprPolynomial(univariate_polynomial(var, max, std::move(dict))), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_uint_mpz& dict) :
        SeriesBase(UnivariateExprPolynomial(convert_poly(std::move(dict))), var->get_name(), precision) {}

UnivariateSeries::UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<mpz_class> &v) :
        SeriesBase(UnivariateExprPolynomial(convert_vector(v)), var->get_name(), precision) {}

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t, const std::string &x, unsigned int prec) {
    SeriesVisitor<UnivariateExprPolynomial, Expression, UnivariateSeries> visitor(UnivariateExprPolynomial(std::stoi(x)), x, prec);
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
    return p_.get_basic()->__cmp__(*s.p_.get_basic()); //__cmp__()*/
    /*if (not is_a<UnivariateSeries>(other))
        throw std::domain_error("cannot compare with UnivariateSeries");
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return p_ == (o.p_);
}

bool UnivariateSeries::operator==(const UnivariateSeries &other) const {
    return p_ == (other.p_);
}

RCP<const Basic> UnivariateSeries::as_basic() const {
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const {
    umap_int_basic map;
    for (int i = 0; i <= degree_; i++) 
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

RCP<const UnivariatePolynomial> UnivariateSeries::convert_poly(const map_uint_mpz &d) {
    map_uint_Expr dict_trunc;
    unsigned int max = 0;
    for (const auto &it : d) {
        if (it.first < prec_) {

            if (max < it.first)
                max = it.first;
            dict_trunc[it.first] = Expression(mp_get_ui(it.second));
        }
    }
    return univariate_polynomial(symbol(var_), max, std::move(dict_trunc));
}

std::string UnivariateSeries::__str__() const
{
    std::ostringstream o;
    bool first = true;
    for (const auto& it : p_.get_univariate_poly()->get_dict()) {
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
            o << Expression(abs(it.second.get_basic()));
            continue;
        }
        if (Expression(abs(it.second.get_basic())) == 1)
            o << var_;
        else
            o << Expression(abs(it.second.get_basic())) << "*" << var_;
        if (it.first > 1)
            o << "**" << it.first;
    }
    if (o.str() != "0")
        o << " + O(" << var_ << "**" << prec_ << ")";
    return o.str();
}

RCP<const UnivariatePolynomial> UnivariateSeries::convert_vector(const std::vector<integer_class> &v) {
    std::vector<Expression> vtrunc;
    for (const auto &it : v)
        if (mp_get_si(it) < prec_) 
            vtrunc.push_back(mp_get_si(it));
    return UnivariatePolynomial::create(symbol(var_), vtrunc);
}

unsigned UnivariateSeries::ldegree(const UnivariateExprPolynomial &s) {
    throw std::runtime_error("Not Implemented");
/*    mpq_class cl_rat(p_.find_cf({i}).get_mpq_view());
    cl_rat.canonicalize();
    RCP<const Basic> basic;
    if (cl_rat.get_den() == 1)
        basic = make_rcp<const Integer>(cl_rat.get_num());
    else
        basic = make_rcp<const Rational>(cl_rat);
    return std::move(basic);*//*
}

UnivariateExprPolynomial UnivariateSeries::mul(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &r, unsigned prec) {
    // No prec mul
    return s * r;
}

UnivariateExprPolynomial UnivariateSeries::pow(const UnivariateExprPolynomial &s, int n, unsigned prec) {
    // No prec pow
    return pow_ex(Expression(s.get_basic()), Expression(n));
}

Expression UnivariateSeries::find_cf(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, unsigned deg) {
    return (s.get_univariate_poly()->get_dict()).at(deg);
}

Expression UnivariateSeries::root(Expression &c, unsigned n) {
    return pow_ex(c, 1/Expression(n));
}

UnivariateExprPolynomial UnivariateSeries::diff(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    return UnivariateExprPolynomial(Expression(s.get_univariate_poly()->diff(var.get_univariate_poly()->get_var())));
}

UnivariateExprPolynomial UnivariateSeries::integrate(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var) {
    map_uint_Expr dict;
    for (auto &it : s.get_univariate_poly()->get_dict())
        dict.insert(std::pair<unsigned, Expression>(it.first + 1, it.second / (it.first + 1)));
    return UnivariateExprPolynomial(univariate_polynomial(var.get_univariate_poly()->get_var(), (dict.end()--)->first, std::move(dict))); 
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

UnivariateExprPolynomial UnivariateSeries::subs(const UnivariateExprPolynomial &s, const UnivariateExprPolynomial &var, const UnivariateExprPolynomial &r, unsigned prec) {
    map_basic_basic x{{r.get_basic(), var.get_basic()}};
    return UnivariateExprPolynomial(Expression(s.get_basic()->subs(x)));
}

Expression UnivariateSeries::sin(const Expression& c) {
    return SymEngine::sin(c.get_basic());
}

Expression UnivariateSeries::cos(const Expression& c) {
    return SymEngine::cos(c.get_basic());
}

Expression UnivariateSeries::tan(const Expression& c) {
    return SymEngine::tan(c.get_basic());
}

Expression UnivariateSeries::asin(const Expression& c) {
    return SymEngine::asin(c.get_basic());
}

Expression UnivariateSeries::acos(const Expression& c) {
    return SymEngine::acos(c.get_basic());
}

Expression UnivariateSeries::atan(const Expression& c) {
    return SymEngine::atan(c.get_basic());
}

Expression UnivariateSeries::sinh(const Expression& c) {
    return SymEngine::sinh(c.get_basic());
}

Expression UnivariateSeries::cosh(const Expression& c) {
    return SymEngine::cosh(c.get_basic());
}

Expression UnivariateSeries::tanh(const Expression& c) {
    return SymEngine::tanh(c.get_basic());
}

Expression UnivariateSeries::asinh(const Expression& c) {
    return SymEngine::asinh(c.get_basic());
}

Expression UnivariateSeries::atanh(const Expression& c) {
    return SymEngine::atanh(c.get_basic());
}

Expression UnivariateSeries::exp(const Expression& c) {
    return SymEngine::exp(c.get_basic());
}

Expression UnivariateSeries::log(const Expression& c) {
    return SymEngine::log(c.get_basic());
}

RCP<const UnivariateSeries> add_uni_series (const UnivariateSeries& a, const UnivariateSeries &b) {
    map_uint_Expr dict;
    SYMENGINE_ASSERT(a == b)
    unsigned int minprec = (a.prec_ < b.prec_)? a.prec_ : b.prec_;
    for (const auto &it : a.get_poly().get_univariate_poly()->get_dict()) {
        if (it.first >= minprec)
            break;
        dict[it.first] = it.second;
    }

    unsigned int max = 0;
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
    return make_rcp<const UnivariateSeries>(symbol(a.get_var()), a.prec_, std::move(neg_uni_poly(*(a.get_poly().get_univariate_poly()))));
}
    
RCP<const UnivariateSeries> sub_uni_series (const UnivariateSeries& a, const UnivariateSeries &b) {
    return add_uni_series(a, *neg_uni_series(b));
}

RCP<const UnivariateSeries> mul_uni_series (const UnivariateSeries& a, const UnivariateSeries &b) {
    map_uint_Expr dict;
    SYMENGINE_ASSERT(a == b)
    const unsigned int minprec = (a.prec_ < b.prec_)? a.prec_ : b.prec_;
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
