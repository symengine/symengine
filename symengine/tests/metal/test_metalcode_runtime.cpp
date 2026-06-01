#include "catch.hpp"

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <cmath>
#include <ostream>
#include <sstream>
#include <stdexcept>
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
using SymEngine::E;
using SymEngine::Ge;
using SymEngine::Gt;
using SymEngine::Inf;
using SymEngine::integer;
using SymEngine::interval;
using SymEngine::LambdaRealDoubleVisitor;
using SymEngine::logical_and;
using SymEngine::logical_not;
using SymEngine::logical_or;
using SymEngine::logical_xor;
using SymEngine::Lt;
using SymEngine::max;
using SymEngine::metalcode;
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

std::ostream &operator<<(std::ostream &os, const NS::Error *error)
{
    if (error == nullptr) {
        os << "unknown Metal error";
        return os;
    }

    const NS::String *description = error->localizedDescription();
    if (description == nullptr || description->utf8String() == nullptr) {
        os << "unknown Metal error";
        return os;
    }

    os << description->utf8String();
    return os;
}

struct TestInput {
    double x;
    double y;
    double z;
};

struct RuntimeCase {
    std::string name;
    RCP<const Basic> expr;
};

struct KernelPayload {
    float x;
    float y;
    float z;
    float out;
};

struct DeviceKernel {
    MTL::Device *device = nullptr;
    MTL::ComputePipelineState *pipeline = nullptr;
    MTL::CommandQueue *queue = nullptr;
};

constexpr double kFloatRelativeTolerance = 1e-6;
constexpr double kHalfRelativeTolerance = 1e-3;

std::string build_kernel_source(const std::string &metal_code,
                                CodePrinterPrecision precision)
{
    std::ostringstream source;
    source << "#include <metal_stdlib>\n";
    source << "using namespace metal;\n";
    source << "\n";
    source << "struct KernelPayload {\n";
    source << "    float x;\n";
    source << "    float y;\n";
    source << "    float z;\n";
    source << "    float out;\n";
    source << "};\n";
    source << "\n";
    source
        << "kernel void evaluate(device KernelPayload *data [[buffer(0)]])\n";
    source << "{\n";
    if (precision == CodePrinterPrecision::Float) {
        source << "    const float x = data[0].x;\n";
        source << "    const float y = data[0].y;\n";
        source << "    const float z = data[0].z;\n";
        source << "    data[0].out = " << metal_code << ";\n";
    } else if (precision == CodePrinterPrecision::Half) {
        source << "    const half x = half(data[0].x);\n";
        source << "    const half y = half(data[0].y);\n";
        source << "    const half z = half(data[0].z);\n";
        source << "    data[0].out = float(" << metal_code << ");\n";
    } else {
        throw std::runtime_error("Unsupported Metal runtime precision");
    }
    source << "}\n";
    return source.str();
}

DeviceKernel compile_kernel(MTL::Device *device, const std::string &metal_code,
                            CodePrinterPrecision precision)
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
    const std::string source = build_kernel_source(metal_code, precision);
    NS::Error *error = nullptr;
    MTL::Library *library = device->newLibrary(
        NS::String::string(source.c_str(), NS::UTF8StringEncoding), nullptr,
        &error);
    INFO(error);
    REQUIRE(library != nullptr);

    MTL::Function *function = library->newFunction(
        NS::String::string("evaluate", NS::UTF8StringEncoding));
    REQUIRE(function != nullptr);

    error = nullptr;
    DeviceKernel kernel;
    kernel.device = device;
    kernel.pipeline = device->newComputePipelineState(function, &error);
    INFO(error);
    REQUIRE(kernel.pipeline != nullptr);
    kernel.queue = device->newCommandQueue();
    REQUIRE(kernel.queue != nullptr);

    function->release();
    library->release();
    pool->release();
    return kernel;
}

void unload_kernel(DeviceKernel &kernel)
{
    kernel.pipeline->release();
    kernel.queue->release();
}

