#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <symengine/polynomial.h>
#include <symengine/complex_double.h>
#include <symengine/complex_mpc.h>

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

bool has_symbol(const Basic &b, const RCP<const Symbol> &x)
{
    HasSymbolVisitor v;
    return v.apply(b, x);
}

RCP<const Basic> coeff(const Basic &b, const RCP<const Basic> &x,
                       const RCP<const Basic> &n)
{
    if (!is_a<Symbol>(*x)) {
        throw std::runtime_error("Not implemented for non Symbols.");
    }
    CoeffVisitor v;
    return v.apply(b, rcp_static_cast<const Symbol>(x), n);
}

class FreeSymbolsVisitor : public BaseVisitor<FreeSymbolsVisitor>
{
public:
    set_basic s;

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
            p->accept(*this);
        }
    }

    void bvisit(const Basic &x)
    {
        for (const auto &p : x.get_args()) {
            p->accept(*this);
        }
    }

    set_basic apply(const Basic &b)
    {
        b.accept(*this);
        return s;
    }
};

set_basic free_symbols(const Basic &b)
{
    FreeSymbolsVisitor visitor;
    return visitor.apply(b);
}

} // SymEngine
