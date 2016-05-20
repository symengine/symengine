/**
 * \file add.h
 * Class Add
 *
 **/

#ifndef SYMENGINE_ADD_H
#define SYMENGINE_ADD_H

#include <symengine/basic.h>
#include <symengine/dict.h>

namespace SymEngine
{

class Add : public Basic
{
public:                      // TODO: make this private
    RCP<const Number> coef_; //! The coefficient (e.g. `2` in `2+x+y`)
    umap_basic_num dict_; //! The dictionary of the rest (e.g. `x+y` in `2+x+y`)

public:
    IMPLEMENT_TYPEID(ADD)
    /*! Constructs Add from a dictionary by copying the contents of the
        dictionary
    */
    Add(const RCP<const Number> &coef, umap_basic_num &&dict);
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
    static RCP<const Basic> from_dict(const RCP<const Number> &coef,
                                      umap_basic_num &&d);
    /*!
    * Adds `(coeff*t)` to the dict `d`
    */
    static void dict_add_term(umap_basic_num &d, const RCP<const Number> &coef,
                              const RCP<const Basic> &t);
    /*!
    * Adds `(c*term)` to the number `coeff` (in case both are numbers) or dict
    * `d` (as a pair `c, term`).
    * In case `term` is `Add` and `c=1`, expands the `Add` into the `coeff` and
    * `d`.
    */
    static void coef_dict_add_term(const Ptr<RCP<const Number>> &coef,
                                   umap_basic_num &d,
                                   const RCP<const Number> &c,
                                   const RCP<const Basic> &term);
    //! Converts the add dict into two appropriate instances
    void as_two_terms(const Ptr<RCP<const Basic>> &a,
                      const Ptr<RCP<const Basic>> &b) const;
    //! Converts into the form of coefficient and term
    static void as_coef_term(const RCP<const Basic> &self,
                             const Ptr<RCP<const Number>> &coef,
                             const Ptr<RCP<const Basic>> &term);
    //! \return `true` if it is in canonical form
    bool is_canonical(const RCP<const Number> &coef,
                      const umap_basic_num &dict) const;

    virtual vec_basic get_args() const;
};

//! \return Add made from `a + b`
RCP<const Basic> add(const RCP<const Basic> &a, const RCP<const Basic> &b);
RCP<const Basic> add(const vec_basic &a);
//! \return Add made from `a - b`
RCP<const Basic> sub(const RCP<const Basic> &a, const RCP<const Basic> &b);
//! \return expanded version of Add
RCP<const Basic> add_expand(const RCP<const Add> &self);

} // SymEngine

#endif
