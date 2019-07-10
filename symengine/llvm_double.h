#ifndef SYMENGINE_LLVM_DOUBLE_H
#define SYMENGINE_LLVM_DOUBLE_H

#include <symengine/basic.h>
#include <symengine/visitor.h>

#ifdef HAVE_SYMENGINE_LLVM

// Forward declare llvm types
namespace llvm
{
class Module;
class Value;
class Function;
class ExecutionEngine;
class MemoryBufferRef;
class LLVMContext;
class Pass;
namespace legacy
{
class FunctionPassManager;
}
}

namespace SymEngine
{

class IRBuilder;

class LLVMDoubleVisitor : public BaseVisitor<LLVMDoubleVisitor>
{
protected:
    vec_basic symbols;
    std::vector<llvm::Value *> symbol_ptrs;
    std::map<RCP<const Basic>, llvm::Value *, RCPBasicKeyLess>
        replacement_symbol_ptrs;
    llvm::Value *result_;
    std::shared_ptr<llvm::LLVMContext> context;
    std::shared_ptr<llvm::ExecutionEngine> executionengine;
    std::shared_ptr<llvm::legacy::FunctionPassManager> fpm;
    intptr_t func;

    // Following are invalid after the init call.
    IRBuilder *builder;
    llvm::Module *mod;
    std::string membuffer;
    llvm::Function *get_function_type(llvm::LLVMContext *);

public:
    llvm::Value *apply(const Basic &b);
    void init(const vec_basic &x, const Basic &b,
              const bool symbolic_cse = false, int opt_level = 2);
    void init(const vec_basic &x, const Basic &b, const bool symbolic_cse,
              const std::vector<llvm::Pass *> &passes);
    void init(const vec_basic &inputs, const vec_basic &outputs,
              const bool symbolic_cse = false, int opt_level = 2);
    void init(const vec_basic &inputs, const vec_basic &outputs,
              const bool symbolic_cse, const std::vector<llvm::Pass *> &passes);

    static std::vector<llvm::Pass *> create_default_passes(int optlevel);

    double call(const std::vector<double> &vec);
    void call(double *outs, const double *inps);

    // Helper functions
    void set_double(double d);
    llvm::Function *get_external_function(const std::string &name,
                                          size_t nargs = 1);
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
    void bvisit(const Log &x);
    void bvisit(const Abs &x);
    void bvisit(const Symbol &x);
    void bvisit(const Constant &x);
    void bvisit(const Basic &);
    void bvisit(const Sin &x);
    void bvisit(const Cos &x);
    void bvisit(const Tan &x);
    void bvisit(const ASin &x);
    void bvisit(const ACos &x);
    void bvisit(const ATan &x);
    void bvisit(const ATan2 &x);
    void bvisit(const Sinh &x);
    void bvisit(const Cosh &x);
    void bvisit(const Tanh &x);
    void bvisit(const ASinh &x);
    void bvisit(const ACosh &x);
    void bvisit(const ATanh &x);
    void bvisit(const Gamma &x);
    void bvisit(const LogGamma &x);
    void bvisit(const Erf &x);
    void bvisit(const Erfc &x);
    void bvisit(const Piecewise &x);
    void bvisit(const BooleanAtom &x);
    void bvisit(const And &x);
    void bvisit(const Or &x);
    void bvisit(const Xor &x);
    void bvisit(const Not &x);
    void bvisit(const Equality &x);
    void bvisit(const Unequality &x);
    void bvisit(const LessThan &x);
    void bvisit(const StrictLessThan &x);
    void bvisit(const Max &x);
    void bvisit(const Min &x);
    void bvisit(const Contains &x);
    void bvisit(const Infty &x);
    void bvisit(const Floor &x);
    void bvisit(const Ceiling &x);
    void bvisit(const Truncate &x);
    void bvisit(const Sign &x);
    // Return the compiled function as a binary string which can be loaded using
    // `load`
    const std::string &dumps() const;
    // Load a previously compiled function from a string
    void loads(const std::string &s);
};
}
#endif
#endif // SYMENGINE_LAMBDA_DOUBLE_H
