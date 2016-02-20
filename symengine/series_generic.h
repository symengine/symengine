/**
 *  \file series_generic.h
 *  Class for univariate series.
 *
 **/
#ifndef SYMENGINE_SERIES_GENERIC_H
#define SYMENGINE_SERIES_GENERIC_H

#include <symengine/polynomial.h>
#include <symengine/basic.h>
#include <symengine/expression.h>
#include <symengine/rational.h>
#include <symengine/series.h>


namespace SymEngine {
using SymEngine::UnivariateExprPolynomial; //Polynomial type
//using sp_t = RCP //Coefficient type
//! UnivariateSeries Class
class UnivariateSeries : public SeriesBase<SymEngine::UnivariateExprPolynomial, SymEngine::Expression, UnivariateSeries> {
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
    UnivariateSeries(const SymEngine::UnivariateExprPolynomial sp, const std::string varname, const unsigned degree);
    IMPLEMENT_TYPEID(UNIVARIATESERIES)
    virtual int compare(const Basic &o) const;
    virtual std::size_t __hash__() const;
    virtual RCP<const Basic> as_basic() const;
    virtual umap_int_basic as_dict() const;
    virtual RCP<const Basic> get_coeff(int) const;

    static RCP<const UnivariateSeries> series(const RCP<const Basic> &t, const std::string &x, unsigned int prec);
    static SymEngine::UnivariateExprPolynomial convert(const Integer &x);
    static SymEngine::UnivariateExprPolynomial convert(const mpq_class &x);
    static SymEngine::UnivariateExprPolynomial var(const std::string &s);
    static SymEngine::UnivariateExprPolynomial convert(const Rational &x);
    static SymEngine::UnivariateExprPolynomial convert(const Number &x);

    static SymEngine::UnivariateExprPolynomial mul(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &r, unsigned prec);
    static SymEngine::UnivariateExprPolynomial pow(const SymEngine::UnivariateExprPolynomial &s, int n, unsigned prec);
    static unsigned ldegree(const SymEngine::UnivariateExprPolynomial &s);
    static SymEngine::UnivariateExprPolynomial find_cf(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &var, unsigned deg);
    static SymEngine::UnivariateExprPolynomial root(SymEngine::UnivariateExprPolynomial &c, unsigned n);
    static SymEngine::UnivariateExprPolynomial diff(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &var);
    static SymEngine::UnivariateExprPolynomial integrate(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &var);
    static SymEngine::UnivariateExprPolynomial subs(const SymEngine::UnivariateExprPolynomial &s, const SymEngine::UnivariateExprPolynomial &var, const SymEngine::UnivariateExprPolynomial &r, unsigned prec);
    
    //! \return true if canonical
    // bool is_canonical(const UnivariatePolynomial&, const unsigned int &) const;
    //! \return size of the hash

};

/*inline RCP<const UnivariateSeries> univariate_series(RCP<const Symbol> i, unsigned int prec, const map_uint_mpz& dict)
{
    return make_rcp<const UnivariateSeries>(i, prec, dict);
}*/

}  //SymEngine
#endif //SYMENGINE_SERIES_GENERIC_H
