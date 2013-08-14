#ifndef CSYMPY_MUL_H
#define CSYMPY_MUL_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Mul : public Basic {
public: // TODO: make this private
    Teuchos::RCP<Number> coef_; // The coefficient (e.g. "2" in 2*x*y)
    map_basic_basic dict_;   // the dictionary of the rest (e.g. "x*y" in 2*x*y)

public:
    // Constructs Mul from a dictionary by copying the contents of the
    // dictionary:
    Mul(const Teuchos::RCP<Number> &coef, const map_basic_basic& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;

    void as_coef_term(const Teuchos::Ptr<Teuchos::RCP<Number>> &coef,
            const Teuchos::Ptr<Teuchos::RCP<Basic>> &term);
    // Performs canonicalization first:
    static Teuchos::RCP<Basic> from_dict(const Teuchos::RCP<Number> &coef,
            const map_basic_basic &d);
    static void dict_add_term(map_basic_basic &d,
            const Teuchos::RCP<Basic> &coef, const Teuchos::RCP<Basic> &t);
    static void as_base_exp(const Teuchos::RCP<Basic> &self,
            const Teuchos::Ptr<Teuchos::RCP<Basic>> &exp,
            const Teuchos::Ptr<Teuchos::RCP<Basic>> &base);
    void as_two_terms(const Teuchos::Ptr<Teuchos::RCP<Basic>> &a,
            const Teuchos::Ptr<Teuchos::RCP<Basic>> &b);
    Teuchos::RCP<Basic> power_all_terms(const Teuchos::RCP<Basic> &exp);

    // Returns true if both 'coef' and 'dict' are in canonical form
    bool is_canonical(const Teuchos::RCP<Number> &coef,
            const map_basic_basic& dict);

    virtual Teuchos::RCP<Basic> diff(const Teuchos::RCP<Symbol> &x) const;
};

Teuchos::RCP<Basic> mul(const Teuchos::RCP<Basic> &a,
        const Teuchos::RCP<Basic> &b);

Teuchos::RCP<Basic> div(const Teuchos::RCP<Basic> &a,
        const Teuchos::RCP<Basic> &b);


Teuchos::RCP<Basic> mul_expand(const Teuchos::RCP<Mul> &self);

} // CSymPy

#endif
