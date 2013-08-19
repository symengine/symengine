#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Add : public Basic {
public: // TODO: make this private
    Teuchos::RCP<Number> coef_; // The coefficient (e.g. "2" in 2+x+y)
    umap_basic_int dict_; // the dictionary of the rest (e.g. "x+y" in 2+x+y)

    // This is calculated from dict_ only when needed:
    mutable map_basic_int ordered_dict_;

public:
    // Constructs Add from a dictionary by copying the contents of the
    // dictionary:
    Add(const Teuchos::RCP<Number> &coef, const umap_basic_int& dict);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    static Teuchos::RCP<Basic> from_dict(const Teuchos::RCP<Number> &coef, const umap_basic_int &d);
    static void dict_add_term(umap_basic_int &d,
            const Teuchos::RCP<Number> &coef, const Teuchos::RCP<Basic> &t);

    bool is_canonical(const Teuchos::RCP<Number> &coef,
            const umap_basic_int& dict);

    virtual Teuchos::RCP<Basic> diff(const Teuchos::RCP<Symbol> &x) const;

    inline const map_basic_int& get_ordered_dict() const {
        if (ordered_dict_.size() == 0)
            ordered_dict_ = map_basic_int(dict_.begin(), dict_.end());
        return ordered_dict_;
    }
};

Teuchos::RCP<Basic> add(const Teuchos::RCP<Basic> &a,
        const Teuchos::RCP<Basic> &b);

Teuchos::RCP<Basic> sub(const Teuchos::RCP<Basic> &a,
        const Teuchos::RCP<Basic> &b);

Teuchos::RCP<Basic> add_expand(const Teuchos::RCP<Add> &self);

} // CSymPy

#endif
