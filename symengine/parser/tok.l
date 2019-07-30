%{
    enum Tokens {
        TOK_IDENTIFIER,
        TOK_NUMERIC,
        TOK_IMPLICIT_MUL,
        TOK_EQ,
        TOK_LE,
        TOK_GE,
        TOK_UMINUS,
        TOK_POW,
        TOK_NOT,
        TOK_OPERATOR,
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
