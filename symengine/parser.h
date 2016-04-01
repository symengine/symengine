#ifndef SYMENGINE_PARSER_H
#define SYMENGINE_PARSER_H

#include <symengine/basic.h>
#include <symengine/dict.h>

namespace SymEngine
{

RCP<const Basic> parse(const std::string &s);

} // SymEngine

#endif