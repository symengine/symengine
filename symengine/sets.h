/**
 *  \file sets.h
 *
 **/
#ifndef SYMENGINE_SETS_H
#define SYMENGINE_SETS_H

#include <symengine/functions.h>
#include <symengine/complex.h>
#include <symengine/real_double.h>
#include <symengine/real_mpfr.h>
#include <symengine/rational.h>
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
inline RCP<const Set> set_union(const set_set &in, bool solve = true)
{
    set_set input;
    if (solve == false && in.size() > 1)
        return make_rcp<const Union>(in);
    set_basic combined_FiniteSet;
    for (auto it = in.begin(); it != in.end(); ++it) {
        if (is_a<FiniteSet>(**it)) {
            const FiniteSet &other = down_cast<const FiniteSet &>(**it);
            combined_FiniteSet.insert(other.get_container().begin(),
                                      other.get_container().end());
        } else if (is_a<UniversalSet>(**it)) {
            return universalset();
        } else if (not is_a<EmptySet>(**it)) {
            input.insert(*it);
        }
    }
    if (input.empty()) {
        return finiteset(combined_FiniteSet);
    } else if (input.size() == 1 && combined_FiniteSet.empty()) {
        return rcp_dynamic_cast<const Set>(*input.begin());
    }
    // Now we rely on respective containers' own rules
    // TODO: Improve it to O(log n)
    RCP<const Set> combined_Rest = finiteset(combined_FiniteSet);
    for (auto it = input.begin(); it != input.end(); ++it) {
        RCP<const Set> other = rcp_dynamic_cast<const Set>(*it);
        combined_Rest = combined_Rest->set_union(other);
    }
    return combined_Rest;
}

// ! \return RCP<const Set>
RCP<const Set> set_intersection(const set_set &in, bool solve = true)
{
    if (solve == false)
        throw std::runtime_error(
            "Not implemented"); // No intersection class yet

    // Global rules
    // If found any emptyset then return emptyset
    for (const auto &input : in)
        if (is_a<EmptySet>(*input))
            return emptyset();
    if (in.size() == 1)
        return rcp_dynamic_cast<const Set>(*in.begin());

    // Handle finite sets
    set_set fsets, othersets;
    for (const auto &input : in) {
        if (is_a<FiniteSet>(*input))
            fsets.insert(input);
        else
            othersets.insert(input);
    }
    if (fsets.size() != 0) {
        const FiniteSet &fs = down_cast<const FiniteSet &>(**fsets.begin());
        fsets.erase(fsets.begin());
        set_basic finalfs;
        for (const auto &fselement : fs.get_container()) {
            bool present = true;
            for (const auto &fset : fsets) {
                present
                    = present and eq(*(fset->contains(fselement)), *boolTrue);
            }
            if (!present)
                continue;
            for (const auto &oset : othersets) {
                present
                    = present and eq(*(oset->contains(fselement)), *boolTrue);
            }
            if (present)
                finalfs.insert(fselement);
        }
        return finiteset(finalfs);
    }

    // If any of the sets is union, then return a Union of Intersections
    for (const auto &input : in) {
        if (is_a<Union>(*input)) {
            set_set other_sets = in;
            auto it = other_sets.find(input);
            other_sets.erase(it);
            auto other = SymEngine::set_intersection(other_sets);
            auto &container = down_cast<const Union &>(*input).get_container();
            set_set usets;
            for (const auto &c : container) {
                usets.insert(SymEngine::set_intersection({c, other}));
            }
            return SymEngine::set_union(usets);
        }
    }

    // Simplify if any of the sets is a complement
    // Not implemented as we don't have a class for complement yet.

    // Pair-wise rules
    bool shouldContinue = true;
    auto incopy = in;
    while (shouldContinue) {
        for (const auto &input : incopy) {
            shouldContinue = false;
            set_set other_sets = incopy;
            auto it = other_sets.find(input);
            other_sets.erase(it);
            set_set newinput;
            for (const auto &oset : other_sets) {
                // Try-Catch is a hack for now, but needs improvements
                try {
                    auto input_oset = input->set_intersection(oset);
                    newinput = other_sets;
                    it = newinput.find(oset);
                    newinput.erase(it);
                    shouldContinue = true;
                    break;
                } catch (std::runtime_error &err) {
                    continue;
                }
            }
            if (not newinput.empty()) {
                incopy = newinput;
                break;
            }
        }
    }

    if (incopy.size() == 1)
        return *incopy.begin();
    else
        return SymEngine::set_intersection(incopy, false);
}
}
#endif
