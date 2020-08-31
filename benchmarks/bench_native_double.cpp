#define NONIUS_RUNNER
#include "nonius.h++"

#include "bench_common.h"

NONIUS_BENCHMARK("native_expr1_call", [](nonius::chronometer meter) {
    std::vector<double> s{0.0, 0.0, 0.0};
    std::vector<double> d{0.0, 0.0, 0.0};
    std::vector<double> x{1.0, 4.4365, 12.8};
    meter.measure([&]() {
        for (std::size_t j = 0; j < call_iterations; ++j) {
            x[0] += 1.0;
            x[1] += 2.0;
            x[2] += 3.0;
            call_compiled_expression_1(d.data(), x.data());
            s[0] += d[0];
            s[1] += d[1];
            s[2] += d[2];
        }
    });
})

NONIUS_BENCHMARK("native_expr2_call", [](nonius::chronometer meter) {
    std::vector<double> s{0.0, 0.0, 0.0};
    std::vector<double> d{0.0, 0.0, 0.0};
    std::vector<double> x{1.0, 4.4365, 12.8};
    meter.measure([&]() {
        for (std::size_t j = 0; j < call_iterations; ++j) {
            x[0] += 1.0;
            x[1] += 2.0;
            x[2] += 3.0;
            call_compiled_expression_2(d.data(), x.data());
            s[0] += d[0];
            s[1] += d[1];
            s[2] += d[2];
        }
    });
})
