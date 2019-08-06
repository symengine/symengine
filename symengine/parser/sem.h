#ifndef SYMENGINE_PARSER_SEM_H
#define SYMENGINE_PARSER_SEM_H

#include "symengine/pow.h"

using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::add;
using SymEngine::sub;
using SymEngine::symbol;
using SymEngine::integer;

#define ADD(x, y) add(x, y)
#define SUB(x, y) sub(x, y)
#define MUL(x, y) mul(x, y)
#define DIV(x, y) div(x, y)
#define POW(x, y) pow(x, y)
#define SYMBOL(x) symbol(x)
#define INTEGER(x) integer(x)


#endif
