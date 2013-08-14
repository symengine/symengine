#ifndef CSYMPY_SYMBOL_H
#define CSYMPY_SYMBOL_H

#include "basic.h"

namespace CSymPy {

class Symbol : public Basic {
private:
    std::string name_;

public:
    Symbol(const std::string &name);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;
};

inline Teuchos::RCP<Symbol> symbol(const std::string &name)
{
    return Teuchos::rcp(new Symbol(name));
}

} // CSymPy

#endif
