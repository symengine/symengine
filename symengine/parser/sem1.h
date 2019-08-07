#ifndef SYMENGINE_PARSER_SEM1_H
#define SYMENGINE_PARSER_SEM1_H

// Computer 1: 40ms 440ms
// Computer 2: 40ms

#include <string>
#include "symengine/basic.h"
#include "symengine/add.h"
#include "symengine/pow.h"

using SymEngine::add;
using SymEngine::sub;
using SymEngine::mul;
using SymEngine::div;
using SymEngine::pow;
using SymEngine::symbol;
using SymEngine::integer;

#define TYPE SymEngine::RCP<const SymEngine::Basic>
#define ADD(x, y) add(x, y)
#define SUB(x, y) sub(x, y)
#define MUL(x, y) mul(x, y)
#define DIV(x, y) div(x, y)
#define POW(x, y) pow(x, y)
#define SYMBOL(x) symbol(x)
#define INTEGER(x) integer(x)
#define PRINT(x) std::cout << *x << std::endl


#endif
