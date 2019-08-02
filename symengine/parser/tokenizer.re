#include "tokenizer.h"

namespace SymEngine {

void Tokenizer::set_string(std::string &str) {
    // The input string must be NULL terminated, otherwise the tokenizer will
    // not detect the end of string. After C++, the std::string is guaranteed
    // to end with \0, but we check this here just in case.
    SYMENGINE_ASSERT(str[str.size()] == '\0');
    cur = (unsigned char*)(&str[0]);
}

int Tokenizer::lex()
{
    for (;;) {
        tok = cur;
        /*!re2c
            re2c:define:YYCURSOR = cur;
            re2c:define:YYMARKER = mar;
            re2c:yyfill:enable = 0;
            re2c:define:YYCTYPE = "unsigned char";

            end = "\x00";
            whitespace = [ \t\v\n\r]+;
            dig = [0-9];
            char =  [\x80-\xff] | [a-zA-Z_];
            operators = "-"|"+"|"/"|"("|")"|"*"|","|"^"|"~"|"<"|">"|"&"|"|";

            pows = "**"|"@";
            le = "<=";
            ge = ">=";
            eqs = "==";
            ident = char (char | dig)*;
            numeric = (dig*"."?dig+([eE][-+]?dig+)?) | (dig+".");
            implicitmul = numeric ident;

            * { throw SymEngine::ParseError("Unknown token: '"+token()+"'"); }
            end { return 0; }
            whitespace { continue; }

            operators { return tok[0]; }
            pows { return Parser::POW; }
            le   { return Parser::LE; }
            ge   { return Parser::GE; }
            eqs  { return Parser::EQ; }
            ident { *(val) = token(); return Parser::IDENTIFIER; }
            numeric { *(val) = token(); return Parser::NUMERIC; }
            implicitmul { *(val) = token(); return Parser::IMPLICIT_MUL; }
        */
    }
}

std::string Tokenizer::token() {
    return std::string((char*)tok, cur-tok);
}

} // namespace SymEngine
