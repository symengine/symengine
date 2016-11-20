#ifndef SYMENGINE_LLVM_DOUBLE_H
#define SYMENGINE_LLVM_DOUBLE_H

#include <symengine/basic.h>
#include <symengine/visitor.h>

#ifdef HAVE_SYMENGINE_LLVM
// byte_swap is a macro defined in flint and it conflicts with LLVM's definition
#undef byte_swap

#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
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

#if LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 9
#include <llvm/Transforms/Scalar/GVN.h>
#endif

namespace SymEngine
{

class LLVMDoubleVisitor : public BaseVisitor<LLVMDoubleVisitor>
{
protected:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    vec_basic symbols;
    std::vector<llvm::Value *> symbol_ptrs;
    llvm::Value *result_;
    intptr_t func;

public:
    llvm::Value *apply(const Basic &b)
    {
        b.accept(*this);
        return result_;
    }

    void init(const vec_basic &x, const Basic &b)
    {
        init(x, {b.rcp_from_this()});
    }

    void init(const vec_basic &inputs, const vec_basic &outputs)
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        context = llvm::make_unique<llvm::LLVMContext>();
        symbols = inputs;

        // Create some module to put our function into it.
        module = llvm::make_unique<llvm::Module>("SymEngine", *context);
        module->setDataLayout("");

        // Create a new pass manager attached to it.
        auto fpm = llvm::make_unique<llvm::legacy::FunctionPassManager>(
            module.get());

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
        fpm->add(llvm::createLoadCombinePass());
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
            llvm::Type::getVoidTy(*context), inp, false);
        auto F = llvm::Function::Create(
            function_type, llvm::Function::InternalLinkage, "", module.get());
        F->setCallingConv(llvm::CallingConv::C);
        {
            llvm::SmallVector<llvm::AttributeSet, 4> attrs;
            llvm::AttributeSet PAS;
            {
                llvm::AttrBuilder B;
                B.addAttribute(llvm::Attribute::ReadOnly);
                B.addAttribute(llvm::Attribute::NoCapture);
                PAS = llvm::AttributeSet::get(module->getContext(), 1U, B);
            }

            attrs.push_back(PAS);
            {
                llvm::AttrBuilder B;
                B.addAttribute(llvm::Attribute::NoCapture);
                PAS = llvm::AttributeSet::get(module->getContext(), 2U, B);
            }

            attrs.push_back(PAS);
            {
                llvm::AttrBuilder B;
                B.addAttribute(llvm::Attribute::NoUnwind);
                B.addAttribute(llvm::Attribute::UWTable);
                PAS = llvm::AttributeSet::get(module->getContext(), ~0U, B);
            }

            attrs.push_back(PAS);

            F->setAttributes(
                llvm::AttributeSet::get(module->getContext(), attrs));
        }

        // Add a basic block to the function. As before, it automatically
        // inserts
        // because of the last argument.
        llvm::BasicBlock *BB
            = llvm::BasicBlock::Create(*context, "EntryBlock", F);

        // Create a basic block builder with default parameters.  The builder
        // will
        // automatically append instructions to the basic block `BB'.
        builder = llvm::make_unique<llvm::IRBuilder<>>(BB);
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
            result_
                = builder->CreateLoad(llvm::Type::getDoubleTy(*context), ptr);
            symbol_ptrs.push_back(result_);
        }

        auto it = ++(F->args().begin());
        auto out = &(*it);
        std::vector<llvm::Value *> output_vals;

        // Generate Ir for all the output exprs and save references
        for (unsigned i = 0; i < outputs.size(); i++) {
            output_vals.push_back(apply(*outputs[i]));
        }

        // Store all the output exprs at the end
        for (unsigned i = 0; i < outputs.size(); i++) {
            auto index
                = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), i);
            auto ptr = builder->CreateGEP(llvm::Type::getDoubleTy(*context),
                                          out, index);
            builder->CreateStore(output_vals[i], ptr);
        }

        // Create the return instruction and add it to the basic block
        builder->CreateRetVoid();

        // Validate the generated code, checking for consistency.
        llvm::verifyFunction(*F);

        // std::cout << "LLVM IR" << std::endl;
        // module->dump();

        // Optimize the function.
        fpm->run(*F);

        // std::cout << "Optimized LLVM IR" << std::endl;
        // module->dump();

        // Now we create the JIT.
        std::string error;
        auto executionengine
            = llvm::EngineBuilder(std::move(module))
                  .setEngineKind(llvm::EngineKind::Kind::JIT)
                  .setOptLevel(llvm::CodeGenOpt::Level::Aggressive)
                  .setErrorStr(&error)
                  .create();
        // std::cout << error << std::endl;
        executionengine->finalizeObject();

        // Get the symbol's address
        func = (intptr_t)executionengine->getPointerToFunction(F);
    }

    double call(const std::vector<double> &vec)
    {
        double ret;
        ((double (*)(const double *, double *))func)(vec.data(), &ret);
        return ret;
    }

    void call(double *outs, const double *inps)
    {
        ((double (*)(const double *, double *))func)(inps, outs);
    }

    void set_double(double d)
    {
        result_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context), d);
    }

    void bvisit(const Integer &x, bool as_int32 = false)
    {
        if (as_int32) {
            int d = mp_get_si(x.i);
            result_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context),
                                             d, true);
        } else {
            result_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context),
                                            mp_get_d(x.i));
        }
    }

    void bvisit(const Rational &x)
    {
        set_double(mp_get_d(x.i));
    }

    void bvisit(const RealDouble &x)
    {
        set_double(x.i);
    }