double evaluate_on_device(const DeviceKernel &kernel, const TestInput &input)
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
    MTL::Buffer *buffer = kernel.device->newBuffer(
        sizeof(KernelPayload), MTL::ResourceStorageModeShared);
    REQUIRE(buffer != nullptr);

    auto *payload = static_cast<KernelPayload *>(buffer->contents());
    payload->x = static_cast<float>(input.x);
    payload->y = static_cast<float>(input.y);
    payload->z = static_cast<float>(input.z);
    payload->out = 0.0f;

    MTL::CommandBuffer *command_buffer = kernel.queue->commandBuffer();
    REQUIRE(command_buffer != nullptr);
    MTL::ComputeCommandEncoder *encoder
        = command_buffer->computeCommandEncoder();
    REQUIRE(encoder != nullptr);

    encoder->setComputePipelineState(kernel.pipeline);
    encoder->setBuffer(buffer, 0, 0);
    const MTL::Size grid_size = {1, 1, 1};
    const MTL::Size threadgroup_size = {1, 1, 1};
    encoder->dispatchThreadgroups(grid_size, threadgroup_size);
    encoder->endEncoding();
    command_buffer->commit();
    command_buffer->waitUntilCompleted();

    const double out = static_cast<double>(payload->out);
    buffer->release();
    pool->release();
    return out;
}

double evaluate_on_host(const vec_basic &symbols, const Basic &expr,
                        const TestInput &input)
{
    LambdaRealDoubleVisitor visitor;
    visitor.init(symbols, expr);
    return visitor.call({input.x, input.y, input.z});
}

void require_codegen_match(const Basic &expr, const vec_basic &symbols,
                           const DeviceKernel &kernel,
                           CodePrinterPrecision precision,
                           const TestInput &input)
{
    const double host_value = evaluate_on_host(symbols, expr, input);
    const double device_value = evaluate_on_device(kernel, input);

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
    const double tolerance = precision == CodePrinterPrecision::Half
                                 ? kHalfRelativeTolerance
                                 : kFloatRelativeTolerance;
    REQUIRE(std::fabs(device_value - host_value) <= tolerance * scale);
}

} // namespace

TEST_CASE("Metal code matches Lambda visitor", "[metal][metalcode]")
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
    MTL::Device *device = MTL::CreateSystemDefaultDevice();
    REQUIRE(device != nullptr);

    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");
    auto sign_expr = sign(add(x, mul(integer(-1), z)));

    vec_basic symbols = {x, y, z};

    auto arithmetic = add(add(add(add(x, mul(x, y)), pow(x, y)), cbrt(x)),
                          sqrt(integer(2)));
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

    std::vector<RuntimeCase> cases = {
        {"arithmetic", arithmetic},
        {"integer base power", pow(integer(2), x)},
        {"abs", SymEngine::abs(x)},
        {"sin", SymEngine::sin(x)},
        {"atan2", SymEngine::atan2(x, z)},
        {"exp", SymEngine::exp(y)},
        {"log", SymEngine::log(add(x, integer(2)))},
        {"max", max({x, y, z})},
        {"min", SymEngine::min({x, y, z})},
        {"piecewise", piecewise_expr},
        {"constant E", E},
        {"constant pi", pi},
        {"positive infinity", Inf},
        {"nan", Nan},
        {"booleans and relationals", relational_expr},
        {"logical operators", logical_expr},
        {"sign", sign_expr},
        {"unevaluated expr", unevaluated_expr(add(x, y))},
    };

    const std::vector<double> x_values = {1.2, 3.0, 7.0};
    const std::vector<double> y_values = {0.2, 0.5, 0.8};
    const std::vector<double> z_values = {1.2, 3.0, 7.0};
    for (const auto precision :
         {CodePrinterPrecision::Float, CodePrinterPrecision::Half}) {
        for (const auto &runtime_case : cases) {
            const char *precision_name
                = precision == CodePrinterPrecision::Float ? "float" : "half";
            INFO(runtime_case.name);
            INFO(precision_name);
            DeviceKernel kernel = compile_kernel(
                device, metalcode(*runtime_case.expr, precision), precision);
            for (double x_value : x_values) {
                for (double y_value : y_values) {
                    for (double z_value : z_values) {
                        require_codegen_match(*runtime_case.expr, symbols,
                                              kernel, precision,
                                              {x_value, y_value, z_value});
                    }
                }
            }
            unload_kernel(kernel);
        }
    }

    pool->release();
}
