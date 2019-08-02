#ifndef SYMENGINE_TOKENIZER_H
#define SYMENGINE_TOKENIZER_H

#include <algorithm>
#include <istream>

#include "parserbase.h"

struct input_t;

namespace SymEngine
{

class Tokenizer
{
    std::unique_ptr<input_t> m_in;

public:
    SymEngine::ParserBase::STYPE__ *dval;
    Tokenizer();
    ~Tokenizer();
    void scan_stream(std::istream &stream);
    int lex();
};

} // namespace SymEngine

#endif
