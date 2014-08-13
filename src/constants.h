/**
 *  \file constants.h
 *  Declare all the special constants in this file
 *
 **/

#ifndef CSYMPY_CONSTANTS_H
#define CSYMPY_CONSTANTS_H

#include "basic.h"
#include "number.h"
#include "integer.h"

namespace CSymPy {

extern RCP<const Integer> zero;
extern RCP<const Integer> one;
extern RCP<const Integer> minus_one;
extern RCP<const Number> I;

} // CSymPy

#endif