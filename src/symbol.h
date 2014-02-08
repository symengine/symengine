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
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;
    inline std::string get_name() const {
        return name_;
    }

    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

inline RCP<const Symbol> symbol(const std::string &name)
{
    return rcp(new Symbol(name));
}

} // CSymPy

#endif
