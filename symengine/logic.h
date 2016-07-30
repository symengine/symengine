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

class Boolean : public Basic
{
};

class BooleanAtom : public Boolean
{
private:
    bool b_;

public:
    IMPLEMENT_TYPEID(BOOLEAN_ATOM)
    BooleanAtom(bool b);
    //! \return the hash
    std::size_t __hash__() const;
    bool get_val() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
};

extern SYMENGINE_EXPORT RCP<const BooleanAtom> boolTrue;
extern SYMENGINE_EXPORT RCP<const BooleanAtom> boolFalse;

class Contains : public Boolean
{
private:
    RCP<const Basic> expr_;
    RCP<const Set> set_;

public:
    IMPLEMENT_TYPEID(PIECEWISE)
    //! Constructor
    Contains(const RCP<const Basic> &expr, const RCP<const Set> &set);
    std::size_t __hash__() const;
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

class Piecewise : public Basic
{
private:
    PiecewiseVec vec_;

public:
    IMPLEMENT_TYPEID(PIECEWISE)
    //! Constructor
    Piecewise(PiecewiseVec &&vec);
    std::size_t __hash__() const;
    const PiecewiseVec &get_vec() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
};

// Vec is a pair of RCP<const Basic>. (Relation, Expression)
inline RCP<const Basic> piecewise(PiecewiseVec &&vec)
{
    return make_rcp<Piecewise>(std::move(vec));
}

} // SymEngine

#endif
