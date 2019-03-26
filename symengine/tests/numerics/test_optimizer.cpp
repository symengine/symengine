#include "catch.hpp"
#include "user_defined_number.hpp"

#include <symengine/symengine_config.h>
#include <symengine/dict.h>
#include <symengine/expression.h>
#include <symengine/numerics/number_visitor.h>
#include <symengine/numerics/optimizer.h>

#include <initializer_list>

using namespace SymEngine::Numerics;

using expression_t = SymEngine::Expression;
using substitution_map_t = SymEngine::map_basic_basic;
using symbol_vector_t = SymEngine::vec_basic;

TEST_CASE("BatchOptimizer : Constructor", "[numerics]")
{
    // Constructor: Default
    {
        BatchOptimizer<double, expression_t> optimizer;
        REQUIRE(optimizer.optimization_method() == OptimizerType::dictionary);
        REQUIRE(optimizer.optimization_flags()
                == OptimizationFlags::optimize_default);
        REQUIRE(optimizer.use_symbolic_CSE() == false);
        REQUIRE(optimizer.values_substituted() == false);
        REQUIRE(optimizer.n_independent_symbols() == 0);
        REQUIRE(optimizer.n_dependent_expressions() == 0);
    }

    // Constructor: Default with inline settings
    {
        BatchOptimizer<double, expression_t> optimizer(
            OptimizerType::lambda, OptimizationFlags::optimize_all);
        REQUIRE(optimizer.optimization_method() == OptimizerType::lambda);
        REQUIRE(optimizer.optimization_flags()
                == OptimizationFlags::optimize_all);
        REQUIRE(optimizer.use_symbolic_CSE() == true);
    }

#ifdef HAVE_SYMENGINE_LLVM
    // Constructor: Default with inline settings
    {
        BatchOptimizer<double, expression_t> optimizer(
            OptimizerType::llvm, OptimizationFlags::optimize_all);
        REQUIRE(optimizer.optimization_method() == OptimizerType::llvm);
        REQUIRE(optimizer.optimization_flags()
                == OptimizationFlags::optimize_all);
        REQUIRE(optimizer.use_symbolic_CSE() == true);
    }
#else
    // Constructor: Default with inline settings
    // Since LLVM is not available, this should default back to lambda
    {
        BatchOptimizer<double, expression_t> optimizer(
            OptimizerType::llvm, OptimizationFlags::optimize_all);
        REQUIRE(optimizer.optimization_method() == OptimizerType::lambda);
        REQUIRE(optimizer.optimization_flags()
                == OptimizationFlags::optimize_all);
        REQUIRE(optimizer.use_symbolic_CSE() == true);
    }
#endif

    // Constructor: Default with post-construction settings
    {
        BatchOptimizer<double, expression_t> optimizer;
        optimizer.set_optimization_method(OptimizerType::lambda,
                                          OptimizationFlags::optimize_all);
        REQUIRE(optimizer.optimization_method() == OptimizerType::lambda);
        REQUIRE(optimizer.optimization_flags()
                == OptimizationFlags::optimize_all);
        REQUIRE(optimizer.use_symbolic_CSE() == true);
    }
}

