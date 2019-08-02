#ifndef SYMENGINE_TOKENIZER_H
#define SYMENGINE_TOKENIZER_H

#include <algorithm>
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
    void set_string(std::string &str);
    int lex();
    std::string token();
};

} // namespace SymEngine

#endif
