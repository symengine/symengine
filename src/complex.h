/**
 *  \file complex.h
 *  Class for Complex built on top of Number class
 *
 **/
#ifndef CSYMPY_COMPLEX_H
#define CSYMPY_COMPLEX_H

#include <gmpxx.h>

#include "basic.h"
#include "number.h"
#include "integer.h"
#include "rational.h"

namespace CSymPy {
//! Complex Class
class Complex : public Number {
public:
	//! `real_` : Real part of the complex Number
    //! `imaginary_` : Imaginary part of the complex Number
    // Complex Number is of the form `real + i(imaginary)`
    mpq_class real_;
    mpq_class imaginary_;

public:
	//! Constructor of Complex class
    Complex(mpq_class real, mpq_class imaginary);
    /*! \param `re` must already be in mpq_class canonical form
        \param `im` must already be in mpq_class canonical form
    *   \return Complex or Rational depending on imaginary part.
    * */
    static RCP<const Number> from_mpq(const mpq_class re, const mpq_class im);
    //! \return true if canonical
    bool is_canonical(const mpq_class &real, const mpq_class &imaginary);
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    //! \return stringify version of `self`s
    virtual std::string __str__() const;

     /*! Constructs Complex from re, im. If im is 0
    *   it will return a Rational instead.
    * */
    static RCP<const Number> from_two_rats(const RCP<const Rational> &re,
        const RCP<const Rational> &im);

    //! \return `true` if `0`
    virtual bool is_zero() const { return ((this->real_ == 0) && (this->imaginary_ == 0)); }
    //! \return `true` if `1`
    virtual bool is_one() const { return ((this->real_ == 1) && (this->imaginary_ == 0)); }
};

} // CSymPy

#endif
