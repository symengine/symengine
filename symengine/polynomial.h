/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UnivariateIntPolynomial and Polynomial
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
//! UnivariateIntPolynomial Class
class UnivariateIntPolynomial : public Basic{
public:
    //! `degree` : Degree of UnivariateIntPolynomial
    //! `var_` : Variable of the uni-variate UnivariateIntPolynomial
    //! `dict_` : holds the UnivariateIntPolynomial
    // UnivariateIntPolynomial x**2 + 2*x + 1 has dict_ = {{0, 1}, {1, 2}, {2, 1}} with var_ = "x"
    unsigned int degree_;
    RCP<const Symbol> var_;
    map_uint_mpz dict_;
public:
    IMPLEMENT_TYPEID(UNIVARIATEINTPOLYNOMIAL)
    //! Constructor of UnivariatePolynomial class
    UnivariateIntPolynomial(const RCP<const Symbol> &var, const unsigned int &degree, map_uint_mpz&& dict);

    static RCP<const UnivariateIntPolynomial> create(const RCP<const Symbol> &var,
	   const std::vector<integer_class> &v) {
        return UnivariateIntPolynomial::from_vec(var, v);
    }

    //! \return true if canonical
    bool is_canonical(const unsigned int &degree, const map_uint_mpz& dict) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    //creates a UnivariateIntPolynomial in cannonical form based on the dictionary.
    static RCP<const UnivariateIntPolynomial> from_dict(const RCP<const Symbol> &var, map_uint_mpz &&d);
    //create a UnivariateIntPolynomial from a dense vector of integer_class coefficients
    static RCP<const UnivariateIntPolynomial> from_vec(const RCP<const Symbol> &var, const std::vector<integer_class> &v);

    static RCP<const UnivariateIntPolynomial> create(const RCP<const Symbol> &var,
						  const std::vector<integer_class> &v) {
        return UnivariateIntPolynomial::from_vec(var, v);
    }
    
    /*!
    * Adds coef*var_**n to the dict_
    */
    static void dict_add_term(map_uint_mpz &d,
            const integer_class &coef, const unsigned int &n);
    integer_class max_coef() const;
    //! Evaluates the UnivariateIntPolynomial at value x
    integer_class eval(const integer_class &x) const;
    //! Evaluates the UnivariateIntPolynomial at value 2**x
    integer_class eval_bit(const int &x) const;

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

    inline RCP<const Symbol> get_var() const {
        return var_;
    }
    inline const map_uint_mpz& get_dict() const {
        return dict_;
    };

}; //UnivariateIntPolynomial

//! Adding two UnivariateIntPolynomial a and b
RCP<const UnivariateIntPolynomial> add_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b);
//! Negative of a UnivariateIntPolynomial
RCP<const UnivariateIntPolynomial> neg_poly(const UnivariateIntPolynomial &a);
//! Subtracting two UnivariateIntPolynomial a and b
RCP<const UnivariateIntPolynomial> sub_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b);
//! Multiplying two UnivariateIntPolynomial a and b
RCP<const UnivariateIntPolynomial> mul_poly(RCP<const UnivariateIntPolynomial> a, RCP<const UnivariateIntPolynomial> b);

inline RCP<const UnivariateIntPolynomial> univariate_int_polynomial(RCP<const Symbol> i, map_uint_mpz&& dict)
{
    return UnivariateIntPolynomial::from_dict(i, std::move(dict));
}

}  //SymEngine

#endif
