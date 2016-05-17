/**
 *  \file pow.h
 *  Power Class
 *
 **/
#ifndef SYMENGINE_POW_H
#define SYMENGINE_POW_H

#include <symengine/basic.h>
#include <symengine/functions.h>
#include <symengine/dict.h>
#include <symengine/mul.h>
#include <symengine/integer.h>
#include <symengine/ntheory.h>
#include <symengine/constants.h>

namespace SymEngine
{

class Pow : public Basic
{
private:
    RCP<const Basic> base_, exp_; //! base**exp

public:
    IMPLEMENT_TYPEID(POW)
    //! Pow Constructor
    Pow(const RCP<const Basic> &base, const RCP<const Basic> &exp);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const Basic &base, const Basic &exp) const;
    //! \return `base` of `base**exp`
    inline RCP<const Basic> get_base() const
    {
        return base_;
    }
    //! \return `exp` of `base**exp`
    inline RCP<const Basic> get_exp() const
    {
        return exp_;
    }

    virtual vec_basic get_args() const;
};

//! \return Pow from `a` and `b`
RCP<const Basic> pow(const RCP<const Basic> &a, const RCP<const Basic> &b);

//! Returns the natural exponential function `E**x = pow(E, x)`
RCP<const Basic> exp(const RCP<const Basic> &x);

void multinomial_coefficients(int m, int n, map_vec_int &r);
void multinomial_coefficients_mpz(int m, int n, map_vec_mpz &r);
//! Expand the power expression
RCP<const Basic> pow_expand(const RCP<const Pow> &self);
//! \return square root of `x`
inline RCP<const Basic> sqrt(const RCP<const Basic> &x)
{
    return pow(x, div(one, integer(2)));
}

class Log : public OneArgFunction
{
    // Logarithms are taken with the natural base, `e`. To get
    // a logarithm of a different base `b`, use `log(x, b)`,
    // which is essentially short-hand for `log(x)/log(b)`.
public:
    IMPLEMENT_TYPEID(LOG)
    //! Log Constructor
    Log(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const Basic &arg) const;
    //! \return canonicalized `log`
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Returns the Natural Logarithm from argument `arg`
RCP<const Basic> log(const RCP<const Basic> &arg);
//! \return Log from argument `arg` wrt base `b`
RCP<const Basic> log(const RCP<const Basic> &arg, const RCP<const Basic> &b);

} // SymEngine

#endif
