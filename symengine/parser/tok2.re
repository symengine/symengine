#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <istream>
#include <fstream>
#include <memory>

#include "parser.h"


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
    std::string token;

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

static num_t lex(input_t &in)
{
    in.tok = in.cur;
    /*!re2c
        re2c:define:YYCURSOR = in.cur;
        re2c:define:YYMARKER = in.mar;
        re2c:define:YYLIMIT = in.lim;
        re2c:yyfill:enable = 1;
        re2c:define:YYFILL = "if (!in.fill(@@)) return ERR_BUF;";
        re2c:define:YYFILL:naked = 1;
        re2c:define:YYCTYPE = char;

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

        *   { return ERR_UNKNOWN_TOKEN; }
        end {
                if (in.lim - in.tok == YYMAXFILL) {
                    return END;
                } else {
                    return ERR_NULL;
                }
            }
        whitespace { return WS; }

        operators { in.token = *in.tok; return OPERATOR; }
        pows { return POW; }
        le   { return LE; }
        ge   { return GE; }
        eqs  { return EQ; }
        ident { in.token.assign((char*)in.tok, in.cur-in.tok); return IDENTIFIER; }
        numeric { in.token.assign((char*)in.tok, in.cur-in.tok); return NUMERIC; }
        implicitmul { in.token.assign((char*)in.tok, in.cur-in.tok); return IMPLICIT_MUL; }
    */
}

std::unique_ptr<input_t> in;
using SymEngine::Parser;
SymEngine::ParserBase::STYPE__ *dval;

void yy_scan_stream(std::istream &stream)
{
    in = std::make_unique<input_t>(stream);
}

int yylex()
{
    for (;;) {
        num_t t = lex(*in);
        switch (t) {
            case END: return 0;
            case ERR_BUF: printf("ERR BUF.\n"); return 0;
            case ERR_NULL: printf("ERR NULL.\n"); return 0;
            case ERR_UNKNOWN_TOKEN: printf("ERR unknown token\n"); return 0;
            case WS: break;
            case OPERATOR:
                return in->tok[0];
            case POW: return Parser::POW;
            case LE: return Parser::LE;
            case EQ: return Parser::EQ;
            case GE: return Parser::GE;
            case IDENTIFIER:
                *dval = std::string((char*)in->tok, in->cur-in->tok);
                return Parser::IDENTIFIER;
            case NUMERIC:
                *dval = std::string((char*)in->tok, in->cur-in->tok);
                return Parser::NUMERIC;
            case IMPLICIT_MUL:
                *dval = std::string((char*)in->tok, in->cur-in->tok);
                return Parser::IMPLICIT_MUL;
        }
    }
}

/*
int main(int argc, char **argv)
{
    if (argc != 2) {
        printf ("usage: ./a.out <filename>\n");
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);

    input_t in(file);
    for (;;) {
        num_t t = lex(in);
        if (t == END) {
            printf("END.\n");
            break;
        } else if (t == ERR_BUF) {
            printf("ERR BUF.\n");
            break;
        } else if (t == ERR_NULL) {
            printf("ERR NULL.\n");
            break;
        }
        switch (t) {
            case ERR_UNKNOWN_TOKEN: printf("ERR unknown token\n"); break;
            case ERR_NULL: printf("NULL token\n"); break;
            case WS: printf("WS\n"); break;
            case OPERATOR: printf("OPERATOR: %s\n", in.token.c_str()); break;
            case POW: printf("POW\n"); break;
            case LE: printf("LE\n"); break;
            case EQ: printf("EQ\n"); break;
            case GE: printf("GE\n"); break;
            case IDENTIFIER: printf("IDENTIFIER: %s\n", in.token.c_str()); break;
            case NUMERIC: printf("NUMERIC: %s\n", in.token.c_str()); break;
            case IMPLICIT_MUL: printf("IMPLICIT_MUL: %s\n", in.token.c_str()); break;
        }
    }

    return 0;
}*/
