#ifndef CSYMPY_BASIC_H
#define CSYMPY_BASIC_H

namespace CSymPy {

class Basic {
public:
    virtual std::size_t __hash__() const = 0;
};

} // CSymPy

#endif
