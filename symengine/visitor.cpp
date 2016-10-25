#include <symengine/visitor.h>

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

class HasSymbolVisitor : public BaseVisitor<HasSymbolVisitor, StopVisitor>
{
protected:
    Ptr<const Symbol> x_;
    bool has_;

public:
    HasSymbolVisitor(Ptr<const Symbol> x) : x_(x)
    {
    }

    void bvisit(const Symbol &x)
    {
        if (x_->__eq__(x)) {
            has_ = true;
            stop_ = true;
        }
    }

    void bvisit(const Basic &x){};

    bool apply(const Basic &b)
    {
        has_ = false;
        stop_ = false;
        preorder_traversal_stop(b, *this);
        return has_;
    }
};

bool has_symbol(const Basic &b, const Symbol &x)
{
    // We are breaking a rule when using ptrFromRef() here, but since
    // HasSymbolVisitor is only instantiated and freed from here, the `x` can
    // never go out of scope, so this is safe.
    HasSymbolVisitor v(ptrFromRef(x));
    return v.apply(b);
}

class CoeffVisitor : public BaseVisitor<CoeffVisitor, StopVisitor>
{
protected:
    Ptr<const Symbol> x_;
    Ptr<const Basic> n_;
    RCP<const Basic> coeff_;

public:
    CoeffVisitor(Ptr<const Symbol> x, Ptr<const Basic> n) : x_(x), n_(n)
    {
    }

    void bvisit(const Add &x)
    {
        umap_basic_num dict;
        RCP<const Number> coef = zero;
        for (auto &p : x.dict_) {
            p.first->accept(*this);
            if (neq(*coeff_, *zero)) {
                Add::coef_dict_add_term(outArg(coef), dict, p.second, coeff_);
            }
        }
        coeff_ = Add::from_dict(coef, std::move(dict));
    }

    void bvisit(const Mul &x)
    {
        for (auto &p : x.dict_) {
            if (eq(*p.first, *x_) and eq(*p.second, *n_)) {
                map_basic_basic dict = x.dict_;
                dict.erase(p.first);
                coeff_ = Mul::from_dict(x.coef_, std::move(dict));
                return;
            }
        }
        coeff_ = zero;
    }

    void bvisit(const Pow &x)
    {
        if (eq(*x.get_base(), *x_) and eq(*x.get_exp(), *n_)) {
            coeff_ = one;
        } else {
            coeff_ = zero;
        }
    }

    void bvisit(const Symbol &x)
    {
        if (eq(x, *x_) and eq(*one, *n_)) {
            coeff_ = one;
        } else {
            coeff_ = zero;
        }
    }

    void bvisit(const Basic &x)
    {
        coeff_ = zero;
    }

    RCP<const Basic> apply(const Basic &b)
    {
        coeff_ = zero;
        b.accept(*this);
        return coeff_;
    }
};

RCP<const Basic> coeff(const Basic &b, const Basic &x, const Basic &n)
{
    if (!is_a<Symbol>(x)) {
        throw NotImplementedError("Not implemented for non Symbols.");
    }
    CoeffVisitor v(ptrFromRef(static_cast<const Symbol &>(x)), ptrFromRef(n));
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

} // SymEngine
