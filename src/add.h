#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Add : public Basic {
public: // TODO: make this private
    RCP<Number> coef_; // The coefficient (e.g. "2" in 2+x+y)
    umap_basic_int dict_; // the dictionary of the rest (e.g. "x+y" in 2+x+y)

public:
    // Constructs Add from a dictionary by copying the contents of the
    // dictionary:
    Add(const RCP<Number> &coef, const umap_basic_int& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    static RCP<Basic> from_dict(const RCP<Number> &coef, const umap_basic_int &d);
    static void dict_add_term(umap_basic_int &d,
            const RCP<Number> &coef, const RCP<Basic> &t);
    void as_two_terms(const Ptr<RCP<Basic>> &a,
            const Ptr<RCP<Basic>> &b);

    bool is_canonical(const RCP<Number> &coef,
            const umap_basic_int& dict);

    virtual RCP<Basic> diff(const RCP<Symbol> &x) const;
};

RCP<Basic> add(const RCP<Basic> &a,
        const RCP<Basic> &b);

RCP<Basic> sub(const RCP<Basic> &a,
        const RCP<Basic> &b);

RCP<Basic> add_expand(const RCP<Add> &self);

} // CSymPy

#endif
