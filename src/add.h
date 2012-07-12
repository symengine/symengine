#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Add : public Basic {
private:
    std::unordered_map<int, int> dict;

public:
    Add(std::unordered_map<int, int> &dict);
    virtual std::size_t __hash__() const;
};

} // CSymPy

#endif
