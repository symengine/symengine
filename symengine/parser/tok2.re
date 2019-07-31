#include <stdio.h>

enum num_t { ERR, OPERATOR, POW, LE, EQ, GE, IDENTIFIER, NUMERIC,
    IMPLICIT_MUL};

static num_t lex(const char *YYCURSOR)
{
    const char *YYMARKER;
    /*!re2c
        re2c:define:YYCTYPE = char;
        re2c:yyfill:enable = 0;

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

        *       { return ERR; }
        operators { return OPERATOR; }
        pows { return POW; }
        le   { return LE; }
        ge   { return GE; }
        eqs  { return EQ; }
        ident { return IDENTIFIER; }
        numeric { return NUMERIC; }
        implicitmul { return IMPLICIT_MUL; }
    */
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        switch (lex(argv[i])) {
            case ERR: printf("error\n"); break;
            case OPERATOR: printf("OPERATOR\n"); break;
            case POW: printf("POW\n"); break;
            case LE: printf("LE\n"); break;
            case EQ: printf("EQ\n"); break;
            case GE: printf("GE\n"); break;
            case IDENTIFIER: printf("IDENTIFIER\n"); break;
            case NUMERIC: printf("NUMERIC\n"); break;
            case IMPLICIT_MUL: printf("IMPLICIT_MUL\n"); break;
        }
    }
    return 0;
}
