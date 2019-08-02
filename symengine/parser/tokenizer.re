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

enum num_t {
    // Unrecognized token. This is caused by a syntax error, unless there is a
    // bug in the tokenizer rules.
    ERR_UNKNOWN_TOKEN,

    // Null character \x00 encountered in the input file (we use it to
    // terminate). With a different SIZE, this Null character might be
    // mis-recognized as the END token. Solution: ensure the file does not
    // contain the null character.
    ERR_NULL,

    // The buffer input_t::buf is not big enough to hold `need` chars. The
    // `need <= YYMAXFILL`, where `YYMAXFILL` is the longest (sequence) of
    // tokens needed to be in the buffer simultaneously.
    // Solution: Increase SIZE.
    // Note: Whenever the buffer fills, the data (all the loaded tokens in the
    // buffer) gets moved in it and we start over. To ensure this happens
    // rarely, make SIZE large enough, for example 64*1024 (64 KB). The amount
    // of data to move is typically low, so this size should be large enough
    // not to be noticeable. One can run a benchmark study what size stops
    // being noticeable for a large file.
    ERR_BUF,

    // We reached the end of input.
    END,
    WS, OPERATOR, POW, LE, EQ, GE, IDENTIFIER, NUMERIC,
    IMPLICIT_MUL};

using SymEngine::Parser;


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

            *   { throw SymEngine::ParseError("Unknown token"); }
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

namespace SymEngine {

Tokenizer::Tokenizer() {
}

Tokenizer::~Tokenizer() {
}

void Tokenizer::scan_string(std::string &str) {
    m_in = SymEngine::make_unique<input_t>(str);
    m_in->val = dval;
}

int Tokenizer::lex() {
    return yylex(*m_in);
}

} // SymEngine
