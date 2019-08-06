#ifndef SYMENGINE_PARSER_STYPE_H
#define SYMENGINE_PARSER_STYPE_H

#include <string>
#include "symengine/basic.h"

namespace SymEngine
{

struct YYSTYPE {
    //SymEngine::RCP<const SymEngine::Basic> basic;
    int basic;
    std::string string;
    // Constructor
    YYSTYPE() = default;
    // Destructor
    ~YYSTYPE() = default;
    // Copy constructor and assignment
    YYSTYPE(const YYSTYPE &) = default;
    YYSTYPE &operator=(const YYSTYPE &) = default;
    // Move constructor and assignment
    YYSTYPE(YYSTYPE &&) = default;
    YYSTYPE &operator=(YYSTYPE &&) = default;
};

} // namespace SymEngine

/*
#include "symengine/add.h"
#include "symengine/pow.h"

using SymEngine::add;
using SymEngine::sub;
using SymEngine::mul;
using SymEngine::div;
using SymEngine::pow;
using SymEngine::symbol;
using SymEngine::integer;

#define ADD(x, y) add(x, y)
#define SUB(x, y) sub(x, y)
#define MUL(x, y) mul(x, y)
#define DIV(x, y) div(x, y)
#define POW(x, y) pow(x, y)
#define SYMBOL(x) symbol(x)
#define INTEGER(x) integer(x)
*/

#define ADD(x, y) x+y
#define SUB(x, y) x-y
#define MUL(x, y) x*y
#define DIV(x, y) x/y
#define POW(x, y) std::pow(x,y)
#define SYMBOL(x) 3
#define INTEGER(x) std::stoi(x)
#define PRINT(x) std::cout << x << std::endl

#endif
