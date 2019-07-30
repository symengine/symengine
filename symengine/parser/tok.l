%{
#include "parser.h"
SymEngine::ParserBase::STYPE__ *dval;
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
                        return SymEngine::Parser::POW;
                    }
{le}                {
                        return SymEngine::Parser::LE;
                    }
{ge}                {
                        return SymEngine::Parser::GE;
                    }
{eqs}               {
                        return SymEngine::Parser::EQ;
                    }
{ident}             {
                        *dval = std::string(yytext);
                        return SymEngine::Parser::IDENTIFIER;
                    }
{implicitmul}       {
                        *dval = std::string(yytext);
                        return SymEngine::Parser::IMPLICIT_MUL;
                    }
{numeric}           {
                        *dval = std::string(yytext);
                        return SymEngine::Parser::NUMERIC;
                    }
\n              |
\t              |
" "                 {
                    }
.                   {
                    }
