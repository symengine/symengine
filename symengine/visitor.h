/**
 *  \file visitor.h
 *  Class Visitor
 *
 **/
#ifndef SYMENGINE_VISITOR_H
#define SYMENGINE_VISITOR_H

#include <symengine/polys/uintpoly_flint.h>
#include <symengine/polys/uintpoly_piranha.h>
#include <symengine/polys/uexprpoly.h>
#include <symengine/polys/msymenginepoly.h>
#include <symengine/polys/uratpoly.h>
#include <symengine/complex_mpc.h>
#include <symengine/series_generic.h>
#include <symengine/series_piranha.h>
#include <symengine/series_flint.h>
#include <symengine/series_generic.h>
#include <symengine/series_piranha.h>
#include <symengine/sets.h>
#include <symengine/fields.h>
#include <symengine/logic.h>
#include <symengine/infinity.h>

namespace SymEngine
{

class Visitor
{
public:
#define SYMENGINE_ENUM(TypeID, Class) virtual void visit(const Class &) = 0;
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
};

void preorder_traversal(const Basic &b, Visitor &v);
void postorder_traversal(const Basic &b, Visitor &v);

template <class Derived, class Base = Visitor>
class BaseVisitor : public Base
{

public:
#if defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ < 8
    // Following two ctors can be replaced by `using Base::Base` if inheriting
    // constructors are allowed by the compiler. GCC 4.8 is the earliest
    // version supporting this.
    template <typename... Args,
              typename
              = enable_if_t<std::is_constructible<Base, Args...>::value>>
    BaseVisitor(Args &&... args)
        : Base(std::forward<Args>(args)...)
    {
    }

    BaseVisitor() : Base()
    {
    }
#else
    using Base::Base;
#endif

#define SYMENGINE_ENUM(TypeID, Class)                                          \
    virtual void visit(const Class &x)                                         \
    {                                                                          \
        static_cast<Derived *>(this)->bvisit(x);                               \
    };
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
};

// Defined in basic_conversions.h
template <typename P>
RCP<const P> from_basic(const RCP<const Basic> &basic,
                        const RCP<const Basic> &gen, bool ex = false);
template <typename P>
RCP<const P> from_basic(const RCP<const Basic> &basic, bool ex = false);

template <typename CRTPVisitor>
class DoubleDispatchVisitor : public Visitor
{
private:
    const RCP<const Basic> &a_;
    RCP<const Basic> result;

public:
    inline DoubleDispatchVisitor(const RCP<const Basic> &a) : a_(a)
    {
    }
    RCP<const Basic> apply(const RCP<const Basic> &b)
    {
        b->accept(*this);
        return std::move(result);
    }

    // TODO: Series op Series: this should be fixed by adding a conversion 
    // system to choose which series type to use from a and b
    template <typename P,
              typename
              = enable_if_t<std::is_base_of<SeriesCoeffInterface, P>::value>>
    static inline RCP<const Basic> dispatch(const P &a,
                                            const SeriesCoeffInterface &b)
    {
        auto p = P::series(b.as_basic(), a.get_var(),
                           std::min(a.get_degree(), b.get_degree()));
        return CRTPVisitor::dispatch(a, *p);
    }

    // TODO: Series op Poly: this should be fixed by adding a conversion system
    template <typename P, typename Container, typename Poly,
              typename
              = enable_if_t<std::is_base_of<SeriesCoeffInterface, P>::value>>
    static inline RCP<const Basic> dispatch(const P &a,
                                            const UPolyBase<Container, Poly> &b)
    {
        auto p = P::series(static_cast<const Poly &>(b).as_symbolic(),
                           a.get_var(), a.get_degree());
        return CRTPVisitor::dispatch(a, *p);
    }

    // TODO: Poly op Series: this should be fixed by adding a conversion system
    template <typename P, typename Container, typename Poly,
              typename
              = enable_if_t<std::is_base_of<SeriesCoeffInterface, P>::value>>
    static inline RCP<const Basic> dispatch(const UPolyBase<Container, Poly> &a,
                                            const P &b)
    {
        auto p = P::series(static_cast<const Poly &>(a).as_symbolic(),
                           b.get_var(), b.get_degree());
        return CRTPVisitor::dispatch(*p, b);
    }

