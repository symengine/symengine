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
#include "llvm/Transforms/Vectorize.h"
#include "llvm/ExecutionEngine/ObjectCache.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>
#include <fstream>

#if (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 9)                       \
    || (LLVM_VERSION_MAJOR > 3)
#include <llvm/Transforms/Scalar/GVN.h>
#endif

#if (LLVM_VERSION_MAJOR >= 7)
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/InstSimplifyPass.h>
#include <llvm/Transforms/Utils.h>
#endif

#include <symengine/llvm_double.h>
#include <symengine/eval_double.h>

namespace {
    template<typename T>
    struct has_real_type_llvm : std::false_type {
    }
    template<typename T>
    struct real_type_getter_llvm {
        static_assert(has_real_type_llvm<T>::value, "LLVM does not support type");
    }
    template<>
    struct real_type_getter_llvm<float> {
        const static auto& getTy = llvm::Type::getFloatTy;
    }
    template<>
    struct real_type_getter_llvm<double> {
        const static auto& getTy = llvm::Type::getDoubleTy;
    }
    template<>
    struct real_type_getter_llvm<long double> {
        const static auto& getTy =
#ifdef HAVE_SYMENGINE_FP128
        llvm::Type::getFP128Ty
#else
        llvm::Type::getX86_FP80Ty
#endif
            ;
    }
}

