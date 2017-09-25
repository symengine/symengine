/**
 * \file add.h
 * Header containing definition of Add and related functions add, sub
 *
 **/

#ifndef SYMENGINE_ADD_H
#define SYMENGINE_ADD_H

#include <symengine/basic.h>

namespace SymEngine
{
/*! \class Add
   Add class keeps an addition of symbolic expressions. Internal representation
   of an Add is a numeric coefficient `coef_` and a dictionary `dict_` of
   key-value pairs.

        Add(coef_, {{key1, value1}, {key2, value2}, ... })

   This represents the following expression,

        coef_ + key1*value1 + key2*value2 + ...

   `coef_` and the values of dictionary are numeric coefficients like Integer,
   RealDouble, Complex
   `key`s can be any symbolic expression except numeric coefficients and `Mul`
   objects with coefficient != 1.

   For example, the following are valid representations

        Add(1, {{x, 2}, {y, 5}})
        Add(0, {{x, 1}, {y, 4}, {z, 3}})

   Following are invalid representations. (valid equivalent is shown next to
   them)

        Add(1, {{x, 1}, {2*y, 3})   -> Add(1, {{x, 1}, {y, 6}})
        Add(0, {{x, 2}})             -> Mul(2, {{x, 1}})
        Add(1, {{x, 2}, {4, 6}})    -> Add(25, {{x, 2}})
*/
class Add : public Basic
{
private:
    RCP<const Number> coef_; //! The coefficient (e.g. `2` in `2+x+y`)
    umap_basic_num dict_; //! The dictionary of the rest (e.g. `x+y` in `2+x+y`)

public:
    IMPLEMENT_TYPEID(ADD)
    /*! Constructs Add from a dictionary by copying the contents of the
        dictionary. Assumes that the input is in canonical form
    */
    Add(const RCP<const Number> &coef, umap_basic_num &&dict);
    virtual hash_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
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
    //! Converts the Add into a sum of two Basic objects and returns them.
    void as_two_terms(const Ptr<RCP<const Basic>> &a,
                      const Ptr<RCP<const Basic>> &b) const;
    //! Converts a Basic `self` into the form of `coefficient * term`
    static void as_coef_term(const RCP<const Basic> &self,
                             const Ptr<RCP<const Number>> &coef,
                             const Ptr<RCP<const Basic>> &term);
    //! \return `true` if a given dictionary and a coefficient is in canonical
    //! form
    bool is_canonical(const RCP<const Number> &coef,
                      const umap_basic_num &dict) const;

    /*!
        Returns the arguments of the Add.
        For an Add of the form,

            Add(coef_, {{key1, value1}, {key2, value2}, ... })

        if coef_ is non-zero it returns,

            {coef_, key1*value1, key2*value2, ... }

        otherwise it returns,

            {key1*value1, key2*value2, ... }

        \return list of arguments
    */
    virtual vec_basic get_args() const;

    //! \return const reference to the coefficient of the Add
    inline const RCP<const Number> &get_coef() const
    {
        return coef_;
    }
    //! \return const reference to the dictionary of the Add
    inline const umap_basic_num &get_dict() const
    {
        return dict_;
    }
};

/*!
    Add the Basic classes `a` and `b`
    This'll return the most appropriate type.
    For example if `x` and `y` are symbols,

        x + y will return an Add
        x + x will return a Mul (2*x)

    \return `a + b`
    \see Add, Mul
*/
RCP<const Basic> add(const RCP<const Basic> &a, const RCP<const Basic> &b);
/*!
    Sums the elements of a vector. For `n` elements, this method should be
    faster than doing `n-1` adds.
    \return Sum of the elements of vector `a`
*/
RCP<const Basic> add(const vec_basic &a);
//! Subtracts `b` from `a`
//! \return `a - b`
RCP<const Basic> sub(const RCP<const Basic> &a, const RCP<const Basic> &b);

} // SymEngine

#endif
