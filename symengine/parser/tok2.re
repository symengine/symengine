#include <stdio.h>

enum num_t { ERR, OP, POW, LE, EQ, GE };

static num_t lex(const char *YYCURSOR)
{
    const char *YYMARKER;
    /*!re2c
        re2c:define:YYCTYPE = char;
        re2c:yyfill:enable = 0;

        end = "\x00";
        bin = '0b' [01]+;
        oct = "0" [0-7]*;
        dec = [1-9][0-9]*;
        hex = '0x' [0-9a-fA-F]+;

        dig = [0-9];
        char =  [\x80-\xff]|[a-zA-Z_];
        operators = "-"|"+"|"/"|"("|")"|"*"|","|"^"|"~"|"<"|">"|"&"|"|";

        pows = ("**"|"@");
        le = ("<=");
        ge = (">=");
        eqs = ("==");
        ident = (char(char|dig)*);
        numeric = (dig*\.?dig+([eE][-+]?dig+)?)|(dig+\.);
        implicitmul = (numeric ident);

        *       { return ERR; }
        operators { return OP; }
        pows { return POW; }
        le   { return LE; }
        ge   { return GE; }
        eqs  { return EQ; }
    */
    /*
        bin end { return BIN; }
        oct end { return OCT; }
        dec end { return DEC; }
        hex end { return HEX; }
    */
        //ident { return IDENTIFIER; }
        //implicitmul { return IMPLICIT_MUL; }
        //numeric { return NUMERIC; }
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        switch (lex(argv[i])) {
            case ERR: printf("error\n"); break;
            case OP: printf("OP\n"); break;
            case POW: printf("POW\n"); break;
            case LE: printf("LE\n"); break;
            case EQ: printf("EQ\n"); break;
            case GE: printf("GE\n"); break;
        }
    }
    return 0;
}
