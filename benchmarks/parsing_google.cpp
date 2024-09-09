#include <benchmark/benchmark.h>
#include <symengine/parser.h>
#include <symengine/parser/parser.h>

using SymEngine::Basic;
using SymEngine::parse;
using SymEngine::RCP;

static void parse_0(benchmark::State &state)
{
    RCP<const Basic> a;
    for (auto _ : state) {
        a = parse("0");
        benchmark::DoNotOptimize(a);
    }
}

static void parse_double(benchmark::State &state)
{
    std::string text = "-.123e-11";
    RCP<const Basic> a;
    for (auto _ : state) {
        a = parse(text);
        benchmark::DoNotOptimize(a);
    }
}

static void parse_many_doubles(benchmark::State &state)
{
    std::string text;
    for (int i = 0; i < state.range(0); i++) {
        text.append("+24.45645 - 0.9834e-2 - 23.243 + 0.058263e-11");
    }
    RCP<const Basic> a;
    for (auto _ : state) {
        a = parse(text);
        benchmark::DoNotOptimize(a);
    }
    state.SetComplexityN(4 * state.range(0));
}

static void parse_many_doubles_implicit_mul(benchmark::State &state)
{
    std::string text;
    for (int i = 0; i < state.range(0); i++) {
        text.append("+24.45645x - 0.9834e-2y - 23.243z + 0.058263e-11w");
    }
    RCP<const Basic> a;
    for (auto _ : state) {
        a = parse(text);
        benchmark::DoNotOptimize(a);
    }
    state.SetComplexityN(4 * state.range(0));
}

static void parse_long_expr1(benchmark::State &state)
{
    std::string text = "1";
    std::string t0 = " * (x + y - sin(x)/(z**2-4) - x**(y**z))";
    for (int i = 0; i < state.range(0); i++) {
        text.append(t0);
    }
    RCP<const Basic> a;
    for (auto _ : state) {
        a = parse(text);
        benchmark::DoNotOptimize(a);
    }
    state.SetComplexityN(state.range(0));
}

static void parse_long_expr2(benchmark::State &state)
{
    std::string text = "1";
    std::string t0 = " * (cos(x) + arcsinh(y - sin(x))/(z**2-4) - x**(y**z))";
    for (int i = 0; i < state.range(0); i++) {
        text.append(t0);
    }
    RCP<const Basic> a;
    for (auto _ : state) {
        a = parse(text);
        benchmark::DoNotOptimize(a);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(parse_0);
BENCHMARK(parse_double);
BENCHMARK(parse_many_doubles)->Range(1, 4096)->Complexity();
BENCHMARK(parse_many_doubles_implicit_mul)->Range(1, 4096)->Complexity();
BENCHMARK(parse_long_expr1)->Range(2, 4096)->Complexity();
BENCHMARK(parse_long_expr2)->Range(2, 4096)->Complexity();

BENCHMARK_MAIN();
