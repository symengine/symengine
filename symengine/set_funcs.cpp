#ifndef SYMENGINE_SET_FUNCS_H
#define SYMENGINE_SET_FUNCS_H

#include <symengine/visitor.h>
#include <symengine/functions.h>

namespace SymEngine
{

class SupVisitor : public BaseVisitor<SupVisitor>
{
private:
    RCP<const Basic> sup_;

public:
    SupVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const Set &x)
    {
        throw SymEngineException(
            "Set not partially ordered: supremum undefined");
    };

    void bvisit(const Reals &x)
    {
        sup_ = infty(1);
    };

    void bvisit(const Rationals &x)
    {
        sup_ = infty(1);
    };

    void bvisit(const Integers &x)
    {
        sup_ = infty(1);
    };

    void bvisit(const Interval &x)
    {
        sup_ = x.get_end();
    };

    void bvisit(const FiniteSet &x)
    {
        const set_basic &container = x.get_container();
        vec_basic v(container.begin(), container.end());
        sup_ = max(v);
    };

    void bvisit(const Union &x)
    {
        vec_basic suprema;
        for (auto &a : x.get_container()) {
            a->accept(*this);
            suprema.push_back(sup_);
        }
        sup_ = max(suprema);
    };

    void bvisit(const Complement &x)
    {
        throw NotImplementedError("sup for Complement not implemented");
    };

    void bvisit(const ImageSet &x)
    {
        throw NotImplementedError("sup for ImageSet not implemented");
    };

    RCP<const Basic> apply(const Set &s)
    {
        s.accept(*this);
        return sup_;
    };
};

class InfVisitor : public BaseVisitor<InfVisitor>
{
private:
    RCP<const Basic> inf_;

public:
    InfVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const Set &x)
    {
        throw SymEngineException(
            "Set not partially ordered: infimum undefined");
    };

    void bvisit(const Reals &x)
    {
        inf_ = infty(-1);
    };

    void bvisit(const Rationals &x)
    {
        inf_ = infty(-1);
    };

    void bvisit(const Integers &x)
    {
        inf_ = infty(-1);
    };

    void bvisit(const Interval &x)
    {
        inf_ = x.get_start();
    };

    void bvisit(const FiniteSet &x)
    {
        const set_basic &container = x.get_container();
        vec_basic v(container.begin(), container.end());
        inf_ = min(v);
    };

    void bvisit(const Union &x)
    {
        vec_basic infima;
        for (auto &a : x.get_container()) {
            a->accept(*this);
            infima.push_back(inf_);
        }
        inf_ = min(infima);
    };

    void bvisit(const Complement &x)
    {
        throw NotImplementedError("inf for Complement not implemented");
    };

    void bvisit(const ImageSet &x)
    {
        throw NotImplementedError("inf for ImageSet not implemented");
    };

    RCP<const Basic> apply(const Set &s)
    {
        s.accept(*this);
        return inf_;
    };
};

RCP<const Basic> sup(const Set &s)
{
    SupVisitor visitor;
    return visitor.apply(s);
}

RCP<const Basic> inf(const Set &s)
{
    InfVisitor visitor;
    return visitor.apply(s);
}

} // namespace SymEngine

#endif // SYMENGINE_SET_FUNCS_H
