#include "catch.hpp"
#include "cudacode_runtime_config.h"

#include <cuda.h>
#include <cuda_runtime.h>
#include <nvrtc.h>

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/functions.h>
#include <symengine/lambda_double.h>
#include <symengine/logic.h>
#include <symengine/pow.h>
#include <symengine/printers.h>
#include <symengine/sets.h>

using SymEngine::add;
using SymEngine::Basic;
using SymEngine::boolFalse;
using SymEngine::boolTrue;
using SymEngine::cbrt;
using SymEngine::CodePrinterPrecision;
using SymEngine::contains;
using SymEngine::cudacode;
using SymEngine::E;
using SymEngine::gamma;
using SymEngine::Ge;
using SymEngine::Gt;
using SymEngine::Inf;
using SymEngine::integer;
using SymEngine::interval;
using SymEngine::LambdaRealDoubleVisitor;
using SymEngine::loggamma;
using SymEngine::logical_and;
using SymEngine::logical_not;
using SymEngine::logical_or;
using SymEngine::logical_xor;
using SymEngine::Lt;
using SymEngine::max;
using SymEngine::mul;
using SymEngine::Nan;
using SymEngine::NegInf;
using SymEngine::pi;
using SymEngine::piecewise;
using SymEngine::pow;
using SymEngine::RCP;
using SymEngine::sign;
using SymEngine::sqrt;
using SymEngine::symbol;
using SymEngine::unevaluated_expr;
using SymEngine::vec_basic;

