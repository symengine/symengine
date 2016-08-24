/**
 *  \file logic.h
 *
 **/

#ifndef SYMENGINE_LOGIC_H
#define SYMENGINE_LOGIC_H

#include <symengine/basic.h>
#include <symengine/sets.h>

namespace SymEngine
{
typedef std::set<RCP<const Boolean>, RCPBasicKeyLess> set_boolean;
// Parent class for expressing boolean statements
class Boolean : public Basic
{
};

// Booleans True and False
class BooleanAtom : public Boolean
{
private:
    bool b_;

public:
    IMPLEMENT_TYPEID(BOOLEAN_ATOM)
    BooleanAtom(bool b);
    //! \return the hash
    hash_t __hash__() const;
    bool get_val() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
};

extern SYMENGINE_EXPORT RCP<const BooleanAtom> boolTrue;
extern SYMENGINE_EXPORT RCP<const BooleanAtom> boolFalse;

inline RCP<const BooleanAtom> boolean(bool b)
{
    return b ? boolTrue : boolFalse;
}

// Represents `expr_` is inside set `set_`
// `set_` can be any `Set` including `Interval`, `FiniteSet`
class Contains : public Boolean
{
private:
    RCP<const Basic> expr_;
    RCP<const Set> set_;

public:
    IMPLEMENT_TYPEID(CONTAINS)
    //! Constructor
    Contains(const RCP<const Basic> &expr, const RCP<const Set> &set);
    hash_t __hash__() const;
    RCP<const Basic> get_expr() const;
    RCP<const Set> get_set() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
};

RCP<const Boolean> contains(const RCP<const Basic> &expr,
                            const RCP<const Set> &set);

typedef std::vector<std::pair<RCP<const Basic>, RCP<const Boolean>>>
    PiecewiseVec;

// Represents a piecewise function
// Keeps a vector of (Expr, Condition) pairs
class Piecewise : public Basic
{
private:
    PiecewiseVec vec_;

public:
    IMPLEMENT_TYPEID(PIECEWISE)
    //! Constructor
    Piecewise(PiecewiseVec &&vec);
    hash_t __hash__() const;
    const PiecewiseVec &get_vec() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
};

// Vec is vector of pairs of RCP<const Basic> and RCP<const Boolean> to
// represent (Expr, Condition) pairs
inline RCP<const Basic> piecewise(PiecewiseVec &&vec)
{
    return make_rcp<Piecewise>(std::move(vec));
}

class And : public Boolean
{
private:
    set_boolean container_;

public:
    IMPLEMENT_TYPEID(AND)
    And(const set_boolean &s);
    bool is_canonical(const set_boolean &container_);
    //! \return the hash
    hash_t __hash__() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
    const set_boolean &get_container() const;
};

class Or : public Boolean
{
private:
    set_boolean container_;

public:
    IMPLEMENT_TYPEID(OR)
    Or(const set_boolean &s);
    bool is_canonical(const set_boolean &container_);
    //! \return the hash
    hash_t __hash__() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
    const set_boolean &get_container() const;
};

class Not : public Boolean
{
private:
    RCP<const Boolean> arg_;

public:
    IMPLEMENT_TYPEID(NOT)
    Not(const RCP<const Boolean> &s);
    bool is_canonical(const RCP<const Boolean> &s);
    //! \return the hash
    hash_t __hash__() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
    RCP<const Boolean> get_arg() const;
};

RCP<const Boolean> logical_and(const set_boolean &s);
RCP<const Boolean> logical_or(const set_boolean &s);
RCP<const Boolean> logical_not(const RCP<const Boolean> &s);
} // SymEngine

#endif
