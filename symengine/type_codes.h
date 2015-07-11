/**
 *  \file type_codes.h
 *  Includes the enumeration of TypeID
 *
 **/

#ifndef SYMENGINE_TYPE_CODES_H
#define SYMENGINE_TYPE_CODES_H

typedef enum TypeID {
    INTEGER, RATIONAL, COMPLEX, COMPLEX_DOUBLE, REAL_MPFR, COMPLEX_MPC, REAL_DOUBLE,
    // 'REAL_DOUBLE' returns the number of subclasses of Number.
    // All subclasses of Number must be added before it. Do not assign
    // non subclasses of Number before it.
    SYMBOL, MUL, ADD, POW, UNIVARIATEPOLYNOMIAL, LOG, CONSTANT,
    SIN, COS, TAN, COT, CSC, SEC,
    ASIN, ACOS, ASEC, ACSC, ATAN, ACOT, ATAN2,
    SINH, COSH, TANH, COTH,
    ASINH, ACOSH, ATANH, ACOTH, ASECH,
    LAMBERTW, ZETA, DIRICHLET_ETA, KRONECKERDELTA,
    LEVICIVITA, GAMMA, LOWERGAMMA, UPPERGAMMA,
    FUNCTIONSYMBOL, FUNCTIONWRAPPER, DERIVATIVE, SUBS, ABS,

    // The 'TypeID_Count' returns the number of elements in 'TypeID'. For this
    // to work, do not assign numbers to the elements above (or if you do, you
    // must assign the correct count below).
    TypeID_Count
} TypeID;

#endif //SYMENGINE_TYPE_CODES_H
