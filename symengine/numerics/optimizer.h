
#ifndef SYMENGINE_NUMERICS_OPTIMIZER_H
#define SYMENGINE_NUMERICS_OPTIMIZER_H

// Low level
#include <symengine/symengine_config.h>
#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/expression.h>
#include <symengine/symengine_exception.h>
#include <symengine/symengine_rcp.h>

// Optimization
#include <symengine/lambda_double.h>
#include <symengine/visitor.h>
#ifdef HAVE_SYMENGINE_LLVM
#include <symengine/llvm_double.h>
#endif

// Numerics
#include <symengine/numerics/number_visitor.h>
#include <symengine/numerics/utilities.h>

#include <algorithm>
#include <complex>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace SymEngine
{
namespace Numerics
{

// Forward declarations
template <typename ReturnType, typename ExpressionType>
class BatchOptimizer;

/*
 * An enumeration to distinguish between different optimization methods
 * that can be used by SymEngine to more rapidly evaluate complex
 * symbolic expressions.
 */
enum class OptimizerType {
    /*
     * Use no specific optimization method.
     */
    off,
    /*
     * Use dictionary substitution. This is SymEngine's default method.
     */
    dictionary = off,
    /*
     * Convert the symbolic expression into a collection of
     * <tt>std::function<\tt>s.
     */
    lambda,
    /*
     * Use the LLVM JIT compiler to compile the expression into an
     * aggressively optimized, stand-alone function.
     */
    llvm
};

/**
 * Output operator which outputs update flags as a set of or'd text values.
 *
 * @ref UpdateFlags
 */
inline std::ostream &operator<<(std::ostream &s, const OptimizerType o)
{
    if (o == OptimizerType::off || o == OptimizerType::dictionary)
        s << "dictionary";
    else if (o == OptimizerType::lambda)
        s << "lambda";
    else if (o == OptimizerType::llvm)
        s << "llvm";
    else {
        SYMENGINE_ASSERT_MSG(false, "Unknown optimization method.");
    }

    return s;
}

/**
 * An enumeration to specify which special techniques, over and above
 * those used with the chosen OptimizerType, to be applied to the
 * set of expressions that are to be optimized.
 */
enum class OptimizationFlags : unsigned char {
    /**
     * No additional optimization
     */
    optimize_default = 0,
    /**
     * Apply common subexpression elimination
     */
    optimize_cse = 0x0001,
    //    /**
    //     * Apply expression simplification
    //     */
    //    optimize_simplify = 0x0002,
    /**
     * Apply all possible optimizations
     */
    optimize_all = optimize_cse
};

/**
 * Global operator which returns an object in which all bits are set which
 * are either set in the first or the second argument. This operator exists
 * since if it did not then the result of the bit-or <tt>operator |</tt>
 * would be an integer which would in turn trigger a compiler warning when
 * we tried to assign it to an object of type OptimizationFlags.
 *
 * @ref UpdateFlags
 */
inline OptimizationFlags operator|(const OptimizationFlags f1,
                                   const OptimizationFlags f2)
{
    return static_cast<OptimizationFlags>(static_cast<unsigned int>(f1)
                                          | static_cast<unsigned int>(f2));
}

/**
 * Global operator which sets the bits from the second argument also in the
 * first one.
 *
 * @ref UpdateFlags
 */
inline OptimizationFlags &operator|=(OptimizationFlags &f1,
                                     const OptimizationFlags f2)
{
    f1 = f1 | f2;
    return f1;
}

/**
 * Global operator which returns an object in which all bits are set which
 * are set in the first as well as the second argument. This operator exists
 * since if it did not then the result of the bit-and <tt>operator &</tt>
 * would be an integer which would in turn trigger a compiler warning when
 * we tried to assign it to an object of type OptimizationFlags.
 *
 * @ref UpdateFlags
 */
inline OptimizationFlags operator&(const OptimizationFlags f1,
                                   const OptimizationFlags f2)
{
    return static_cast<OptimizationFlags>(static_cast<unsigned int>(f1)
                                          & static_cast<unsigned int>(f2));
}

/**
 * Global operator which clears all the bits in the first argument if they
 * are not also set in the second argument.
 *
 * @ref UpdateFlags
 */
inline OptimizationFlags &operator&=(OptimizationFlags &f1,
                                     const OptimizationFlags f2)
{
    f1 = f1 & f2;
    return f1;
}

/**
 * Output operator which outputs update flags as a set of or'd text values.
 *
 * @ref UpdateFlags
 */
inline std::ostream &operator<<(std::ostream &s, const OptimizationFlags o)
{
    s << " OptimizationFlags|";
    if (static_cast<unsigned int>(o & OptimizationFlags::optimize_cse))
        s << "cse|";

    return s;
}

namespace internal
{
// Dummy "optimizer" for dictionary-based
// substitution
template <typename ReturnType, typename ExpressionType, typename = void>
struct DictionaryOptimizer;

// Define, create and initialise the lambda function
// optimiser for each number type
template <typename ReturnType, typename = void>
struct LambdaOptimizer;

#ifdef HAVE_SYMENGINE_LLVM
template <typename ReturnType, typename = void>
struct LLVMOptimizer;
#endif

// Floating point numbers
template <typename ReturnType_, typename ExpressionType>
struct DictionaryOptimizer<ReturnType_, ExpressionType,
                           typename std::
                               enable_if<std::is_arithmetic<ReturnType_>::
                                             value>::type> {
    typedef double ReturnType;
    typedef DictionarySubstitutionVisitor<ReturnType, ExpressionType>
        VisitorType;

    static void print(std::ostream &stream, const VisitorType &optimizer,
                      const bool print_independent_symbols = false,
                      const bool print_dependent_expressions = false,
                      const bool print_cse_reductions = true)
    {
        optimizer.print(stream, print_independent_symbols,
                        print_dependent_expressions, print_cse_reductions);
    }
};

template <typename ReturnType_, typename ExpressionType>
struct DictionaryOptimizer<ReturnType_, ExpressionType,
                           typename std::
                               enable_if<Utilities::is_complex<ReturnType_>::
                                             value
                                         && std::is_arithmetic<
                                                typename ReturnType_::
                                                    value_type>::value>::type> {
    typedef std::complex<double> ReturnType;
    typedef DictionarySubstitutionVisitor<ReturnType, ExpressionType>
        VisitorType;

    static void print(std::ostream &stream, const VisitorType &optimizer,
                      const bool print_independent_symbols = false,
                      const bool print_dependent_expressions = false,
                      const bool print_cse_reductions = true)
    {
        optimizer.print(stream, print_independent_symbols,
                        print_dependent_expressions, print_cse_reductions);
    }
};

template <typename ReturnType_>
struct LambdaOptimizer<ReturnType_,
                       typename std::enable_if<std::is_arithmetic<ReturnType_>::
                                                   value>::type> {
    typedef double ReturnType;
    typedef SymEngine::LambdaRealDoubleVisitor VisitorType;

    static void print(std::ostream & /*stream*/,
                      const VisitorType & /*optimizer*/,
                      const bool /*print_independent_symbols*/ = false,
                      const bool /*print_dependent_expressions*/ = false,
                      const bool /*print_cse_reductions*/ = true)
    {
        // No built-in print function
    }
};

template <typename ReturnType_>
struct LambdaOptimizer<ReturnType_,
                       typename std::
                           enable_if<Utilities::is_complex<ReturnType_>::value
                                     && std::is_arithmetic<
                                            typename ReturnType_::value_type>::
                                            value>::type> {
    typedef std::complex<double> ReturnType;
    typedef SymEngine::LambdaComplexDoubleVisitor VisitorType;

    static void print(std::ostream & /*stream*/,
                      const VisitorType & /*optimizer*/,
                      const bool /*print_independent_symbols*/ = false,
                      const bool /*print_dependent_expressions*/ = false,
                      const bool /*print_cse_reductions*/ = true)
    {
        // No built-in print function
    }
};

#ifdef HAVE_SYMENGINE_LLVM
template <typename ReturnType_>
struct LLVMOptimizer<ReturnType_,
                     typename std::enable_if<std::is_arithmetic<ReturnType_>::
                                                 value>::type> {
    typedef double ReturnType;
    typedef SymEngine::LLVMDoubleVisitor VisitorType;

    static void print(std::ostream & /*stream*/,
                      const VisitorType & /*optimizer*/,
                      const bool /*print_independent_symbols*/ = false,
                      const bool /*print_dependent_expressions*/ = false,
                      const bool /*print_cse_reductions*/ = true)
    {
        // No built-in print function
    }
};

template <typename ReturnType_>
struct LLVMOptimizer<ReturnType_,
                     typename std::
                         enable_if<Utilities::is_complex<ReturnType_>::value
                                   && std::is_arithmetic<
                                          typename ReturnType_::value_type>::
                                          value>::type> {
    //          static_assert(false, "LLVM complex double visitor has not
    //          yet been implemented") typedef ReturnType_ ReturnType;
    //          typedef SymEngine::LLVMComplexDoubleVisitor VisitorType;

    //          template <typename Stream>
    //          static void
    //          print (Stream              &stream,
    //                 const VisitorType &optimizer,
    //                 const bool           print_independent_symbols =
    //                 false, const bool print_dependent_expressions =
    //                 false, const bool           print_cse_reductions =
    //                 true)
    //          {
    //            // No built-in print function
    //          }
};
#endif

/**
 * End-point for recursive template function
 */
template <typename NumberType, typename ExpressionType, typename T>
void register_dependent_expressions(
    BatchOptimizer<NumberType, ExpressionType> &optimizer, const T &function)
{
    optimizer.register_dependent_expression(function);
}

template <typename NumberType, typename ExpressionType, typename T,
          typename... Args>
void register_dependent_expressions(
    BatchOptimizer<NumberType, ExpressionType> &optimizer, const T &function,
    const Args &... other_functions)
{
    register_dependent_expressions(optimizer, function);
    register_dependent_expressions(optimizer, other_functions...);
}

template <typename ReturnType, typename Optimizer, typename = void>
struct OptimizerHelper;

static inline bool use_symbolic_CSE(const OptimizationFlags &flags)
{
    return static_cast<int>(flags & OptimizationFlags::optimize_cse);
}

template <typename ReturnType, typename Optimizer>
struct OptimizerHelper<ReturnType, Optimizer,
                       typename std::enable_if<std::is_same<ReturnType,
                                                            typename Optimizer::
                                                                ReturnType>::
                                                   value>::type> {
    static void initialize(typename Optimizer::VisitorType *optimizer,
                           const SymEngine::vec_basic &symbol_vec,
                           const SymEngine::vec_basic &dependent_variables,
                           const OptimizationFlags &optimization_flags)
    {
        const bool use_symbolic_cse = use_symbolic_CSE(optimization_flags);
        optimizer->init(symbol_vec, dependent_variables, use_symbolic_cse);
    }

    static void substitute(typename Optimizer::VisitorType *optimizer,
                           std::vector<ReturnType> &outputs,
                           const std::vector<ReturnType> &inputs)
    {
        optimizer->call(outputs.data(), inputs.data());
    }

    static void print(std::ostream &stream,
                      typename Optimizer::VisitorType *optimizer,
                      const bool print_independent_symbols = false,
                      const bool print_dependent_expressions = false,
                      const bool print_cse_reductions = true)
    {
        // Some optimizers don't have a print function, so
        // we filter them out through the specializations of
        // the Optimizer class
        Optimizer::print(stream, *optimizer, print_independent_symbols,
                         print_dependent_expressions, print_cse_reductions);
    }
};

template <typename ReturnType, typename Optimizer>
struct OptimizerHelper<ReturnType, Optimizer,
                       typename std::
                           enable_if<!std::is_same<ReturnType,
                                                   typename Optimizer::
                                                       ReturnType>::value>::
                               type> {
    static void initialize(typename Optimizer::VisitorType *optimizer,
                           const SymEngine::vec_basic &symbol_vec,
                           const SymEngine::vec_basic &dependent_variables,
                           const OptimizationFlags &optimization_flags)
    {
        const bool use_symbolic_cse = use_symbolic_CSE(optimization_flags);
        optimizer->init(symbol_vec, dependent_variables, use_symbolic_cse);
    }

    static void substitute(typename Optimizer::VisitorType *optimizer,
                           std::vector<ReturnType> &outputs,
                           const std::vector<ReturnType> &inputs)
    {
        // Intermediate values to accommodate the difference in
        // value types.
        std::vector<typename Optimizer::ReturnType> int_outputs(outputs.size());
        std::vector<typename Optimizer::ReturnType> int_inputs(inputs);

        optimizer->call(int_outputs.data(), int_inputs.data());
        std::copy(int_outputs.begin(), int_outputs.end(), outputs.begin());
    }

    static void print(std::ostream &stream,
                      typename Optimizer::VisitorType *optimizer,
                      const bool print_cse_reductions = true,
                      const bool print_independent_symbols = false,
                      const bool print_dependent_expressions = false)
    {
        optimizer->print(stream, print_independent_symbols,
                         print_dependent_expressions, print_cse_reductions);
    }
};

} // namespace internal

/**
 * A class that facilitates the optimization of symbol expressions.
 *
 * This expression will be optimized by this class; that is to say that
 * the code path taken to substitute the set of (independent) symbols
 * into a collection of (dependent) symbolic functions will be optimized
 * using a chosen approach.
 *
 * General usage:
 * @code
 * typedef NumberType double;
 * Numerics::BatchOptimizer<NumberType> optimizer
 * (Numerics::OptimizerType::lambda, Numerics::OptimizationFlags::optimize_cse);
 *
 * optimizer.register_independent_symbols(...);
 * optimizer.register_dependent_expressions(...);
 *
 * optimizer.optimize(); // Expensive calls
 *
 * optimizer.substitute(...); // Now quicker than dictionary substitution
 * optimizer.evaluate();
 * @endcode
 *
 * @tparam ReturnType The number type that is to be returned after
 * value substitution and evaluation.
 *
 * @warning This class is not thread-safe.
 *
 * @author Jean-Paul Pelteret, Isuru Fernando, 2017
 */
template <typename ReturnType, typename ExpressionType = Expression>
class BatchOptimizer
{
public:
    /**
     * Default constructor.
     *
     * By default, dictionary substitution will be selected when this
     * constructor is called. In order to select a specific optimization
     * approach, a call set_optimization_method() is necessary.
     */
    BatchOptimizer();

    /**
     * Constructor.
     *
     * @param[in] method The optimization method that is to be employed.
     * @param[in] flags  The optimization flags that indicate which
     * expression manipulation mechanisms are to be employed.
     *
     * @note As the optimization method is fixed, a further call to
     * set_optimization_method() is not necessary and will result in an
     * error being thrown.
     *
     * @note In the case that the @p method is not implemented for the
     * required @p ReturnType, or the desired feature is not active,
     * then a safe default will be selected.
     */
    BatchOptimizer(const OptimizerType &method,
                   const OptimizationFlags &flags
                   = OptimizationFlags::optimize_all);

    /**
     * Copy constructor.
     *
     * The @p copy_initialized flag, which is set to <code>true</code> by
     * default, determines whether or not all of the optimized data is copied
     * over from the @p other optimizer instance. Only with the flag set to
     * <code>false</code> is it possible to re-optimize the data stored in
     * this class with a different optimization scheme.
     */
    BatchOptimizer(const BatchOptimizer &other /*,
                     const bool            copy_initialized = true*/);

    /**
     * Move constructor.
     */
    BatchOptimizer(BatchOptimizer &&) = default;

    /**
     * Destructor.
     */
    ~BatchOptimizer();

    /**
     * Select the @p optimization_method to be used in conjunction with the
     * chosen @p optimization_flags.
     *
     * @note In the case that the @p optimization_method is not implemented
     * for the required @p ReturnType, or the desired feature is not active,
     * then a safe default will be selected.
     */
    void set_optimization_method(const OptimizerType &optimization_method,
                                 const OptimizationFlags &optimization_flags
                                 = OptimizationFlags::optimize_all);

    /**
     * Return the optimization method that has been
     * selected for use.
     */
    OptimizerType optimization_method() const;

    /**
     * Return the optimization flags that have been
     * selected for use.
     */
    OptimizationFlags optimization_flags() const;

    /**
     * State whether the internal selection of optimization
     * methods and flags will render an optimizer that uses
     * CSE.
     */
    bool use_symbolic_CSE() const;

    /**
     * Returns a flag which indicates whether the optimizer
     * has been initialized and is ready for use.
     */
    bool initialized() const;

    /**
     * Returns a flag which indicates whether the optimize()
     * function has been called and the class is finalized.
     */
    bool optimized() const;

    /**
     * Returns a flag to indicate whether the substitute()
     * function has been called and if there are meaningful
     * values that will be returned upon evaluation().
     */
    bool values_substituted() const;

    /**
     * Print some information on state of the internal data
     * structures stored in the class.
     */
    void print(std::ostream &stream, const bool print_cse = true) const;

    /**
     * @name Independent variables
     */
    //@{

    /**
     * Register a collection of symbols that represents an independent
     * variable. These symbols are stored as the <tt>key</tt> to the
     * @p symbol_values map.
     */
    void register_independent_symbols(
        const typename SymEngine::map_basic_basic &symbol_values);

    /**
     * Register a collection of symbols that represents independent variables.
     *
     * @warning When using this function is no mechanism to check that the
     * ordering of the later used @p substitution_values vector or map matches
     * the internal ordering of the registered symbols. This function is
     * therefore typically used in conjunction with the substitute() function
     * that takes in a vector of values. With this pair of functions to the
     * class interface, the management of symbol ordering is maintained by the
     * user.
     */
    void
    register_independent_symbols(const typename SymEngine::vec_basic &symbols);

    /**
     * Return the list of symbols that have been registered as independent
     * variables.
     */
    SymEngine::vec_basic get_independent_symbols(void) const;

    /**
     * The number of independent variables that this optimizer will recognize.
     * This is equal to the number of unique symbols passed to this class
     * instance through the register_independent_symbols() function.
     */
    std::size_t n_independent_symbols(void) const;

    //@}

    /**
     * @name Dependent variables
     */
    //@{

    /**
     * Register a symbolic expression that represents a dependent variable.
     */
    void register_dependent_expression(
        const SymEngine::RCP<const SymEngine::Basic> &func);

    /**
     * Register a symbolic expression that represents a dependent variable.
     */
    void register_dependent_expression(const Expression &func);

    /**
     * Register a collection of symbolic expressions that represent dependent
     * variables.
     */
    void register_dependent_expressions(
        const typename SymEngine::vec_basic &functions);

    /**
     * Register a collection of symbolic expressions that represents dependent
     * variables.
     *
     * Both the type @p T and those collected in the @p Args parameter pack,
     * i.e. the @p function and @p other_functions, must conform to types that
     * are supported by the other implementations of
     * register_dependent_expressions().
     */
    template <typename T, typename... Args>
    void register_dependent_expressions(const T &function,
                                        const Args &... other_functions);

    /**
     * Return the list of symbols that have been registered as dependent
     * functions.
     */
    const SymEngine::vec_basic &get_dependent_expressions(void) const;

    /**
     * The number of dependent symbolic expressions that this optimizer
     * will optimize. This is equal to the number of unique symbolic functions
     * passed to this class instance through the
     * register_dependent_expressions()
     * function.
     */
    std::size_t n_dependent_expressions(void) const;

    //@}

    /**
     * @name Optimization
     */
    //@{

    /**
     * Perform the optimization of all registered dependent functions using
     * the registered symbols.
     *
     * @note This may be a time-consuming process, but if the class instance is
     * retained throughout the coarse of a simulation then it need only be
     * performed once.
     *
     * @note This function ,which should only be called once per instance of
     * this
     * class, finalizes the set of accepted independent symbols and dependent
     * functions that are recognized and used by the optimizer.
     */
    void optimize();

    //@}

    /**
     * @name Substitution / evaluation and data extraction
     */
    //@{

    /**
     * Perform batch substitution of all of the registered symbols
     * into the registered functions. The result is cached and can
     * be extracted by calls to evaluate().
     *
     * @note Calling substitute() again with a new set of
     * @p substitution_values overwrites any previously computed
     * results.
     */
    void substitute(
        const typename SymEngine::map_basic_basic &substitution_values) const;

    /**
     * Perform batch substitution of all of the registered symbols
     * into the registered functions. The result is cached and can
     * be extracted by calls to evaluate().
     *
     * @note Calling substitute() again with a new set of
     * @p substitution_values overwrites any previously computed
     * results.
     */
    void substitute(const typename SymEngine::vec_basic &symbols,
                    const std::vector<ReturnType> &substitution_values) const;

    /**
     * Returns the result of a value substitution into the optimized
     * counterpart of all dependent functions. This function fetches those
     * cached values.
     *
     * These values were computed by substituting a @p substitution_values map
     * during substitute() call.
     */
    const std::vector<ReturnType> &get_evaluated_dependent_expressions() const;

    /**
     * Returns the result of a value substitution into the optimized
     * counterpart of @p func. This function fetches that cached value.
     *
     * This value was computed by substituting a @p substitution_values map
     * during substitute() call.
     */
    ReturnType evaluate(const Expression &func) const;

    //@}

private:
    /**
     * The optimization methods that is to be employed.
     */
    OptimizerType method;

    /**
     * The optimization flags that indicate which expression manipulation
     * mechanisms are to be employed.
     */
    OptimizationFlags flags;

    /**
     * A map that represents the symbols that form the set of independent
     * variables upon which optimized symbolic expressions are to be based.
     *
     * @note As the ordering of the input symbols is fixed at the time at
     * which optimization is performed, we store all of the entries in
     * a map to ensure that both we and the user never mistakenly
     * swap the order of two or more symbols during evaluation.
     */
    SymEngine::map_basic_basic independent_symbols;

    /**
     * A set of symbolic expressions to be optimized. It is required that
     * the symbols on which these dependent functions be based are are
     * registered in the @p independent_symbols map.
     */
    SymEngine::vec_basic dependent_expressions;

    /**
     * The output of substituting symbolic values with floating point
     * values through the use of the @p optimizer.
     *
     * It is necessary to use this intermediate storage mechanism
     * to store the result of a substitution sweep as some optimizers
     * work on all symbolic expressions in a single batch. In this way
     * they can employ methods such as common subexpression elimination
     * to minimise the number of terms evaluated across all symbolic
     * functions.
     *
     * This variable is marked as mutable. This facilitates the substitution
     * functionality being used in logically constant "get_*" functions.
     */
    mutable std::vector<ReturnType> evaluated_dependent_expressions;

    /**
     * A map type used to indicate which dependent variable is associated
     * with which entry in the output vector.
     *
     * We use a custom comparator here because otherwise we can't use
     * std::map::find; it is sensitive to the some data from the
     * underlying SymEngine::Basic other than the value that it represents.
     */
    using map_dependent_expression_to_vector_entry_t
        = std::map<SymEngine::RCP<const SymEngine::Basic>, std::size_t,
                   SymEngine::RCPBasicKeyLess>;

    /**
     * A map indicating which dependent variable is associated with which
     * entry in the output vector.
     */
    map_dependent_expression_to_vector_entry_t map_dep_ptr_basic_vec_entry;

    /**
     * A pointer to an instance of an optimizer that will be used to
     * reformulate the substitution of symbolic expressions in a
     * manner that is more efficient than plain dictionary-based
     * approach.
     */
    mutable std::unique_ptr<SymEngine::Visitor> optimizer;

    /**
     * A flag to record whether or not substitution has taken place and
     * values can now be extracted.
     *
     * This variable is marked as mutable. This facilitates the substitution
     * functionality being used in logically constant "get_*" functions.
     */
    mutable bool ready_for_value_extraction;

    /**
     * Register a single symbolic expression that represents an dependent
     * variable.
     */
    void register_scalar_dependent_expression(
        const SymEngine::RCP<const SymEngine::Basic> &func);

    /**
     * Register a collection of symbolic expressions that represent a dependent
     * variables.
     */
    void register_vector_dependent_expressions(
        const typename SymEngine::vec_basic &funcs);

    /**
     * Create an instance of the selected optimizer.
     */
    void create_optimizer(std::unique_ptr<SymEngine::Visitor> &optimizer);

    /**
     * Perform batch substitution of all of the registered symbols
     * into the registered functions. The result is cached and can
     * be extracted by calls to evaluate().
     *
     * @note Calling substitute() again with a new set of
     * @p substitution_values overwrites any previously computed
     * results.
     *
     * @warning When using this function is no mechanism to check that the
     * ordering
     * of the @p substitution_values vector matches the internal ordering of
     * the registered symbols. This function is therefore typically used in
     * conjunction with the register_independent_symbols() function that takes
     * in a vector
     * of symbols. With this pair of functions to the class interface, the
     * management of symbol ordering is maintained by the user.
     */
    void substitute(const std::vector<ReturnType> &substitution_values) const;
};

/* -------------------- inline and template functions ------------------ */

#ifndef DOXYGEN

template <typename ReturnType, typename ExpressionType>
BatchOptimizer<ReturnType, ExpressionType>::BatchOptimizer()
    : method(OptimizerType::off), flags(OptimizationFlags::optimize_default),
      ready_for_value_extraction(false)
{
}

template <typename ReturnType, typename ExpressionType>
BatchOptimizer<ReturnType, ExpressionType>::BatchOptimizer(
    const OptimizerType &method, const OptimizationFlags &flags)
    : BatchOptimizer()
{
    set_optimization_method(method, flags);
}

template <typename ReturnType, typename ExpressionType>
BatchOptimizer<ReturnType, ExpressionType>::BatchOptimizer(
    const BatchOptimizer<ReturnType, ExpressionType> &other /*,
                                               const bool                        copy_initialized*/)
    : method(other.method), flags(other.flags),
      independent_symbols(other.independent_symbols),
      dependent_expressions(other.dependent_expressions),
      evaluated_dependent_expressions(0),
      map_dep_ptr_basic_vec_entry(other.map_dep_ptr_basic_vec_entry),
      ready_for_value_extraction(false)
{
}

template <typename ReturnType, typename ExpressionType>
BatchOptimizer<ReturnType, ExpressionType>::~BatchOptimizer()
{
    optimizer.reset();
    map_dep_ptr_basic_vec_entry.clear();
    evaluated_dependent_expressions.clear();
    dependent_expressions.clear();
    independent_symbols.clear();
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::set_optimization_method(
    const OptimizerType &optimization_method,
    const OptimizationFlags &optimization_flags)
{
    SYMENGINE_ASSERT_MSG(optimized() == false, "Cannot call "
                                               "set_optimization_method() once "
                                               "the optimizer is finalised.");
    //      Assert (this->optimization_method() == OptimizerType::off,
    //              ExcMessage("Cannot call set_optimization_method() more
    //              than once."));

    method = optimization_method;
#ifndef HAVE_SYMENGINE_LLVM
    if (this->optimization_method() == OptimizerType::llvm) {
// Fall-back if the LLVM JIT compiler is not available
#ifdef DEBUG
        std::cout
            << "Warning: The LLVM is not available, so the batch optimizer "
            << "is using a lambda optimizer instead." << std::endl;
#endif
        method = OptimizerType::lambda;
    }
#endif
    flags = optimization_flags;
}

template <typename ReturnType, typename ExpressionType>
OptimizerType
BatchOptimizer<ReturnType, ExpressionType>::optimization_method() const
{
    return method;
}

template <typename ReturnType, typename ExpressionType>
OptimizationFlags
BatchOptimizer<ReturnType, ExpressionType>::optimization_flags() const
{
    return flags;
}

template <typename ReturnType, typename ExpressionType>
bool BatchOptimizer<ReturnType, ExpressionType>::use_symbolic_CSE() const
{
    return internal::use_symbolic_CSE(flags);
}

template <typename ReturnType, typename ExpressionType>
bool BatchOptimizer<ReturnType, ExpressionType>::initialized() const
{
    return optimized();
}

template <typename ReturnType, typename ExpressionType>
bool BatchOptimizer<ReturnType, ExpressionType>::optimized() const
{
    if (evaluated_dependent_expressions.size() > 0) {
        SYMENGINE_ASSERT(evaluated_dependent_expressions.size()
                         == dependent_expressions.size());
        return true;
    }

    return false;
}

template <typename ReturnType, typename ExpressionType>
bool BatchOptimizer<ReturnType, ExpressionType>::values_substituted() const
{
    return ready_for_value_extraction;
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::register_independent_symbols(
    const typename SymEngine::map_basic_basic &symbol_values)
{
    SYMENGINE_ASSERT_MSG(
        optimized() == false,
        "Cannot register symbols once the optimizer is finalised.");

#ifdef DEBUG
    // Ensure that all of the keys in the map are actually symbolic
    // in nature
    for (const auto &symb_val : symbol_values) {
        (void)symb_val;
        SYMENGINE_ASSERT_MSG(
            SymEngine::is_a<SymEngine::Symbol>(*symb_val.first),
            "Key entry in map is not a symbol: " + symb_val.first->__str__());
    }
#endif
    // Merge the two maps, in the process ensuring that there is no
    // duplication of symbols
    independent_symbols.insert(symbol_values.begin(), symbol_values.end());
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::register_independent_symbols(
    const typename SymEngine::vec_basic &symbols)
{
    SYMENGINE_ASSERT_MSG(
        optimized() == false,
        "Cannot register symbols once the optimizer is finalised.");

    for (const auto &symb : symbols) {
        SYMENGINE_ASSERT_MSG(
            independent_symbols.find(symb) == independent_symbols.end(),
            "Symbol is already in the map: " + symb->__str__());
        independent_symbols.insert(std::make_pair(symb, ExpressionType(0)));
    }
}

template <typename ReturnType, typename ExpressionType>
SymEngine::vec_basic
BatchOptimizer<ReturnType, ExpressionType>::get_independent_symbols(void) const
{
    return Utilities::extract_symbols(independent_symbols);
}

template <typename ReturnType, typename ExpressionType>
std::size_t
BatchOptimizer<ReturnType, ExpressionType>::n_independent_symbols(void) const
{
    return independent_symbols.size();
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::register_dependent_expression(
    const Expression &func)
{
    SYMENGINE_ASSERT_MSG(
        optimized() == false,
        "Cannot register functions once the optimizer is finalised.");

    register_dependent_expression(
        static_cast<SymEngine::RCP<const SymEngine::Basic>>(func));
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::register_dependent_expression(
    const SymEngine::RCP<const SymEngine::Basic> &func)
{
    SYMENGINE_ASSERT_MSG(
        optimized() == false,
        "Cannot register functions once the optimizer is finalised.");

    register_scalar_dependent_expression(func);
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::register_dependent_expressions(
    const typename SymEngine::vec_basic &functions)
{
    SYMENGINE_ASSERT_MSG(
        optimized() == false,
        "Cannot register functions once the optimizer is finalised.");

    register_vector_dependent_expressions(functions);
}

template <typename ReturnType, typename ExpressionType>
template <typename T, typename... Args>
void BatchOptimizer<ReturnType, ExpressionType>::register_dependent_expressions(
    const T &function, const Args &... other_functions)
{
    internal::register_dependent_expressions(*this, function);
    internal::register_dependent_expressions(*this, other_functions...);
}

template <typename ReturnType, typename ExpressionType>
const SymEngine::vec_basic &
BatchOptimizer<ReturnType, ExpressionType>::get_dependent_expressions(
    void) const
{
    return dependent_expressions;
}

template <typename ReturnType, typename ExpressionType>
std::size_t
BatchOptimizer<ReturnType, ExpressionType>::n_dependent_expressions(void) const
{
    SYMENGINE_ASSERT(map_dep_ptr_basic_vec_entry.size()
                     == dependent_expressions.size());
    return dependent_expressions.size();
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::optimize()
{
    SYMENGINE_ASSERT_MSG(optimized() == false,
                         "Cannot call optimize() more than once.");

    // Create and configure the optimizer
    create_optimizer(optimizer);
    SYMENGINE_ASSERT(optimizer);

    const typename SymEngine::vec_basic symbol_vec
        = Utilities::extract_symbols(independent_symbols);
    if (typename internal::DictionaryOptimizer<ReturnType,
                                               ExpressionType>::VisitorType *opt
        = dynamic_cast<
            typename internal::DictionaryOptimizer<ReturnType, ExpressionType>::
                VisitorType *>(optimizer.get())) {
        SYMENGINE_ASSERT(optimization_method() == OptimizerType::off);
        internal::
            OptimizerHelper<ReturnType,
                            internal::DictionaryOptimizer<ReturnType,
                                                          ExpressionType>>::
                initialize(opt, symbol_vec, dependent_expressions,
                           optimization_flags());
    } else if (typename internal::LambdaOptimizer<ReturnType>::VisitorType *opt
               = dynamic_cast<typename internal::LambdaOptimizer<ReturnType>::
                                  VisitorType *>(optimizer.get())) {
        SYMENGINE_ASSERT(optimization_method() == OptimizerType::lambda);
        internal::OptimizerHelper<ReturnType,
                                  internal::LambdaOptimizer<ReturnType>>::
            initialize(opt, symbol_vec, dependent_expressions,
                       optimization_flags());
    }
#ifdef HAVE_SYMENGINE_LLVM
    else if (typename internal::LLVMOptimizer<ReturnType>::VisitorType *opt
             = dynamic_cast<
                 typename internal::LLVMOptimizer<ReturnType>::VisitorType *>(
                 optimizer.get())) {
        SYMENGINE_ASSERT(optimization_method() == OptimizerType::llvm);
        internal::OptimizerHelper<ReturnType,
                                  internal::LLVMOptimizer<ReturnType>>::
            initialize(opt, symbol_vec, dependent_expressions,
                       optimization_flags());
    }
#endif
    else {
        SYMENGINE_ASSERT_MSG(false, "Unknown optimizer type.");
    }

    // The size of the outputs is now fixed, as is the number and
    // order of the symbols to be substituted.
    // Note: When no optimisation is actually used
    // (i.e. optimization_method() == off), we could conceptually go
    // without this data structure.
    // However, since the user expects to perform substitution of all
    // dependent variables in one go, we still require it for intermediate
    // storage of results.
    evaluated_dependent_expressions.resize(n_dependent_expressions());
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::substitute(
    const typename SymEngine::map_basic_basic &substitution_values) const
{
    SYMENGINE_ASSERT_MSG(
        optimized() == true,
        "The optimizer is not configured to perform substitution. "
        "This action can only performed after optimize() has been called.");
    SYMENGINE_ASSERT(optimizer);

// Check that the registered symbol map and the input map are compatible
// with one another
#ifdef DEBUG
    const typename SymEngine::vec_basic symbol_sub_vec
        = Utilities::extract_symbols(substitution_values);
    const typename SymEngine::vec_basic symbol_vec
        = Utilities::extract_symbols(independent_symbols);
    SYMENGINE_ASSERT(symbol_sub_vec.size() == symbol_vec.size());
    for (unsigned int i = 0; i < symbol_sub_vec.size(); ++i) {
        SYMENGINE_ASSERT_MSG(
            SymEngine::eq(*symbol_sub_vec[i], *symbol_vec[i]),
            "The input substitution map is either incomplete, or does "
            "not match that used in the register_independent_symbols() call.");
    }
#endif

    // Extract the values from the substitution map, and use the other
    // function
    const std::vector<ReturnType> values
        = Utilities::extract_values<ReturnType, ExpressionType>(
            substitution_values);
    substitute(values);
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::substitute(
    const typename SymEngine::vec_basic &symbols,
    const std::vector<ReturnType> &substitution_values) const
{
    // Zip the two vectors and use the other function call
    // This ensures the ordering of the input vectors matches that of the
    // stored map.
    substitute(make_symbol_value_map(symbols, substitution_values));
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::substitute(
    const std::vector<ReturnType> &substitution_values) const
{
    SYMENGINE_ASSERT_MSG(
        optimized() == true,
        "The optimizer is not configured to perform substitution. "
        "This action can only performed after optimize() has been called.");
    SYMENGINE_ASSERT(optimizer);
    SYMENGINE_ASSERT(substitution_values.size() == independent_symbols.size());

    if (typename internal::DictionaryOptimizer<ReturnType,
                                               ExpressionType>::VisitorType *opt
        = dynamic_cast<
            typename internal::DictionaryOptimizer<ReturnType, ExpressionType>::
                VisitorType *>(optimizer.get())) {
        SYMENGINE_ASSERT(optimization_method() == OptimizerType::off);
        internal::
            OptimizerHelper<ReturnType,
                            internal::DictionaryOptimizer<ReturnType,
                                                          ExpressionType>>::
                substitute(opt, evaluated_dependent_expressions,
                           substitution_values);
    } else if (typename internal::LambdaOptimizer<ReturnType>::VisitorType *opt
               = dynamic_cast<typename internal::LambdaOptimizer<ReturnType>::
                                  VisitorType *>(optimizer.get())) {
        SYMENGINE_ASSERT(optimization_method() == OptimizerType::lambda);
        internal::OptimizerHelper<ReturnType,
                                  internal::LambdaOptimizer<ReturnType>>::
            substitute(opt, evaluated_dependent_expressions,
                       substitution_values);
    }
#ifdef HAVE_SYMENGINE_LLVM
    else if (typename internal::LLVMOptimizer<ReturnType>::VisitorType *opt
             = dynamic_cast<
                 typename internal::LLVMOptimizer<ReturnType>::VisitorType *>(
                 optimizer.get())) {
        SYMENGINE_ASSERT(optimization_method() == OptimizerType::llvm);
        internal::OptimizerHelper<ReturnType,
                                  internal::LLVMOptimizer<ReturnType>>::
            substitute(opt, evaluated_dependent_expressions,
                       substitution_values);
    }
#endif
    else {
        throw NotImplementedError("Not Implemented");
    }

    ready_for_value_extraction = true;
}

template <typename ReturnType, typename ExpressionType>
const std::vector<ReturnType> &
BatchOptimizer<ReturnType,
               ExpressionType>::get_evaluated_dependent_expressions() const
{
    SYMENGINE_ASSERT_MSG(
        values_substituted() == true,
        "The optimizer is not configured to perform evaluation. "
        "This action can only performed after substitute() has been called.");

    return evaluated_dependent_expressions;
}

template <typename ReturnType, typename ExpressionType>
ReturnType BatchOptimizer<ReturnType, ExpressionType>::evaluate(
    const Expression &func) const
{
    SYMENGINE_ASSERT_MSG(
        values_substituted() == true,
        "The optimizer is not configured to perform evaluation. "
        "This action can only performed after substitute() has been called.");
    SYMENGINE_ASSERT(map_dep_ptr_basic_vec_entry.size() > 0);

    const auto &it = map_dep_ptr_basic_vec_entry.find(func.get_basic());
    SYMENGINE_ASSERT_MSG(it != map_dep_ptr_basic_vec_entry.end(),
                         "Function has not been registered: "
                             + func.get_basic()->__str__());
    SYMENGINE_ASSERT(it->second < n_dependent_expressions());

    return evaluated_dependent_expressions[it->second];
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::
    register_scalar_dependent_expression(
        const SymEngine::RCP<const SymEngine::Basic> &func)
{
    SYMENGINE_ASSERT_MSG(evaluated_dependent_expressions.size() == 0,
                         "Cannot register function as the optimizer has "
                         "already been finalized.");
    evaluated_dependent_expressions.reserve(n_dependent_expressions() + 1);
    const bool entry_registered = (map_dep_ptr_basic_vec_entry.find(func)
                                   != map_dep_ptr_basic_vec_entry.end());

    if (entry_registered == false) {
        dependent_expressions.push_back(func);
        map_dep_ptr_basic_vec_entry[func] = dependent_expressions.size() - 1;
    }
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::
    register_vector_dependent_expressions(const SymEngine::vec_basic &funcs)
{
    SYMENGINE_ASSERT_MSG(evaluated_dependent_expressions.size() == 0,
                         "Cannot register function as the optimizer has "
                         "already been finalized.");
    const std::size_t n_dependents_old = n_dependent_expressions();
    evaluated_dependent_expressions.reserve(n_dependents_old + funcs.size());
    dependent_expressions.reserve(n_dependents_old + funcs.size());

    for (const auto &func : funcs) {
        const bool entry_registered = (map_dep_ptr_basic_vec_entry.find(func)
                                       != map_dep_ptr_basic_vec_entry.end());

        if (entry_registered == false) {
            dependent_expressions.push_back(func);
            map_dep_ptr_basic_vec_entry[func]
                = dependent_expressions.size() - 1;
        }
    }
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::create_optimizer(
    std::unique_ptr<SymEngine::Visitor> &optimizer)
{
    SYMENGINE_ASSERT_MSG(!optimizer, "Optimizer has already been created.");

    if (optimization_method() == OptimizerType::off
        || optimization_method() == OptimizerType::dictionary) {
        optimizer.reset(
            new typename internal::
                DictionaryOptimizer<ReturnType, ExpressionType>::VisitorType());
    } else if (optimization_method() == OptimizerType::lambda) {
        optimizer.reset(
            new typename internal::LambdaOptimizer<ReturnType>::VisitorType());
    } else if (optimization_method() == OptimizerType::llvm) {
#ifdef HAVE_SYMENGINE_LLVM
        optimizer.reset(
            new typename internal::LLVMOptimizer<ReturnType>::VisitorType());
#else
        SYMENGINE_ASSERT_MSG(false, "The LLVM compiler is not available.");
#endif
    } else {
        SYMENGINE_ASSERT_MSG(false, "Unknown optimizer selected.");
    }
}

template <typename ReturnType, typename ExpressionType>
void BatchOptimizer<ReturnType, ExpressionType>::print(
    std::ostream &stream, const bool /*print_cse*/) const
{
    // Settings
    stream << "Method? " << optimization_method() << "\n";
    stream << "Flags: " << optimization_flags() << "\n";
    stream << "Optimized? " << (optimized() ? "Yes" : "No") << "\n";
    stream << "Values substituted? " << values_substituted() << "\n\n";

    // Independent variables
    stream << "Symbols (" << n_independent_symbols()
           << " independent variables):"
           << "\n";
    int cntr = 0;
    for (auto const &it : independent_symbols) {
        stream << cntr << ": " << *it.first << "\n";
        cntr++;
    }
    stream << "\n" << std::flush;

    // Dependent functions
    stream << "Functions (" << n_dependent_expressions()
           << " dependent variables):"
           << "\n";
    cntr = 0;
    for (auto const &it : dependent_expressions) {
        stream << cntr << ": " << *it << "\n";
        cntr++;
    }
    stream << "\n" << std::flush;

    // Common subexpression
    if (optimized() == true && use_symbolic_CSE() == true) {
        SYMENGINE_ASSERT(optimizer);
        const bool print_cse_reductions = true;
        const bool print_independent_symbols = false;
        const bool print_dependent_functions = false;

        // TODO: Why does this not work with dynamic_cast, but rather requires
        // a static_cast?
        //       Dynamic_casts work perfectly fine in the other functions.
        //       Maybe this is related:
        //       https://stackoverflow.com/questions/590371/dynamic-cast-fails
        //          if (typename
        //          internal::DictionaryOptimizer<ReturnType>::OptimizerType*
        //              opt = dynamic_cast<typename
        //              internal::DictionaryOptimizer<ReturnType>::OptimizerType
        //              *> (optimizer.get()))
        if (optimization_method() == OptimizerType::dictionary) {
            internal::OptimizerHelper<ReturnType,
                                      internal::
                                          DictionaryOptimizer<ReturnType,
                                                              ExpressionType>>::
                print(
                    stream,
                    static_cast<
                        typename internal::DictionaryOptimizer<ReturnType,
                                                               ExpressionType>::
                            VisitorType *>(optimizer.get()),
                    print_independent_symbols, print_dependent_functions,
                    print_cse_reductions);
            stream << "\n" << std::flush;
        } else if (optimization_method() == OptimizerType::lambda) {
            internal::OptimizerHelper<ReturnType,
                                      internal::LambdaOptimizer<ReturnType>>::
                print(
                    stream,
                    static_cast<typename internal::LambdaOptimizer<ReturnType>::
                                    VisitorType *>(optimizer.get()),
                    print_independent_symbols, print_dependent_functions,
                    print_cse_reductions);
        }
#ifdef HAVE_SYMENGINE_LLVM
        else if (optimization_method() == OptimizerType::llvm) {
            internal::OptimizerHelper<ReturnType,
                                      internal::LLVMOptimizer<ReturnType>>::
                print(stream,
                      static_cast<typename internal::LLVMOptimizer<ReturnType>::
                                      VisitorType *>(optimizer.get()),
                      print_independent_symbols, print_dependent_functions,
                      print_cse_reductions);
        }
#endif
        else {
            SYMENGINE_ASSERT_MSG(false, "Unknown optimizer type.");
        }
    }

    if (values_substituted()) {
        stream << "Evaluated functions:"
               << "\n";
        stream << std::flush;
        cntr = 0;
        for (auto const &it : evaluated_dependent_expressions) {
            stream << cntr << ": " << it << "\n";
        }
        stream << "\n" << std::flush;
    }
}

#endif // DOXYGEN

} // namespace Numerics
} // namespace SymEngine

#endif // SYMENGINE_NUMERICS_OPTIMIZER_H
