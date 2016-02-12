/**
 *  \file series_generic.h
 *  Class for univariate series.
 *
 **/
#ifndef SYMENGINE_SERIES_GENERIC_H
#define SYMENGINE_SERIES_GENERIC_H

#include <list>

#include <symengine/polynomial.h>
#include <symengine/rational.h>
#include <symengine/integer.h>
#include <symengine/series.h>
#include <symengine/expression.h>

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
    IMPLEMENT_TYPEID(UNIVARIATESERIES)
    //! Constructor of UnivariateSeries class
    UnivariateSeries(const s_coef sp, const std::string varname, const unsigned degree);
    static RCP<const UnivariateSeries> series(const RCP<const Basic> &t, const std::string &x, unsigned int prec);
    static SymEngine::Integer convert(const Integer &x);
    static SymEngine::Rational convert(const mpq_class &x);
    //static pp_t var(const std::string &s);
    static SymEngine::Rational convert(const Rational &x);
    static SymEngine::Rational convert(const Number &x);

    //   UnivariateSeries(const RCP<const Symbol> &var, const unsigned int& precision, const unsigned int& max_exp, map_uint_mpz&& dict);
    //UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_uint_mpz &dict);
    //! Constructor using a dense vector of mpz_class coefficients
    //  UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<mpz_class> &v);
    /*
    static RCP<const UnivariateSeries> create(const RCP<const Symbol> &var,
            const unsigned int &prec, const std::vector<mpz_class> &v) {
        return make_rcp<const UnivariateSeries>(var, prec, v);
    }
    */
    //! \return true if canonical
    bool is_canonical(const UnivariatePolynomial&, const unsigned int &) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */

    /*bool operator==(const UnivariateSeries &o) const {
        return (var_ == o.var_ and p_->__eq__(o.p_) and degree_ == o.degree_);
    }*/
    //bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    std::string __str__() const;
    virtual vec_basic get_args() const { return {}; }

    static s_coef series_sin(const s_coef &s, const s_coef &var, unsigned int prec);
};

  /*
inline RCP<const UnivariateSeries> univariate_series(RCP<const Symbol> i,       unsigned int prec, const map_uint_mpz& dict)
{
    return make_rcp<const UnivariateSeries>(i, prec, dict);
}

  */
RCP<const UnivariateSeries> add_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);
RCP<const UnivariateSeries> neg_uni_series (const UnivariateSeries& a);
RCP<const UnivariateSeries> sub_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);
RCP<const UnivariateSeries> mul_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);

}  //SymEngine
#endif
