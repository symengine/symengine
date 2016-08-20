#include <symengine/logic.h>

namespace SymEngine
{

BooleanAtom::BooleanAtom(bool b) : b_{b}
{
}

hash_t BooleanAtom::__hash__() const
{
    hash_t seed = BOOLEAN_ATOM;
    if (b_)
        ++seed;
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

Contains::Contains(const RCP<const Basic> &expr, const RCP<const Set> &set)
    : expr_{expr}, set_{set}
{
}

hash_t Contains::__hash__() const
{
    hash_t seed = CONTAINS;
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
           and unified_eq(get_expr(),
                          static_cast<const Contains &>(o).get_expr())
           and unified_eq(get_set(),
                          static_cast<const Contains &>(o).get_set());
}

int Contains::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Contains>(o))
    const Contains &c = static_cast<const Contains &>(o);
    int cmp = unified_compare(get_expr(), c.get_expr());
    if (cmp != 0)
        return cmp;
    return unified_compare(get_set(), c.get_set());
}

RCP<const Boolean> contains(const RCP<const Basic> &expr,
                            const RCP<const Set> &set)
{
    if (is_a_Number(*expr)) {
        return set->contains(expr);
    } else {
        return make_rcp<Contains>(expr, set);
    }
}

Piecewise::Piecewise(PiecewiseVec &&vec) : vec_(vec)
{
}

hash_t Piecewise::__hash__() const
{
    hash_t seed = this->get_type_code();
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
    for (auto &p : vec_) {
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

And::And(const set_boolean &s) : container_{s}
{
    SYMENGINE_ASSERT(is_canonical(s));
}

hash_t And::__hash__() const
{
    hash_t seed = AND;
    for (const auto &a : container_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

vec_basic And::get_args() const
{
    vec_basic v(container_.begin(), container_.end());
    return v;
}

bool And::__eq__(const Basic &o) const
{
    return is_a<And>(o) and unified_eq(container_, static_cast<const And &>(o)
                                                       .get_container());
}

int And::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<And>(o))
    return unified_compare(container_,
                           static_cast<const And &>(o).get_container());
}

bool And::is_canonical(const set_boolean &container_)
{
    if (container_.size() >= 2) {
        for (auto &a : container_) {
            if (is_a<BooleanAtom>(*a) or is_a<And>(*a))
                return false;
            if (container_.find(logical_not(a)) != container_.end())
                return false;
        }
        return true;
    }
    return false;
}

const set_boolean &And::get_container() const
{
    return container_;
}

Or::Or(const set_boolean &s) : container_{s}
{
    SYMENGINE_ASSERT(is_canonical(s));
}

hash_t Or::__hash__() const
{
    hash_t seed = OR;
    for (const auto &a : container_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

vec_basic Or::get_args() const
{
    vec_basic v(container_.begin(), container_.end());
    return v;
}

bool Or::__eq__(const Basic &o) const
{
    return is_a<Or>(o) and unified_eq(container_, static_cast<const Or &>(o)
                                                      .get_container());
}

int Or::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Or>(o))
    return unified_compare(container_,
                           static_cast<const Or &>(o).get_container());
}

bool Or::is_canonical(const set_boolean &container_)
{
    if (container_.size() >= 2) {
        for (auto &a : container_) {
            if (is_a<BooleanAtom>(*a) or is_a<Or>(*a))
                return false;
            if (container_.find(logical_not(a)) != container_.end())
                return false;
        }
        return true;
    }
    return false;
}

const set_boolean &Or::get_container() const
{
    return container_;
}

Not::Not(const RCP<const Boolean> &in) : arg_{in}
{
    SYMENGINE_ASSERT(is_canonical(in));
}

hash_t Not::__hash__() const
{
    hash_t seed = NOT;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

vec_basic Not::get_args() const
{
    vec_basic v;
    v.push_back(arg_);
    return v;
}

bool Not::__eq__(const Basic &o) const
{
    return is_a<Not>(o) and eq(*arg_, *static_cast<const Not &>(o).get_arg());
}

int Not::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Not>(o))
    return arg_->__cmp__(*static_cast<const Not &>(o).get_arg());
}

bool Not::is_canonical(const RCP<const Boolean> &in)
{
    if (is_a<BooleanAtom>(*in) or is_a<Not>(*in))
        return false;
    return true;
}

RCP<const Boolean> Not::get_arg() const
{
    return arg_;
}

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
        if (is_a<caller>(*a)) {
            const caller &to_insert = static_cast<const caller &>(*a);
            auto container = to_insert.get_container();
            args.insert(container.begin(), container.end());
            continue;
        }
        args.insert(a);
    }
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

RCP<const Boolean> logical_not(const RCP<const Boolean> &s)
{
    if (is_a<BooleanAtom>(*s)) {
        const BooleanAtom &a = static_cast<const BooleanAtom &>(*s);
        return boolean(not a.get_val());
    } else if (is_a<Not>(*s)) {
        const Not &a = static_cast<const Not &>(*s);
        return a.get_arg();
    } else if (is_a<Or>(*s)) {
        auto container = static_cast<const Or &>(*s).get_container();
        set_boolean cont;
        for (auto &a : container) {
            cont.insert(logical_not(a));
        }
        return make_rcp<const And>(cont);
    } else if (is_a<And>(*s)) {
        auto container = static_cast<const And &>(*s).get_container();
        set_boolean cont;
        for (auto &a : container) {
            cont.insert(logical_not(a));
        }
        return make_rcp<const Or>(cont);
    } else {
        return make_rcp<const Not>(s);
    }
}

RCP<const Boolean> logical_and(const set_boolean &s)
{
    return and_or<And>(s, false);
}

RCP<const Boolean> logical_or(const set_boolean &s)
{
    return and_or<Or>(s, true);
}
}
