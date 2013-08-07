#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Add : public Basic {
public: // TODO: make this private
    Teuchos::RCP<Basic> coef; // The coefficient (e.g. "2" in 2+x+y)
    Dict_int dict;

public:
    // Constructs Add from a dictionary by copying the contents of the
    // dictionary:
    Add(const Dict_int& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;

    static Teuchos::RCP<Basic> from_dict(const Dict_int &d);
    static void dict_add_term(Dict_int &d,
            const Teuchos::RCP<Integer> &coef, const Teuchos::RCP<Basic> &t);
};

Teuchos::RCP<Basic> add(const Teuchos::RCP<Basic> &a,
        const Teuchos::RCP<Basic> &b);

} // CSymPy

Teuchos::RCP<CSymPy::Basic> operator+(const Teuchos::RCP<CSymPy::Basic> &a,
        const Teuchos::RCP<CSymPy::Basic> &b);

Teuchos::RCP<CSymPy::Basic> operator-(const Teuchos::RCP<CSymPy::Basic> &a,
        const Teuchos::RCP<CSymPy::Basic> &b);

#endif
