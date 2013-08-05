#ifndef CSYMPY_MUL_H
#define CSYMPY_MUL_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Mul : public Basic {
public: // TODO: make this private
    Teuchos::RCP<Basic> coef; // The coefficient (e.g. "2" in 2*x*y)
    Dict_int dict;   // the dictionary of the rest (e.g. "x*y" in 2*x*y)

public:
    // Constructs Mul from a dictionary by copying the contents of the
    // dictionary:
    Mul(const Teuchos::RCP<Basic> &coef, const Dict_int& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;

    void as_coef_term(const Teuchos::Ptr<Teuchos::RCP<Basic>> &coef,
            const Teuchos::Ptr<Teuchos::RCP<Basic>> &term);
    // Performs canonicalization first:
    static Teuchos::RCP<Basic> from_dict(const Teuchos::RCP<Basic> &coef,
            const Dict_int &d);
    static void dict_add_term(Dict_int &d,
            const Teuchos::RCP<Integer> &coef, const Teuchos::RCP<Basic> &t);
};

} // CSymPy

Teuchos::RCP<CSymPy::Basic> operator*(const Teuchos::RCP<CSymPy::Basic> &a,
        const Teuchos::RCP<CSymPy::Basic> &b);

Teuchos::RCP<CSymPy::Basic> operator/(const Teuchos::RCP<CSymPy::Basic> &a,
        const Teuchos::RCP<CSymPy::Basic> &b);

#endif
