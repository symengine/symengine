#ifndef SYMENGINE_LLVM_DOUBLE_H
#define SYMENGINE_LLVM_DOUBLE_H

#include <symengine/basic.h>
#include <symengine/visitor.h>

#ifdef HAVE_SYMENGINE_LLVM
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

namespace SymEngine
{

class LLVMDoubleVisitor : public BaseVisitor<LLVMDoubleVisitor>
{
protected:
    llvm::Value *result_;
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    llvm::Module *M;
    llvm::Function *F;
    llvm::ExecutionEngine *EE;
    llvm::FunctionType *FT;
    vec_basic symbols;
    intptr_t func;

public:
    llvm::Value *apply(const Basic &b)
    {
        b.accept(*this);
        return result_;
    }

    void init(const vec_basic &x, const Basic &b)
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        context = llvm::make_unique<llvm::LLVMContext>();
        symbols = x;

        // Create some module to put our function into it.
        std::unique_ptr<llvm::Module> owner
            = llvm::make_unique<llvm::Module>("SymEngine", *context);
        M = owner.get();

        // Create a new pass manager attached to it.
        auto fpm = llvm::make_unique<llvm::legacy::FunctionPassManager>(M);

        // Do simple "peephole" optimizations and bit-twiddling optzns.
        fpm->add(llvm::createInstructionCombiningPass());
        // Reassociate expressions.
        fpm->add(llvm::createReassociatePass());
        // Eliminate Common SubExpressions.
        fpm->add(llvm::createGVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        fpm->add(llvm::createCFGSimplificationPass());
        fpm->add(llvm::createScalarReplAggregatesPass());

        fpm->doInitialization();

        std::vector<llvm::Type *> inp;
        inp.push_back(
            llvm::PointerType::get(llvm::Type::getDoubleTy(*context), 0));
        FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), inp,
                                     false);
        F = llvm::Function::Create(FT, llvm::Function::InternalLinkage, "", M);

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

        apply(b);

        // Create the return instruction and add it to the basic block
        builder->CreateRet(result_);

        // Validate the generated code, checking for consistency.
        llvm::verifyFunction(*F);

        // std::cout << "LLVM IR for " << b.__str__() << std::endl;
        // M->dump();

        // Optimize the function.
        fpm->run(*F);

        // std::cout << "LLVM IR for " << b.__str__() << std::endl;
        // M->dump();

        // Now we create the JIT.
        std::string error;
        EE = llvm::EngineBuilder(std::move(owner)).setErrorStr(&error).create();
        // std::cout << error << std::endl;
        EE->finalizeObject();

        // Get the symbol's address
        func = (intptr_t)EE->getPointerToFunction(F);
    }

    double call(const std::vector<double> &vec)
    {
        return ((double (*)(const double *))func)(vec.data());
    }

    void set_double(double d)
    {
        result_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context), d);
    }

    void bvisit(const Integer &x, bool as_int64 = false)
    {
        if (as_int64) {
            int64_t d = mp_get_si(x.i);
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
        llvm::Value *tmp = apply(*x.coef_);
        llvm::Value *tmp1, *tmp2;
        for (const auto &p : x.dict_) {
            tmp1 = apply(*(p.first));
            tmp2 = apply(*(p.second));
            tmp = builder->CreateFAdd(tmp, builder->CreateFMul(tmp1, tmp2));
        }
        result_ = tmp;
    }

    void bvisit(const Mul &x)
    {
        llvm::Value *tmp = apply(*x.coef_);
        for (const auto &p : x.get_args()) {
            tmp = builder->CreateFMul(tmp, apply(*p));
        }
        result_ = tmp;
    }

    llvm::Function *get_pow()
    {
        std::vector<llvm::Type *> arg_type;
        arg_type.push_back(llvm::Type::getDoubleTy(*context));
        arg_type.push_back(llvm::Type::getDoubleTy(*context));
        return llvm::Intrinsic::getDeclaration(M, llvm::Intrinsic::pow,
                                               arg_type);
    }

    llvm::Function *get_double_intrinsic(llvm::Intrinsic::ID id)
    {
        std::vector<llvm::Type *> arg_type;
        arg_type.push_back(llvm::Type::getDoubleTy(*context));
        return llvm::Intrinsic::getDeclaration(M, id, arg_type);
    }

    void bvisit(const Pow &x)
    {
        std::vector<llvm::Value *> args;
        llvm::Function *fun;
        if (eq(*(x.get_base()), *E)) {
            args.push_back(apply(*x.get_exp()));
            fun = get_double_intrinsic(llvm::Intrinsic::exp);

        } else if (eq(*(x.get_base()), *integer(2))) {
            args.push_back(apply(*x.get_exp()));
            fun = get_double_intrinsic(llvm::Intrinsic::exp2);

        } else {
            args.push_back(apply(*x.get_base()));
            args.push_back(apply(*x.get_exp()));
            fun = get_pow();
        }
        result_ = builder->CreateCall(fun, args);
    }

    void bvisit(const Sin &x)
    {
        std::vector<llvm::Value *> args;
        llvm::Function *fun;
        args.push_back(apply(*x.get_arg()));
        fun = get_double_intrinsic(llvm::Intrinsic::sin);
        result_ = builder->CreateCall(fun, args);
    }

    void bvisit(const Cos &x)
    {
        std::vector<llvm::Value *> args;
        llvm::Function *fun;
        args.push_back(apply(*x.get_arg()));
        fun = get_double_intrinsic(llvm::Intrinsic::cos);
        result_ = builder->CreateCall(fun, args);
    }

    void bvisit(const Tan &x)
    {
        throw std::runtime_error("Not implemented.");
    }

    void bvisit(const Symbol &x)
    {
        unsigned i = 0;
        for (auto &symb : symbols) {
            if (static_cast<const Symbol &>(*symb).get_name() == x.get_name()) {
                auto inp = &(*(F->args().begin()));
                auto index = llvm::ConstantInt::get(
                    llvm::Type::getInt32Ty(*context), i);
                auto ptr = builder->CreateGEP(llvm::Type::getDoubleTy(*context),
                                              inp, index);
                result_ = builder->CreateLoad(llvm::Type::getDoubleTy(*context),
                                              ptr);
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
        throw std::runtime_error("Not implemented.");
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
