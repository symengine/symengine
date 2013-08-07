#ifndef CSYMPY_INTEGER_H
#define CSYMPY_INTEGER_H

#include <gmpxx.h>

#include "basic.h"

namespace CSymPy {

class Integer : public Basic {
public:
    // TODO: make this private
    long long int i;
    mpz_class ii;

public:
    Integer(int i);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;
};

} // CSymPy

Teuchos::RCP<CSymPy::Integer> operator+(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b);

Teuchos::RCP<CSymPy::Integer> operator-(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b);

Teuchos::RCP<CSymPy::Integer> operator*(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b);

Teuchos::RCP<CSymPy::Integer> operator/(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b);

Teuchos::RCP<CSymPy::Integer> operator-(const Teuchos::RCP<CSymPy::Integer> &a);

#endif
