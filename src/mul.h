/**
 *  \file mul.h
 *  Multiplication class
 *
 **/
#ifndef CSYMPY_MUL_H
#define CSYMPY_MUL_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Mul : public Basic {
public: // TODO: make this private
    RCP<const Number> coef_; //! The coefficient (e.g. `2` in `2*x*y`)
    map_basic_basic dict_;   //! the dictionary of the rest (e.g. `x*y` in `2*x*y`)

public:
    //! Constructs Mul from a dictionary by copying the contents of the
    //! dictionary:
    Mul(const RCP<const Number> &coef, const map_basic_basic& dict);
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version of `self`
    virtual std::string __str__() const;

    // Performs canonicalization first:
    //! Create a Mul from a dict
    static RCP<const Basic> from_dict(const RCP<const Number> &coef,
            const map_basic_basic &d);
    //! Add terms to dict
    static void dict_add_term(map_basic_basic &d,
            const RCP<const Basic> &coef, const RCP<const Basic> &t);
    //! Convert to a base and exponent form
    static void as_base_exp(const RCP<const Basic> &self,
            const Ptr<RCP<const Basic>> &exp,
            const Ptr<RCP<const Basic>> &base);
    //! Rewrite as 2 terms
    /*!
		Example: if this=3*x^2*y^2*z^2`, then `a=x^2` and `b=3*y^2*z^2`
	* */
    void as_two_terms(const Ptr<RCP<const Basic>> &a,
            const Ptr<RCP<const Basic>> &b) const;
    //! Power all terms with the exponent `exp`
    RCP<const Basic> power_all_terms(const RCP<const Basic> &exp) const;

    //! \return true if both `coef` and `dict` are in canonical form
    bool is_canonical(const RCP<const Number> &coef,
            const map_basic_basic& dict);
	//! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};
//! Multiplication
RCP<const Basic> mul(const RCP<const Basic> &a,
        const RCP<const Basic> &b);
//! Division
RCP<const Basic> div(const RCP<const Basic> &a,
        const RCP<const Basic> &b);
//! Negation
RCP<const Basic> neg(const RCP<const Basic> &a);

//! Expand `self`
RCP<const Basic> mul_expand(const RCP<const Mul> &self);

} // CSymPy

#endif