    // TODO: Poly op Poly: this should be fixed by adding a conversion system
    template <typename Container, typename Poly, typename Container2, typename Poly2,
              typename
              = enable_if_t<not std::is_same<Poly, Poly2>::value>>
    static inline RCP<const Basic> dispatch(const UPolyBase<Container, Poly> &a,
                                            const UPolyBase<Container2, Poly2> &b)
    {
        throw std::runtime_error("Not implemented");
    }

    template <typename P,
              typename
              = enable_if_t<std::is_base_of<SeriesCoeffInterface, P>::value>>
    static inline RCP<const Basic> dispatch(const P &a, const Symbolic &b)
    {
        auto p = P::series(b.rcp_from_this(), a.get_var(), a.get_degree());
        return CRTPVisitor::dispatch(a, *p);
    }

    template <typename P,
              typename
              = enable_if_t<std::is_base_of<SeriesCoeffInterface, P>::value>>
    static inline RCP<const Basic> dispatch(const Symbolic &a, const P &b)
    {
        auto p = P::series(a.rcp_from_this(), b.get_var(), b.get_degree());
        return CRTPVisitor::dispatch(*p, b);
    }

    template <typename Container, typename Poly>
    static inline RCP<const Poly>
    dispatch(const UIntPolyBase<Container, Poly> &a, const Symbolic &b)
    {
        auto p = from_basic<Poly>(b.rcp_from_this());
        return CRTPVisitor::dispatch(static_cast<const Poly &>(a), *p);
    }

    template <typename Container, typename Poly>
    static inline RCP<const Poly>
    dispatch(const URatPolyBase<Container, Poly> &a, const Symbolic &b)
    {
        throw std::runtime_error("Not supported");
    }

    template <typename Container, typename Poly>
    static inline RCP<const Poly>
    dispatch(const UExprPolyBase<Container, Poly> &a, const Symbolic &b)
    {
        auto p = from_basic<Poly>(b.rcp_from_this(), a.get_var());
        return CRTPVisitor::dispatch(static_cast<const Poly &>(a), *p);
    }

    template <typename Container, typename Poly>
    static inline RCP<const Poly>
    dispatch(const Symbolic &a, const UIntPolyBase<Container, Poly> &b)
    {
        auto p = from_basic<Poly>(a.rcp_from_this());
        return CRTPVisitor::dispatch(*p, static_cast<const Poly &>(b));
    }

    template <typename Container, typename Poly>
    static inline RCP<const Poly>
    dispatch(const Symbolic &a, const URatPolyBase<Container, Poly> &b)
    {
        throw std::runtime_error("Not supported");
    }

    template <typename Container, typename Poly>
    static inline RCP<const Poly>
    dispatch(const Symbolic &a, const UExprPolyBase<Container, Poly> &b)
    {
        auto p = from_basic<Poly>(a.rcp_from_this(), b.get_var());
        return CRTPVisitor::dispatch(*p, static_cast<const Poly &>(b));
    }

    template <typename T>
    inline void visitall(const T &b);
#define SYMENGINE_ENUM(TypeID, Class)                                          \
    virtual void visit(const Class &b)                                         \
    {                                                                          \
        visitall(b);                                                           \
    };
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
};

template <typename CRTPVisitor, typename T>
class _DoubleDispatchVisitor : public Visitor
{
protected:
    const T &b_;
    RCP<const Basic> result;

public:
    inline _DoubleDispatchVisitor(const T &b) : b_(b)
    {
    }
    RCP<const Basic> apply(const RCP<const Basic> &a)
    {
        a->accept(*this);
        return std::move(result);
    }
#define SYMENGINE_ENUM(TypeID, Class)                                          \
    inline void visit(const Class &a)                                          \
    {                                                                          \
        result = CRTPVisitor::dispatch(a, b_);                                 \
    };
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
};

template <typename CRTPVisitor>
template <typename T>
inline void DoubleDispatchVisitor<CRTPVisitor>::visitall(const T &b)
{
    _DoubleDispatchVisitor<CRTPVisitor, T> visitor(b);
    result = visitor.apply(a_);
}

class StopVisitor : public Visitor
{
public:
    bool stop_;
};

void preorder_traversal_stop(const Basic &b, StopVisitor &v);
void postorder_traversal_stop(const Basic &b, StopVisitor &v);

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

bool has_symbol(const Basic &b, const Symbol &x);

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

RCP<const Basic> coeff(const Basic &b, const Basic &x, const Basic &n);

set_basic free_symbols(const Basic &b);

} // SymEngine

#endif