namespace
{

struct TestInput {
    double x;
    double y;
    double z;
};

struct RuntimeCase {
    std::string name;
    RCP<const Basic> expr;
};

enum class CudaScalarMode { Double, Float };

constexpr double kDoubleRelativeTolerance = 1e-11;
constexpr double kFloatRelativeTolerance = 1e-5;

struct DeviceKernel {
    CUmodule module = nullptr;
    CUfunction function = nullptr;
};

std::string build_kernel_source(const std::string &cuda_code,
                                const char *scalar_type)
{
    std::ostringstream source;
    source << "#include <cuda_runtime.h>\n";
    source << "#include <math_constants.h>\n";
    source << "\n";
    source << "extern \"C\" __global__ void evaluate(" << scalar_type << " x, "
           << scalar_type << " y, " << scalar_type << " z, " << scalar_type
           << " *out)\n";
    source << "{\n";
    source << "    *out = " << cuda_code << ";\n";
    source << "}\n";
    return source.str();
}

std::string compile_to_ptx(const std::string &source)
{
    nvrtcProgram program = nullptr;
    REQUIRE(nvrtcCreateProgram(&program, source.c_str(),
                               "symengine_cudacode.cu", 0, nullptr, nullptr)
            == NVRTC_SUCCESS);

    std::vector<std::string> options = {
        "--std=c++14",
        "--fmad=false",
    };

    std::stringstream stream(SYMENGINE_CUDA_INCLUDE_DIRS);
    std::string path;
    while (std::getline(stream, path, ';')) {
        if (not path.empty()) {
            options.push_back("--include-path=" + path);
        }
    }

    std::vector<const char *> option_ptrs;
    option_ptrs.reserve(options.size());
    for (const auto &option : options) {
        option_ptrs.push_back(option.c_str());
    }

    REQUIRE(nvrtcCompileProgram(program, static_cast<int>(option_ptrs.size()),
                                option_ptrs.data())
            == NVRTC_SUCCESS);

    size_t ptx_size = 0;
    REQUIRE(nvrtcGetPTXSize(program, &ptx_size) == NVRTC_SUCCESS);

    std::string ptx(ptx_size, '\0');
    REQUIRE(nvrtcGetPTX(program, &ptx[0]) == NVRTC_SUCCESS);
    REQUIRE(nvrtcDestroyProgram(&program) == NVRTC_SUCCESS);

    return ptx;
}

DeviceKernel compile_kernel(const std::string &cuda_code,
                            const char *scalar_type)
{
    REQUIRE(cuInit(0) == CUDA_SUCCESS);
    REQUIRE(cudaSetDevice(0) == cudaSuccess);
    const std::string ptx
        = compile_to_ptx(build_kernel_source(cuda_code, scalar_type));

    DeviceKernel kernel;
    REQUIRE(cuModuleLoadDataEx(&kernel.module, ptx.c_str(), 0, nullptr, nullptr)
            == CUDA_SUCCESS);
    REQUIRE(cuModuleGetFunction(&kernel.function, kernel.module, "evaluate")
            == CUDA_SUCCESS);
    return kernel;
}

void unload_kernel(DeviceKernel &kernel)
{
    REQUIRE(cuModuleUnload(kernel.module) == CUDA_SUCCESS);
}

template <typename Scalar>
Scalar evaluate_on_device_typed(const DeviceKernel &kernel,
                                const TestInput &input)
{
    Scalar *device_output = nullptr;
    Scalar x = static_cast<Scalar>(input.x);
    Scalar y = static_cast<Scalar>(input.y);
    Scalar z = static_cast<Scalar>(input.z);
    void *kernel_args[] = {
        &x,
        &y,
        &z,
        &device_output,
    };
    Scalar out = 0;

    REQUIRE(
        cudaMalloc(reinterpret_cast<void **>(&device_output), sizeof(Scalar))
        == cudaSuccess);
    REQUIRE(cuLaunchKernel(kernel.function, 1, 1, 1, 1, 1, 1, 0, nullptr,
                           kernel_args, nullptr)
            == CUDA_SUCCESS);
    REQUIRE(cuCtxSynchronize() == CUDA_SUCCESS);
    REQUIRE(
        cudaMemcpy(&out, device_output, sizeof(Scalar), cudaMemcpyDeviceToHost)
        == cudaSuccess);
    REQUIRE(cudaFree(device_output) == cudaSuccess);
    return out;
}

double evaluate_on_device(const DeviceKernel &kernel, CudaScalarMode mode,
                          const TestInput &input)
{
    if (mode == CudaScalarMode::Double) {
        return evaluate_on_device_typed<double>(kernel, input);
    }

    return static_cast<double>(evaluate_on_device_typed<float>(kernel, input));
}

double evaluate_on_host(const vec_basic &symbols, const Basic &expr,
                        const TestInput &input)
{
    LambdaRealDoubleVisitor visitor;
    visitor.init(symbols, expr);
    return visitor.call({input.x, input.y, input.z});
}

void require_codegen_match(const Basic &expr, const vec_basic &symbols,
                           const DeviceKernel &kernel, CudaScalarMode mode,
                           const TestInput &input)
{
    const double host_value = evaluate_on_host(symbols, expr, input);
    const double device_value = evaluate_on_device(kernel, mode, input);
    if (std::isnan(host_value) || std::isnan(device_value)) {
        REQUIRE(std::isnan(host_value));
        REQUIRE(std::isnan(device_value));
        return;
    }
    if (std::isinf(host_value) || std::isinf(device_value)) {
        REQUIRE(std::isinf(host_value));
        REQUIRE(std::isinf(device_value));
        REQUIRE(std::signbit(host_value) == std::signbit(device_value));
        return;
    }

    const double scale = std::fmax(
        1.0, std::fmax(std::fabs(host_value), std::fabs(device_value)));
    REQUIRE(std::fabs(device_value - host_value)
            <= (mode == CudaScalarMode::Double ? kDoubleRelativeTolerance
                                               : kFloatRelativeTolerance)
                   * scale);
}

} // namespace

