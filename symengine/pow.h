/**
 *  \file pow.h
 *  Power Class
 *
 **/
#ifndef SYMENGINE_POW_H
#define SYMENGINE_POW_H

#include <symengine/functions.h>
#include <symengine/mul.h>
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
    virtual hash_t __hash__() const;
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

} // SymEngine

#endif
