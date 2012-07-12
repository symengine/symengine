#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Add : public Basic {
private:
    Dict_int dict;

public:
    // Constructs Add from a dictionary by copying the contents of the
    // dictionary:
    Add(const Dict_int& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
};

} // CSymPy

#endif
