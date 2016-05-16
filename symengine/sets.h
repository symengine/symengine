/**
 *  \file sets.h
 *
 **/
#ifndef SYMENGINE_SETS_H
#define SYMENGINE_SETS_H

#include <symengine/basic.h>
#include <symengine/functions.h>
#include <symengine/complex.h>
#include <symengine/dict.h>

namespace SymEngine
{

class Set : public Basic
{
public:
    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const = 0;
    virtual RCP<const Set> set_union(const RCP<const Set> &o) const = 0;
    virtual bool contains(const RCP<const Number> &a) const = 0;
    virtual bool is_subset(const RCP<const Set> &o) const = 0;
    virtual bool is_proper_subset(const RCP<const Set> &o) const = 0;
    virtual bool is_superset(const RCP<const Set> &o) const = 0;
    virtual bool is_proper_superset(const RCP<const Set> &o) const = 0;
};

class EmptySet : public Set
{
private:
    EmptySet(){};

public:
    IMPLEMENT_TYPEID(EMPTYSET)
    // EmptySet(EmptySet const&) = delete;
    void operator=(EmptySet const &) = delete;
    const static RCP<const EmptySet> &getInstance();
    virtual std::size_t __hash__() const;
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
    virtual bool contains(const RCP<const Number> &a) const
    {
        return false;
    };
    virtual bool is_subset(const RCP<const Set> &o) const
    {
        return true;
    };
    virtual bool is_proper_subset(const RCP<const Set> &o) const;
    virtual bool is_superset(const RCP<const Set> &o) const;
    virtual bool is_proper_superset(const RCP<const Set> &o) const
    {
        return false;
    };
};

class UniversalSet : public Set
{
private:
    UniversalSet(){};

public:
    IMPLEMENT_TYPEID(UNIVERSALSET)
    // UniversalSet(UniversalSet const&) = delete;
    void operator=(UniversalSet const &) = delete;
    const static RCP<const UniversalSet> &getInstance();
    virtual std::size_t __hash__() const;
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
    virtual bool contains(const RCP<const Number> &a) const
    {
        return true;
    };
    virtual bool is_subset(const RCP<const Set> &o) const;
    virtual bool is_proper_subset(const RCP<const Set> &o) const
    {
        return false;
    };
    virtual bool is_superset(const RCP<const Set> &o) const
    {
        return true;
    };
    virtual bool is_proper_superset(const RCP<const Set> &o) const;
};

class FiniteSet : public Set
{
public:
    set_number container_;

public:
    IMPLEMENT_TYPEID(FINITESET)
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {};
    }

    FiniteSet(const set_number container);
    static bool is_canonical(const set_number container);

    virtual RCP<const Set> set_union(const RCP<const Set> &o) const;
    virtual RCP<const Set> set_intersection(const RCP<const Set> &o) const;
    virtual bool contains(const RCP<const Number> &a) const;
    virtual bool is_subset(const RCP<const Set> &o) const;
    virtual bool is_proper_subset(const RCP<const Set> &o) const;
    virtual bool is_superset(const RCP<const Set> &o) const;
    virtual bool is_proper_superset(const RCP<const Set> &o) const;
};

class Interval : public Set
{
public:
    RCP<const Number> start_;
    RCP<const Number> end_;
    bool left_open_, right_open_;

public:
    IMPLEMENT_TYPEID(INTERVAL)
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {start_, end_};
    }

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
    virtual bool contains(const RCP<const Number> &a) const;
    virtual bool is_subset(const RCP<const Set> &o) const;
    virtual bool is_proper_subset(const RCP<const Set> &o) const;
    virtual bool is_superset(const RCP<const Set> &o) const;
    virtual bool is_proper_superset(const RCP<const Set> &o) const;
};

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
inline RCP<const Set> interval(const RCP<const Number> &start,
                               const RCP<const Number> &end,
                               const bool left_open = false,
                               const bool right_open = false)
{
    if (Interval::is_canonical(start, end, left_open, right_open)) {
        return make_rcp<const Interval>(start, end, left_open, right_open);
    }
    return emptyset();
}

//! \return RCP<const Set>
inline RCP<const Set> finiteset(const set_number &container)
{
    if (FiniteSet::is_canonical(container)) {
        return make_rcp<const FiniteSet>(container);
    }
    return emptyset();
}
}
#endif
