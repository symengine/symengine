#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

#if (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 9)                       \
    || (LLVM_VERSION_MAJOR > 3)
#include <llvm/Transforms/Scalar/GVN.h>
#endif

#include <symengine/llvm_double.h>
#include <symengine/eval_double.h>

namespace SymEngine
{

class IRBuilder : public llvm::IRBuilder<>
{
};

llvm::Value *LLVMDoubleVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return result_;
}

void LLVMDoubleVisitor::init(const vec_basic &x, const Basic &b, bool cse)
{
    init(x, {b.rcp_from_this()}, cse);
}

void LLVMDoubleVisitor::init(const vec_basic &inputs, const vec_basic &outputs,
                             bool cse)
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    std::unique_ptr<llvm::LLVMContext> context
        = llvm::make_unique<llvm::LLVMContext>();
    symbols = inputs;

    // Create some module to put our function into it.
    std::unique_ptr<llvm::Module> module
        = llvm::make_unique<llvm::Module>("SymEngine", *context);
    module->setDataLayout("");
    mod = module.get();

    // Create a new pass manager attached to it.
    auto fpm = llvm::make_unique<llvm::legacy::FunctionPassManager>(mod);

    // Provide basic AliasAnalysis support for GVN.
    // fpm->add(llvm::createBasicAliasAnalysisPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    fpm->add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    fpm->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    fpm->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    fpm->add(llvm::createCFGSimplificationPass());
    fpm->add(llvm::createPartiallyInlineLibCallsPass());
#if (LLVM_VERSION_MAJOR < 5)
    fpm->add(llvm::createLoadCombinePass());
#endif
    fpm->add(llvm::createInstructionSimplifierPass());
    fpm->add(llvm::createMemCpyOptPass());
    fpm->add(llvm::createMergedLoadStoreMotionPass());
    fpm->add(llvm::createBitTrackingDCEPass());
    fpm->add(llvm::createAggressiveDCEPass());

    fpm->doInitialization();

    std::vector<llvm::Type *> inp;
    for (int i = 0; i < 2; i++) {
        inp.push_back(
            llvm::PointerType::get(llvm::Type::getDoubleTy(*context), 0));
    }
    llvm::FunctionType *function_type = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*context), inp, /*isVarArgs=*/false);
    auto F = llvm::Function::Create(function_type,
                                    llvm::Function::InternalLinkage, "", mod);
    F->setCallingConv(llvm::CallingConv::C);
#if (LLVM_VERSION_MAJOR < 5)
    {
        llvm::SmallVector<llvm::AttributeSet, 4> attrs;
        llvm::AttributeSet PAS;
        {
            llvm::AttrBuilder B;
            B.addAttribute(llvm::Attribute::ReadOnly);
            B.addAttribute(llvm::Attribute::NoCapture);
            PAS = llvm::AttributeSet::get(mod->getContext(), 1U, B);
        }

        attrs.push_back(PAS);
        {
            llvm::AttrBuilder B;
            B.addAttribute(llvm::Attribute::NoCapture);
            PAS = llvm::AttributeSet::get(mod->getContext(), 2U, B);
        }

        attrs.push_back(PAS);
        {
            llvm::AttrBuilder B;
            B.addAttribute(llvm::Attribute::NoUnwind);
            B.addAttribute(llvm::Attribute::UWTable);
            PAS = llvm::AttributeSet::get(mod->getContext(), ~0U, B);
        }

        attrs.push_back(PAS);

        F->setAttributes(llvm::AttributeSet::get(mod->getContext(), attrs));
    }
#else
    F->addParamAttr(1, llvm::Attribute::ReadOnly);
    F->addParamAttr(1, llvm::Attribute::NoCapture);
    F->addParamAttr(2, llvm::Attribute::NoCapture);
    F->addFnAttr(llvm::Attribute::NoUnwind);
    F->addFnAttr(llvm::Attribute::UWTable);
