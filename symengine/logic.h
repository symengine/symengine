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
    std::size_t __hash__() const;
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
    std::size_t __hash__() const;
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
public:
    set_boolean args;

public:
    IMPLEMENT_TYPEID(AND)
    And(const set_boolean &s);
    //! \return the hash
    std::size_t __hash__() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
};

class Or : public Boolean
{
public:
    set_boolean args;

public:
    IMPLEMENT_TYPEID(OR)
    Or(const set_boolean &s);
    //! \return the hash
    std::size_t __hash__() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
};

class Not : public Boolean
{
public:
    RCP<const Boolean> arg;

public:
    IMPLEMENT_TYPEID(NOT)
    Not(const RCP<const Boolean> &s);
    //! \return the hash
    std::size_t __hash__() const;
    virtual vec_basic get_args() const;
    virtual bool __eq__(const Basic &o) const;
    //! Structural equality comparator
    virtual int compare(const Basic &o) const;
};

RCP<const Boolean> logical_not(const RCP<const Boolean> &s);

template <typename caller>
RCP<const Boolean> and_or(const set_boolean &s, const bool &op_x_notx)
{
    set_boolean args;
    for (auto &a : s) {
        if (is_a<BooleanAtom>(*a)) {
            auto val = static_cast<const BooleanAtom &>(*a).get_val();
            if (val == op_x_notx)
                return boolean(op_x_notx);
            else
                continue;
        }
        args.insert(a);
    }
    std::function<set_boolean(set_boolean)> flatten;
    flatten = [&](const set_boolean &s_) {
        set_boolean new_args;
        for (auto &a : s_) {
            if (is_a<caller>(*a)) {
                const caller &to_insert = static_cast<const caller &>(*a);
                set_boolean to_insert_flattened = flatten(to_insert.args);
                new_args.insert(to_insert_flattened.begin(),
                                to_insert_flattened.end());
            } else {
                new_args.insert(a);
            }
        }
        return new_args;
    };
    args = flatten(args);
    for (auto &a : args) {
        if (args.find(logical_not(a)) != args.end())
            return boolean(op_x_notx);
    }
    if (args.size() == 1)
        return *(args.begin());
    else if (args.size() == 0)
        return boolean(not op_x_notx);
    return make_rcp<const caller>(args);
}

inline RCP<const Boolean> logical_and(const set_boolean &s)
{
    return and_or<And>(s, false);
}

inline RCP<const Boolean> logical_or(const set_boolean &s)
{
    return and_or<Or>(s, true);
}

inline RCP<const Boolean> logical_not(const RCP<const Boolean> &s)
{
    if (is_a<BooleanAtom>(*s)) {
        const BooleanAtom &a = static_cast<const BooleanAtom &>(*s);
        return boolean(not a.get_val());
    } else if (is_a<Not>(*s)) {
        const Not &a = static_cast<const Not &>(*s);
        return a.arg;
    } else if (is_a<Or>(*s)) {
        const Or &o = static_cast<const Or &>(*s);
        set_boolean s;
        for (auto &a : o.args) {
            s.insert(logical_not(a));
        }
        return logical_and(s);
    } else if (is_a<And>(*s)) {
        const And &o = static_cast<const And &>(*s);
        set_boolean s;
        for (auto &a : o.args) {
            s.insert(logical_not(a));
        }
        return logical_or(s);
    } else {
        return make_rcp<const Not>(s);
    }
}
} // SymEngine

#endif
