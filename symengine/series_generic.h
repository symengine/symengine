/**
 *  \file series_generic.h
 *  Class for univariate series.
 *
 **/
#ifndef SYMENGINE_SERIES_GENERIC_H
#define SYMENGINE_SERIES_GENERIC_H

#include <list>

#include <symengine/polynomial.h>

namespace SymEngine {
//! UnivariateSeries Class
class UnivariateSeries : public Basic {
public:
    //! `var_` : Variable of the UnivariateSeries
    //! `poly_` : holds the UnivariateIntPolynomial of the series
    //! `prec_` : precision of the UnivariateSeries, i.e. self = poly + O(var^prec)
    // UnivariateSeries 1 + 2*x + x**2 + O(x**5) has dict_ = {{0, 1}, {1, 2}, {2, 1}} with var_ = "x" and prec_ = 5
    RCP<const Symbol> var_;
    RCP<const UnivariateIntPolynomial> poly_;
    unsigned int prec_;
public:
    IMPLEMENT_TYPEID(UNIVARIATESERIES)
    //! Constructor of UnivariateSeries class
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const RCP<const UnivariateIntPolynomial> &poly);
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int& precision, const unsigned int& max_exp, map_uint_mpz&& dict);
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const map_uint_mpz &dict);
    //! Constructor using a dense vector of integer_class coefficients
    UnivariateSeries(const RCP<const Symbol> &var, const unsigned int &precision, const std::vector<integer_class> &v);

    static RCP<const UnivariateSeries> create(const RCP<const Symbol> &var,
            const unsigned int &prec, const std::vector<integer_class> &v) {
        return make_rcp<const UnivariateSeries>(var, prec, v);
    }

    //! \return true if canonical
    bool is_canonical(const UnivariateIntPolynomial&, const unsigned int &) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */

    bool operator==(const UnivariateSeries &o) const {
        return (eq(*var_, *o.var_) and poly_->__eq__(*o.poly_) and prec_ == o.prec_);
    }
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    std::string __str__() const;
    virtual vec_basic get_args() const { return {}; }
};


inline RCP<const UnivariateSeries> univariate_series(RCP<const Symbol> i,       unsigned int prec, const map_uint_mpz& dict)
{
    return make_rcp<const UnivariateSeries>(i, prec, dict);
}

RCP<const UnivariateSeries> add_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);
RCP<const UnivariateSeries> neg_uni_series (const UnivariateSeries& a);
RCP<const UnivariateSeries> sub_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);
RCP<const UnivariateSeries> mul_uni_series (const UnivariateSeries& a, const UnivariateSeries &b);

}  //SymEngine
#endif