#endif

    // Add a basic block to the function. As before, it automatically
    // inserts
    // because of the last argument.
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "EntryBlock", F);

    // Create a basic block builder with default parameters.  The builder
    // will
    // automatically append instructions to the basic block `BB'.
    llvm::IRBuilder<> _builder = llvm::IRBuilder<>(BB);
    builder = reinterpret_cast<IRBuilder *>(&_builder);
    builder->SetInsertPoint(BB);
    auto fmf = llvm::FastMathFlags();
    // fmf.setUnsafeAlgebra();
    builder->setFastMathFlags(fmf);

    // Load all the symbols and create references
    auto input_arg = &(*(F->args().begin()));
    for (unsigned i = 0; i < inputs.size(); i++) {
        if (not is_a<Symbol>(*inputs[i])) {
            throw SymEngineException("Input contains a non-symbol.");
        }
        auto index
            = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), i);
        auto ptr = builder->CreateGEP(llvm::Type::getDoubleTy(*context),
                                      input_arg, index);
        result_ = builder->CreateLoad(llvm::Type::getDoubleTy(*context), ptr);
        symbol_ptrs.push_back(result_);
    }

    auto it = F->args().begin();
#if (LLVM_VERSION_MAJOR < 5)
    auto out = &(*(++it));
#else
    auto out = &(*(it + 1));
#endif
    std::vector<llvm::Value *> output_vals;

    if (cse) {
        vec_basic reduced_exprs;
        vec_pair replacements;
        // cse the outputs
        SymEngine::cse(replacements, reduced_exprs, outputs);
        for (auto &rep : replacements) {
            // Store the replacement symbol values in a dictionary
            replacement_symbol_ptrs[rep.first] = apply(*(rep.second));
        }
        // Generate IR for all the reduced exprs and save references
        for (unsigned i = 0; i < outputs.size(); i++) {
            output_vals.push_back(apply(*reduced_exprs[i]));
        }
    } else {
        // Generate IR for all the output exprs and save references
        for (unsigned i = 0; i < outputs.size(); i++) {
            output_vals.push_back(apply(*outputs[i]));
        }
    }

    // Store all the output exprs at the end
    for (unsigned i = 0; i < outputs.size(); i++) {
        auto index
            = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), i);
        auto ptr
            = builder->CreateGEP(llvm::Type::getDoubleTy(*context), out, index);
        builder->CreateStore(output_vals[i], ptr);
    }

    // Create the return instruction and add it to the basic block
    builder->CreateRetVoid();

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*F);

    //     std::cout << "LLVM IR" << std::endl;
    // #if (LLVM_VERSION_MAJOR < 5)
    //     module->dump();
    // #else
    //     module->print(llvm::errs(), nullptr);
    // #endif

    // Optimize the function.
    fpm->run(*F);

    // std::cout << "Optimized LLVM IR" << std::endl;
    // module->dump();

    // Now we create the JIT.
    std::string error;
    auto executionengine = llvm::EngineBuilder(std::move(module))
                               .setEngineKind(llvm::EngineKind::Kind::JIT)
                               .setOptLevel(llvm::CodeGenOpt::Level::Aggressive)
                               .setErrorStr(&error)
                               .create();

    // std::cout << error << std::endl;
    executionengine->finalizeObject();

    // Get the symbol's address
    func = (intptr_t)executionengine->getPointerToFunction(F);
}

double LLVMDoubleVisitor::call(const std::vector<double> &vec)
{
    double ret;
    ((double (*)(const double *, double *))func)(vec.data(), &ret);
    return ret;
}

void LLVMDoubleVisitor::call(double *outs, const double *inps)
{
    ((double (*)(const double *, double *))func)(inps, outs);
}

void LLVMDoubleVisitor::set_double(double d)
{
    result_
        = llvm::ConstantFP::get(llvm::Type::getDoubleTy(mod->getContext()), d);
}

void LLVMDoubleVisitor::bvisit(const Integer &x, bool as_int32)
{
    if (as_int32) {
        int d = numeric_cast<int>(mp_get_si(x.as_integer_class()));
        result_ = llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(mod->getContext()), d, true);
    } else {
        result_
            = llvm::ConstantFP::get(llvm::Type::getDoubleTy(mod->getContext()),
                                    mp_get_d(x.as_integer_class()));
    }
}

