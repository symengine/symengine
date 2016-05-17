/**
 *  \file mul.h
 *  Multiplication class
 *
 **/
#ifndef SYMENGINE_MUL_H
#define SYMENGINE_MUL_H

#include <symengine/basic.h>
#include <symengine/dict.h>

namespace SymEngine
{

class Mul : public Basic
{
public:                      // TODO: make this private
    RCP<const Number> coef_; //! The coefficient (e.g. `2` in `2*x*y`)
    map_basic_basic
        dict_; //! the dictionary of the rest (e.g. `x*y` in `2*x*y`)

public:
    IMPLEMENT_TYPEID(MUL)
    //! Constructs Mul from a dictionary by copying the contents of the
    //! dictionary:
    Mul(const RCP<const Number> &coef, map_basic_basic &&dict);
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;

    // Performs canonicalization first:
    //! Create a Mul from a dict
    static RCP<const Basic> from_dict(const RCP<const Number> &coef,
                                      map_basic_basic &&d);
    //! Add terms to dict
    static void dict_add_term(map_basic_basic &d, const RCP<const Basic> &exp,
                              const RCP<const Basic> &t);
    static void dict_add_term_new(const Ptr<RCP<const Number>> &coef,
                                  map_basic_basic &d,
                                  const RCP<const Basic> &exp,
                                  const RCP<const Basic> &t);
    //! Convert to a base and exponent form
    static void as_base_exp(const RCP<const Basic> &self,
                            const Ptr<RCP<const Basic>> &exp,
                            const Ptr<RCP<const Basic>> &base);
    //! Rewrite as 2 terms
    /*!
        Example: if this=3*x**2*y**2*z**2`, then `a=x**2` and `b=3*y**2*z**2`
    * */
    void as_two_terms(const Ptr<RCP<const Basic>> &a,
                      const Ptr<RCP<const Basic>> &b) const;
    //! Power all terms with the exponent `exp`
    void power_num(const Ptr<RCP<const Number>> &coef, map_basic_basic &d,
                   const RCP<const Number> &exp) const;

    //! \return true if both `coef` and `dict` are in canonical form
    bool is_canonical(const RCP<const Number> &coef,
                      const map_basic_basic &dict) const;

    virtual vec_basic get_args() const;
};
//! Multiplication
RCP<const Basic> mul(const RCP<const Basic> &a, const RCP<const Basic> &b);
RCP<const Basic> mul(const vec_basic &a);
//! Division
RCP<const Basic> div(const RCP<const Basic> &a, const RCP<const Basic> &b);
//! Negation
RCP<const Basic> neg(const RCP<const Basic> &a);

//! Expand `self`
RCP<const Basic> mul_expand(const RCP<const Mul> &self);

} // SymEngine

#endif
