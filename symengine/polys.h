/**
 *  \file polys.h
 *  Class for sparse Polynomial: Polynomial
 *
 **/
#ifndef SYMENGINE_POLYS_H
#define SYMENGINE_POLYS_H

#include <symengine/basic.h>
#include <symengine/dict.h>

namespace SymEngine {
//! Polynomial Class
class Polynomial : public Basic{
public:
    //! `vars_` : Variables of the Polynomial
    //! `polys_set_` : holds the Polynomial
    vec_symbol vars_;
    hash_set polys_set_;
public:
    IMPLEMENT_TYPEID(POLYNOMIAL)
    //! Constructor of Polynomial class
    Polynomial(const vec_symbol &vars, hash_set&& polys_set);
    //! Constructor from Basic
    Polynomial(const RCP<const Basic> &p, umap_basic_num &vars);

    static RCP<const Polynomial> create(const vec_symbol &vars, hash_set &&polys_set) {
        return make_rcp<const Polynomial>(vars, polys_set);
    }

    //! \return true if canonical
    bool is_canonical(const hash_set& set);
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
RCP<const Polynomial> mul_poly(RCP<const Polynomial> a, RCP<const Polynomial> b);

inline RCP<const Polynomial> polynomial(const vec_symbol &vars, hash_set &&polys_set)
{
    return make_rcp<const Polynomial>(vars, std::move(polys_set));
}

}  //SymEngine

#endif