TEST_CASE("CUDA code matches Lambda visitor", "[cuda][cudacode]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");
    auto sign_expr = sign(add(x, mul(integer(-1), z)));

    vec_basic symbols = {x, y, z};

    auto arithmetic = add(add(add(add(x, mul(x, y)), pow(x, y)), cbrt(x)),
                          sqrt(integer(2)));
    auto max_expr = max({x, y, z});
    auto min_expr = SymEngine::min({x, y, z});
    auto inverse_trig_expr
        = add(SymEngine::asin(y), add(SymEngine::acos(y), SymEngine::atan(x)));
    auto reciprocal_trig_expr
        = add(SymEngine::cot(x), add(SymEngine::csc(x), SymEngine::sec(y)));
    auto reciprocal_inverse_trig_expr
        = add(SymEngine::acot(x), add(SymEngine::acsc(x), SymEngine::asec(x)));
    auto inverse_hyperbolic_expr = add(
        SymEngine::asinh(x), add(SymEngine::acosh(z), SymEngine::atanh(y)));
    auto reciprocal_hyperbolic_expr
        = add(SymEngine::csch(x), add(SymEngine::sech(y), SymEngine::coth(z)));
    auto reciprocal_inverse_hyperbolic_expr = add(
        SymEngine::acsch(x), add(SymEngine::asech(y), SymEngine::acoth(z)));
    auto special_function_expr = add(
        gamma(x), add(loggamma(z), add(SymEngine::erf(y), SymEngine::erfc(x))));
    auto relational_expr = add(add(boolTrue, boolFalse),
                               add(SymEngine::Eq(x, integer(3)),
                                   add(SymEngine::Ne(y, integer(1)),
                                       add(SymEngine::Le(y, z), Lt(x, z)))));
    auto logical_expr
        = add(logical_and({Lt(x, integer(6)), Gt(x, integer(5))}),
              add(logical_or({Lt(x, integer(2)), Gt(z, integer(5))}),
                  add(logical_xor({Lt(x, integer(2)), Gt(z, integer(5))}),
                      logical_not(logical_xor(
                          {Lt(x, integer(2)), Gt(z, integer(5))})))));
    auto piecewise_expr = piecewise(
        {{x, contains(x, interval(NegInf, integer(2), true, false))},
         {y, contains(x, interval(integer(2), integer(5), true, false))},
         {add(x, y), boolTrue}});
    auto llvm_piecewise_expr = add(
        z,
        piecewise(
            {{x, contains(x, interval(NegInf, integer(2), true, false))},
             {y, contains(x, interval(integer(2), integer(5), true, false))},
             {z, Ge(x, integer(7))},
             {add(x, y), logical_and({Lt(x, integer(6)), Gt(x, integer(5))})},
             {add(x, y), boolTrue}}));

    std::vector<RuntimeCase> cases = {
        {"arithmetic", arithmetic},
        {"integer base power", pow(integer(2), x)},
        {"sqrt", sqrt(z)},
        {"cbrt", cbrt(x)},
        {"abs", SymEngine::abs(x)},
        {"sin", SymEngine::sin(x)},
        {"cos", SymEngine::cos(y)},
        {"tan", SymEngine::tan(z)},
        {"atan2", SymEngine::atan2(x, z)},
        {"exp", SymEngine::exp(y)},
        {"log", SymEngine::log(add(x, integer(2)))},
        {"sinh", SymEngine::sinh(x)},
        {"cosh", SymEngine::cosh(y)},
        {"tanh", SymEngine::tanh(z)},
        {"inverse trigonometric", inverse_trig_expr},
        {"reciprocal trigonometric", reciprocal_trig_expr},
        {"reciprocal inverse trigonometric", reciprocal_inverse_trig_expr},
        {"inverse hyperbolic", inverse_hyperbolic_expr},
        {"reciprocal hyperbolic", reciprocal_hyperbolic_expr},
        {"reciprocal inverse hyperbolic", reciprocal_inverse_hyperbolic_expr},
        {"floor", SymEngine::floor(x)},
        {"ceiling", SymEngine::ceiling(y)},
        {"truncate", SymEngine::truncate(add(x, y))},
        {"special functions", special_function_expr},
        {"max", max_expr},
        {"min", min_expr},
        {"piecewise", piecewise_expr},
        {"constant E", E},
        {"constant pi", pi},
        {"positive infinity", Inf},
        {"nan", Nan},
        {"booleans and relationals", relational_expr},
        {"logical operators", logical_expr},
        {"sign", sign_expr},
        {"unevaluated expr", unevaluated_expr(add(x, y))},
        {"llvm piecewise", llvm_piecewise_expr},
    };

    const std::vector<double> x_values = {1.2, 3.0, 7.0};
    const std::vector<double> y_values = {0.2, 0.5, 0.8};
    const std::vector<double> z_values = {1.2, 3.0, 7.0};
    for (const auto mode : {CudaScalarMode::Double, CudaScalarMode::Float}) {
        for (const auto &runtime_case : cases) {
            const std::string cuda_code
                = mode == CudaScalarMode::Double
                      ? cudacode(*runtime_case.expr)
                      : cudacode(*runtime_case.expr,
                                 CodePrinterPrecision::Float);
            DeviceKernel kernel = compile_kernel(
                cuda_code, mode == CudaScalarMode::Double ? "double" : "float");
            for (double x_value : x_values) {
                for (double y_value : y_values) {
                    for (double z_value : z_values) {
                        require_codegen_match(*runtime_case.expr, symbols,
                                              kernel, mode,
                                              {x_value, y_value, z_value});
                    }
                }
            }
            unload_kernel(kernel);
        }
    }
}
