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

class StopVisitor : public Visitor
{
public:
    bool stop_;
};

void preorder_traversal_stop(const Basic &b, StopVisitor &v);
void postorder_traversal_stop(const Basic &b, StopVisitor &v);

bool has_symbol(const Basic &b, const Symbol &x);
RCP<const Basic> coeff(const Basic &b, const Basic &x, const Basic &n);
set_basic free_symbols(const Basic &b);

} // SymEngine

#endif
