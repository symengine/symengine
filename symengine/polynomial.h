/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UnivariatePolynomial and Polynomial
 *
 **/
#ifndef SYMENGINE_POLYNOMIALS_H
#define SYMENGINE_POLYNOMIALS_H

#include <symengine/monomials.h>
#include <symengine/dict.h>
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/symbol.h>

namespace SymEngine {
//! UnivariatePolynomial Class
class UnivariatePolynomial : public Basic{
public:
    //! `degree` : Degree of UnivariatePolynomial
    //! `var_` : Variable of the uni-variate UnivariatePolynomial
    //! `dict_` : holds the UnivariatePolynomial
    // UnivariatePolynomial x**2 + 2*x + 1 has dict_ = {{0, 1}, {1, 2}, {2, 1}} with var_ = "x" 
    unsigned int degree_;
    RCP<const Symbol> var_;
    map_uint_mpz dict_;
public:
    IMPLEMENT_TYPEID(UNIVARIATEPOLYNOMIAL)
    //! Constructor of UnivariatePolynomial class
    UnivariatePolynomial(const RCP<const Symbol> &var, const unsigned int &degree, map_uint_mpz&& dict);
    //! Constructor using a dense vector of mpz_class coefficients
    UnivariatePolynomial(const RCP<const Symbol> &var, const std::vector<mpz_class> &v);
    //! \return true if canonical
    bool is_canonical(const unsigned int &degree, const map_uint_mpz& dict);
    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    /*! Creates appropriate instance (i.e Symbol, Integer,
    * Mul, Pow, UnivariatePolynomial) depending on the size of dictionary `d`.
    */
    static RCP<const Basic> from_dict(const RCP<const Symbol> &var, map_uint_mpz &&d);
    /*!
    * Adds coef*var_**n to the dict_
    */
    static void dict_add_term(map_uint_mpz &d,
            const mpz_class &coef, const unsigned int &n);
    mpz_class max_coef() const;
    //! Differentiates w.r.t symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! Evaluates the UnivariatePolynomial at value x
    mpz_class eval(const mpz_class &x) const;
    //! Evaluates the UnivariatePolynomial at value 2**x
    mpz_class eval_bit(const int &x) const;

    //! \return `true` if `0`
    bool is_zero() const;
    //! \return `true` if `1`
    bool is_one() const;
    //! \return `true` if `-1`    
    bool is_minus_one() const;
    //! \return `true` if integer  
    bool is_integer() const;
    //! \return `true` if symbol 
    bool is_symbol() const;
    //! \return `true` if mul   
    bool is_mul() const;
    //! \return `true` if pow    
    bool is_pow() const;

    virtual vec_basic get_args() const;

    virtual void accept(Visitor &v) const;

}; //UnivariatePolynomial

//! Adding two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> add_uni_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b);
//! Negative of a UnivariatePolynomial
RCP<const UnivariatePolynomial> neg_uni_poly(const UnivariatePolynomial &a);
//! Subtracting two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> sub_uni_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b);
//! Multiplying two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> mul_uni_poly(RCP<const UnivariatePolynomial> a, RCP<const UnivariatePolynomial> b);

inline RCP<const UnivariatePolynomial> univariate_polynomial(RCP<const Symbol> i, unsigned int deg, map_uint_mpz&& dict)
{
    return rcp(new UnivariatePolynomial(i, deg, std::move(dict)));
}

}  //SymEngine

#endif
