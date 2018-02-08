#include <symengine/visitor.h>
#include <symengine/polys/basic_conversions.h>
#include <symengine/sets.h>

#define ACCEPT(CLASS)                                                          \
    void CLASS::accept(Visitor &v) const                                       \
    {                                                                          \
        v.visit(*this);                                                        \
    }

namespace SymEngine
{

#define SYMENGINE_ENUM(TypeID, Class) ACCEPT(Class)
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM

void preorder_traversal(const Basic &b, Visitor &v)
{
    b.accept(v);
    for (const auto &p : b.get_args())
        preorder_traversal(*p, v);
}

void postorder_traversal(const Basic &b, Visitor &v)
{
    for (const auto &p : b.get_args())
        postorder_traversal(*p, v);
    b.accept(v);
}

void preorder_traversal_stop(const Basic &b, StopVisitor &v)
{
    b.accept(v);
    if (v.stop_)
        return;
    for (const auto &p : b.get_args()) {
        preorder_traversal_stop(*p, v);
        if (v.stop_)
            return;
    }
}

void postorder_traversal_stop(const Basic &b, StopVisitor &v)
{
    for (const auto &p : b.get_args()) {
        postorder_traversal_stop(*p, v);
        if (v.stop_)
            return;
    }
    b.accept(v);
}

bool has_symbol(const Basic &b, const Symbol &x)
{
    // We are breaking a rule when using ptrFromRef() here, but since
    // HasSymbolVisitor is only instantiated and freed from here, the `x` can
    // never go out of scope, so this is safe.
    HasSymbolVisitor v(ptrFromRef(x));
    return v.apply(b);
}

RCP<const Basic> coeff(const Basic &b, const Basic &x, const Basic &n)
{
    if (!is_a<Symbol>(x)) {
        throw NotImplementedError("Not implemented for non Symbols.");
    }
    CoeffVisitor v(ptrFromRef(down_cast<const Symbol &>(x)), ptrFromRef(n));
    return v.apply(b);
}

class FreeSymbolsVisitor : public BaseVisitor<FreeSymbolsVisitor>
{
public:
    set_basic s;
    uset_basic v;

    void bvisit(const Symbol &x)
    {
        s.insert(x.rcp_from_this());
    }

    void bvisit(const Subs &x)
    {
        set_basic set_ = free_symbols(*x.get_arg());
        for (const auto &p : x.get_variables()) {
            set_.erase(p);
        }
        s.insert(set_.begin(), set_.end());
        for (const auto &p : x.get_point()) {
            auto iter = v.insert(p->rcp_from_this());
            if (iter.second) {
                p->accept(*this);
            }
        }
    }

    void bvisit(const Basic &x)
    {
        for (const auto &p : x.get_args()) {
            auto iter = v.insert(p->rcp_from_this());
            if (iter.second) {
                p->accept(*this);
            }
        }
    }

    set_basic apply(const Basic &b)
    {
        b.accept(*this);
        return s;
    }

    set_basic apply(const MatrixBase &m)
    {
        for (unsigned i = 0; i < m.nrows(); i++) {
            for (unsigned j = 0; j < m.ncols(); j++) {
                m.get(i, j)->accept(*this);
            }
        }
        return s;
    }
};

set_basic free_symbols(const MatrixBase &m)
{
    FreeSymbolsVisitor visitor;
    return visitor.apply(m);
}

set_basic free_symbols(const Basic &b)
{
    FreeSymbolsVisitor visitor;
    return visitor.apply(b);
}

RCP<const Basic> TransformVisitor::apply(const RCP<const Basic> &x)
{
    x->accept(*this);
    return result_;
}

void TransformVisitor::bvisit(const Basic &x)
{
    result_ = x.rcp_from_this();
}

void TransformVisitor::bvisit(const Add &x)
{
    vec_basic newargs;
    for (const auto &a : x.get_args()) {
        newargs.push_back(apply(a));
    }
    result_ = add(newargs);
}

void TransformVisitor::bvisit(const Mul &x)
{
    vec_basic newargs;
    for (const auto &a : x.get_args()) {
        newargs.push_back(apply(a));
    }
    result_ = mul(newargs);
}

void TransformVisitor::bvisit(const Pow &x)
{
    auto base_ = x.get_base(), exp_ = x.get_exp();
    auto newarg1 = apply(base_), newarg2 = apply(exp_);
    if (base_ != newarg1 or exp_ != newarg2) {
        result_ = pow(newarg1, newarg2);
    } else {
        result_ = x.rcp_from_this();
    }
}

void TransformVisitor::bvisit(const OneArgFunction &x)
{
    auto farg = x.get_arg();
    auto newarg = apply(farg);
    if (eq(*newarg, *farg)) {
        result_ = x.rcp_from_this();
    } else {
        result_ = x.create(newarg);
    }
}

void TransformVisitor::bvisit(const MultiArgFunction &x)
{
    auto fargs = x.get_args();
    vec_basic newargs;
    for (const auto &a : fargs) {
        newargs.push_back(apply(a));
    }
    auto nbarg = x.create(newargs);
    result_ = nbarg;
}

void preorder_traversal_local_stop(const Basic &b, LocalStopVisitor &v)
{
    b.accept(v);
    if (v.stop_ or v.local_stop_)
        return;
    for (const auto &p : b.get_args()) {
        preorder_traversal_local_stop(*p, v);
        if (v.stop_)
            return;
    }
}

} // SymEngine
