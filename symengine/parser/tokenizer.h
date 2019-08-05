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
    std::string sval;
    void set_string(std::string &str);
    int lex(YYSTYPE &yylval);
    std::string token();
};

} // namespace SymEngine

#endif