TEST_CASE("BatchOptimizer : Registration", "[numerics]")
{
    // Independent symbol registration by map
    {
        const auto symb_s1 = expression_t("s1");
        const auto symb_s2 = expression_t("s2");

        BatchOptimizer<double, expression_t> optimizer;
        const substitution_map_t sub_vals_optim(
            {{symb_s1, expression_t(2.0)}, {symb_s2, expression_t(1.0)}});
        optimizer.register_independent_symbols(sub_vals_optim);
        REQUIRE(optimizer.n_independent_symbols() == 2);

        // Expect these to be dictionary ordered
        REQUIRE(SymEngine::eq(*optimizer.get_independent_symbols()[0],
                              *symb_s1.get_basic()));
        REQUIRE(SymEngine::eq(*optimizer.get_independent_symbols()[1],
                              *symb_s2.get_basic()));
    }

    // Independent symbol registration by vector
    {
        const auto symb_s1 = expression_t("s1");
        const auto symb_s2 = expression_t("s2");

        BatchOptimizer<double, expression_t> optimizer;
        const symbol_vector_t sub_vals_optim({symb_s2, symb_s1});
        optimizer.register_independent_symbols(sub_vals_optim);
        REQUIRE(optimizer.n_independent_symbols() == 2);

        // Expect these to still be dictionary ordered
        REQUIRE(SymEngine::eq(*optimizer.get_independent_symbols()[0],
                              *symb_s1.get_basic()));
        REQUIRE(SymEngine::eq(*optimizer.get_independent_symbols()[1],
                              *symb_s2.get_basic()));
    }

    // Dependent function registration
    {
        const auto symb_s1 = expression_t("s1");
        const auto symb_s2 = expression_t("s2");

        const expression_t s1(symb_s1);
        const expression_t s2(symb_s2);
        const expression_t f1 = s1 + s2;
        const expression_t f2 = s1 * s2;
        const expression_t f3 = 2.0 * s1 / s2;

        BatchOptimizer<double, expression_t> optimizer;
        const substitution_map_t sub_vals_optim(
            {{symb_s1, expression_t(2.0)}, {symb_s2, expression_t(1.0)}});
        optimizer.register_independent_symbols(sub_vals_optim);
        REQUIRE(optimizer.n_independent_symbols() == 2);
        optimizer.register_dependent_expression(f1);
        optimizer.register_dependent_expressions(f2,
                                                 f3); // Using variadic function
        REQUIRE(optimizer.n_dependent_expressions() == 3);

        REQUIRE(SymEngine::eq(*optimizer.get_dependent_expressions()[0],
                              *f1.get_basic()));
        REQUIRE(SymEngine::eq(*optimizer.get_dependent_expressions()[1],
                              *f2.get_basic()));
        REQUIRE(SymEngine::eq(*optimizer.get_dependent_expressions()[2],
                              *f3.get_basic()));
    }
}

// Function and its derivatives
template <typename NumberType>
struct OptimizerTestScalarScalarCoupled {

    static NumberType psi(const NumberType &s1, const NumberType &s2)
    {
        using std::pow;
        using SymEngine::pow;
        return 2.0 * pow(s1, 4) * pow(s2, 3);
    };

    static NumberType dpsi_ds1(const NumberType &s1, const NumberType &s2)
    {
        using std::pow;
        using SymEngine::pow;
        return 8.0 * pow(s1, 3) * pow(s2, 3);
    };

    static NumberType dpsi_ds2(const NumberType &s1, const NumberType &s2)
    {
        using std::pow;
        using SymEngine::pow;
        return 6.0 * pow(s1, 4) * pow(s2, 2);
    };

    static NumberType d2psi_ds1_ds1(const NumberType &s1, const NumberType &s2)
    {
        using std::pow;
        using SymEngine::pow;
        return 24.0 * pow(s1, 2) * pow(s2, 3);
    };

    static NumberType d2psi_ds2_ds1(const NumberType &s1, const NumberType &s2)
    {
        using std::pow;
        using SymEngine::pow;
        return 24.0 * pow(s1, 3) * pow(s2, 2);
    };

    static NumberType d2psi_ds1_ds2(const NumberType &s1, const NumberType &s2)
    {
        return d2psi_ds2_ds1(s1, s2);
    };

    static NumberType d2psi_ds2_ds2(const NumberType &s1, const NumberType &s2)
    {
        using std::pow;
        using SymEngine::pow;
        return 12.0 * pow(s1, 4) * pow(s2, 1);
    };
};

