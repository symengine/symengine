#ifndef SERIES_GENERIC_MULTIVARIATE_H
#define SERIES_GENERIC_MULTIVARIATE_H

//#include <list>
#include <symengine/polynomial_multivariate.h>
#include <symengine/rational.h>
#include <symengine/series.h>

namespace SymEngine
{

//! MultivariateSeries Class
class MultivariateSeries : public SeriesBase<MultivariatePolynomialExpr,
                                             Expression, MultivariateSeries>
{
public:
    map_basic_uint precs_;

public:
    IMPLEMENT_TYPEID(MULTIVARIATESERIES)
    MultivariateSeries(const MultivariatePolynomialExpr &sp,
                       const std::string varname, const unsigned degree);

    MultivariateSeries(const MultivariatePolynomialExpr &sp,
                       const std::string varname, const unsigned degree,
                       const map_basic_uint &precs);

    bool is_canonical(const MultivariatePolynomialExpr p, const std::string var,
                      const long degree, const map_basic_uint precs_);

    static RCP<const MultivariateSeries>
    create(const RCP<const Symbol> &var, const unsigned int &prec,
           const MultivariatePolynomialExpr &s)
    {
        return make_rcp<const MultivariateSeries>(std::move(s), var->get_name(),
                                                  prec);
    }

    static RCP<const MultivariateSeries>
    series(const RCP<const Basic> &t, const std::string &x, unsigned int prec);
    virtual std::size_t __hash__() const;
    virtual int compare(const Basic &o) const;
    virtual RCP<const Basic> as_basic() const;
    umap_int_basic as_dict() const;

    bool __eq__(const Basic &o) const;
    RCP<const Number> add(const Number &other) const;
    RCP<const Number> mul(const Number &other) const;
    // RCP<const Number> pow(const Number &other) const;

    RCP<const Basic> get_coeff(int) const;

    static MultivariatePolynomialExpr add(const MultivariatePolynomialExpr &s,
                                          const MultivariatePolynomialExpr &r,
                                          map_basic_uint prec);
    static MultivariatePolynomialExpr mul(const MultivariatePolynomialExpr &s,
                                          const MultivariatePolynomialExpr &r,
                                          map_basic_uint prec);
    // static MultivariatePolynomialExpr pow (const MultivariatePolynomialExpr
    // &s, int n, map_basic_uint prec);

    static MultivariatePolynomialExpr var(const std::string &s);

    static Expression convert(const Basic &x);

    static int ldegree(const MultivariatePolynomialExpr &s);

    // NOTE: Because these functions are static, we will assume that prec
    // restricts the first variable.
    static MultivariatePolynomialExpr mul(const MultivariatePolynomialExpr &s,
                                          const MultivariatePolynomialExpr &r,
                                          unsigned prec);
    static MultivariatePolynomialExpr pow(const MultivariatePolynomialExpr &s,
                                          int n, unsigned prec);
    static Expression find_cf(const MultivariatePolynomialExpr &s,
                              const MultivariatePolynomialExpr &var, int deg);
    static Expression root(Expression &c, unsigned n);
    static MultivariatePolynomialExpr
    diff(const MultivariatePolynomialExpr &s,
         const MultivariatePolynomialExpr &var);
    static MultivariatePolynomialExpr
    integrate(const MultivariatePolynomialExpr &s,
              const MultivariatePolynomialExpr &var);
    static MultivariatePolynomialExpr
    subs(const MultivariatePolynomialExpr &s,
         const MultivariatePolynomialExpr &var,
         const MultivariatePolynomialExpr &r, unsigned prec);

    static Expression sin(const Expression &c);
    static Expression cos(const Expression &c);
    static Expression tan(const Expression &c);
    static Expression asin(const Expression &c);
    static Expression acos(const Expression &c);
    static Expression atan(const Expression &c);
    static Expression sinh(const Expression &c);
    static Expression cosh(const Expression &c);
    static Expression tanh(const Expression &c);
    static Expression asinh(const Expression &c);
    static Expression acosh(const Expression &c);
    static Expression atanh(const Expression &c);
    static Expression exp(const Expression &c);
    static Expression log(const Expression &c);
};

inline RCP<const MultivariateSeries>
multivariate_series(RCP<const Symbol> i, unsigned int prec,
                    const MultivariatePolynomialExpr &s)
{
    return make_rcp<const MultivariateSeries>(std::move(s), i->get_name(),
                                              prec);
}

// Calculate multivariable Taylor series expansions.

// Use repeated differentiation
RCP<const MultivariateSeries> mult_series1(RCP<const Basic> func,
                                           const map_sym_uint &&precs);

template <typename Series>
RCP<const MultivariateSeries> mult_series(RCP<const Basic> func,
                                          const map_basic_uint &&precs)
{
    vec_basic vars;
    umap_vec_expr dict;
    vec_int v;
    v.resize(precs.size(), 0);
    dict.insert(std::pair<vec_int, Expression>(v, Expression(func)));
    for (auto bucket : precs) {
        vars.push_back(bucket.first);
    }
    unsigned int whichvar = 0;
    for (RCP<const Basic> variable : vars) {
        std::vector<std::pair<vec_int, Expression>> temp;
        for (auto &bucket : dict) {
            RCP<const Series> s
                = Series::series(bucket.second.get_basic(), variable->__str__(),
                                 precs.find(variable)->second);
            bucket.second = s->get_coeff(0);
            for (int i = Series::ldegree(s->get_poly());
                 i < 0
                 || static_cast<unsigned int>(i) < precs.find(variable)->second;
                 i++) {
                if (i != 0 && !eq(*s->get_coeff(i), *zero)) {
                    vec_int exps = bucket.first;
                    exps[whichvar] = i;
                    temp.push_back(std::pair<vec_int, Expression>(
                        exps, Expression(s->get_coeff(i))));
                }
            }
        }
        for (auto term : temp)
            dict.insert(term);
        whichvar++;
    }

    vec_basic vb;
    vb.insert(vb.begin(), vars.begin(), vars.end());
    return make_rcp<const MultivariateSeries>(
        MultivariatePolynomialExpr::create(vars, std::move(dict)),
        (*vars.begin())->__str__(), precs.begin()->second, std::move(precs));
}

} // SymEngine

#endif
