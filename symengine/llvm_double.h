#ifndef SYMENGINE_LLVM_DOUBLE_H
#define SYMENGINE_LLVM_DOUBLE_H

#include <symengine/basic.h>
#include <symengine/visitor.h>

#ifdef HAVE_SYMENGINE_LLVM

// Forward declare llvm types
namespace llvm
{
struct Module;
struct Value;
struct Function;
}

namespace SymEngine
{

struct IRBuilder;

class LLVMDoubleVisitor : public BaseVisitor<LLVMDoubleVisitor>
{
protected:
    vec_basic symbols;
    std::vector<llvm::Value *> symbol_ptrs;
    llvm::Value *result_;
    intptr_t func;

    // Following are invalid after the init call.
    IRBuilder *builder;
    llvm::Module *mod;

public:
    llvm::Value *apply(const Basic &b);
    void init(const vec_basic &x, const Basic &b);
    void init(const vec_basic &inputs, const vec_basic &outputs);

    double call(const std::vector<double> &vec);
    void call(double *outs, const double *inps);

    // Helper functions
    void set_double(double d);
    llvm::Function *get_external_function(const std::string &name);
    llvm::Function *get_powi();

    void bvisit(const Integer &x, bool as_int32 = false);
    void bvisit(const Rational &x);
    void bvisit(const RealDouble &x);
#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x);
#endif
    void bvisit(const Add &x);
    void bvisit(const Mul &x);
    void bvisit(const Pow &x);
    void bvisit(const Sin &x);
    void bvisit(const Cos &x);
    void bvisit(const Log &x);
    void bvisit(const Tan &x);
    void bvisit(const Abs &x);
    void bvisit(const Symbol &x);
    void bvisit(const Constant &x);
    void bvisit(const Basic &);
};
}
#endif
#endif // SYMENGINE_LAMBDA_DOUBLE_H