template <typename number_t>
void evaluate_batch_optimizer(BatchOptimizer<number_t, expression_t> &optimizer,
                              const number_t &s1, const number_t &s2)
{
    // Symbols
    const expression_t symb_s1("s1");
    const expression_t symb_s2("s2");

    // Function and its derivatives
    using func_sd = OptimizerTestScalarScalarCoupled<expression_t>;

    // Value
    const expression_t symb_psi = func_sd::psi(symb_s1, symb_s2);
    // First derivative
    const expression_t symb_dpsi_ds1 = symb_psi.diff(symb_s1);
    const expression_t symb_dpsi_ds2 = symb_psi.diff(symb_s2);
    // Second derivatives
    const expression_t symb_d2psi_ds1_ds1 = symb_dpsi_ds1.diff(symb_s1);
    const expression_t symb_d2psi_ds1_ds2 = symb_dpsi_ds1.diff(symb_s2);
    const expression_t symb_d2psi_ds2_ds1 = symb_dpsi_ds2.diff(symb_s1);
    const expression_t symb_d2psi_ds2_ds2 = symb_dpsi_ds2.diff(symb_s2);

    const bool initialised = optimizer.optimized();
    if (initialised == false) {
        SYMENGINE_ASSERT(optimizer.n_independent_symbols() == 0);
        SYMENGINE_ASSERT(optimizer.n_dependent_expressions() == 0);

        // Register independent variables
        const expression_t dummy_val(0);
        const substitution_map_t sub_vals_optim(
            {{symb_s1.get_basic(), dummy_val.get_basic()},
             {symb_s2.get_basic(), dummy_val.get_basic()}});
        optimizer.register_independent_symbols(sub_vals_optim);

        // Value
        optimizer.register_dependent_expression(symb_psi);
        // First derivative
        optimizer.register_dependent_expression(symb_dpsi_ds1);
        optimizer.register_dependent_expression(symb_dpsi_ds2);
        // Second derivatives
        optimizer.register_dependent_expression(symb_d2psi_ds1_ds1);
        optimizer.register_dependent_expression(symb_d2psi_ds1_ds2);
        optimizer.register_dependent_expression(symb_d2psi_ds2_ds1);
        optimizer.register_dependent_expression(symb_d2psi_ds2_ds2);

        optimizer.optimize();

// Print optimiser
#ifdef DEBUG
        const bool print_optimiser = true;
#else
        const bool print_optimiser = false;
#endif
        if (print_optimiser == true) {
            std::cout << "Optimizer" << std::endl;
            optimizer.print(std::cout);
        }
    } else {
        SYMENGINE_ASSERT(optimizer.n_independent_symbols() > 0);
        SYMENGINE_ASSERT(optimizer.n_dependent_expressions() > 0);
    }

    // Perform substitution
    const substitution_map_t sub_vals(
        {{symb_s1.get_basic(), expression_t(s1).get_basic()},
         {symb_s2.get_basic(), expression_t(s2).get_basic()}});
    optimizer.substitute(sub_vals);

    // Value
    const number_t psi = optimizer.evaluate(symb_psi);
    // First derivatives
    const number_t dpsi_ds1 = optimizer.evaluate(symb_dpsi_ds1);
    const number_t dpsi_ds2 = optimizer.evaluate(symb_dpsi_ds2);
    // Second derivatives
    const number_t d2psi_ds1_ds1 = optimizer.evaluate(symb_d2psi_ds1_ds1);
    const number_t d2psi_ds1_ds2 = optimizer.evaluate(symb_d2psi_ds1_ds2);
    const number_t d2psi_ds2_ds1 = optimizer.evaluate(symb_d2psi_ds2_ds1);
    const number_t d2psi_ds2_ds2 = optimizer.evaluate(symb_d2psi_ds2_ds2);

    // Verify the result
    using func = OptimizerTestScalarScalarCoupled<number_t>;
    const double tol = 1e-9;

    // Value
    REQUIRE(std::abs(psi - func::psi(s1, s2)) < tol);
    // First derivative
    REQUIRE(std::abs(dpsi_ds1 - func::dpsi_ds1(s1, s2)) < tol);
    REQUIRE(std::abs(dpsi_ds2 - func::dpsi_ds2(s1, s2)) < tol);
    // Second derivative
    REQUIRE(std::abs(d2psi_ds1_ds1 - func::d2psi_ds1_ds1(s1, s2)) < tol);
    REQUIRE(std::abs(d2psi_ds1_ds2 - func::d2psi_ds1_ds2(s1, s2)) < tol);
    REQUIRE(std::abs(d2psi_ds2_ds1 - func::d2psi_ds2_ds1(s1, s2)) < tol);
    REQUIRE(std::abs(d2psi_ds2_ds2 - func::d2psi_ds2_ds2(s1, s2)) < tol);
}

