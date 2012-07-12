#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Add : public Basic {
private:
    Dict_int dict;

public:
    Add(Dict_int& dict);
    virtual std::size_t __hash__() const;
};

} // CSymPy

#endif
