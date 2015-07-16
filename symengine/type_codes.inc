#ifndef EXCLUDE_INTEGER
SYMENGINE_ENUM(INTEGER, Integer)
#endif
#ifndef EXCLUDE_RATIONAL
SYMENGINE_ENUM(RATIONAL, Rational)
#endif
#ifndef EXCLUDE_COMPLEX
SYMENGINE_ENUM(COMPLEX, Complex)
#endif
#ifndef EXCLUDE_COMPLEX_DOUBLE
SYMENGINE_ENUM(COMPLEX_DOUBLE, ComplexDouble)
#endif
#ifndef EXCLUDE_REAL_MPFR
SYMENGINE_ENUM(REAL_MPFR, RealMPFR)
#endif
#ifndef EXCLUDE_COMPLEX_MPC
SYMENGINE_ENUM(COMPLEX_MPC, ComplexMPC)
#endif
#ifndef EXCLUDE_REAL_DOUBLE
SYMENGINE_ENUM(REAL_DOUBLE, RealDouble)
#endif
// 'REAL_DOUBLE' returns the number of subclasses of Number.
// All subclasses of Number must be added before it. Do not assign
// non subclasses of Number before it.
#ifndef EXCLUDE_SYMBOL
SYMENGINE_ENUM(SYMBOL, Symbol)
#endif
#ifndef EXCLUDE_MUL
SYMENGINE_ENUM(MUL, Mul)
#endif
#ifndef EXCLUDE_ADD
SYMENGINE_ENUM(ADD, Add)
#endif
#ifndef EXCLUDE_POW
SYMENGINE_ENUM(POW, Pow)
#endif
#ifndef EXCLUDE_UNIVARIATEPOLYNOMIAL
SYMENGINE_ENUM(UNIVARIATEPOLYNOMIAL, UnivariatePolynomial)
#endif
#ifndef EXCLUDE_LOG
SYMENGINE_ENUM(LOG, Log)
#endif
#ifndef EXCLUDE_CONSTANT
SYMENGINE_ENUM(CONSTANT, Constant)
#endif
#ifndef EXCLUDE_SIN
SYMENGINE_ENUM(SIN, Sin)
#endif
#ifndef EXCLUDE_COS
SYMENGINE_ENUM(COS, Cos)
#endif
#ifndef EXCLUDE_TAN
SYMENGINE_ENUM(TAN, Tan)
#endif
#ifndef EXCLUDE_COT
SYMENGINE_ENUM(COT, Cot)
#endif
#ifndef EXCLUDE_CSC
SYMENGINE_ENUM(CSC, Csc)
#endif
#ifndef EXCLUDE_SEC
SYMENGINE_ENUM(SEC, Sec)
#endif
#ifndef EXCLUDE_ASIN
SYMENGINE_ENUM(ASIN, ASin)
#endif
#ifndef EXCLUDE_ACOS
SYMENGINE_ENUM(ACOS, ACos)
#endif
#ifndef EXCLUDE_ASEC
SYMENGINE_ENUM(ASEC, ASec)
#endif
#ifndef EXCLUDE_ACSC
SYMENGINE_ENUM(ACSC, ACsc)
#endif
#ifndef EXCLUDE_ATAN
SYMENGINE_ENUM(ATAN, ATan)
#endif
#ifndef EXCLUDE_ACOT
SYMENGINE_ENUM(ACOT, ACot)
#endif
#ifndef EXCLUDE_ATAN2
SYMENGINE_ENUM(ATAN2, ATan2)
#endif
#ifndef EXCLUDE_SINH
SYMENGINE_ENUM(SINH, Sinh)
#endif
#ifndef EXCLUDE_COSH
SYMENGINE_ENUM(COSH, Cosh)
#endif
#ifndef EXCLUDE_TANH
SYMENGINE_ENUM(TANH, Tanh)
#endif
#ifndef EXCLUDE_COTH
SYMENGINE_ENUM(COTH, Coth)
#endif
#ifndef EXCLUDE_ASINH
SYMENGINE_ENUM(ASINH, ASinh)
#endif
#ifndef EXCLUDE_ACOSH
SYMENGINE_ENUM(ACOSH, ACosh)
#endif
#ifndef EXCLUDE_ATANH
SYMENGINE_ENUM(ATANH, ATanh)
#endif
#ifndef EXCLUDE_ACOTH
SYMENGINE_ENUM(ACOTH, ACoth)
#endif
#ifndef EXCLUDE_ASECH
SYMENGINE_ENUM(ASECH, ASech)
#endif
#ifndef EXCLUDE_LAMBERTW
SYMENGINE_ENUM(LAMBERTW, LambertW)
#endif
#ifndef EXCLUDE_ZETA
SYMENGINE_ENUM(ZETA, Zeta)
#endif
#ifndef EXCLUDE_DIRICHLET_ETA
SYMENGINE_ENUM(DIRICHLET_ETA, Dirichlet_eta)
#endif
#ifndef EXCLUDE_KRONECKERDELTA
SYMENGINE_ENUM(KRONECKERDELTA, KroneckerDelta)
#endif
#ifndef EXCLUDE_LEVICIVITA
SYMENGINE_ENUM(LEVICIVITA, LeviCivita)
#endif
#ifndef EXCLUDE_GAMMA
SYMENGINE_ENUM(GAMMA, Gamma)
#endif
#ifndef EXCLUDE_LOWERGAMMA
SYMENGINE_ENUM(LOWERGAMMA, LowerGamma)
#endif
#ifndef EXCLUDE_UPPERGAMMA
SYMENGINE_ENUM(UPPERGAMMA, UpperGamma)
#endif
#ifndef EXCLUDE_FUNCTIONSYMBOL
SYMENGINE_ENUM(FUNCTIONSYMBOL, FunctionSymbol)
#endif
#ifndef EXCLUDE_FUNCTIONWRAPPER
SYMENGINE_ENUM(FUNCTIONWRAPPER, FunctionWrapper)
#endif
#ifndef EXCLUDE_DERIVATIVE
SYMENGINE_ENUM(DERIVATIVE, Derivative)
#endif
#ifndef EXCLUDE_SUBS
SYMENGINE_ENUM(SUBS, Subs)
#endif
#ifndef EXCLUDE_ABS
SYMENGINE_ENUM(ABS, Abs)
#endif

// The 'TypeID_Count' returns the number of elements in 'TypeID'. For this
// to work, do not assign numbers to the elements above (or if you do, you
// must assign the correct count below).
#ifndef EXCLUDE_TypeID_Count
SYMENGINE_ENUM(TypeID_Count, )
#endif
