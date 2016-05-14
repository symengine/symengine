/**
 *  \file univariate_int_generic.h
 *  Class for univariate int polynomials.
 *
 **/
#ifndef SYMENGINE_UNIVARIATE_INT_GENERIC_H
#define SYMENGINE_UNIVARIATE_INT_GENERIC_H

#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/symbol.h>
#include <symengine/univariate_int.h>

namespace SymEngine
{
//! UnivariateInt Class
class UnivariateInt
    : public UnivariateIntBase<integer_class, map_uint_mpz, UnivariateInt>
{

public:
    IMPLEMENT_TYPEID(UNIVARIATEINT)

    //! Constructor of UnivariateInt class
    UnivariateInt(const RCP<const Symbol> &var, const unsigned int &degree,
                  map_uint_mpz &&dict);
    //! Constructor using a dense vector of integer_class coefficients
    UnivariateInt(const RCP<const Symbol> &var,
                  const std::vector<integer_class> &v);

    //! \return true if canonical
    bool is_canonical(const unsigned int &degree,
                      const map_uint_mpz &dict) const;
    //! \return size of the hash
    std::size_t __hash__() const;

    int compare(const Basic &o) const;

    //! \return the number terms in the polynomial
    unsigned int get_length() const;
    //! \return evaluated value at `x`
    integer_class eval(const integer_class &x) const;
    //! \return evaluated value at `2**x`
    integer_class eval_bit(const unsigned int &x) const;

    //????
    virtual vec_basic get_args() const;

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
};

//! Adding two UnivariateInt a and b
RCP<const UnivariateInt> add_poly(const UnivariateInt &a,
                                  const UnivariateInt &b);
//! Negative of a UnivariateInt
RCP<const UnivariateInt> neg_poly(const UnivariateInt &a);
//! Subtracting two UnivariateInt a and b
RCP<const UnivariateInt> sub_poly(const UnivariateInt &a,
                                  const UnivariateInt &b);

// TODODO not able to move them to the .h file :(
inline RCP<const UnivariateInt> univariate_int(RCP<const Symbol> var,
                                               map_uint_mpz &&d)
{
    if (var->get_name() == "")
        if (d.size() > 1 or (d.size() == 1 and d.begin()->first != 0))
            throw std::runtime_error("Error: empty variable cannot be used");

    auto iter = d.begin();
    while (iter != d.end()) {
        if (iter->second == 0) {
            auto toErase = iter;
            iter++;
            d.erase(toErase);
        } else {
            iter++;
        }
    }
    unsigned int degree = 0;
    if (!d.empty())
        degree = (--(d.end()))->first;

    return make_rcp<const UnivariateInt>(var, degree, std::move(d));
}

inline RCP<const UnivariateInt>
univariate_int(RCP<const Symbol> var, const std::vector<integer_class> &v)
{
    return make_rcp<const UnivariateInt>(var, std::move(v));
}
} // SymEngine

#endif