void LLVMDoubleVisitor::bvisit(const Rational &x)
{
    set_double(mp_get_d(x.as_rational_class()));
}

void LLVMDoubleVisitor::bvisit(const RealDouble &x)
{
    set_double(x.i);
}

#ifdef HAVE_SYMENGINE_MPFR
void LLVMDoubleVisitor::bvisit(const RealMPFR &x)
{
    set_double(mpfr_get_d(x.i.get_mpfr_t(), MPFR_RNDN));
}
#endif

void LLVMDoubleVisitor::bvisit(const Add &x)
{
    llvm::Value *tmp, *tmp1, *tmp2;
    auto it = x.get_dict().begin();

    if (eq(*x.get_coef(), *zero)) {
        // `x + 0.0` is not optimized out
        if (eq(*one, *(it->second))) {
            tmp = apply(*(it->first));
        } else {
            tmp1 = apply(*(it->first));
            tmp2 = apply(*(it->second));
            tmp = builder->CreateFMul(tmp1, tmp2);
        }
        ++it;
    } else {
        tmp = apply(*x.get_coef());
    }

    for (; it != x.get_dict().end(); ++it) {
        if (eq(*one, *(it->second))) {
            tmp1 = apply(*(it->first));
            tmp = builder->CreateFAdd(tmp, tmp1);
        } else {
            //    std::vector<llvm::Value *> args({tmp1, tmp2, tmp});
            //    tmp =
            //    builder->CreateCall(get_double_intrinsic(llvm::Intrinsic::fma,
            //    3, context), args);
            tmp1 = apply(*(it->first));
            tmp2 = apply(*(it->second));
            tmp = builder->CreateFAdd(tmp, builder->CreateFMul(tmp1, tmp2));
        }
    }
    result_ = tmp;
}

void LLVMDoubleVisitor::bvisit(const Mul &x)
{
    llvm::Value *tmp = nullptr;
    bool first = true;
    for (const auto &p : x.get_args()) {
        if (first) {
            tmp = apply(*p);
        } else {
            tmp = builder->CreateFMul(tmp, apply(*p));
        }
        first = false;
    }
    result_ = tmp;
}

llvm::Function *LLVMDoubleVisitor::get_powi()
{
    std::vector<llvm::Type *> arg_type;
    arg_type.push_back(llvm::Type::getDoubleTy(mod->getContext()));
    arg_type.push_back(llvm::Type::getInt32Ty(mod->getContext()));
    return llvm::Intrinsic::getDeclaration(mod, llvm::Intrinsic::powi,
                                           arg_type);
}

llvm::Function *get_double_intrinsic(llvm::Intrinsic::ID id, unsigned n,
                                     llvm::Module *mod)
{
    std::vector<llvm::Type *> arg_type(
        n, llvm::Type::getDoubleTy(mod->getContext()));
    return llvm::Intrinsic::getDeclaration(mod, id, arg_type);
}

