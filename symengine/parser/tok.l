%{
#include "parser.h"
SymEngine::ParserBase::STYPE__ *dval;
using SymEngine::Parser;
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
                        return yytext[0];
                    }
{pows}              {
                        return Parser::POW;
                    }
{le}                {
                        return Parser::LE;
                    }
{ge}                {
                        return Parser::GE;
                    }
{eqs}               {
                        return Parser::EQ;
                    }
{ident}             {
                        *dval = std::string(yytext);
                        return Parser::IDENTIFIER;
                    }
{implicitmul}       {
                        *dval = std::string(yytext);
                        return Parser::IMPLICIT_MUL;
                    }
{numeric}           {
                        *dval = std::string(yytext);
                        return Parser::NUMERIC;
                    }
\n              |
\t              |
" "                 {
                    }
.                   {
                    }