template <typename number_t>
void test_batch_optimizer(const number_t &s1, const number_t &s2,
                          const number_t &s3, const number_t &s4)
{
    auto test_func = [&](const OptimizerType opt_method,
                         const OptimizationFlags opt_flags) {
        std::cout << "BatchOptimizer (primative types): " << std::endl;
        std::cout << "  Method: " << opt_method << std::endl;
        std::cout << "  Flags: " << opt_flags << std::endl;
        // Note: The optimiser is created outside the tester function so that we
        // can test that its state remains intact and it is completely
        // functional
        // after the underlying symbols go out of scope.
        BatchOptimizer<number_t, expression_t> optimizer(opt_method, opt_flags);
        evaluate_batch_optimizer<number_t>(optimizer, s1, s2);
        evaluate_batch_optimizer<number_t>(optimizer, s3, s4);
    };

    // Dictionary + no flags
    {
        const OptimizerType opt_method = OptimizerType::dictionary;
        const OptimizationFlags opt_flags = OptimizationFlags::optimize_default;
        test_func(opt_method, opt_flags);
    }

    // Dictionary + all flags
    {
        const OptimizerType opt_method = OptimizerType::dictionary;
        const OptimizationFlags opt_flags = OptimizationFlags::optimize_all;
        test_func(opt_method, opt_flags);
    }

    // Lambda + no flags
    {
        const OptimizerType opt_method = OptimizerType::lambda;
        const OptimizationFlags opt_flags = OptimizationFlags::optimize_default;
        test_func(opt_method, opt_flags);
    }

    // Lambda + all flags
    {
        const OptimizerType opt_method = OptimizerType::lambda;
        const OptimizationFlags opt_flags = OptimizationFlags::optimize_all;
        test_func(opt_method, opt_flags);
    }

    // LLVM + no flags
    // Note: This should fall back to OptimizerType::lambda if
    // SymEngine is not compiled with LLVM support.
    {
        const OptimizerType opt_method = OptimizerType::llvm;
        const OptimizationFlags opt_flags = OptimizationFlags::optimize_default;
        test_func(opt_method, opt_flags);
    }

    // LLVM + all flags
    // Note: This should fall back to OptimizerType::lambda if
    // SymEngine is not compiled with LLVM support.
    {
        const OptimizerType opt_method = OptimizerType::llvm;
        const OptimizationFlags opt_flags = OptimizationFlags::optimize_all;
        test_func(opt_method, opt_flags);
    }
}

TEST_CASE("BatchOptimizer : Substitution and evaluation", "[numerics]")
{
    //  // Float
    //  {
    //    test_batch_optimizer(5.8f,2.6f, 1.3f,9.9f);
    //  }

    // Double
    {
        test_batch_optimizer<double>(5.8, 2.6, 1.3, 9.9);
    }

    //  // Complex float
    //  {
    //    test_batch_optimizer<std::complex<float>>({5.8f,2.6f}, {6.2f,-7.1f},
    //    {1.3f,9.9f}, {-3.5f,1.4f});
    //  }
    //
    //  // Complex double
    //  {
    //    test_batch_optimizer<std::complex<double>>({5.8,2.6}, {6.2,-7.1},
    //    {1.3,9.9}, {-3.5,1.4});
    //  }
}