namespace SymEngine
{

class IRBuilder : public llvm::IRBuilder<>
{
};

template <typename T>
llvm::Value *LLVMVisitor<T>::apply(const Basic &b)
{
    b.accept(*this);
    return result_;
}

template <typename T>
void LLVMVisitor<T>::init(const vec_basic &x, const Basic &b,
                             bool symbolic_cse, int opt_level)
{
    init(x, b, symbolic_cse,
         LLVMVisitor::create_default_passes(opt_level));
}

template <typename T>
void LLVMVisitor<T>::init(const vec_basic &x, const Basic &b,
                             bool symbolic_cse,
                             const std::vector<llvm::Pass *> &passes)
{
    init(x, {b.rcp_from_this()}, symbolic_cse, passes);
}

template <typename T>
llvm::Function *LLVMVisitor<T>::get_function_type(llvm::LLVMContext *context)
{
    std::vector<llvm::Type *> inp;
    for (int i = 0; i < 2; i++) {
        inp.push_back(
            llvm::PointerType::get(real_type_getter_llvm<T>::getTy(*context), 0));
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
    F->addParamAttr(0, llvm::Attribute::ReadOnly);
    F->addParamAttr(0, llvm::Attribute::NoCapture);
    F->addParamAttr(1, llvm::Attribute::NoCapture);
    F->addFnAttr(llvm::Attribute::NoUnwind);
    F->addFnAttr(llvm::Attribute::UWTable);
#endif
    return F;
}

template <typename T>
std::vector<llvm::Pass *> LLVMVisitor<T>::create_default_passes(int optlevel)
{
    std::vector<llvm::Pass *> passes;
    if (optlevel == 0) {
        return passes;
    }
#if (LLVM_VERSION_MAJOR < 4)
    passes.push_back(llvm::createInstructionCombiningPass());
#else
    passes.push_back(llvm::createInstructionCombiningPass(optlevel > 1));
#endif
    passes.push_back(llvm::createDeadInstEliminationPass());
    passes.push_back(llvm::createPromoteMemoryToRegisterPass());
    passes.push_back(llvm::createReassociatePass());
    passes.push_back(llvm::createGVNPass());
    passes.push_back(llvm::createCFGSimplificationPass());
    passes.push_back(llvm::createPartiallyInlineLibCallsPass());
#if (LLVM_VERSION_MAJOR < 5)
    passes.push_back(llvm::createLoadCombinePass());
#endif
#if LLVM_VERSION_MAJOR >= 7
    passes.push_back(llvm::createInstSimplifyLegacyPass());
#else
    passes.push_back(llvm::createInstructionSimplifierPass());
#endif
    passes.push_back(llvm::createMemCpyOptPass());
    passes.push_back(llvm::createSROAPass());
    passes.push_back(llvm::createMergedLoadStoreMotionPass());
    passes.push_back(llvm::createBitTrackingDCEPass());
    passes.push_back(llvm::createAggressiveDCEPass());
    if (optlevel > 2) {
        passes.push_back(llvm::createSLPVectorizerPass());
#if LLVM_VERSION_MAJOR >= 7
        passes.push_back(llvm::createInstSimplifyLegacyPass());
#else
        passes.push_back(llvm::createInstructionSimplifierPass());
#endif
    }
    return passes;
}

template <typename T>
void LLVMVisitor<T>::init(const vec_basic &inputs, const vec_basic &outputs,
                             const bool symbolic_cse, int opt_level)
{
    init(inputs, outputs, symbolic_cse,
         LLVMVisitor<T>::create_default_passes(opt_level));
}

template <typename T>
void LLVMVisitor<T>::init(const vec_basic &inputs, const vec_basic &outputs,
                             const bool symbolic_cse,
                             const std::vector<llvm::Pass *> &passes)
{
    executionengine.reset();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    context = std::make_shared<llvm::LLVMContext>();
    symbols = inputs;

    // Create some module to put our function into it.
    std::unique_ptr<llvm::Module> module
        = llvm::make_unique<llvm::Module>("SymEngine", *context.get());
    module->setDataLayout("");
    mod = module.get();

    // Create a new pass manager attached to it.
    fpm = std::make_shared<llvm::legacy::FunctionPassManager>(mod);
    for (auto pass : passes) {
        fpm->add(pass);
    }
    fpm->doInitialization();

    auto F = get_function_type(context.get());

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
        auto ptr = builder->CreateGEP(real_type_getter_llvm<T>::getTy(*context),
                                      input_arg, index);
        result_ = builder->CreateLoad(real_type_getter_llvm<T>::getTy(*context), ptr);
        symbol_ptrs.push_back(result_);
    }

    auto it = F->args().begin();
#if (LLVM_VERSION_MAJOR < 5)
    auto out = &(*(++it));
#else
    auto out = &(*(it + 1));
#endif
    std::vector<llvm::Value *> output_vals;

    if (symbolic_cse) {
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
            = builder->CreateGEP(real_type_getter_llvm<T>::getTy(*context), out, index);
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
    executionengine = std::shared_ptr<llvm::ExecutionEngine>(
        llvm::EngineBuilder(std::move(module))
            .setEngineKind(llvm::EngineKind::Kind::JIT)
            .setOptLevel(llvm::CodeGenOpt::Level::Aggressive)
            .setErrorStr(&error)
            .create());

    // This is a hack to get the MemoryBuffer of a compiled object.
    class MemoryBufferRefCallback : public llvm::ObjectCache
    {
    public:
        std::string &ss_;
        MemoryBufferRefCallback(std::string &ss) : ss_(ss)
        {
        }

        virtual void notifyObjectCompiled(const llvm::Module *M,
                                          llvm::MemoryBufferRef obj)
        {
            const char *c = obj.getBufferStart();
            // Saving the object code in a std::string
            ss_.assign(c, obj.getBufferSize());
        }

        virtual std::unique_ptr<llvm::MemoryBuffer>
        getObject(const llvm::Module *M)
        {
            return NULL;
        }
    };

    MemoryBufferRefCallback callback(membuffer);
    executionengine->setObjectCache(&callback);
    // std::cout << error << std::endl;
    executionengine->finalizeObject();

    // Get the symbol's address
    func = (intptr_t)executionengine->getPointerToFunction(F);
    symbol_ptrs.clear();
    replacement_symbol_ptrs.clear();
    symbols.clear();
}

template <typename T>
T LLVMVisitor<T>::call(const std::vector<T> &vec)
{
    T ret;
    ((T (*)(const T *, T *))func)(vec.data(), &ret);
    return ret;
}

template <typename T>
void LLVMVisitor<T>::call(T *outs, const T *inps)
{
    ((T (*)(const T *, T *))func)(inps, outs);
}

template <typename T>
void LLVMVisitor<T>::set_real(T d)
{
    result_
        = llvm::ConstantFP::get(real_type_getter_llvm<T>::getTy(mod->getContext()), d);
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Integer &x, bool as_int32)
{
    if (as_int32) {
        int d = numeric_cast<int>(mp_get_si(x.as_integer_class()));
        result_ = llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(mod->getContext()), d, true);
    } else {
        result_
            = llvm::ConstantFP::get(real_type_getter_llvm<T>::getTy(mod->getContext()),
                                    mp_get_d(x.as_integer_class()));
    }
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Rational &x)
{
    set_real(mp_get_d(x.as_rational_class())); // TODO: mp_get*
}

template <typename T>
void LLVMVisitor<T>::bvisit(const RealDouble &x)
{
    set_real(x.i);
}

#ifdef HAVE_SYMENGINE_MPFR
template <typename T>
void LLVMVisitor<T>::bvisit(const RealMPFR &x)
{
    set_real(mpfr_get_d(x.i.get_mpfr_t(), MPFR_RNDN));
}
#endif

template <typename T>
void LLVMVisitor<T>::bvisit(const Add &x)
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
            tmp1 = apply(*(it->first));
            tmp2 = apply(*(it->second));
            tmp = builder->CreateFAdd(tmp, builder->CreateFMul(tmp1, tmp2));
        }
    }
    result_ = tmp;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Mul &x)
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

