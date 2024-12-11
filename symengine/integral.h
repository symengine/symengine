#ifndef SYMENGINE_INTEGRALS_H
#define SYMENGINE_INTEGRALS_H

#include <symengine/visitor.h>
#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>
#include <symengine/functions.h>
#include <symengine/number.h> 
#include <symengine/constants.h>
// #include <symengine/subs.h>
// #include <symengine/symengine_casts.h>

namespace SymEngine {

/* Class for Integral Visitor 
*/
class IntegralVisitor : public BaseVisitor<IntegralVisitor, Visitor> {
public:
    RCP<const Symbol> var_; // Variable of integration
    RCP<const Basic> result_; // result of evaluated integral 

    IntegralVisitor() : result_{zero} {}
    // IntegralVisitor Class functions
    bool depends_on(const RCP<const Basic> &expr, const RCP<const Symbol> &sym);
    void set_var(const RCP<const Symbol> &var) { var_ = var; }

    // general bvisit functions
    void bvisit(const Integer &x);
    void bvisit(const Rational &x);
    void bvisit(const Constant &x);
    void bvisit(const Symbol &x);
    void bvisit(const Add &x);
    void bvisit(const Mul &x);
    void bvisit(const Pow &x);
    void bvisit(const Sin &x);
    void bvisit(const Cos &x);
    void bvisit(const Basic &x);
};

// Integrate function that fully evaluates integral
RCP<const Basic> integrate(const RCP<const Basic> &expr, const RCP<const Symbol> &var);

} // namespace SymEngine

#endif // SYMENGINE_INTEGRALS_H
