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
#include <symengine/expression.h>
#include <symengine/rational.h>
#include <symengine/series.h>


namespace SymEngine {/*
using s_coef = SymEngine::Expression; //Polynomial type
//using sp_t = RCP //Coefficient type
//! UnivariateSeries Class
class UnivariateSeries : public SeriesBase<s_coef, SymEngine::Expression, UnivariateSeries> {
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
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const RCP<const UnivariatePolynomial> &poly);
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int& precision, const unsigned int& max_exp, map_uint_mpz&& dict);
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_uint_mpz &dict);
    //! Constructor using a dense vector of integer_class coefficients
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<integer_class> &v);

    static RCP<const UnivariateSeries> create(const RCP<const Symbol> &var,
            const unsigned int &prec, const std::vector<integer_class> &v) {
        return make_rcp<const UnivariateSeries>(var, prec, v);
    }
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
*/
/*inline RCP<const UnivariateSeries> univariate_series(RCP<const Symbol> i, unsigned int prec, const map_uint_mpz& dict)
{
    return make_rcp<const UnivariateSeries>(i, prec, dict);
}*/

}  //SymEngine
#endif //SYMENGINE_SERIES_GENERIC_H
