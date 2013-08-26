#ifndef CSYMPY_MUL_H
#define CSYMPY_MUL_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Mul : public Basic {
public: // TODO: make this private
    RCP<Number> coef_; // The coefficient (e.g. "2" in 2*x*y)
    map_basic_basic dict_;   // the dictionary of the rest (e.g. "x*y" in 2*x*y)

public:
    // Constructs Mul from a dictionary by copying the contents of the
    // dictionary:
    Mul(const RCP<Number> &coef, const map_basic_basic& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    void as_coef_term(const Ptr<RCP<Number>> &coef,
            const Ptr<RCP<Basic>> &term);
    // Performs canonicalization first:
    static RCP<Basic> from_dict(const RCP<Number> &coef,
            const map_basic_basic &d);
    static void dict_add_term(map_basic_basic &d,
            const RCP<Basic> &coef, const RCP<Basic> &t);
    static void as_base_exp(const RCP<Basic> &self,
            const Ptr<RCP<Basic>> &exp,
            const Ptr<RCP<Basic>> &base);
    void as_two_terms(const Ptr<RCP<Basic>> &a,
            const Ptr<RCP<Basic>> &b);
    RCP<Basic> power_all_terms(const RCP<Basic> &exp);

    // Returns true if both 'coef' and 'dict' are in canonical form
    bool is_canonical(const RCP<Number> &coef,
            const map_basic_basic& dict);

    virtual RCP<Basic> diff(const RCP<Symbol> &x) const;
    virtual RCP<Basic> subs(const map_basic_basic &subs_dict) const;
};

RCP<Basic> mul(const RCP<Basic> &a,
        const RCP<Basic> &b);

RCP<Basic> div(const RCP<Basic> &a,
        const RCP<Basic> &b);

RCP<Basic> neg(const RCP<Basic> &a);


RCP<Basic> mul_expand(const RCP<Mul> &self);

} // CSymPy

#endif
