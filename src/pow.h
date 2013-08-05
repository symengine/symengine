#ifndef CSYMPY_POW_H
#define CSYMPY_POW_H

#include "basic.h"

namespace CSymPy {

class Pow : public Basic {
public: // TODO: make this private
    Teuchos::RCP<Basic> base, exp; // base^exp

public:
    Pow(const Teuchos::RCP<Basic> &base, const Teuchos::RCP<Basic> &exp);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;

};


void multinomial_coefficients(int m, int n);

} // CSymPy

#endif
