#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <istream>
#include <fstream>
#include <memory>

#include "tokenizer.h"
#include "unique.h"


namespace SymEngine {

Tokenizer::Tokenizer() {
}

Tokenizer::~Tokenizer() {
}

void Tokenizer::scan_string(std::string &str) {
    // The input string must be NULL terminated, otherwise the tokenizer will
    // not detect the end of string.
    SYMENGINE_ASSERT(str[str.size()-1] == "\x00");
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

            * {
                std::string s = std::string((char*)tok, cur-tok);
                throw SymEngine::ParseError("Unknown token: '" + s + "'");
            }
            end { return 0; }
            whitespace { continue; }

            operators { return tok[0]; }
            pows { return Parser::POW; }
            le   { return Parser::LE; }
            ge   { return Parser::GE; }
            eqs  { return Parser::EQ; }
            ident {
                *(val) = std::string((char*)tok, cur-tok);
                return Parser::IDENTIFIER;
            }
            numeric {
                *(val) = std::string((char*)tok, cur-tok);
                return Parser::NUMERIC;
            }
            implicitmul {
                *(val) = std::string((char*)tok, cur-tok);
                return Parser::IMPLICIT_MUL;
            }
        */
    }
}


} // namespace SymEngine
