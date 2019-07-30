%{
#include "tokh.h"
%}

%%

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
                        //return matched()[0];
                        return OP;
                    }
{pows}              {
                        return POW;
                    }
{le}                {
                        return LE;
                    }
{ge}                {
                        return GE;
                    }
{eqs}               {
                        return EQ;
                    }
{ident}             {
                        //*dval = std::string(matched());
                        return IDENTIFIER;
                    }
{implicitmul}       {
                        //*dval = std::string(matched());
                        return IMPLICIT_MUL;
                    }
{numeric}           {
                        //*dval = std::string(matched());
                        return NUMERIC;
                    }
\n              |
\t              |
" "                 {
                    }
.                   {
                    }
