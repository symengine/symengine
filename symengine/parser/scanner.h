#ifndef SYMENGINE_SCANNER_H
#define SYMENGINE_SCANNER_H

#include "scannerbase.h"
#include "parserbase.h"
#include <string>

namespace SymEngine
{

// $insert classHead
class Scanner : public ScannerBase
{
public:
    explicit Scanner(std::istream &in = std::cin,
                     std::ostream &out = std::cout);

    Scanner(std::string const &infile, std::string const &outfile);
    int lex();
    ParserBase::STYPE__ *dval;

private:
    int lex__();
    int executeAction__(size_t ruleNr);
    void print();
    // re-implement this function for code that must
    // be exec'ed before the patternmatching starts
    void preCode();
    // re-implement this function for code that must
    // be exec'ed after the rules's actions.
    void postCode(PostEnum__ type);
};

inline Scanner::Scanner(std::istream &in, std::ostream &out)
    : ScannerBase(in, out)
{
}

inline Scanner::Scanner(std::string const &infile, std::string const &outfile)
    : ScannerBase(infile, outfile)
{
}

inline int Scanner::lex()
{
    return lex__();
}

inline void Scanner::preCode()
{
}

inline void Scanner::postCode(PostEnum__ type)
{
}

inline void Scanner::print()
{
    print__();
}
}

#endif
