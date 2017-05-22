/**
 * \file relationals.h
 *
 */

#ifndef SYMENGINE_RELATIONALS_H
#define SYMENGINE_RELATIONALS_H

#include <symengine/basic.h>
#include <symengine/logic.h>

namespace SymEngine
{

class Relational : public Boolean
{
private:
    RCP<const Basic> lhs_;
    RCP<const Basic> rhs_;

public:
    //! Constructor
    Relational(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);
    //! \return the hash
    hash_t __hash__() const;
    //! \return the left-hand side of the relation
    const RCP<const Basic> &get_lhs() const
    {
        return lhs_;
    }
    //! \return the right-hand side of the relation
    const RCP<const Basic> &get_rhs() const
    {
        return rhs_;
    }
    //! \return the args
    virtual vec_basic get_args() const
    {
        return {lhs_, rhs_};
    }
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    virtual bool is_canonical(const RCP<const Basic> &lhs,
                              const RCP<const Basic> &rhs) const;
};

class Equality : public Relational
{
    //! Class for operator `==`.
public:
    IMPLEMENT_TYPEID(EQUALITY);
    Equality(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);
};

//! Returns the canonicalized Equality object from the two arguments
RCP<const Basic> Eq(const RCP<const Basic> &lhs);
//! Returns the canonicalized Equality object from the two arguments
RCP<const Basic> Eq(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);

class Unequality : public Relational
{
    //! Class for operator `!=`.
public:
    IMPLEMENT_TYPEID(UNEQUALITY);
    Unequality(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);
};

//! Returns the canonicalized Unequality object from the arguments
RCP<const Basic> Ne(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);

class _Less : public Relational
{
    //! Not intended for general use.
    //! `_Less` is only used so that `LessThan` and `StrictLessThan` may
    //! subclass it for `get_gts()` and `get_lts()` functions
public:
    //! Constructor for passing to Relational
    _Less(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);
    //! \return the greater side of the relation
    const RCP<const Basic> &get_gts() const;
    //! \return the lesser side of the relation
    const RCP<const Basic> &get_lts() const;
};

class LessThan : public _Less
{
    //! Class for operator `<=`.
public:
    IMPLEMENT_TYPEID(LESSTHAN);
    LessThan(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);
};

//! Returns the canonicalized LessThan object from the arguments
RCP<const Basic> Le(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);

class StrictLessThan : public _Less
{
    //! Class for operator `<`.
public:
    IMPLEMENT_TYPEID(STRICTLESSTHAN);
    StrictLessThan(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);
};

//! Returns the canonicalized StrictLessThan object from the arguments
RCP<const Basic> Lt(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs);

inline bool is_a_Relational(const Basic &b)
{
    return (b.get_type_code() == EQUALITY || b.get_type_code() == UNEQUALITY
            || b.get_type_code() == LESSTHAN
            || b.get_type_code() == STRICTLESSTHAN);
}

} // SymEngine
#endif