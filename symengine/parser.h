#ifndef SYMENGINE_PARSER_H
#define SYMENGINE_PARSER_H

#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/parser.cpp>

namespace SymEngine {

inline RCP<const Basic> parse(std::string& s) {
	ExpressionParser p;
	return p.parse_expr(s);
}

} // SymEngine

#endif