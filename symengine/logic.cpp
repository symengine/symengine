#include <symengine/logic.h>

namespace SymEngine {

BooleanAtom::BooleanAtom(bool b) : b_{b} {

}

std::size_t BooleanAtom::__hash__() const
{
    std::size_t seed = BOOLEAN_ATOM;
    if (b_) ++seed;
    return seed;
}
bool BooleanAtom::get_val() const
{
    return b_;
}

vec_basic BooleanAtom::get_args() const
{
    return {};
}

bool BooleanAtom::__eq__(const Basic &o) const
{
    return is_a<BooleanAtom>(o)
           and get_val() == static_cast<const BooleanAtom &>(o).get_val();
}

int BooleanAtom::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<BooleanAtom>(o))
    bool ob = static_cast<const BooleanAtom &>(o).get_val();
    if (get_val()) {
        return (ob) ? 0 : 1;
    } else {
        return (ob) ? -1 : 0;
    }
}

RCP<const BooleanAtom> boolTrue = make_rcp<BooleanAtom>(true);
RCP<const BooleanAtom> boolFalse = make_rcp<BooleanAtom>(false);

Contains::Contains(const RCP<const Basic> &expr, const RCP<const Set> &set) : expr_{expr}, set_{set} {

}

std::size_t Contains::__hash__() const
{
    std::size_t seed = CONTAINS;
    hash_combine<Basic>(seed, *expr_);
    hash_combine<Basic>(seed, *set_);
    return seed;
}

RCP<const Basic> Contains::get_expr() const
{
    return expr_;
}

RCP<const Set> Contains::get_set() const
{
    return set_;
}

vec_basic Contains::get_args() const
{
    vec_basic v;
    v.push_back(expr_);
    v.push_back(set_);
    return v;
}

bool Contains::__eq__(const Basic &o) const
{
    return is_a<Contains>(o)
           and unified_eq(get_expr(), static_cast<const Contains &>(o).get_expr())
           and unified_eq(get_set(), static_cast<const Contains &>(o).get_set());
}

int Contains::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Contains>(o))
    const Contains& c = static_cast<const Contains &>(o);
    int cmp = unified_compare(get_expr(), c.get_expr());
    if (cmp != 0)
        return cmp;
    return unified_compare(get_set(), c.get_set());
}


RCP<const Boolean> contains(const RCP<const Basic> &expr, const RCP<const Set> &set) {
    if (is_a_Number(*expr)) {
        return make_rcp<BooleanAtom>(set->contains(expr));
    } else {
        return make_rcp<Contains>(expr, set);
    }
}

Piecewise::Piecewise(PiecewiseVec &&vec) : vec_(vec)
{
}

std::size_t Piecewise::__hash__() const
{
    std::size_t seed = this->get_type_code();
    for (auto &p : vec_) {
        hash_combine<Basic>(seed, *p.first);
        hash_combine<Basic>(seed, *p.second);
    }
    return seed;
}

const PiecewiseVec &Piecewise::get_vec() const
{
    return vec_;
}

vec_basic Piecewise::get_args() const
{
    vec_basic v;
    for (auto &p: vec_) {
        v.push_back(p.first);
        v.push_back(p.second);
    }
    return v;
}

bool Piecewise::__eq__(const Basic &o) const
{
    return is_a<Piecewise>(o)
           and unified_eq(get_vec(),
                          static_cast<const Piecewise &>(o).get_vec());
}

int Piecewise::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_same_type(*this, o))
    RCP<const Piecewise> t = o.rcp_from_this_cast<Piecewise>();
    return unified_compare(get_vec(), t->get_vec());
}


}
