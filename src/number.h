#ifndef CSYMPY_NUMBER_H
#define CSYMPY_NUMBER_H

#include "basic.h"

namespace CSymPy {

class Number : public Basic {
public:
    virtual bool is_zero() const = 0;
    virtual bool is_one() const = 0;
};

} // CSymPy

#endif
