/**
 *  \file sets.h
 *
 **/
#ifndef SYMENGINE_SETS_H
#define SYMENGINE_SETS_H

#include <symengine/functions.h>
#include <symengine/complex.h>
#include <symengine/symengine_casts.h>
#include <iterator>
namespace SymEngine
{
class Set;
class BooleanAtom;
class Boolean;
RCP<const BooleanAtom> boolean(bool b);
}
#include <symengine/logic.h>

namespace SymEngine
{
typedef std::set<RCP<const Set>, RCPBasicKeyLess> set_set;
class Set : public Basic
{
public:
    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const = 0;
    virtual RCP<const Set> set_union(const RCP<const Set> &o) const = 0;
    virtual RCP<const Set> set_complement(const RCP<const Set> &o) const = 0;
    virtual RCP<const Boolean> contains(const RCP<const Basic> &a) const = 0;
    bool is_subset(const RCP<const Set> &o) const
    {
        return eq(*this->set_intersection(o), *this);
    }
    bool is_proper_subset(const RCP<const Set> &o) const
    {
        return not eq(*this, *o) and this->is_subset(o);
    }
    bool is_superset(const RCP<const Set> &o) const
    {
        return o->is_subset(rcp_from_this_cast<const Set>());
    }
    bool is_proper_superset(const RCP<const Set> &o) const
    {
        return not eq(*this, *o) and this->is_superset(o);
    }
};

class EmptySet : public Set
{
public:
    EmptySet()
    {
        SYMENGINE_ASSIGN_TYPEID()
    }

    IMPLEMENT_TYPEID(EMPTYSET)
    // EmptySet(EmptySet const&) = delete;
    void operator=(EmptySet const &) = delete;
    const static RCP<const EmptySet> &getInstance();
    virtual hash_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {};
    }

    template <typename T_, typename... Args>
    friend inline RCP<T_> make_rcp(Args &&... args);

    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_union(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_complement(const RCP<const Set> &o) const;
    virtual RCP<const Boolean> contains(const RCP<const Basic> &a) const
    {
        return boolean(false);
    };
};

class UniversalSet : public Set
{
public:
    UniversalSet()
    {
        SYMENGINE_ASSIGN_TYPEID()
    }

public:
    IMPLEMENT_TYPEID(UNIVERSALSET)
    // UniversalSet(UniversalSet const&) = delete;
    void operator=(UniversalSet const &) = delete;
    const static RCP<const UniversalSet> &getInstance();
    virtual hash_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {};
    }

    template <typename T_, typename... Args>
    friend inline RCP<T_> make_rcp(Args &&... args);

    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_union(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_complement(const RCP<const Set> &o) const;
    virtual RCP<const Boolean> contains(const RCP<const Basic> &a) const
    {
        return boolean(true);
    };
};

class FiniteSet : public Set
{
private:
    set_basic container_;

public:
    IMPLEMENT_TYPEID(FINITESET)
    virtual hash_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {};
    }

    FiniteSet(const set_basic container);
    static bool is_canonical(const set_basic container);

    virtual RCP<const Set> set_union(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_complement(const RCP<const Set> &o) const;
    virtual RCP<const Boolean> contains(const RCP<const Basic> &a) const;

    inline const set_basic &get_container() const
    {
        return this->container_;
    }
};

class Interval : public Set
{
private:
    RCP<const Number> start_;
    RCP<const Number> end_;
    bool left_open_, right_open_;

public:
    IMPLEMENT_TYPEID(INTERVAL)
    virtual hash_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;

    Interval(const RCP<const Number> &start, const RCP<const Number> &end,
             const bool left_open = false, const bool right_open = false);

    RCP<const Set> open() const;
    RCP<const Set> close() const;
    RCP<const Set> Lopen() const;
    RCP<const Set> Ropen() const;

    static bool is_canonical(const RCP<const Number> &start,
                             const RCP<const Number> &end, bool left_open,
                             bool right_open);

