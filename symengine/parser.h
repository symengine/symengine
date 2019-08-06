#ifndef SYMENGINE_PARSER_H
#define SYMENGINE_PARSER_H

#include <symengine/basic.h>

namespace SymEngine
{

void parse(const std::string &s);
RCP<const Basic> parse_old(const std::string &s, bool convert_xor = true);
}

#endif
