#ifndef SYMENGINE_PARSER_STYPE_H
#define SYMENGINE_PARSER_STYPE_H

#include <string>
#include "symengine/basic.h"

namespace SymEngine
{

struct ParserSType {
    SymEngine::RCP<const SymEngine::Basic> basic;
    SymEngine::vec_basic basic_vec;
    std::string string;
    // Constructor
    ParserSType() = default;
    // Destructor
    ~ParserSType() = default;
    // Copy constructor and assignment
    ParserSType(const ParserSType &) = default;
    ParserSType &operator=(const ParserSType &) = default;
    // Move constructor and assignment
    ParserSType(ParserSType &&) = default;
    ParserSType &operator=(ParserSType &&) = default;
};

} // namespace SymEngine

#endif