    virtual RCP<const Set> set_union(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_complement(const RCP<const Set> &o) const;
    virtual RCP<const Boolean> contains(const RCP<const Basic> &a) const;
    virtual vec_basic get_args() const;

    inline const RCP<const Number> &get_start() const
    {
        return start_;
    }
    inline const RCP<const Number> &get_end() const
    {
        return end_;
    }
    inline const bool &get_left_open() const
    {
        return this->left_open_;
    }
    inline const bool &get_right_open() const
    {
        return this->right_open_;
    }
};

class Union : public Set
{
private:
    set_set container_;

public:
    IMPLEMENT_TYPEID(UNION)
    virtual hash_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {};
    }
    Union(set_set in);

    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_union(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_complement(const RCP<const Set> &o) const;
    virtual RCP<const Boolean> contains(const RCP<const Basic> &a) const;

    inline const set_set &get_container() const
    {
        return this->container_;
    }
};

class Complement : public Set
{
private:
    // represents universe_ - container_
    RCP<const Set> universe_;
    RCP<const Set> container_;

public:
    IMPLEMENT_TYPEID(COMPLEMENT)
    virtual hash_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {universe_, container_};
    }
    Complement(const RCP<const Set> &universe, const RCP<const Set> &container);

    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_union(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_complement(const RCP<const Set> &o) const;
    virtual RCP<const Boolean> contains(const RCP<const Basic> &a) const;

    inline const RCP<const Set> &get_universe() const
    {
        return this->universe_;
    }
    inline const RCP<const Set> &get_container() const
    {
        return this->container_;
    }
};

class ConditionSet : public Set
{
private:
    RCP<const Basic> sym;
    RCP<const Boolean> condition_;

public:
    IMPLEMENT_TYPEID(CONDITIONSET)
    virtual hash_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {};
    }
    ConditionSet(const RCP<const Basic> sym, RCP<const Boolean> condition);
    static bool is_canonical(const RCP<const Basic> sym,
                             RCP<const Boolean> condition);

    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_union(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_complement(const RCP<const Set> &o) const;
    virtual RCP<const Boolean> contains(const RCP<const Basic> &a) const;

    inline const RCP<const Basic> &get_symbol() const
    {
        return this->sym;
    }
    inline const RCP<const Boolean> &get_condition() const
    {
        return this->condition_;
    }
};

inline bool is_a_Set(const Basic &b)
{
    return (b.get_type_code() == EMPTYSET || b.get_type_code() == UNIVERSALSET
            || b.get_type_code() == FINITESET || b.get_type_code() == COMPLEMENT
            || b.get_type_code() == CONDITIONSET
            || b.get_type_code() == INTERVAL || b.get_type_code() == UNION);
}

//! \return RCP<const EmptySet>
inline RCP<const EmptySet> emptyset()
{
    return EmptySet::getInstance();
}

//! \return RCP<const UniversalSet>
inline RCP<const UniversalSet> universalset()
{
    return UniversalSet::getInstance();
}

//! \return RCP<const Set>
inline RCP<const Set> finiteset(const set_basic &container)
{
    if (FiniteSet::is_canonical(container)) {
        return make_rcp<const FiniteSet>(container);
    }
    return emptyset();
}

//! \return RCP<const Set>
inline RCP<const Set> interval(const RCP<const Number> &start,
                               const RCP<const Number> &end,
                               const bool left_open = false,
                               const bool right_open = false)
{
    if (Interval::is_canonical(start, end, left_open, right_open))
        return make_rcp<const Interval>(start, end, left_open, right_open);
    if (eq(*start, *end) and not(left_open or right_open))
        return finiteset({start});
    return emptyset();
}

// ! \return RCP<const Set>
RCP<const Set> set_union(const set_set &in, bool solve = true);

// ! \return RCP<const Set>
RCP<const Set> set_intersection(const set_set &in);

RCP<const Set> set_complement_helper(const RCP<const Set> &container,
                                     const RCP<const Set> &universe);

// ! \return RCP<const Set>
RCP<const Set> set_complement(const RCP<const Set> &universe,
                              const RCP<const Set> &container);

//! \return RCP<const Set>
RCP<const Set> conditionset(const RCP<const Basic> &sym,
                            const RCP<const Boolean> &condition);
}
#endif
