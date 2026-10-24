#include <benchmark/benchmark.h>
#include "visitor_expressions.h"

struct CompiledExpr1 {
    void call(double *d, const double *v)
    {
        double r1
            = std::sin(v[0] + std::cos((v[1] * v[2]) + std::pow(v[0], 2)));
        double r2 = (3 + r1) * (2 + r1);
        *d = std::pow((5 + r2), (r2 - 2));
    }
    void call(float *d, const float *v)
    {
        float r1 = sinf(v[0] + cosf((v[1] * v[2]) + powf(v[0], 2)));
        float r2 = (3 + r1) * (2 + r1);
        *d = powf((5 + r2), (r2 - 2));
    }
};

struct CompiledExpr2 {
    template <typename Real>
    void call(Real *d, const Real *v)
    {
        d[0] = static_cast<Real>(2.0) * (v[0] + v[0] + (v[1] * v[2]));
        d[1] = v[0] + v[0] + (v[2] * v[1]);
        d[2] = -static_cast<Real>(2.0) * (v[0] + v[0] + (v[1] * v[2]));
    }
};

void init(CompiledExpr1 &v, const vec_basic &args, const vec_basic &expr,
          bool cse, unsigned opt_level){};

void init(CompiledExpr2 &v, const vec_basic &args, const vec_basic &expr,
          bool cse, unsigned opt_level){};

constexpr std::size_t n_samples{64};

template <typename Visitor, typename Expr, typename Real>
static void Call(benchmark::State &state)
{
    Expr e;
    vec_basic inputs{e.vec};
    vec_basic outputs{e.expr()};
    const std::size_t n_inputs{inputs.size()};
    std::vector<Real> d(outputs.size(), 0.0);
    std::vector<Real> x(n_samples * n_inputs, 0.0);
    for (std::size_t s = 0; s < n_samples; ++s) {
        for (std::size_t i = 0; i < n_inputs; ++i) {
            x[s * n_inputs + i] = static_cast<Real>(
                i + 0.05 + 0.2 * (s + 0.5) / static_cast<double>(n_samples));
        }
    }
    Visitor v;
    bool cse{static_cast<bool>(state.range(0))};
    unsigned opt_level{static_cast<unsigned>(state.range(1))};
    init(v, inputs, outputs, cse, opt_level);
    benchmark::DoNotOptimize(x.data());
    benchmark::DoNotOptimize(d.data());
    std::size_t sample{0};
    for (auto _ : state) {
        benchmark::ClobberMemory();
        v.call(d.data(), x.data() + sample * n_inputs);
        benchmark::ClobberMemory();
        sample = (sample + 1) % n_samples;
    }
    state.SetLabel(to_label(cse, opt_level));
}

SYMENGINE_BENCHMARK_VISITORS(Call);

// repeat benchmarks with natively compiled version of expressions
BENCHMARK_TEMPLATE(Call, CompiledExpr1, Expr1, double)->Args({0, 0});
BENCHMARK_TEMPLATE(Call, CompiledExpr1, Expr1, float)->Args({0, 0});
BENCHMARK_TEMPLATE(Call, CompiledExpr2, Expr2, double)->Args({0, 0});
BENCHMARK_TEMPLATE(Call, CompiledExpr2, Expr2, float)->Args({0, 0});

BENCHMARK_MAIN();
