%filenames="scanner"
%lex-source="scanner.cpp"

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
                        return matched()[0];
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
                        *dval = std::string(matched());
                        return Parser::IDENTIFIER;
                    }
{implicitmul}       {
                        *dval = std::string(matched());
                        return Parser::IMPLICIT_MUL;
                    }
{numeric}           {
                        *dval = std::string(matched());
                        return Parser::NUMERIC;
                    }
\n              |
\t              |
" "                 {
                    }
.                   {
                    }
