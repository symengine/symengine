#include <stdio.h>

enum num_t { ERR, BIN, OCT, DEC, HEX,    OP };

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
        bin end { return BIN; }
        oct end { return OCT; }
        dec end { return DEC; }
        hex end { return HEX; }

        operators { return OP; }
        pows { return POW; }
        le   { return LE; }
        ge   { return GE; }
        eqs  { return EQ; }
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
            case BIN: printf("binary\n"); break;
            case OCT: printf("octal\n"); break;
            case DEC: printf("decimal\n"); break;
            case HEX: printf("hexadecimal\n"); break;
            case OP: printf("OP\n"); break;
        }
    }
    return 0;
}
