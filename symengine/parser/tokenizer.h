#ifndef SYMENGINE_TOKENIZER_H
#define SYMENGINE_TOKENIZER_H

#include <string>

#include "parserbase.h"

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
    int lex();
    std::string token();
};

} // namespace SymEngine

#endif
