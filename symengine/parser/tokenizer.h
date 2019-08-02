#ifndef SYMENGINE_TOKENIZER_H
#define SYMENGINE_TOKENIZER_H

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <algorithm>

#include "parserbase.h"

namespace SymEngine {

class Tokenizer {
public:
    void scan_stream(std::istream &stream);
    int lex();
    SymEngine::ParserBase::STYPE__ *dval;
};

} // SymEngine


#endif
