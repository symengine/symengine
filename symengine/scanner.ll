%filenames="scanner"
%lex-source="scanner.cpp"

dig [0-9]
char  [a-zA-Z_]
operators ("-"|"+"|"/"|"("|")"|"*"|",")
// cannot have function symbols havinig the same name as constants
constants ("e"|"E"|"EulerGamma"|"Catalan"|"GoldenRatio"|"pi"|"I")
pows      ("**"|"^")


%%
//ADD YOUR CODE HERE
// TODO why need typecasting?
{constants}                 {
                                std::cout<<"found constant "<<matched()<<std::endl;
                                *dval = std::string(matched());
                                return Parser::CONSTANT;
                            }

{dig}+                      {
                                std::cout<<"found integer "<<matched()<<std::endl;
                                *dval = integer(integer_class(matched().c_str()));
                                return Parser::INTEGER;
                            }

{operators}                 {
                                std::cout<<"found op "<<matched()<<std::endl;
                                return matched()[0];
                            }

{pows}                      {
                                std::cout<<"found pow "<<matched()<<std::endl;
                                return Parser::POW;
                            }

{char}({char}|{dig})*       {
                                std::cout<<"found idtfier "<<matched()<<std::endl;
                                *dval = std::string(matched());
                                return Parser::IDENTIFIER;
                            }


\n              |
\t              |
" "                         {
                            }

.                           {
                            }
