#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Add : public Basic {
public: // TODO: make this private
    RCP<const Number> coef_; // The coefficient (e.g. "2" in 2+x+y)
    umap_basic_int dict_; // the dictionary of the rest (e.g. "x+y" in 2+x+y)

public:
    // Constructs Add from a dictionary by copying the contents of the
    // dictionary:
    Add(const RCP<const Number> &coef, const umap_basic_int& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    static RCP<const Basic> from_dict(const RCP<const Number> &coef, const umap_basic_int &d);
    static void dict_add_term(umap_basic_int &d,
            const RCP<const Number> &coef, const RCP<const Basic> &t);
    void as_two_terms(const Ptr<RCP<const Basic>> &a,
            const Ptr<RCP<const Basic>> &b) const;
    static void as_coef_term(const RCP<const Basic> &self,
        const Ptr<RCP<const Number>> &coef, const Ptr<RCP<const Basic>> &term);

    bool is_canonical(const RCP<const Number> &coef,
            const umap_basic_int& dict);

    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

RCP<const Basic> add(const RCP<const Basic> &a,
        const RCP<const Basic> &b);

RCP<const Basic> sub(const RCP<const Basic> &a,
        const RCP<const Basic> &b);

RCP<const Basic> add_expand(const RCP<const Add> &self);

} // CSymPy

#endif
