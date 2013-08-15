#ifndef CSYMPY_SYMBOL_H
#define CSYMPY_SYMBOL_H

#include "basic.h"

namespace CSymPy {

class Symbol : public Basic {
private:
    std::string name_;
    std::size_t hash_;

public:
    Symbol(const std::string &name);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    virtual Teuchos::RCP<Basic> diff(const Teuchos::RCP<Symbol> &x) const;
};

inline Teuchos::RCP<Symbol> symbol(const std::string &name)
{
    return Teuchos::rcp(new Symbol(name));
}

} // CSymPy

#endif
