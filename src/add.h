/**
 * \file add.h
 * Class Add
 * 
 **/

#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Add : public Basic {
public: // TODO: make this private
    RCP<const Number> coef_; //! The coefficient (e.g. `2` in `2+x+y`)
    umap_basic_num dict_; //! The dictionary of the rest (e.g. `x+y` in `2+x+y`)

public:
    IMPLEMENT_TYPEID(ADD)
    /*! Constructs Add from a dictionary by copying the contents of the
        dictionary
    */
    Add(const RCP<const Number> &coef, umap_basic_num&& dict);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    /*! Comparison operator
     * \param o - Object to be compared with
     * \return `0` if equal, `-1` , `1` according to string compare
     * */
    virtual int compare(const Basic &o) const;
    /*! Creates appropriate instance (i.e Add , Symbol, Integer,
    * Mul) depending on the size of dictionary `d`.
    */
    static RCP<const Basic> from_dict(const RCP<const Number> &coef, umap_basic_num &&d);
    /*!
    * Adds `(coeff*t)` to the dict `d`
    */
    static void dict_add_term(umap_basic_num &d,
            const RCP<const Number> &coef, const RCP<const Basic> &t);
    //! Converts the add dict into two appropriate instances
    void as_two_terms(const Ptr<RCP<const Basic>> &a,
            const Ptr<RCP<const Basic>> &b) const;
    //! Converts into the form of coefficient and term 
    static void as_coef_term(const RCP<const Basic> &self,
        const Ptr<RCP<const Number>> &coef, const Ptr<RCP<const Basic>> &term);
    //! \return `true` if it is in canonical form
    bool is_canonical(const RCP<const Number> &coef,
            const umap_basic_num& dict);
    //! Differentiates w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! Substitutes the dict
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;

    virtual vec_basic get_args() const;

    virtual void accept(Visitor &v) const;
};

//! \return Add made from `a + b`
RCP<const Basic> add(const RCP<const Basic> &a,
        const RCP<const Basic> &b);
//! \return Add made from `a - b`
RCP<const Basic> sub(const RCP<const Basic> &a,
        const RCP<const Basic> &b);
//! \return expanded version of Add
RCP<const Basic> add_expand(const RCP<const Add> &self);

} // CSymPy

#endif
