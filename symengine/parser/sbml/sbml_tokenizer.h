#ifndef SYMENGINE_SBML_TOKENIZER_H
#define SYMENGINE_SBML_TOKENIZER_H

#include <symengine/parser/sbml/sbml_parser_stype.h>

namespace SymEngine
{

class SbmlTokenizer
{
    unsigned char *cur;
    unsigned char *mar;
    unsigned char *tok;

public:
    // Set the string to tokenize. The caller must ensure `str` will stay valid
    // as long as `lex` is being called.
    void set_string(const std::string &str);

    // Get next token. Token ID is returned as function result, the semantic
    // value is put into `yylval`.
    int lex(SBMLSTYPE &yylval);

    // Return the current token
    std::string token() const
    {
        return std::string((char *)tok, cur - tok);
    }
};

} // namespace SymEngine

#endif
