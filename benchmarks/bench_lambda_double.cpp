#define NONIUS_RUNNER
#include "nonius.h++"

#include "bench_common.h"
#include <symengine/lambda_double.h>

using SymEngine::LambdaRealDoubleVisitor;

#define SYMENGINE_LAMBDA_BENCH_CALL(NAME, EXPR, CSE)                           \
    NONIUS_BENCHMARK(NAME, [](nonius::chronometer meter) {                     \
        auto vec = get_vec();                                                  \
        auto expr = get_expression_##EXPR(vec);                                \
        LambdaRealDoubleVisitor v;                                             \
        v.init(vec, expr, CSE);                                                \
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

#define SYMENGINE_LAMBDA_BENCH_INIT(NAME, EXPR, CSE)                           \
    NONIUS_BENCHMARK(NAME, [](nonius::chronometer meter) {                     \
        auto vec = get_vec();                                                  \
        auto expr = get_expression_##EXPR(vec);                                \
        LambdaRealDoubleVisitor v;                                             \
        meter.measure([&]() { v.init(vec, expr, CSE); });                      \
    })

SYMENGINE_LAMBDA_BENCH_CALL("lambda_real_double_visitor_expr1_call", 1, false);
SYMENGINE_LAMBDA_BENCH_CALL("lambda_real_double_visitor_expr1_cse_call", 1,
                            true);

SYMENGINE_LAMBDA_BENCH_INIT("lambda_real_double_visitor_expr1_init", 1, false);
SYMENGINE_LAMBDA_BENCH_INIT("lambda_real_double_visitor_expr1_cse_init", 1,
                            true);

SYMENGINE_LAMBDA_BENCH_CALL("lambda_real_double_visitor_expr2_call", 2, false);
SYMENGINE_LAMBDA_BENCH_CALL("lambda_real_double_visitor_expr2_cse_call", 2,
                            true);

SYMENGINE_LAMBDA_BENCH_INIT("lambda_real_double_visitor_expr2_init", 2, false);
SYMENGINE_LAMBDA_BENCH_INIT("lambda_real_double_visitor_expr2_cse_init", 2,
                            true);
