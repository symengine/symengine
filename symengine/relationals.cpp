#include <symengine/relationals.h>

namespace SymEngine
{

Relational::Relational(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : lhs_{lhs}, rhs_{rhs}
{
}

hash_t Relational::__hash__() const
{
    hash_t seed = this->get_type_code();
    hash_combine<Basic>(seed, *lhs_);
    hash_combine<Basic>(seed, *rhs_);
    return seed;
}

inline bool Relational::__eq__(const Basic &o) const
{
    if (is_a_Relational(o)) {
        const Relational &s = down_cast<const Relational &>(o);
        if (this->get_type_code() == s.get_type_code()) {
            if (is_a<Equality>(o) or is_a<Unequality>(o))
                return (
                    (eq(*lhs_, *(s.get_lhs())) and eq(*rhs_, *(s.get_rhs())))
                    or (eq(*lhs_, *(s.get_rhs()))
                        and eq(*rhs_, *(s.get_lhs()))));
            return (eq(*lhs_, *(s.get_lhs())) and eq(*rhs_, *(s.get_rhs())));
        }
    }
    return false;
}

inline int Relational::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a_Relational(o));
    const Relational &r = down_cast<const Relational &>(o);
    int cmp = lhs_->__cmp__(*(r.get_lhs()));
    if (cmp != 0)
        return cmp;
    cmp = rhs_->__cmp__(*(r.get_rhs()));
    return cmp;
}

inline bool Relational::is_canonical(const RCP<const Basic> &lhs,
                                     const RCP<const Basic> &rhs) const
{
    if (eq(*lhs, *rhs))
        return false;
    if (is_a_Number(*lhs) and is_a_Number(*rhs))
        return false;
    if (is_a<BooleanAtom>(*lhs) and is_a<BooleanAtom>(*rhs))
        return false;
    return true;
}

Equality::Equality(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : Relational(lhs, rhs)
{
    SYMENGINE_ASSIGN_TYPEID();
    SYMENGINE_ASSERT(is_canonical(lhs, rhs));
}

RCP<const Basic> Eq(const RCP<const Basic> &lhs)
{
    return Eq(lhs, zero);
}

RCP<const Basic> Eq(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    if (is_a<NaN>(*lhs) or is_a<NaN>(*rhs))
        return boolean(false);
    bool b = eq(*lhs, *rhs);
    if (b)
        return boolean(true);
    else {
        if ((is_a_Number(*lhs) and is_a_Number(*rhs))
            or (is_a<BooleanAtom>(*lhs) and is_a<BooleanAtom>(*rhs)))
            return boolean(false);
        return make_rcp<Equality>(lhs, rhs);
    }
}

Unequality::Unequality(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : Relational(lhs, rhs)
{
    SYMENGINE_ASSIGN_TYPEID();
    SYMENGINE_ASSERT(is_canonical(lhs, rhs));
}

RCP<const Basic> Ne(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    RCP<const Basic> r = Eq(lhs, rhs);
    if (is_a<BooleanAtom>(*r)) {
        if (eq(*boolTrue, *r))
            return boolFalse;
        return boolTrue;
    }
    return make_rcp<Unequality>(lhs, rhs);
}

_Less::_Less(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : Relational(lhs, rhs)
{
}

inline const RCP<const Basic> &_Less::get_gts() const
{
    return this->get_rhs();
}

inline const RCP<const Basic> &_Less::get_lts() const
{
    return this->get_lhs();
}

LessThan::LessThan(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : _Less(lhs, rhs)
{
    SYMENGINE_ASSIGN_TYPEID();
    SYMENGINE_ASSERT(is_canonical(lhs, rhs));
}

RCP<const Basic> Le(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    if (is_a_Complex(*lhs) or is_a_Complex(*rhs))
        throw SymEngineException("Invalid comparison of complex numbers.");
    if (is_a<NaN>(*lhs) or is_a<NaN>(*rhs))
        throw SymEngineException("Invalid NaN comparison.");
    if (eq(*lhs, *ComplexInf) or eq(*rhs, *ComplexInf))
        throw SymEngineException("Invalid comparison of complex zoo.");
    if (is_a<BooleanAtom>(*lhs) or is_a<BooleanAtom>(*rhs))
        throw SymEngineException("Invalid comparison of Boolean objects.");
    if (eq(*lhs, *rhs))
        return boolean(true);
    if (is_a_Number(*lhs) and is_a_Number(*rhs)) {
        RCP<const Number> s = down_cast<const Number &>(*lhs)
                                  .sub(down_cast<const Number &>(*rhs));
        if (s->is_negative())
            return boolean(true);
        return boolean(false);
    }
    return make_rcp<const LessThan>(lhs, rhs);
}

StrictLessThan::StrictLessThan(const RCP<const Basic> &lhs,
                               const RCP<const Basic> &rhs)
    : _Less(lhs, rhs)
{
    SYMENGINE_ASSIGN_TYPEID();
    SYMENGINE_ASSERT(is_canonical(lhs, rhs));
}

RCP<const Basic> Lt(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    if (is_a_Complex(*lhs) or is_a_Complex(*rhs))
        throw SymEngineException("Invalid comparison of complex numbers.");
    if (is_a<NaN>(*lhs) or is_a<NaN>(*rhs))
        throw SymEngineException("Invalid NaN comparison.");
    if (eq(*lhs, *ComplexInf) or eq(*rhs, *ComplexInf))
        throw SymEngineException("Invalid comparison of complex zoo.");
    if (is_a<BooleanAtom>(*lhs) or is_a<BooleanAtom>(*rhs))
        throw SymEngineException("Invalid comparison of Boolean objects.");
    if (eq(*lhs, *rhs))
        return boolean(false);
    if (is_a_Number(*lhs) and is_a_Number(*rhs)) {
        RCP<const Number> s = down_cast<const Number &>(*lhs)
                                  .sub(down_cast<const Number &>(*rhs));
        if (s->is_negative())
            return boolean(true);
        return boolean(false);
    }
    return make_rcp<const StrictLessThan>(lhs, rhs);
}
}