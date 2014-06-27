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
};

} // CSymPy

#endif
