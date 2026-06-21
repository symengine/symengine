#ifndef SYMENGINE_CALCULATE_H
#define SYMENGINE_CALCULATE_H

#include <symengine/visitor.h>
#include <symengine/basic.h>
#include <symengine/assumptions.h>

namespace SymEngine
{

RCP<const Basic> gamma_multiple_2(const RCP<const Basic> &arg);
RCP<const Basic> gamma_positive_int(const RCP<const Basic> &arg);

class CalculateVisitor : public BaseVisitor<CalculateVisitor, TransformVisitor>
{
public:
    using TransformVisitor::bvisit;

    CalculateVisitor() : BaseVisitor<CalculateVisitor, TransformVisitor>() {}

    void bvisit(const Gamma &x);
};

RCP<const Basic> calculate(const RCP<const Basic> &x);

} // namespace SymEngine

#endif
