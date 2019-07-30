%{
    enum Tokens {
        TOK_IDENTIFIER  =    10,
        TOK_NUMERIC,      // 11
        TOK_IMPLICIT_MUL, // 12
        TOK_EQ,           // 13
        TOK_LE,           // 14
        TOK_GE,           // 15
        TOK_UMINUS,       // 16
        TOK_POW,          // 17
        TOK_NOT,          // 18
        TOK_OPERATOR,     // 19
    };
%}

%option noyywrap

dig [0-9]
char  [\x80-\xff]|[a-zA-Z_]
operators ("-"|"+"|"/"|"("|")"|"*"|","|"^"|"~"|"<"|">"|"&"|"|")

pows      ("**"|"@")
le  ("<=")
ge  (">=")
eqs  ("==")
ident ({char}({char}|{dig})*)
numeric ({dig}*\.?{dig}+([eE][-+]?{dig}+)?)|({dig}+\.)
implicitmul ({numeric}{ident})

%%

{operators}         {
                        /*return matched()[0];*/
                        return TOK_OPERATOR;
                    }
{pows}              {
                        return TOK_POW;
                    }
{le}                {
                        return TOK_LE;
                    }
{ge}                {
                        return TOK_GE;
                    }
{eqs}               {
                        return TOK_EQ;
                    }
{ident}             {
                        /**dval = std::string(matched());*/
                        return TOK_IDENTIFIER;
                    }
{implicitmul}       {
                        /**dval = std::string(matched());*/
                        return TOK_IMPLICIT_MUL;
                    }
{numeric}           {
                        /**dval = std::string(matched());*/
                        return TOK_NUMERIC;
                    }
\n              |
\t              |
" "                 {
                    }
.                   {
                    }

%%

int main()
{
    for (int i=0; i <= 50; i++) {
        int r = yylex();
        if (r == 0) {
            printf("END.\n");
            break;
        }
        printf("TOK: %d: %s\n", r, yytext);
    }
}
