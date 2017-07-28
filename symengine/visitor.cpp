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

RCP<const Basic> TransformVisitor::apply(const Basic &x)
{
    return apply(x.rcp_from_this());
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

class IsALinearArgTrigVisitor
    : public BaseVisitor<IsALinearArgTrigVisitor, StopVisitor>
{
protected:
    Ptr<const Symbol> x_;
    bool is_;

public:
    IsALinearArgTrigVisitor(Ptr<const Symbol> x) : x_(x)
    {
    }

    void bvisit(const Basic &x)
    {
    }

    void bvisit(const Symbol &x)
    {
        if (x_->__eq__(x)) {
            is_ = false;
            stop_ = true;
        }
    }

    template <typename T,
              typename
              = enable_if_t<std::is_base_of<TrigFunction, T>::value
                            or std::is_base_of<HyperbolicFunction, T>::value>>
    void bvisit(const T &x)
    {
        is_ = (from_basic<UExprPoly>(x.get_args()[0], (*x_).rcp_from_this())
                   ->get_degree()
               <= 1);
        stop_ = true;
    }

    bool apply(const Basic &b)
    {
        is_ = true;
        stop_ = false;
        preorder_traversal_stop(b, *this);
        return is_;
    }
};

bool is_a_LinearArgTrigEquation(const Basic &b, const Symbol &x)
{
    IsALinearArgTrigVisitor v(ptrFromRef(x));
    return v.apply(b);
}

class InvertComplexVisitor : public BaseVisitor<InvertComplexVisitor>
{
protected:
    RCP<const Set> result_;
    RCP<const Set> gY_;
    RCP<const Dummy> nD_;
    RCP<const Symbol> sym_;
    RCP<const Set> domain_;

public:
    InvertComplexVisitor(RCP<const Set> gY, RCP<const Dummy> nD,
                         RCP<const Symbol> sym, RCP<const Set> domain)
        : gY_(gY), nD_(nD), sym_(sym), domain_(domain)
    {
    }

    void bvisit(const Basic &x)
    {
        result_ = gY_;
    }

    void bvisit(const Add &x)
    {
        vec_basic f1X, f2X;
        for (auto &elem : x.get_args()) {
            if (has_symbol(*elem, *sym_)) {
                f1X.push_back(elem);
            } else {
                f2X.push_back(elem);
            }
        }
        auto depX = add(f1X), indepX = add(f2X);
        if (not eq(*indepX, *zero)) {
            gY_ = imageset(nD_, sub(nD_, indepX), gY_);
            result_ = apply(*depX);
        } else {
            result_ = gY_;
        }
    }

    void bvisit(const Mul &x)
    {
        vec_basic f1X, f2X;
        for (auto &elem : x.get_args()) {
            if (has_symbol(*elem, *sym_)) {
                f1X.push_back(elem);
            } else {
                f2X.push_back(elem);
            }
        }
        auto depX = mul(f1X), indepX = mul(f2X);
        if (not eq(*indepX, *one)) {
            if (eq(*indepX, *NegInf) or eq(*indepX, *Inf)
                or eq(*indepX, *ComplexInf)) {
                result_ = emptyset();
            } else {
                gY_ = imageset(nD_, div(nD_, indepX), gY_);
                result_ = apply(*depX);
            }
        } else {
            result_ = gY_;
        }
    }

    void bvisit(const Pow &x)
    {
        if (eq(*x.get_base(), *E) and is_a<FiniteSet>(*gY_)) {
            set_set inv;
            for (const auto &elem :
                 down_cast<const FiniteSet &>(*gY_).get_container()) {
                if (eq(*elem, *zero))
                    continue;
                RCP<const Basic> re, im;
                as_real_imag(elem, outArg(re), outArg(im));
                auto logabs = log(add(mul(re, re), mul(im, im)));
                auto logarg = atan2(im, re);
                inv.insert(imageset(
                    nD_, add(mul(add(mul({integer(2), nD_, pi}), logarg), I),
                             div(logabs, integer(2))),
                    interval(NegInf, Inf, true,
                             true))); // TODO : replace interval(-oo,oo) with
                                      // Set of // Integers once Class for
                // Range is implemented.
            }
            gY_ = set_union(inv);
            apply(*x.get_exp());
            return;
        }
        result_ = gY_;
    }

    RCP<const Set> apply(const Basic &b)
    {
        result_ = gY_;
        b.accept(*this);
        return set_intersection({domain_, result_});
    }
};

RCP<const Set> invertComplex(const RCP<const Basic> &fX,
                             const RCP<const Set> &gY,
                             const RCP<const Symbol> &sym,
                             const RCP<const Dummy> &nD,
                             const RCP<const Set> &domain)
{
    InvertComplexVisitor v(gY, nD, sym, domain);
    return v.apply(*fX);
}

} // SymEngine
