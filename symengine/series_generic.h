/**
 *  \file series_generic.h
 *  Class for univariate series.
 *
 **/
#ifndef SYMENGINE_SERIES_GENERIC_H
#define SYMENGINE_SERIES_GENERIC_H

#include <list>
#include <symengine/polynomial.h>
#include <symengine/series.h>



namespace SymEngine
{
//! UnivariateSeries Class
class UnivariateSeries : public Basic
{
public:
    //! `var_` : Variable of the UnivariateSeries
    //! `poly_` : holds the UnivariateIntPolynomial of the series
    //! `prec_` : precision of the UnivariateSeries, i.e. self = poly +
    //! O(var^prec)
    // UnivariateSeries 1 + 2*x + x**2 + O(x**5) has dict_ = {{0, 1}, {1, 2},
    // {2, 1}} with var_ = "x" and prec_ = 5
    RCP<const Symbol> var_;
    RCP<const UnivariateIntPolynomial> poly_;
    unsigned int prec_;

public:
    IMPLEMENT_TYPEID(UNIVARIATESERIES)
    //! Constructor of UnivariateSeries class
    UnivariateSeries(const RCP<const Symbol> &var,
                     const unsigned int &precision,
                     const RCP<const UnivariateIntPolynomial> &poly);
    UnivariateSeries(const RCP<const Symbol> &var,
                     const unsigned int &precision, const unsigned int &max_exp,
                     map_uint_mpz &&dict);
    UnivariateSeries(const RCP<const Symbol> &var,
                     const unsigned int &precision, const map_uint_mpz &dict);
    //! Constructor using a dense vector of integer_class coefficients
    UnivariateSeries(const RCP<const Symbol> &var,
                     const unsigned int &precision,
                     const std::vector<integer_class> &v);

    static RCP<const UnivariateSeries>
    create(const RCP<const Symbol> &var, const unsigned int &prec,
           const std::vector<integer_class> &v)
    {
        return make_rcp<const UnivariateSeries>(var, prec, v);
    }

    //! \return true if canonical
    bool is_canonical(const UnivariateIntPolynomial &,
                      const unsigned int &) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */

    bool operator==(const UnivariateSeries &o) const
    {
        return (eq(*var_, *o.var_) and poly_->__eq__(*o.poly_)
                and prec_ == o.prec_);
    }
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    std::string __str__() const;
    virtual vec_basic get_args() const
    {
        return {};
    }
};

inline RCP<const UnivariateSeries> univariate_series(RCP<const Symbol> i,
                                                     unsigned int prec,
                                                     const map_uint_mpz &dict)
{
    return make_rcp<const UnivariateSeries>(i, prec, dict);
}

RCP<const UnivariateSeries> add_uni_series(const UnivariateSeries &a,
                                           const UnivariateSeries &b);
RCP<const UnivariateSeries> neg_uni_series(const UnivariateSeries &a);
RCP<const UnivariateSeries> sub_uni_series(const UnivariateSeries &a,
                                           const UnivariateSeries &b);
RCP<const UnivariateSeries> mul_uni_series(const UnivariateSeries &a,
                                           const UnivariateSeries &b);





//! MultivariateSeries Class
class MultivariateSeries : public SeriesBase<MultivariateExprPolynomial, Expression, MultivariateSeries>
{
    unsigned int whichvar; // index of the variable we are expanding around in the set.

public:
    IMPLEMENT_TYPEID(MULTIVARIATESERIES)
    MultivariateSeries(const MultivariateExprPolynomial &sp,
                     const std::string varname, const unsigned degree) 
        : SeriesBase(std::move(sp), varname, degree)
    {        
    }
    
    static RCP<const MultivariateSeries>
    create(const RCP<const Symbol> &var, const unsigned int &prec,
           const MultivariateExprPolynomial &s) 
    {
        return make_rcp<const MultivariateSeries>(std::move(s), var->get_name(), prec);
    }
      
    static RCP<const MultivariateSeries> series(const RCP<const Basic> &t, const std::string &x, unsigned int prec);
    virtual std::size_t __hash__() const;
    virtual int compare(const Basic &o) const;
    bool operator==(const MultivariateSeries &u) const;
    virtual RCP<const Basic> as_basic() const;
    umap_int_basic as_dict() const;
    RCP<const Basic> get_coeff(int) const;
    static MultivariateExprPolynomial var(const std::string &s);

    static Expression convert(const Basic &x);

    static int ldegree(const MultivariateExprPolynomial &s);
    static MultivariateExprPolynomial mul(const MultivariateExprPolynomial &s, const MultivariateExprPolynomial &r, unsigned prec);
    static MultivariateExprPolynomial pow(const MultivariateExprPolynomial &s, int n, unsigned prec);
    static Expression find_cf(const MultivariateExprPolynomial &s, const MultivariateExprPolynomial &var, int deg);
    static Expression root(Expression &c, unsigned n);
    static MultivariateExprPolynomial diff(const MultivariateExprPolynomial &s, const MultivariateExprPolynomial &var);
    static MultivariateExprPolynomial integrate(const MultivariateExprPolynomial &s, const MultivariateExprPolynomial &var);
    static MultivariateExprPolynomial subs(const MultivariateExprPolynomial &s, const MultivariateExprPolynomial &var, const MultivariateExprPolynomial &r, unsigned prec);
   
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

inline RCP<const MultivariateSeries> multivariate_series(RCP<const Symbol> i,
                                                     unsigned int prec,
                                                     const MultivariateExprPolynomial& s)
{
    return make_rcp<const MultivariateSeries>(std::move(s), i->get_name(), prec);
}

} // SymEngine
#endif
