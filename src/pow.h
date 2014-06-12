/**
 *  \file pow.h
 *  Power Class 
 *
 **/
#ifndef CSYMPY_POW_H
#define CSYMPY_POW_H

#include "basic.h"
#include "dict.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {

class Pow : public Basic {
public: // TODO: make this private
    RCP<const Basic> base_, exp_; //! base^exp

public:
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
    //! \return stringify version
    virtual std::string __str__() const;
	//! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &base,
            const RCP<const Basic> &exp);
    //! \return `base` of `base^exp`
    inline RCP<const Basic> get_base() const { return base_; }
    //! \return `exp` of `base^exp`
    inline RCP<const Basic> get_exp() const { return exp_; }
	//! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

//! \return Pow from `a` and `b`
RCP<const Basic> pow(const RCP<const Basic> &a,
        const RCP<const Basic> &b);

class Exp : public Pow {
public:
    RCP<const Basic> exp_; //! E^exp

public:
    //! Exp Constructor
    Exp(const RCP<const Basic> &exp);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

//! \return Pow from `E` and `e`
RCP<const Basic> exp(const RCP<const Basic> &e);

void multinomial_coefficients(int m, int n, map_vec_int &r);
//! Expand the power expression
RCP<const Basic> pow_expand(const RCP<const Pow> &self);
//! \return square root of `x`
inline RCP<const Basic> sqrt(const RCP<const Basic> &x) {
    return pow(x, div(one, integer(2)));
}

} // CSymPy

#endif
