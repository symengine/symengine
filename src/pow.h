#ifndef CSYMPY_POW_H
#define CSYMPY_POW_H

#include "basic.h"
#include "dict.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {

class Pow : public Basic {
public: // TODO: make this private
    Teuchos::RCP<Basic> base_, exp_; // base^exp

public:
    Pow(const Teuchos::RCP<Basic> &base, const Teuchos::RCP<Basic> &exp);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;

    bool is_canonical(const Teuchos::RCP<Basic> &base,
            const Teuchos::RCP<Basic> &exp);

    virtual Teuchos::RCP<Basic> diff(const Teuchos::RCP<Symbol> &x) const;
};

Teuchos::RCP<Basic> pow(const Teuchos::RCP<Basic> &a,
        const Teuchos::RCP<Basic> &b);


void multinomial_coefficients(int m, int n, map_vec_int &r);

Teuchos::RCP<Basic> pow_expand(const Teuchos::RCP<Pow> &self);

inline Teuchos::RCP<Basic> sqrt(const Teuchos::RCP<Basic> &x) {
    return pow(x, div(one, integer(2)));
}

} // CSymPy

#endif
