#ifndef SYMENGINE_TOKENIZER_H
#define SYMENGINE_TOKENIZER_H

#include <string>

struct YYSTYPE;

namespace SymEngine
{

class Tokenizer
{
    unsigned char *cur;
    unsigned char *mar;
    unsigned char *tok;

public:
    /* Set the string to tokenize. The caller must ensure `str` will stay valid
    as long as `lex` is being called. */
    void set_string(const std::string &str);
    int lex(YYSTYPE &yylval);
    std::string token() const;
};

} // namespace SymEngine

#endif
