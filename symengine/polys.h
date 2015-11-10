/**
 *  \file polys.h
 *  Class for sparse Polynomial: Polynomial
 *
 **/
#ifndef SYMENGINE_POLYS_H
#define SYMENGINE_POLYS_H

#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/symbol.h>
#include <unordered_set>

namespace SymEngine {

#ifdef HAVE_SYMENGINE_PIRANHA
    typedef hash_set polys_set;
#else
    typedef unordered_set polys_set;
#endif

//! Polynomial Class
class Polynomial : public Basic{
public:
    //! `vars_` : Variables of the Polynomial
    //! `polys_set_` : holds the Polynomial
    vec_symbol vars_;
    polys_set polys_set_;

public:
    IMPLEMENT_TYPEID(POLYNOMIAL)

    //! Constructor of Polynomial class
    Polynomial(const vec_symbol &vars, polys_set the_set);
    static RCP<const Polynomial> create(const vec_symbol &vars, polys_set&& the_set) {
        return make_rcp<const Polynomial>(vars, std::move(the_set));
    }
    //! \return true if canonical
    bool is_canonical(const polys_set& set);

    //! Constructor from Basic
    Polynomial(const RCP<const Basic> &p, umap_basic_num &vars);

    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    //! Evaluates the Polynomial at value x
    mpz_class eval(const vec_int &x) const;
    //! Differentiates w.r.t symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;

    //! Get the Basic from the Polynomial
    RCP<const Basic> get_basic() const;

    virtual vec_basic get_args() const;

    virtual void accept(Visitor &v) const;

}; //Polynomial

//! Adding two Polynomial a and b
RCP<const Polynomial> add_poly(const Polynomial &a, const Polynomial &b);
//! Negative of a Polynomial
RCP<const Polynomial> neg_poly(const Polynomial &a);
//! Subtracting two Polynomial a and b
RCP<const Polynomial> sub_poly(const Polynomial &a, const Polynomial &b);
//! Multiplying two Polynomial a and b
RCP<const Polynomial> mul_poly(const Polynomial &a, const Polynomial &b);

inline RCP<const Polynomial> polynomial(const vec_symbol &vars, polys_set the_set)
{
    return make_rcp<const Polynomial>(vars, std::move(the_set));
}

}  //SymEngine

#endif