template <typename T>
llvm::Function *LLVMVisitor<T>::get_powi()
{
    std::vector<llvm::Type *> arg_type;
    arg_type.push_back(real_type_getter_llvm<T>::getTy(mod->getContext()));
    arg_type.push_back(llvm::Type::getInt32Ty(mod->getContext()));
    return llvm::Intrinsic::getDeclaration(mod, llvm::Intrinsic::powi,
                                           arg_type);
}

template <typename T>
llvm::Function *get_real_intrinsic(llvm::Intrinsic::ID id, unsigned n,
                                   llvm::Module *mod)
{
    std::vector<llvm::Type *> arg_type(
        n, real_type_getter_llvm<T>::getTy(mod->getContext()));
    return llvm::Intrinsic::getDeclaration(mod, id, arg_type);
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Pow &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    if (eq(*(x.get_base()), *E)) {
        args.push_back(apply(*x.get_exp()));
        fun = get_real_intrinsic<T>(llvm::Intrinsic::exp, 1, mod);

    } else if (eq(*(x.get_base()), *integer(2))) {
        args.push_back(apply(*x.get_exp()));
        fun = get_real_intrinsic<T>(llvm::Intrinsic::exp2, 1, mod);

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
            fun = get_real_intrinsic<T>(llvm::Intrinsic::pow, 2, mod);
        }
    }
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Sin &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_real_intrinsic<T>(llvm::Intrinsic::sin, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Cos &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_real_intrinsic<T>(llvm::Intrinsic::cos, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Piecewise &x)
{
    std::vector<llvm::BasicBlock> blocks;

    RCP<const Piecewise> pw = x.rcp_from_this_cast<const Piecewise>();

    if (neq(*pw->get_vec().back().second, *boolTrue)) {
        throw SymEngineException(
            "LLVMVisitor requires a (Expr, True) at the end of Piecewise");
    }

    if (pw->get_vec().size() > 2) {
        PiecewiseVec rest = pw->get_vec();
        rest.erase(rest.begin());
        auto rest_pw = piecewise(std::move(rest));
        PiecewiseVec new_pw;
        new_pw.push_back(*pw->get_vec().begin());
        new_pw.push_back({rest_pw, pw->get_vec().back().second});
        pw = piecewise(std::move(new_pw))
                 ->rcp_from_this_cast<const Piecewise>();
    } else if (pw->get_vec().size() < 2) {
        throw SymEngineException("Invalid Piecewise object");
    }

    auto cond_basic = pw->get_vec().front().second;
    llvm::Value *cond = apply(*cond_basic);
    // check if cond != 0.0
    cond = builder->CreateFCmpONE(
        cond,
        llvm::ConstantFP::get(real_type_getter_llvm<T>::getTy(mod->getContext()), 0.0),
        "ifcond");
    llvm::Function *function = builder->GetInsertBlock()->getParent();
    // Create blocks for the then and else cases.  Insert the 'then' block at
    // the
    // end of the function.
    llvm::BasicBlock *then_bb
        = llvm::BasicBlock::Create(mod->getContext(), "then", function);
    llvm::BasicBlock *else_bb
        = llvm::BasicBlock::Create(mod->getContext(), "else");
    llvm::BasicBlock *merge_bb
        = llvm::BasicBlock::Create(mod->getContext(), "ifcont");
    builder->CreateCondBr(cond, then_bb, else_bb);

    // Emit then value.
    builder->SetInsertPoint(then_bb);
    llvm::Value *then_value = apply(*pw->get_vec().front().first);
    builder->CreateBr(merge_bb);

    // Codegen of 'then_value' can change the current block, update then_bb for
    // the PHI.
    then_bb = builder->GetInsertBlock();

    // Emit else block.
    function->getBasicBlockList().push_back(else_bb);
    builder->SetInsertPoint(else_bb);
    llvm::Value *else_value = apply(*pw->get_vec().back().first);
    builder->CreateBr(merge_bb);

    // Codegen of 'else_value' can change the current block, update else_bb for
    // the PHI.
    else_bb = builder->GetInsertBlock();

    // Emit merge block.
    function->getBasicBlockList().push_back(merge_bb);
    builder->SetInsertPoint(merge_bb);
    llvm::PHINode *phi_node
        = builder->CreatePHI(real_type_getter_llvm<T>::getTy(mod->getContext()), 2);

    phi_node->addIncoming(then_value, then_bb);
    phi_node->addIncoming(else_value, else_bb);
    result_ = phi_node;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Sign &x)
{
    const auto x2 = x.get_arg();
    PiecewiseVec new_pw;
    new_pw.push_back({real_double(0.0), Eq(x2, real_double(0.0))});
    new_pw.push_back({real_double(-1.0), Lt(x2, real_double(0.0))});
    new_pw.push_back({real_double(1.0), boolTrue});
    auto pw = rcp_static_cast<const Piecewise>(piecewise(std::move(new_pw)));
    bvisit(*pw);
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Contains &cts)
{
    llvm::Value *expr = apply(*cts.get_expr());
    const auto set = cts.get_set();
    if (is_a<Interval>(*set)) {
        const auto &interv = down_cast<const Interval &>(*set);
        llvm::Value *start = apply(*interv.get_start());
        llvm::Value *end = apply(*interv.get_end());
        const bool left_open = interv.get_left_open();
        const bool right_open = interv.get_right_open();
        llvm::Value *left_ok;
        llvm::Value *right_ok;
        left_ok = (left_open) ? builder->CreateFCmpOLT(start, expr)
                              : builder->CreateFCmpOLE(start, expr);
        right_ok = (right_open) ? builder->CreateFCmpOLT(expr, end)
                                : builder->CreateFCmpOLE(expr, end);
        result_ = builder->CreateAnd(left_ok, right_ok);
        result_ = builder->CreateUIToFP(
            result_, real_type_getter_llvm<T>::getTy(mod->getContext()));
    } else {
        throw SymEngineException("LLVMVisitor<T>: only ``Interval`` "
                                 "implemented for ``Contains``.");
    }
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Infty &x)
{
    if (x.is_negative_infinity()) {
        result_ = llvm::ConstantFP::getInfinity(
            real_type_getter_llvm<T>::getTy(mod->getContext()), true);
    } else if (x.is_positive_infinity()) {
        result_ = llvm::ConstantFP::getInfinity(
            real_type_getter_llvm<T>::getTy(mod->getContext()), false);
    } else {
        throw SymEngineException(
            "LLVMDouble can only represent real valued infinity");
    }
}

template <typename T>
void LLVMVisitor<T>::bvisit(const BooleanAtom &x)
{
    const bool val = x.get_val();
    set_real(val ? 1.0 : 0.0);
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Log &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_real_intrinsic<T>(llvm::Intrinsic::log, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

#define SYMENGINE_LOGIC_FUNCTION(Class, method)                                \
    template <typename T>                                                      \
    void LLVMVisitor<T>::bvisit(const Class &x)                                \
    {                                                                          \
        llvm::Value *value = nullptr;                                          \
        llvm::Value *tmp;                                                      \
        set_real(0.0);                                                         \
        llvm::Value *zero_val = result_;                                       \
        for (auto &p : x.get_container()) {                                    \
            tmp = builder->CreateFCmpONE(apply(*p), zero_val);                 \
            if (value == nullptr) {                                            \
                value = tmp;                                                   \
            } else {                                                           \
                value = builder->method(value, tmp);                           \
            }                                                                  \
        }                                                                      \
        result_ = builder->CreateUIToFP(                                       \
            value, real_type_getter_llvm<T>::getTy(mod->getContext()));                \
    }

SYMENGINE_LOGIC_FUNCTION(And, CreateAnd);
SYMENGINE_LOGIC_FUNCTION(Or, CreateOr);
SYMENGINE_LOGIC_FUNCTION(Xor, CreateXor);

template <typename T>
void LLVMVisitor<T>::bvisit(const Not &x)
{
    builder->CreateNot(apply(*x.get_arg()));
}

#define SYMENGINE_RELATIONAL_FUNCTION(Class, method)                           \
    template <typename T>                                                      \
    void LLVMVisitor<T>::bvisit(const Class &x)                                \
    {                                                                          \
        llvm::Value *left = apply(*x.get_arg1());                              \
        llvm::Value *right = apply(*x.get_arg2());                             \
        result_ = builder->method(left, right);                                \
        result_ = builder->CreateUIToFP(                                       \
            result_, real_type_getter_llvm<T>::getTy(mod->getContext()));              \
    }

SYMENGINE_RELATIONAL_FUNCTION(Equality, CreateFCmpOEQ);
SYMENGINE_RELATIONAL_FUNCTION(Unequality, CreateFCmpONE);
SYMENGINE_RELATIONAL_FUNCTION(LessThan, CreateFCmpOLE);
SYMENGINE_RELATIONAL_FUNCTION(StrictLessThan, CreateFCmpOLT);

#define SYMENGINE_MACRO_EXTERNAL_FUNCTION(Class, ext)                          \
    template <typename T>                                                      \
    void LLVMVisitor<T>::bvisit(const Class &x)                                \
    {                                                                          \
        vec_basic basic_args = x.get_args();                                   \
        llvm::Function *func = get_external_function(#ext, basic_args.size()); \
        std::vector<llvm::Value *> args;                                       \
        for (const auto &arg : basic_args) {                                   \
            args.push_back(apply(*arg));                                       \
        }                                                                      \
        auto r = builder->CreateCall(func, args);                              \
        r->setTailCall(true);                                                  \
        result_ = r;                                                           \
    }

SYMENGINE_MACRO_EXTERNAL_FUNCTION(Tan, tan)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(Sinh, sinh)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(Cosh, cosh)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(Tanh, tanh)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(ASinh, asinh)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(ACosh, acosh)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(ATanh, atanh)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(ASin, asin)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(ACos, acos)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(ATan, atan)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(Gamma, tgamma)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(LogGamma, lgamma)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(Erf, erf)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(Erfc, erfc)
SYMENGINE_MACRO_EXTERNAL_FUNCTION(ATan2, atan2)

template <typename T>
void LLVMVisitor<T>::bvisit(const Abs &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_real_intrinsic<T>(llvm::Intrinsic::fabs, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Min &x)
{
    llvm::Value *value = nullptr;
    llvm::Function *fun;
    fun = get_real_intrinsic<T>(llvm::Intrinsic::minnum, 2, mod);
    for (auto &arg : x.get_vec()) {
        if (value != nullptr) {
            std::vector<llvm::Value *> args;
            args.push_back(value);
            args.push_back(apply(*arg));
            auto r = builder->CreateCall(fun, args);
            r->setTailCall(true);
            value = r;
        } else {
            value = apply(*arg);
        }
    }
    result_ = value;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Max &x)
{
    llvm::Value *value = nullptr;
    llvm::Function *fun;
    fun = get_real_intrinsic<T>(llvm::Intrinsic::maxnum, 2, mod);
    for (auto &arg : x.get_vec()) {
        if (value != nullptr) {
            std::vector<llvm::Value *> args;
            args.push_back(value);
            args.push_back(apply(*arg));
            auto r = builder->CreateCall(fun, args);
            r->setTailCall(true);
            value = r;
        } else {
            value = apply(*arg);
        }
    }
    result_ = value;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Symbol &x)
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

template <typename T>
llvm::Function *
LLVMVisitor<T>::get_external_function(const std::string &name, size_t nargs)
{
    std::vector<llvm::Type *> func_args(
        nargs, real_type_getter_llvm<T>::getTy(mod->getContext()));
    llvm::FunctionType *func_type
        = llvm::FunctionType::get(real_type_getter_llvm<T>::getTy(mod->getContext()),
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

template <typename T>
void LLVMVisitor<T>::bvisit(const Constant &x)
{
    set_real(eval_double(x));
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Basic &)
{
    throw std::runtime_error("Not implemented.");
}

template <typename T>
const std::string &LLVMVisitor<T>::dumps() const
{
    return membuffer;
};

template <typename T>
void LLVMVisitor<T>::loads(const std::string &s)
{
    membuffer = s;
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    context = std::make_shared<llvm::LLVMContext>();

    // Create some module to put our function into it.
    std::unique_ptr<llvm::Module> module
        = llvm::make_unique<llvm::Module>("SymEngine", *context);
    module->setDataLayout("");
    mod = module.get();

    // Only defining the prototype for the function here.
    // Since we know where the function is stored that's enough
    // llvm::ObjectCache is designed for caching objects, but it
    // is used here for loading one specific object.
    auto F = get_function_type(context.get());

    std::string error;
    executionengine = std::shared_ptr<llvm::ExecutionEngine>(
        llvm::EngineBuilder(std::move(module))
            .setEngineKind(llvm::EngineKind::Kind::JIT)
            .setOptLevel(llvm::CodeGenOpt::Level::Aggressive)
            .setErrorStr(&error)
            .create());

    class MCJITObjectLoader : public llvm::ObjectCache
    {
        const std::string &s_;

    public:
        MCJITObjectLoader(const std::string &s) : s_(s)
        {
        }
        virtual void notifyObjectCompiled(const llvm::Module *M,
                                          llvm::MemoryBufferRef obj)
        {
        }

        // No need to check M because there is only one function
        // Return it after reading from the file.
        virtual std::unique_ptr<llvm::MemoryBuffer>
        getObject(const llvm::Module *M)
        {
            return llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(s_));
        }
    };

    MCJITObjectLoader loader(s);
    executionengine->setObjectCache(&loader);
    executionengine->finalizeObject();
    // Set func to compiled function pointer
    func = (intptr_t)executionengine->getPointerToFunction(F);
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Floor &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_real_intrinsic<T>(llvm::Intrinsic::floor, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Ceiling &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_real_intrinsic<T>(llvm::Intrinsic::ceil, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

template <typename T>
void LLVMVisitor<T>::bvisit(const Truncate &x)
{
    std::vector<llvm::Value *> args;
    llvm::Function *fun;
    args.push_back(apply(*x.get_arg()));
    fun = get_real_intrinsic<T>(llvm::Intrinsic::trunc, 1, mod);
    auto r = builder->CreateCall(fun, args);
    r->setTailCall(true);
    result_ = r;
}

struct LLVMDoubleVisitor {}; // backward compatibility

} // namespace SymEngine
