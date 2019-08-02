#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <istream>
#include <fstream>
#include <memory>

#include "parser.h"
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

/*!max:re2c*/
static const size_t SIZE = 64*1024;

size_t sread(char *ptr, size_t size, size_t count, std::istream &stream)
{
    stream.read(ptr, count);
    return stream.gcount();
}

struct input_t {
    unsigned char buf[SIZE + YYMAXFILL];
    unsigned char *lim;
    unsigned char *cur;
    unsigned char *mar;
    unsigned char *tok;
    bool eof;

    std::istream &file;

    input_t(std::istream &f)
        : buf()
        , lim(buf + SIZE)
        , cur(lim)
        , mar(lim)
        , tok(lim)
        , eof(false)
        , file(f)
    {}
    bool fill(size_t need)
    {
        if (eof) {
            return false;
        }
        const size_t free = tok - buf;
        if (free < need) {
            return false;
        }
        memmove(buf, tok, lim - tok);
        lim -= free;
        cur -= free;
        mar -= free;
        tok -= free;
        lim += sread((char*)lim, 1, free, file);
        if (lim < buf + SIZE) {
            eof = true;
            memset(lim, 0, YYMAXFILL);
            lim += YYMAXFILL;
        }
        return true;
    }
};

std::unique_ptr<input_t> in;
using SymEngine::Parser;
SymEngine::ParserBase::STYPE__ *dval;

int yylex()
{
    for (;;) {
        in->tok = in->cur;
        /*!re2c
            re2c:define:YYCURSOR = in->cur;
            re2c:define:YYMARKER = in->mar;
            re2c:define:YYLIMIT = in->lim;
            re2c:yyfill:enable = 1;
            re2c:define:YYFILL = "if (!in->fill(@@)) return 0;";
            re2c:define:YYFILL:naked = 1;
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

            *   { printf("ERR unknown token\n"); return 0; }
            end {
                    if (in->lim - in->tok == YYMAXFILL) {
                        return 0;
                    } else {
                        printf("ERR NULL.\n");
                        return 0;
                    }
                }
            whitespace { continue; }

            operators { return in->tok[0]; }
            pows { return Parser::POW; }
            le   { return Parser::LE; }
            ge   { return Parser::GE; }
            eqs  { return Parser::EQ; }
            ident {
                *dval = std::string((char*)in->tok, in->cur-in->tok);
                return Parser::IDENTIFIER;
            }
            numeric {
                *dval = std::string((char*)in->tok, in->cur-in->tok);
                return Parser::NUMERIC;
            }
            implicitmul {
                *dval = std::string((char*)in->tok, in->cur-in->tok);
                return Parser::IMPLICIT_MUL;
            }
        */
    }
}

void yy_scan_stream(std::istream &stream)
{
    in = SymEngine::make_unique<input_t>(stream);
}

