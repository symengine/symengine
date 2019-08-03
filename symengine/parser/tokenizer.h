#ifndef SYMENGINE_TOKENIZER_H
#define SYMENGINE_TOKENIZER_H

#include <string>

#include "parserbase.h"
#include "parser.tab.hh"

namespace SymEngine
{

class Tokenizer
{
    unsigned char *cur;
    unsigned char *mar;
    unsigned char *tok;

public:
    SymEngine::ParserBase::STYPE__ *val;
    std::string sval;
    void set_string(std::string &str);
    yy::parser::symbol_type lex();
    std::string token();
};

} // namespace SymEngine

#endif
