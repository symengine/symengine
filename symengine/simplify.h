#ifndef SYMENGINE_SIMPLIFY_H
#define SYMENGINE_SIMPLIFY_H

#include <symengine/visitor.h>
#include <symengine/basic.h>
#include <symengine/assumptions.h>

namespace SymEngine
{

class SYMENGINE_EXPORT SimplifyVisitor
    : public BaseVisitor<SimplifyVisitor, TransformVisitor>
{
private:
    std::pair<RCP<const Basic>, RCP<const Basic>>
    simplify_pow(const RCP<const Basic> &e, const RCP<const Basic> &b);

public:
    using TransformVisitor::bvisit;

    SimplifyVisitor() : BaseVisitor<SimplifyVisitor, TransformVisitor>() {}

    void bvisit(const Mul &x);
    void bvisit(const Pow &x);
    void bvisit(const OneArgFunction &x);
};

class SYMENGINE_EXPORT PiecewiseFoldVisitor
    : public BaseVisitor<PiecewiseFoldVisitor, TransformVisitor>
{
private:
    // When rebuilding a node with new args, maps old args to new args
    const map_basic_basic *args_map_ = nullptr;

    // Rebuild x using TransformVisitor with its args replaced as in args_map
    RCP<const Basic> rebuild(const RCP<const Basic> &x,
                             const map_basic_basic &args_map);

public:
    using TransformVisitor::bvisit;

    PiecewiseFoldVisitor()
        : BaseVisitor<PiecewiseFoldVisitor, TransformVisitor>()
    {
    }

    RCP<const Basic> apply(const RCP<const Basic> &x) override;
    void bvisit(const Piecewise &x);
};

//! Takes an expression containing a Piecewise and returns the expression
//! in Piecewise form, i.e. f(Piecewise((a, c1), (b, True))) is rewritten
//! as Piecewise((f(a), c1), (f(b), True)). Nested Piecewise are flattened.
//! Relationals are folded too, e.g. Eq(Piecewise((a, c1), (b, True)), 0)
//! becomes Piecewise((Eq(a, 0), c1), (Eq(b, 0), True)), but conditions of a
//! Piecewise are left untouched.
SYMENGINE_EXPORT
RCP<const Basic> piecewise_fold(const RCP<const Basic> &x);

SYMENGINE_EXPORT
RCP<const Basic> simplify(const RCP<const Basic> &x,
                          const Assumptions *assumptions = nullptr);

} // namespace SymEngine

#endif
