/**
 *  \file visitor.h
 *  Class Visitor
 *
 **/
#ifndef SYMENGINE_VISITOR_H
#define SYMENGINE_VISITOR_H

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/polynomial.h>
#include <symengine/series.h>
#include <symengine/functions.h>
#include <symengine/symbol.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/constants.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>
#include <symengine/real_mpfr.h>
#include <symengine/complex_mpc.h>

namespace SymEngine {

class Visitor {
public:
#   define SYMENGINE_ENUM( TypeID , Class) \
    virtual void visit(const Class &) = 0;
#   include "symengine/type_codes.inc"
#   undef SYMENGINE_ENUM
};

void preorder_traversal(const Basic &b, Visitor &v);
void postorder_traversal(const Basic &b, Visitor &v);

template<class Derived, class Base = Visitor>
class BaseVisitor : public Base {

public:
#   define SYMENGINE_ENUM( TypeID , Class) \
    virtual void visit(const Class &x) { static_cast<Derived*>(this)->bvisit(x); };
#   include "symengine/type_codes.inc"
#   undef SYMENGINE_ENUM
};

class StopVisitor : public Visitor {
public:
    bool stop_;
};

void preorder_traversal_stop(const Basic &b, StopVisitor &v);

class HasSymbolVisitor : public BaseVisitor<HasSymbolVisitor, StopVisitor> {
protected:
    RCP<const Symbol> x_;
    bool has_;
public:

    void bvisit(const Symbol &x) {
        if (x_->__eq__(x)) {
            has_ = true;
            stop_ = true;
        }
    }

    void bvisit(const Basic &x) { };

    bool apply(const Basic &b, const RCP<const Symbol> &x) {
        x_ = x;
        has_ = false;
        stop_ = false;
        preorder_traversal_stop(b, *this);
        return has_;
    }
};

bool has_symbol(const Basic &b, const RCP<const Symbol> &x);

class CoeffVisitor : public BaseVisitor<CoeffVisitor, StopVisitor> {
protected:
    RCP<const Symbol> x_;
    RCP<const Integer> n_;
    RCP<const Basic> coeff_;
public:

    void bvisit(const Add &x) {
        // TODO: Implement coeff for Add
    }

    void bvisit(const Basic &x) {

    }

    RCP<const Basic> apply(const Basic &b, const RCP<const Symbol> &x,
            const RCP<const Integer> &n) {
        x_ = x;
        n_ = n;
        coeff_ = zero;
        b.accept(*this);
        return coeff_;
    }
};

RCP<const Basic> coeff(const Basic &b, const RCP<const Symbol> &x,
        const RCP<const Integer> &n);

set_basic free_symbols(const Basic &b);

} // SymEngine

#endif
