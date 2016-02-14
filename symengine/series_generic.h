/**
 *  \file series_generic.h
 *  Class for univariate series.
 *
 **/
#ifndef SYMENGINE_SERIES_GENERIC_H
#define SYMENGINE_SERIES_GENERIC_H

// #include <list>

// #include <symengine/polynomial.h>
#include <symengine/basic.h>
#include <symengine/rational.h>
#include <symengine/expression.h>
#include <symengine/series.h>


namespace SymEngine {
using s_coef = SymEngine::Expression; //Polynomial type
//using sp_t = RCP //Coefficient type
//! UnivariateSeries Class
class UnivariateSeries : public SeriesBase<s_coef, int, UnivariateSeries> {
//public:
    //! `var_` : Variable of the UnivariateSeries
    //! `poly_` : holds the UnivariatePolynomial of the series
    //! `prec_` : precision of the UnivariateSeries, i.e. self = poly + O(var^prec)
    // UnivariateSeries 1 + 2*x + x**2 + O(x**5) has dict_ = {{0, 1}, {1, 2}, {2, 1}} with var_ = "x" and prec_ = 5
    //    RCP<const Symbol> var_;
    //RCP<const UnivariatePolynomial> poly_;
    //unsigned int prec_;
    //data members now inherited from SeriesBase
public:
    //! Constructor of UnivariateSeries class
    UnivariateSeries(const s_coef sp, const std::string varname, const unsigned degree);
    IMPLEMENT_TYPEID(UNIVARIATESERIES)
    virtual int compare(const Basic &o) const;
    virtual std::size_t __hash__() const;
    virtual RCP<const Basic> as_basic() const;
    virtual umap_int_basic as_dict() const;
    virtual RCP<const Basic> get_coeff(int) const;

    static RCP<const UnivariateSeries> series(const RCP<const Basic> &t, const std::string &x, unsigned int prec);
    static s_coef convert(const Integer &x);
    static s_coef convert(const mpq_class &x);
    static s_coef var(const std::string &s);
    static s_coef convert(const Rational &x);
    static s_coef convert(const Number &x);

    static s_coef mul(const s_coef &s, const s_coef &r, unsigned prec);
    static s_coef pow(const s_coef &s, int n, unsigned prec);
    static unsigned ldegree(const s_coef &s);
    static s_coef find_cf(const s_coef &s, const s_coef &var, unsigned deg);
    static s_coef root(s_coef &c, unsigned n);
    static s_coef diff(const s_coef &s, const s_coef &var);
    static s_coef integrate(const s_coef &s, const s_coef &var);
    static s_coef subs(const s_coef &s, const s_coef &var, const s_coef &r, unsigned prec);
    
    //! \return true if canonical
    // bool is_canonical(const UnivariatePolynomial&, const unsigned int &) const;
    //! \return size of the hash
    

};

}  //SymEngine
#endif //SYMENGINE_SERIES_GENERIC_H