#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x)
    {
        set_double(mpfr_get_d(x.i.get_mpfr_t(), MPFR_RNDN));
    }
#endif

    void bvisit(const Add &x)
    {
        llvm::Value *tmp, *tmp1, *tmp2;
        auto it = x.get_dict().begin();

        if (eq(*x.coef_, *zero)) {
            // `x + 0.0` is not optimized out
            tmp1 = apply(*(it->first));
            tmp2 = apply(*(it->second));
            tmp = builder->CreateFMul(tmp1, tmp2);
            ++it;
        } else {
            tmp = apply(*x.coef_);
        }

        for (; it != x.get_dict().end(); ++it) {
            tmp1 = apply(*(it->first));
            tmp2 = apply(*(it->second));
            // if (eq(*it->second, *one)) {
            tmp = builder->CreateFAdd(tmp, builder->CreateFMul(tmp1, tmp2));
            //} else {
            //    std::vector<llvm::Value *> args({tmp1, tmp2, tmp});
            //    tmp =
            //    builder->CreateCall(get_double_intrinsic(llvm::Intrinsic::fma,
            //    3), args);
            //}
        }
        result_ = tmp;
    }

    void bvisit(const Mul &x)
    {
        set_double(1.0);
        llvm::Value *tmp = result_;
        for (const auto &p : x.get_args()) {
            tmp = builder->CreateFMul(tmp, apply(*p));
        }
        result_ = tmp;
    }

    llvm::Function *get_powi()
    {
        std::vector<llvm::Type *> arg_type;
        arg_type.push_back(llvm::Type::getDoubleTy(*context));
        arg_type.push_back(llvm::Type::getInt32Ty(*context));
        return llvm::Intrinsic::getDeclaration(module.get(),
                                               llvm::Intrinsic::powi, arg_type);
    }

    llvm::Function *get_double_intrinsic(llvm::Intrinsic::ID id, unsigned n = 1)
    {
        std::vector<llvm::Type *> arg_type(n,
                                           llvm::Type::getDoubleTy(*context));
        return llvm::Intrinsic::getDeclaration(module.get(), id, arg_type);
    }

    void bvisit(const Pow &x)
    {
        std::vector<llvm::Value *> args;
        llvm::Function *fun;
        if (eq(*(x.get_base()), *E)) {
            args.push_back(apply(*x.get_exp()));
            fun = get_double_intrinsic(llvm::Intrinsic::exp, 1);

        } else if (eq(*(x.get_base()), *integer(2))) {
            args.push_back(apply(*x.get_exp()));
            fun = get_double_intrinsic(llvm::Intrinsic::exp2, 1);

        } else {
            if (is_a<Integer>(*x.get_exp())) {
                if (eq(*x.get_exp(), *integer(2))) {
                    llvm::Value *tmp = apply(*x.get_base());
                    result_ = builder->CreateFMul(tmp, tmp);
                    return;
                } else {
                    args.push_back(apply(*x.get_base()));
                    bvisit(static_cast<const Integer &>(*x.get_exp()), true);
                    args.push_back(result_);
                    fun = get_powi();
                }
            } else {
                args.push_back(apply(*x.get_base()));
                args.push_back(apply(*x.get_exp()));
                fun = get_double_intrinsic(llvm::Intrinsic::pow, 2);
            }
        }
        auto r = builder->CreateCall(fun, args);
        r->setTailCall(true);
        result_ = r;
    }

    void bvisit(const Sin &x)
    {
        std::vector<llvm::Value *> args;
        llvm::Function *fun;
        args.push_back(apply(*x.get_arg()));
        fun = get_double_intrinsic(llvm::Intrinsic::sin, 1);
        auto r = builder->CreateCall(fun, args);
        r->setTailCall(true);
        result_ = r;
    }

    void bvisit(const Cos &x)
    {
        std::vector<llvm::Value *> args;
        llvm::Function *fun;
        args.push_back(apply(*x.get_arg()));
        fun = get_double_intrinsic(llvm::Intrinsic::cos, 1);
        auto r = builder->CreateCall(fun, args);
        r->setTailCall(true);
        result_ = r;
    }

    void bvisit(const Tan &x)
    {
        throw std::runtime_error("Not implemented.");
    }

    void bvisit(const Symbol &x)
    {
        unsigned i = 0;
        for (auto &symb : symbols) {
            if (eq(x, *symb)) {
                result_ = symbol_ptrs[i];
                return;
            }
            ++i;
        }
        throw std::runtime_error("Symbol not in the symbols vector.");
    };

    void bvisit(const Log &x)
    {
        throw std::runtime_error("Not implemented.");
    };

    void bvisit(const Constant &x)
    {
        if (eq(x, *pi)) {
            set_double(3.1415926535897932);
        } else if (eq(x, *E)) {
            set_double(2.7182818284590452);
        } else if (eq(x, *EulerGamma)) {
            set_double(0.57721566490153286);
        } else {
            throw SymEngineException("Constant " + x.get_name()
                                     + " is not implemented.");
        }
    };

    void bvisit(const Abs &x)
    {
        throw std::runtime_error("Not implemented.");
    };

    void bvisit(const Basic &)
    {
        throw std::runtime_error("Not implemented.");
    };
};
}
#endif
#endif // SYMENGINE_LAMBDA_DOUBLE_H
