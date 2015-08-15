#ifndef SYMENGINE_H_
#define SYMENGINE_H_

#include "ruby.h"

//variable name for a module starts with m
VALUE m_symengine;

//variable names for classes begin with c
VALUE c_basic;
VALUE c_symbol;
VALUE c_integer;
VALUE c_rational;
VALUE c_function;
VALUE c_trigfunction;
VALUE c_sin;
VALUE c_cos;
VALUE c_tan;
VALUE c_cot;
VALUE c_csc;
VALUE c_sec;

#endif //SYMENGINE_H_
