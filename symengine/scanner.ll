%filenames="scanner"
%lex-source="scanner.cpp"

dig [0-9]
char  [a-zA-Z_]
operators ("-"|"+"|"/"|"("|")"|"*"|",")
// TODO : cannot have function symbols havinig the same name as constants
constants ("e"|"E"|"EulerGamma"|"Catalan"|"GoldenRatio"|"pi"|"I")
pows      ("**"|"^")


%%
//ADD YOUR CODE HERE

{constants}                         {
                                        *dval = std::string(matched());
                                        return Parser::CONSTANT;
                                    }

{dig}+                              {
                                        *dval = std::string(matched());
                                        return Parser::INTEGER;
                                    }

{dig}+[eE][+-]?{dig}+             |
{dig}+\.{dig}*([eE][+-]?{dig}+)?  |
{dig}*\.{dig}+([eE][+-]?{dig}+)?    {
                                        *dval = std::string(matched());
                                        return Parser::DOUBLE;
                                    }

{operators}                         {
                                        return matched()[0];
                                    }

{pows}                              {
                                        return Parser::POW;
                                    }

{char}({char}|{dig})*               {
                                        *dval = std::string(matched());
                                        return Parser::IDENTIFIER;
                                    }

\n              |
\t              |
" "                                 {
                                    }

.                                   {
                                    }
