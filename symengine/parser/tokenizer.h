#ifndef SYMENGINE_TOKENIZER_H
#define SYMENGINE_TOKENIZER_H

#include <algorithm>
#include <string>

#include "parserbase.h"


namespace SymEngine
{

struct input_t;

class Tokenizer
{
    std::unique_ptr<input_t> m_in;
public:
    SymEngine::ParserBase::STYPE__ *dval;
    Tokenizer();
    ~Tokenizer();
    void scan_string(std::string &str);
    int lex();
};

} // namespace SymEngine

#endif
