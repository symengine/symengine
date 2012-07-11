#ifndef CSYMPY_SYMBOL_H
#define CSYMPY_SYMBOL_H

#include <string>

#include "basic.h"

namespace CSymPy {

class Symbol : public Basic {
private:
    std::string name;

public:
    Symbol(const std::string &name);
};

} // CSymPy

#endif
