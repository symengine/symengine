#include "sbml_tokenizer.h"
#include "sbml_parser.tab.hh"

namespace SymEngine
{

void SbmlTokenizer::set_string(const std::string &str)
{
    // The input string must be NULL terminated, otherwise the tokenizer will
    // not detect the end of string. After C++11, the std::string is guaranteed
    // to end with \0, but we check this here just in case.
    SYMENGINE_ASSERT(str[str.size()] == '\0');
    cur = (unsigned char *)(&str[0]);
}

int SbmlTokenizer::lex(ParserSType &yylval)
{
    for (;;) {
        tok = cur;
        /*!re2c
            re2c:define:YYCURSOR = cur;
            re2c:define:YYMARKER = mar;
            re2c:yyfill:enable = 0;
            re2c:define:YYCTYPE = "unsigned char";

            end = "\x00";
            whitespace = [ \t\v\n\r]+;
            dig = [0-9];
            char =  [\x80-\xff] | [a-zA-Z_];
            operators = "-"|"+"|"/"|"("|")"|"*"|","|"^"|"<"|">"|"&"|"%"|"!"|"@";

            le = "<=";
            ge = ">=";
            ne = "!=";
            eqs = "==";
            and = "&&";
            or = "||";
            ident = char (char | dig)*;
            numeric = (dig*"."?dig+([eE][-+]?dig+)?) | (dig+".");

            * { throw SymEngine::ParseError("Unknown token: '"+token()+"'"); }
            end { return sbmltokentype::END_OF_FILE; }
            whitespace { continue; }

            // FIXME:
            operators { return tok[0]; }
            le   { return sbmltokentype::LE; }
            ge   { return sbmltokentype::GE; }
            ne   { return sbmltokentype::NE; }
            eqs  { return sbmltokentype::EQ; }
            and  { return sbmltokentype::AND; }
            or   { return sbmltokentype::OR; }
            ident { yylval.string=token(); return sbmltokentype::IDENTIFIER; }
            numeric { yylval.string=token(); return sbmltokentype::NUMERIC; }
        */
    }
}

} // namespace SymEngine
