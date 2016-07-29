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

typedef std::vector<std::pair<RCP<const Interval>, RCP<const Basic>>>
    PiecewiseVec;

class Piecewise : public Basic
{
private:
    PiecewiseVec vec_;
    RCP<const Basic> s_;

public:
    IMPLEMENT_TYPEID(PIECEWISE)
    //! Constructor
    Piecewise(PiecewiseVec &&vec, RCP<const Basic> s) : vec_(vec), s_(s)
    {
    }
    //! \return the hash
    inline std::size_t __hash__() const
    {
        std::size_t seed = this->get_type_code();
        for (auto &p : vec_) {
            hash_combine<Basic>(seed, *p.first);
            hash_combine<Basic>(seed, *p.second);
        }
        return seed;
    }

    const PiecewiseVec &get_vec() const
    {
        return vec_;
    }

    RCP<const Basic> get_symbol() const
    {
        return s_;
    }

    virtual inline vec_basic get_args() const
    {
        throw std::runtime_error("Not implemented");
    }

    virtual inline bool __eq__(const Basic &o) const
    {
        return is_a<Piecewise>(o)
               and unified_eq(get_vec(),
                              static_cast<const Piecewise &>(o).get_vec());
    }
    //! Structural equality comparator
    virtual inline int compare(const Basic &o) const
    {
        SYMENGINE_ASSERT(is_same_type(*this, o))
        RCP<const Piecewise> t = o.rcp_from_this_cast<Piecewise>();
        return unified_compare(get_symbol(), t->get_symbol())
               and unified_compare(get_vec(), t->get_vec());
    }
};

// Vec is a pair of RCP<const Basic>. (Relation, Expression)
inline RCP<const Basic> piecewise(PiecewiseVec &&vec, RCP<const Basic> s)
{
    return make_rcp<Piecewise>(std::move(vec), s);
}

} // SymEngine

#endif
