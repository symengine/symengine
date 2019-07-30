#include <stdio.h>

enum num_t { ERR, BIN, OCT, DEC, HEX };

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

        *       { return ERR; }
        bin end { return BIN; }
        oct end { return OCT; }
        dec end { return DEC; }
        hex end { return HEX; }
    */
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
        }
    }
    return 0;
}
