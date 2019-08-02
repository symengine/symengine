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

struct input_t {
    unsigned char *cur;
    unsigned char *mar;
    unsigned char *tok;

    std::string &str;

    SymEngine::ParserBase::STYPE__ *val;

    input_t(std::string &s)
        : cur((unsigned char*)(&s[0]))
        , str(s)
    {}
};


Tokenizer::Tokenizer() {
}

Tokenizer::~Tokenizer() {
}

void Tokenizer::scan_string(std::string &str) {
    // The input string must be NULL terminated, otherwise the tokenizer will
    // not detect the end of string.
    SYMENGINE_ASSERT(str[str.size()-1] == "\x00");
    m_in = SymEngine::make_unique<input_t>(str);
    m_in->val = dval;
}

int yylex(input_t &in)
{
    for (;;) {
        in.tok = in.cur;
        /*!re2c
            re2c:define:YYCURSOR = in.cur;
            re2c:define:YYMARKER = in.mar;
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
                std::string s = std::string((char*)in.tok, in.cur-in.tok);
                throw SymEngine::ParseError("Unknown token: '" + s + "'");
            }
            end { return 0; }
            whitespace { continue; }

            operators { return in.tok[0]; }
            pows { return Parser::POW; }
            le   { return Parser::LE; }
            ge   { return Parser::GE; }
            eqs  { return Parser::EQ; }
            ident {
                *(in.val) = std::string((char*)in.tok, in.cur-in.tok);
                return Parser::IDENTIFIER;
            }
            numeric {
                *(in.val) = std::string((char*)in.tok, in.cur-in.tok);
                return Parser::NUMERIC;
            }
            implicitmul {
                *(in.val) = std::string((char*)in.tok, in.cur-in.tok);
                return Parser::IMPLICIT_MUL;
            }
        */
    }
}

int Tokenizer::lex() {
    return yylex(*m_in);
}


} // SymEngine
