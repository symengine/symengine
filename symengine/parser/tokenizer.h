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
    Tokenizer();
    ~Tokenizer();
    void scan_string(std::string &str);
    int lex();
};

} // namespace SymEngine

#endif
