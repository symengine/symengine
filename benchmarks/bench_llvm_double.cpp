#define NONIUS_RUNNER
#include "nonius.h++"

#include "bench_common.h"
#include <symengine/llvm_double.h>

using SymEngine::LLVMDoubleVisitor;

#define SYMENGINE_LLVM_BENCH_CALL(NAME, EXPR, CSE, OPT)                        \
    NONIUS_BENCHMARK(NAME, [](nonius::chronometer meter) {                     \
        auto vec = get_vec();                                                  \
        auto expr = get_expression_##EXPR(vec);                                \
        LLVMDoubleVisitor v;                                                   \
        v.init(vec, expr, CSE, OPT);                                           \
        std::vector<double> s{0.0, 0.0, 0.0};                                  \
        std::vector<double> d{0.0, 0.0, 0.0};                                  \
        std::vector<double> x{1.0, 4.4365, 12.8};                              \
        meter.measure([&]() {                                                  \
            for (std::size_t j = 0; j < call_iterations; ++j) {                \
                x[0] += 1.0;                                                   \
                x[1] += 2.0;                                                   \
                x[2] += 3.0;                                                   \
                v.call(d.data(), x.data());                                    \
                s[0] += d[0];                                                  \
                s[1] += d[1];                                                  \
                s[2] += d[2];                                                  \
            }                                                                  \
        });                                                                    \
    })

#define SYMENGINE_LLVM_BENCH_INIT(NAME, EXPR, CSE, OPT)                        \
    NONIUS_BENCHMARK(NAME, [](nonius::chronometer meter) {                     \
        auto vec = get_vec();                                                  \
        auto expr = get_expression_##EXPR(vec);                                \
        LLVMDoubleVisitor v;                                                   \
        meter.measure([&]() { v.init(vec, expr, CSE, OPT); });                 \
    })

SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr1_O0_call", 1, false, 0);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr1_O0_cse_call", 1, true, 0);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr1_O1_call", 1, false, 1);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr1_O1_cse_call", 1, true, 1);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr1_O2_call", 1, false, 2);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr1_O2_cse_call", 1, true, 2);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr1_O3_call", 1, false, 3);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr1_O3_cse_call", 1, true, 3);

SYMENGINE_LLVM_BENCH_INIT("llvm_double_visitor_expr1_O0_init", 1, false, 0);
SYMENGINE_LLVM_BENCH_INIT("llvm_double_visitor_expr1_O1_init", 1, false, 1);
SYMENGINE_LLVM_BENCH_INIT("llvm_double_visitor_expr1_O2_init", 1, false, 2);
SYMENGINE_LLVM_BENCH_INIT("llvm_double_visitor_expr1_O3_init", 1, false, 3);

SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr2_O0_call", 2, false, 0);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr2_O0_cse_call", 2, true, 0);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr2_O1_call", 2, false, 1);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr2_O1_cse_call", 2, true, 1);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr2_O2_call", 2, false, 2);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr2_O2_cse_call", 2, true, 2);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr2_O3_call", 2, false, 3);
SYMENGINE_LLVM_BENCH_CALL("llvm_double_visitor_expr2_O3_cse_call", 2, true, 3);

SYMENGINE_LLVM_BENCH_INIT("llvm_double_visitor_expr2_O0_init", 2, false, 0);
SYMENGINE_LLVM_BENCH_INIT("llvm_double_visitor_expr2_O1_init", 2, false, 1);
SYMENGINE_LLVM_BENCH_INIT("llvm_double_visitor_expr2_O2_init", 2, false, 2);
SYMENGINE_LLVM_BENCH_INIT("llvm_double_visitor_expr2_O3_init", 2, false, 3);
