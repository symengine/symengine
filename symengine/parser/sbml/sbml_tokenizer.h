#ifndef SYMENGINE_SBML_TOKENIZER_H
#define SYMENGINE_SBML_TOKENIZER_H

#include <symengine/parser/parser_stype.h>
#include <symengine/parser/tokenizer.h>

namespace SymEngine
{

class SbmlTokenizer : public Tokenizer
{
public:
    int lex(ParserSType &yylval);
};

} // namespace SymEngine

#endif
