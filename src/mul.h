#ifndef CSYMPY_MUL_H
#define CSYMPY_MUL_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Mul : public Basic {
public:
    Dict_int dict; // TODO: make this private

public:
    // Constructs Mul from a dictionary by copying the contents of the
    // dictionary:
    Mul(const Dict_int& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;
};

} // CSymPy

Teuchos::RCP<CSymPy::Basic> operator*(const Teuchos::RCP<CSymPy::Basic> &a,
        const Teuchos::RCP<CSymPy::Basic> &b);

Teuchos::RCP<CSymPy::Basic> operator/(const Teuchos::RCP<CSymPy::Basic> &a,
        const Teuchos::RCP<CSymPy::Basic> &b);

#endif
