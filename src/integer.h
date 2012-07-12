#ifndef CSYMPY_INTEGER_H
#define CSYMPY_INTEGER_H

#include "basic.h"

namespace CSymPy {

class Integer : public Basic {
private:
    // TODO: This needs to big int, but for now we just use int:
    long long int i;

public:
    Integer(int i);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;
};

} // CSymPy

#endif
