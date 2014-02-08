#ifndef CSYMPY_MUL_H
#define CSYMPY_MUL_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Mul : public Basic {
public: // TODO: make this private
    RCP<const Number> coef_; // The coefficient (e.g. "2" in 2*x*y)
    map_basic_basic dict_;   // the dictionary of the rest (e.g. "x*y" in 2*x*y)

public:
    // Constructs Mul from a dictionary by copying the contents of the
    // dictionary:
    Mul(const RCP<const Number> &coef, const map_basic_basic& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    // Performs canonicalization first:
    static RCP<const Basic> from_dict(const RCP<const Number> &coef,
            const map_basic_basic &d);
    static void dict_add_term(map_basic_basic &d,
            const RCP<const Basic> &coef, const RCP<const Basic> &t);
    static void as_base_exp(const RCP<const Basic> &self,
            const Ptr<RCP<const Basic>> &exp,
            const Ptr<RCP<const Basic>> &base);
    void as_two_terms(const Ptr<RCP<const Basic>> &a,
            const Ptr<RCP<const Basic>> &b) const;
    RCP<const Basic> power_all_terms(const RCP<const Basic> &exp) const;

    // Returns true if both 'coef' and 'dict' are in canonical form
    bool is_canonical(const RCP<const Number> &coef,
            const map_basic_basic& dict);

    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

RCP<const Basic> mul(const RCP<const Basic> &a,
        const RCP<const Basic> &b);

RCP<const Basic> div(const RCP<const Basic> &a,
        const RCP<const Basic> &b);

RCP<const Basic> neg(const RCP<const Basic> &a);


RCP<const Basic> mul_expand(const RCP<const Mul> &self);

} // CSymPy

#endif