TEST_CASE("BatchOptimizer : Custom number", "[numerics]")
{
    // Custom number creation
    {
        const Vector vec_1(1, 2, 3);
        const SymEngine::RCP<const SymEngine::Basic> nw_vec_1
            = SymEngine::make_rcp<const LAVectorWrapper<Vector>>(vec_1);
    }

    // Custom number creation though custom expression class
    {
        const Vector vec_1(1, 2, 3);
        const LAVectorExpression<Vector> se_vec_1(vec_1);
        const Vector vec_2 = static_cast<Vector>(se_vec_1);

        REQUIRE(vec_1 == vec_2);
    }

    // Use in standard expression class
    {
        const Vector vec_1(1, 2, 3);
        const Vector vec_2(4, 5, 6);

        const SymEngine::RCP<const SymEngine::Basic> nw_vec_1
            = SymEngine::make_rcp<const LAVectorWrapper<Vector>>(vec_1);
        const SymEngine::RCP<const SymEngine::Basic> nw_vec_2
            = SymEngine::make_rcp<const LAVectorWrapper<Vector>>(vec_2);

        const SymEngine::Expression se_vec_1(nw_vec_1);
        const SymEngine::Expression se_vec_2 = nw_vec_2;

        const SymEngine::RCP<const SymEngine::Integer> sf_1
            = SymEngine::integer(2);
        const SymEngine::RCP<const SymEngine::RealDouble> sf_2
            = SymEngine::real_double(2.5);

        // This cannot work, as the Expression class knows nothing about the
        // Vector class
        // std::cout << "se_vec_1 + se_vec_2: " <<
        // static_cast<Vector>(se_vec_1+se_vec_2) << std::endl;
    }

    // Use in custom expression class
    {
        const Vector vec_1(1, 2, 3);
        const Vector vec_2(4, 5, 6);
        const Vector vec_3(7, 8, 9);

        const SymEngine::RCP<const SymEngine::Basic> nw_vec_1
            = SymEngine::make_rcp<const LAVectorWrapper<Vector>>(vec_1);
        const SymEngine::RCP<const SymEngine::Basic> nw_vec_2
            = SymEngine::make_rcp<const LAVectorWrapper<Vector>>(vec_2);

        const LAVectorExpression<Vector> se_vec_1(nw_vec_1);
        const LAVectorExpression<Vector> se_vec_2 = nw_vec_2;
        const LAVectorExpression<Vector> se_vec_3(vec_3);

        const SymEngine::RCP<const SymEngine::Integer> sf_1
            = SymEngine::integer(2);
        const SymEngine::RCP<const SymEngine::RealDouble> sf_2
            = SymEngine::real_double(2.5);

        const LAVectorExpression<Vector> vec_1_plus_vec_2(se_vec_1 + se_vec_2);
        const LAVectorExpression<Vector> vec_1_dot_vec_2(
            se_vec_1 * se_vec_2); // Could also be an SymEngine::Expression
        const LAVectorExpression<Vector> sf_1_times_se_vec_1(sf_1 * se_vec_1);
        const LAVectorExpression<Vector> sf_2_times_se_vec_1(sf_2 * se_vec_1);

        REQUIRE(static_cast<Vector>(vec_1_plus_vec_2) == Vector(5, 7, 9));
        REQUIRE(static_cast<double>(vec_1_dot_vec_2) == 32);
        REQUIRE(static_cast<Vector>(sf_1_times_se_vec_1) == Vector(2, 4, 6));
        REQUIRE(static_cast<Vector>(sf_2_times_se_vec_1)
                == Vector(2.5, 5, 7.5));
    }

    // Use with dictionary substitution
    {
        const Vector vec_1(1, 2, 3);
        const Vector vec_2(4, 5, 6);
        const Vector vec_3(7, 8, 9);

        using ReturnType = Vector;
        using ExpressionType = LAVectorExpression<Vector>;

        const auto symb_s1 = ExpressionType("v1");
        const auto symb_s2 = ExpressionType("v2");
        const auto symb_s3 = ExpressionType("v3");

        const ExpressionType se_f1(symb_s1 + symb_s2);
        const ExpressionType se_f2(2 * symb_s1);
        const ExpressionType se_f3(2 * (symb_s1 + symb_s2) + symb_s3);

        const SymEngine::vec_basic inputs = {symb_s1, symb_s2, symb_s3};
        const std::vector<ReturnType> sub_vals = {vec_1, vec_2, vec_3};
        const SymEngine::vec_basic outputs = {se_f1, se_f2, se_f3};
        const unsigned int n_outputs = outputs.size();

        // Without CSE
        {
            DictionarySubstitutionVisitor<ReturnType, ExpressionType> visitor;
            visitor.init(inputs, outputs, false);

            std::vector<ReturnType> results(n_outputs);
            visitor.call(results.data(), sub_vals.data());
            REQUIRE(results[0] == (vec_1 + vec_2));
            REQUIRE(results[1] == (2 * vec_1));
            REQUIRE(results[2] == (2 * (vec_1 + vec_2) + vec_3));
        }

        // With CSE
        {
            DictionarySubstitutionVisitor<ReturnType, ExpressionType> visitor;
            visitor.init(inputs, outputs, true);

            std::vector<ReturnType> results(n_outputs);
            visitor.call(results.data(), sub_vals.data());
            REQUIRE(results[0] == (vec_1 + vec_2));
            REQUIRE(results[1] == (2 * vec_1));
            REQUIRE(results[2] == (2 * (vec_1 + vec_2) + vec_3));
        }

        // Optimizer
        auto evaluate_batch_optimizer = [&inputs, &sub_vals, &outputs](
            BatchOptimizer<ReturnType, ExpressionType> &optimizer) {
            const bool initialised = optimizer.optimized();
            if (initialised == false) {
                SYMENGINE_ASSERT(optimizer.n_independent_symbols() == 0);
                SYMENGINE_ASSERT(optimizer.n_dependent_expressions() == 0);

                // Register independent variables
                const expression_t dummy_val(0);
                SYMENGINE_ASSERT(inputs.size() == 3);
                const substitution_map_t sub_vals_optim(
                    {{inputs[0], dummy_val.get_basic()},
                     {inputs[1], dummy_val.get_basic()},
                     {inputs[2], dummy_val.get_basic()}});
                optimizer.register_independent_symbols(sub_vals_optim);
                optimizer.register_dependent_expressions(outputs);
                optimizer.optimize();

// Print optimiser
#ifdef DEBUG
                const bool print_optimiser = true;
#else
                const bool print_optimiser = false;
#endif
                if (print_optimiser == true) {
                    std::cout << "Optimizer" << std::endl;
                    optimizer.print(std::cout);
                }
            } else {
                SYMENGINE_ASSERT(optimizer.n_independent_symbols() > 0);
                SYMENGINE_ASSERT(optimizer.n_dependent_expressions() > 0);
            }

            // Perform substitution
            SYMENGINE_ASSERT(inputs.size() == 3);
            SYMENGINE_ASSERT(sub_vals.size() == 3);
            const substitution_map_t sub_map(
                {{inputs[0], ExpressionType(sub_vals[0]).get_basic()},
                 {inputs[1], ExpressionType(sub_vals[1]).get_basic()},
                 {inputs[2], ExpressionType(sub_vals[2]).get_basic()}});
            optimizer.substitute(sub_map);

            // Value
            SYMENGINE_ASSERT(outputs.size() == 3);
            const ReturnType result_1 = optimizer.evaluate(outputs[0]);
            const ReturnType result_2 = optimizer.evaluate(outputs[1]);
            const ReturnType result_3 = optimizer.evaluate(outputs[2]);

            const Vector &vec_1 = sub_vals[0];
            const Vector &vec_2 = sub_vals[1];
            const Vector &vec_3 = sub_vals[2];
            REQUIRE(result_1 == (vec_1 + vec_2));
            REQUIRE(result_2 == (2 * vec_1));
            REQUIRE(result_3 == (2 * (vec_1 + vec_2) + vec_3));
        };

        auto test_func = [&](const OptimizerType opt_method,
                             const OptimizationFlags opt_flags) {
            std::cout << "BatchOptimizer (NumberWrapper): " << std::endl;
            std::cout << "  Method: " << opt_method << std::endl;
            std::cout << "  Flags: " << opt_flags << std::endl;
            BatchOptimizer<ReturnType, ExpressionType> optimizer(opt_method,
                                                                 opt_flags);
            evaluate_batch_optimizer(optimizer);
            evaluate_batch_optimizer(optimizer);
        };

        // Dictionary + no flags
        {
            const OptimizerType opt_method = OptimizerType::dictionary;
            const OptimizationFlags opt_flags
                = OptimizationFlags::optimize_default;
            test_func(opt_method, opt_flags);
        }

        // Dictionary + all flags
        {
            const OptimizerType opt_method = OptimizerType::dictionary;
            const OptimizationFlags opt_flags = OptimizationFlags::optimize_all;
            test_func(opt_method, opt_flags);
        }

        // Lambda + no flags
        {
            const OptimizerType opt_method = OptimizerType::lambda;
            const OptimizationFlags opt_flags
                = OptimizationFlags::optimize_default;
            test_func(opt_method, opt_flags);
        }

        // Lambda + all flags
        {
            const OptimizerType opt_method = OptimizerType::lambda;
            const OptimizationFlags opt_flags = OptimizationFlags::optimize_all;
            test_func(opt_method, opt_flags);
        }
    }
}
