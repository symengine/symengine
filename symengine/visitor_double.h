#ifndef SYMENGINE_VISITOR_DOUBLE_H
#define SYMENGINE_VISITOR_DOUBLE_H

#include <symengine/visitor.h>
#include <symengine/polys/basic_conversions.h>

namespace SymEngine
{

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
    template <typename Container, typename Poly, typename Container2,
              typename Poly2,
              typename = enable_if_t<not std::is_same<Poly, Poly2>::value>>
    static inline RCP<const Basic>
    dispatch(const UPolyBase<Container, Poly> &a,
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

} // Namespace SymEngine
#endif // SYMENGINE_VISITOR_DOUBLE_H