void LLVMDoubleVisitor::bvisit(const Pow &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    if (eq(*(x.get_base()), *E)) {
        args.push_back(apply(*x.get_exp()));
        fun = get_double_intrinsic(llvm::Intrinsic::exp, 1, mod);

    } else if (eq(*(x.get_base()), *integer(2))) {
        args.push_back(apply(*x.get_exp()));
        fun = get_double_intrinsic(llvm::Intrinsic::exp2, 1, mod);

    } else {
        if (is_a<Integer>(*x.get_exp())) {
            if (eq(*x.get_exp(), *integer(2))) {
                llvm::Value *tmp = apply(*x.get_base());
                result_ = builder->CreateFMul(tmp, tmp);
                return;
            } else {
                args.push_back(apply(*x.get_base()));
                bvisit(down_cast<const Integer &>(*x.get_exp()), true);
                args.push_back(result_);
                fun = get_powi();
            }
        } else {
            args.push_back(apply(*x.get_base()));
            args.push_back(apply(*x.get_exp()));
            fun = get_double_intrinsic(llvm::Intrinsic::pow, 2, mod);
        }
    }
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

void LLVMDoubleVisitor::bvisit(const Sin &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_double_intrinsic(llvm::Intrinsic::sin, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

void LLVMDoubleVisitor::bvisit(const Cos &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_double_intrinsic(llvm::Intrinsic::cos, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

void LLVMDoubleVisitor::bvisit(const Log &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_double_intrinsic(llvm::Intrinsic::log, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

#define ONE_ARG_EXTERNAL_FUNCTION(Class, ext)                                  \
    void LLVMDoubleVisitor::bvisit(const Class &x)                             \
    {                                                                          \
        llvm::Function *func = get_external_function(#ext);                    \
        auto r = builder->CreateCall(func, {apply(*x.get_arg())});             \
        r->setTailCall(true);                                                  \
        result_ = r;                                                           \
    }

ONE_ARG_EXTERNAL_FUNCTION(Abs, abs)
ONE_ARG_EXTERNAL_FUNCTION(Tan, tan)
ONE_ARG_EXTERNAL_FUNCTION(Sinh, sinh)
ONE_ARG_EXTERNAL_FUNCTION(Cosh, cosh)
ONE_ARG_EXTERNAL_FUNCTION(Tanh, tanh)
ONE_ARG_EXTERNAL_FUNCTION(ASinh, asinh)
ONE_ARG_EXTERNAL_FUNCTION(ACosh, acosh)
ONE_ARG_EXTERNAL_FUNCTION(ATanh, atanh)
ONE_ARG_EXTERNAL_FUNCTION(ASin, asin)
ONE_ARG_EXTERNAL_FUNCTION(ACos, acos)
ONE_ARG_EXTERNAL_FUNCTION(ATan, atan)
ONE_ARG_EXTERNAL_FUNCTION(Gamma, tgamma)
ONE_ARG_EXTERNAL_FUNCTION(LogGamma, lgamma)
ONE_ARG_EXTERNAL_FUNCTION(Erf, erf)
ONE_ARG_EXTERNAL_FUNCTION(Erfc, erfc)

void LLVMDoubleVisitor::bvisit(const Symbol &x)
{
    unsigned i = 0;
    for (auto &symb : symbols) {
        if (eq(x, *symb)) {
            result_ = symbol_ptrs[i];
            return;
        }
        ++i;
    }
    auto it = replacement_symbol_ptrs.find(x.rcp_from_this());
    if (it != replacement_symbol_ptrs.end()) {
        result_ = it->second;
        return;
    }
    throw std::runtime_error("Symbol " + x.__str__()
                             + " not in the symbols vector.");
}

llvm::Function *
LLVMDoubleVisitor::get_external_function(const std::string &name)
{
    std::vector<llvm::Type *> func_args;
    func_args.push_back(llvm::Type::getDoubleTy(mod->getContext()));
    llvm::FunctionType *func_type
        = llvm::FunctionType::get(llvm::Type::getDoubleTy(mod->getContext()),
                                  func_args, /*isVarArgs=*/false);

    llvm::Function *func = mod->getFunction(name);
    if (!func) {
        func = llvm::Function::Create(
            func_type, llvm::GlobalValue::ExternalLinkage, name, mod);
        func->setCallingConv(llvm::CallingConv::C);
    }
#if (LLVM_VERSION_MAJOR < 5)
    llvm::AttributeSet func_attr_set;
    {
        llvm::SmallVector<llvm::AttributeSet, 4> attrs;
        llvm::AttributeSet attr_set;
        {
            llvm::AttrBuilder attr_builder;
            attr_builder.addAttribute(llvm::Attribute::NoUnwind);
            attr_set
                = llvm::AttributeSet::get(mod->getContext(), ~0U, attr_builder);
        }

        attrs.push_back(attr_set);
        func_attr_set = llvm::AttributeSet::get(mod->getContext(), attrs);
    }
    func->setAttributes(func_attr_set);
#else
    func->addFnAttr(llvm::Attribute::NoUnwind);
#endif
    return func;
}

void LLVMDoubleVisitor::bvisit(const Constant &x)
{
    set_double(eval_double(x));
}

void LLVMDoubleVisitor::bvisit(const Basic &)
{
    throw std::runtime_error("Not implemented.");
}

} // namespace SymEngine
