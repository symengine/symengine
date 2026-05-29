#include "catch.hpp"

#include <symengine/logic.h>
#include <symengine/add.h>
#include <symengine/printers/codegen.h>
#include <symengine/sets.h>

using SymEngine::abs;
using SymEngine::acos;
using SymEngine::acosh;
using SymEngine::acoth;
using SymEngine::add;
using SymEngine::asin;
using SymEngine::asinh;
using SymEngine::atan;
using SymEngine::atan2;
using SymEngine::atanh;
using SymEngine::Basic;
using SymEngine::boolFalse;
using SymEngine::boolTrue;
using SymEngine::C89CodePrinter;
using SymEngine::C99CodePrinter;
using SymEngine::cbrt;
using SymEngine::ccode;
using SymEngine::ceiling;
using SymEngine::CodePrinterPrecision;
using SymEngine::cos;
using SymEngine::cosh;
using SymEngine::cot;
using SymEngine::coth;
using SymEngine::cudacode;
using SymEngine::CudaCodePrinter;
using SymEngine::dummy;
using SymEngine::E;
using SymEngine::Eq;
using SymEngine::erf;
using SymEngine::erfc;
using SymEngine::exp;
using SymEngine::floor;
using SymEngine::gamma;
using SymEngine::Ge;
using SymEngine::Gt;
using SymEngine::Inf;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Interval;
using SymEngine::interval;
using SymEngine::jscode;
using SymEngine::JSCodePrinter;
using SymEngine::Le;
using SymEngine::log;
using SymEngine::loggamma;
using SymEngine::logical_not;
using SymEngine::logical_xor;
using SymEngine::Lt;
using SymEngine::max;
using SymEngine::metalcode;
using SymEngine::MetalCodePrinter;
using SymEngine::min;
using SymEngine::Nan;
using SymEngine::Ne;
using SymEngine::NegInf;
using SymEngine::pi;
using SymEngine::piecewise;
using SymEngine::rational;
using SymEngine::sign;
using SymEngine::sin;
using SymEngine::sinh;
using SymEngine::sqrt;
using SymEngine::symbol;
using SymEngine::tan;
using SymEngine::tanh;
using SymEngine::truncate;
using SymEngine::unevaluated_expr;

TEST_CASE("C-code printers", "[CodePrinter]")
{
    C89CodePrinter c89;
    C89CodePrinter c89_float(CodePrinterPrecision::Float);
    C99CodePrinter c99;
    C99CodePrinter c99_float(CodePrinterPrecision::Float);
    REQUIRE(c89.apply(Inf) == "HUGE_VAL");
    REQUIRE(c89_float.apply(Inf) == "HUGE_VAL");
    REQUIRE(c99.apply(Inf) == "INFINITY");
    REQUIRE(c99_float.apply(Inf) == "INFINITY");
    REQUIRE(c89.apply(E) == "exp(1)");
    REQUIRE(c89_float.apply(E) == "expf(1.0f)");
    REQUIRE(c99.apply(E) == "exp(1)");
    REQUIRE(c99_float.apply(E) == "expf(1.0f)");
    REQUIRE(c89.apply(pi) == "acos(-1)");
    REQUIRE(c89_float.apply(pi) == "acosf(-1.0f)");
    REQUIRE(c99.apply(pi) == "acos(-1)");
    REQUIRE(c99_float.apply(pi) == "acosf(-1.0f)");
}

TEST_CASE("CUDA-code printers", "[CudaCodePrinter]")
{
    CudaCodePrinter cuda;
    CudaCodePrinter cuda_float(CodePrinterPrecision::Float);
    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");
    auto p = add(add(add(add(x, mul(x, y)), pow(x, y)), cbrt(x)),
                 sqrt(integer(2)));

    REQUIRE(cuda.apply(Inf) == "CUDART_INF");
    REQUIRE(cuda.apply(E) == "exp(1.0)");
    REQUIRE(cuda.apply(pi) == "acos(-1.0)");
    REQUIRE(cuda.apply(Nan) == "CUDART_NAN");
    REQUIRE(cuda_float.apply(Inf) == "CUDART_INF_F");
    REQUIRE(cuda_float.apply(E) == "expf(1.0f)");
    REQUIRE(cuda_float.apply(pi) == "acosf(-1.0f)");
    REQUIRE(cuda_float.apply(Nan) == "CUDART_NAN_F");
}

TEST_CASE("Metal-code printers", "[MetalCodePrinter]")
{
    MetalCodePrinter metal;
    MetalCodePrinter metal_half(CodePrinterPrecision::Half);

    REQUIRE(metal.apply(Inf) == "INFINITY");
    REQUIRE(metal.apply(E) == "exp(1.0f)");
    REQUIRE(metal.apply(pi) == "acos(-1.0f)");
    REQUIRE(metal.apply(Nan) == "NAN");
    REQUIRE(metal_half.apply(Inf) == "HUGE_VALH");
    REQUIRE(metal_half.apply(E) == "exp(1.0h)");
    REQUIRE(metal_half.apply(pi) == "acos(-1.0h)");
    REQUIRE(metal_half.apply(Nan) == "half(NAN)");
    REQUIRE_THROWS_AS(MetalCodePrinter(CodePrinterPrecision::Double),
                      SymEngine::SymEngineException);
}

TEST_CASE("Metal code generation", "[metalcode]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");
    auto a = symbol("a");
    auto b = symbol("b");
    auto c = symbol("c");
    auto piecewise_expr = piecewise(
        {{x, contains(x, interval(NegInf, integer(2), true, false))},
         {y, contains(x, interval(integer(2), integer(5), true, false))},
         {add(x, y), boolTrue}});

    REQUIRE(metalcode(*boolTrue) == "1.0f");
    REQUIRE(metalcode(*add(add(add(add(x, mul(x, y)), mul(x, x)), pow(x, y)),
                           sqrt(integer(2))))
            == "x + x*y + sqrt(2.0f) + pow(x, 2.0f) + pow(x, y)");
    REQUIRE(metalcode(*rational(1, 3)) == "1.0f/3.0f");
    REQUIRE(metalcode(*boolTrue, CodePrinterPrecision::Half) == "1.0h");
    REQUIRE(metalcode(*add(add(add(add(x, mul(x, y)), mul(x, x)), pow(x, y)),
                           sqrt(integer(2))),
                      CodePrinterPrecision::Half)
            == "x + x*y + sqrt(2.0h) + pow(x, 2.0h) + pow(x, y)");
    REQUIRE(metalcode(*rational(1, 3), CodePrinterPrecision::Half)
            == "1.0h/3.0h");
    REQUIRE(metalcode(*function_symbol("f", pow(integer(2), x)))
            == "f(pow(2.0f, x))");
    REQUIRE(metalcode(*function_symbol("f", pow(integer(2), x)),
                      CodePrinterPrecision::Half)
            == "f(pow(2.0h, x))");
    REQUIRE(metalcode(*abs(x)) == "fabs(x)");
    REQUIRE(metalcode(*sin(x)) == "sin(x)");
    REQUIRE(metalcode(*atan2(x, y)) == "atan2(x, y)");
    REQUIRE(metalcode(*max({x, y, z})) == "fmax(x, fmax(y, z))");
    REQUIRE(metalcode(*min({x, y, z})) == "fmin(x, fmin(y, z))");
    REQUIRE(metalcode(*max({a, b, c, x, y, z}))
            == "fmax(fmax(a, fmax(b, c)), fmax(x, fmax(y, z)))");
    REQUIRE(metalcode(*min({a, b, c, x, y, z}))
            == "fmin(fmin(a, fmin(b, c)), fmin(x, fmin(y, z)))");
    REQUIRE(metalcode(*piecewise_expr)
            == "((x <= 2.0f) ? (\n   x\n)\n: ((x > 2.0f && x <= 5.0f) ? (\n   "
               "y\n)\n: (\n   x + y\n)))");
    REQUIRE_THROWS_AS(metalcode(*x, CodePrinterPrecision::Double),
                      SymEngine::SymEngineException);
    REQUIRE_THROWS_AS(ccode(*x, CodePrinterPrecision::Half),
                      SymEngine::SymEngineException);
    REQUIRE_THROWS_AS(cudacode(*x, CodePrinterPrecision::Half),
                      SymEngine::SymEngineException);
}

TEST_CASE("Codegen boolean support", "[ccode][cudacode]")
{
    auto x = symbol("x");
    auto y = symbol("y");

    REQUIRE(ccode(*boolTrue) == "1.0");
    REQUIRE(ccode(*boolTrue, CodePrinterPrecision::Float) == "1.0f");
    REQUIRE(cudacode(*boolTrue) == "1.0");
    REQUIRE(cudacode(*boolTrue, CodePrinterPrecision::Float) == "1.0f");

    auto xor_expr = logical_xor({Lt(x, integer(2)), Le(y, x)});
    REQUIRE(ccode(*xor_expr) == "(((x < 2) != 0) != ((y <= x) != 0))");
    REQUIRE(ccode(*xor_expr, CodePrinterPrecision::Float)
            == "(((x < 2.0f) != 0) != ((y <= x) != 0))");
    REQUIRE(cudacode(*xor_expr) == "(((x < 2.0) != 0) != ((y <= x) != 0))");
    REQUIRE(cudacode(*xor_expr, CodePrinterPrecision::Float)
            == "(((x < 2.0f) != 0) != ((y <= x) != 0))");

    auto not_expr = logical_not(xor_expr);
    REQUIRE(ccode(*not_expr) == "!((((x < 2) != 0) != ((y <= x) != 0)))");
    REQUIRE(ccode(*not_expr, CodePrinterPrecision::Float)
            == "!((((x < 2.0f) != 0) != ((y <= x) != 0)))");
    REQUIRE(cudacode(*not_expr) == "!((((x < 2.0) != 0) != ((y <= x) != 0)))");
    REQUIRE(cudacode(*not_expr, CodePrinterPrecision::Float)
            == "!((((x < 2.0f) != 0) != ((y <= x) != 0)))");

    auto sign_expr = sign(x);
    REQUIRE(ccode(*sign_expr)
            == "((x == 0.0) ? (0.0) : ((x < 0.0) ? (-1.0) : (1.0)))");
    REQUIRE(ccode(*sign_expr, CodePrinterPrecision::Float)
            == "((x == 0.0f) ? (0.0f) : ((x < 0.0f) ? (-1.0f) : (1.0f)))");
    REQUIRE(cudacode(*sign_expr)
            == "((x == 0.0) ? (0.0) : ((x < 0.0) ? (-1.0) : (1.0)))");
    REQUIRE(cudacode(*sign_expr, CodePrinterPrecision::Float)
            == "((x == 0.0f) ? (0.0f) : ((x < 0.0f) ? (-1.0f) : (1.0f)))");

    auto uneval_expr = unevaluated_expr(add(x, y));
    REQUIRE(ccode(*uneval_expr) == "x + y");
    REQUIRE(ccode(*uneval_expr, CodePrinterPrecision::Float) == "x + y");
    REQUIRE(cudacode(*uneval_expr) == "x + y");
    REQUIRE(cudacode(*uneval_expr, CodePrinterPrecision::Float) == "x + y");
}

TEST_CASE("Dummy", "[CodePrinter]")
{
    C89CodePrinter c89;
    auto foo1 = dummy("foo");
    auto foo2 = dummy("foo");
    REQUIRE(c89.apply(foo1) != c89.apply(foo2));
}
TEST_CASE("Arithmetic", "[ccode][cudacode]")
{
    auto a = symbol("a");
    auto b = symbol("b");
    auto c = symbol("c");
    auto x = symbol("x");
    auto y = symbol("y");
    auto p = add(add(add(add(x, mul(x, y)), pow(x, y)), mul(x, x)),
                 sqrt(integer(2)));
    REQUIRE(ccode(*p) == "x + x*y + sqrt(2) + pow(x, 2) + pow(x, y)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float)
            == "x + x*y + sqrtf(2.0f) + powf(x, 2.0f) + powf(x, y)");
    REQUIRE(cudacode(*p) == "x + x*y + sqrt(2.0) + pow(x, 2.0) + pow(x, y)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float)
            == "x + x*y + sqrtf(2.0f) + powf(x, 2.0f) + powf(x, y)");
}

TEST_CASE("Rational", "[ccode][cudacode]")
{
    auto p = rational(1, 3);
    REQUIRE(ccode(*p) == "1.0/3.0");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "1.0f/3.0f");
    REQUIRE(cudacode(*p) == "1.0/3.0");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "1.0f/3.0f");

    p = rational(1, -3);
    REQUIRE(ccode(*p) == "-1.0/3.0");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "-1.0f/3.0f");
    REQUIRE(cudacode(*p) == "-1.0/3.0");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "-1.0f/3.0f");
}

TEST_CASE("Functions", "[ccode][cudacode][metalcode]")
{
    auto a = symbol("a");
    auto b = symbol("b");
    auto c = symbol("c");
    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");
    auto p = function_symbol("f", x);

    REQUIRE(ccode(*p) == "f(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "f(x)");
    REQUIRE(cudacode(*p) == "f(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "f(x)");

    p = function_symbol("f", pow(integer(2), x));
    REQUIRE(ccode(*p) == "f(pow(2, x))");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "f(powf(2.0f, x))");
    REQUIRE(cudacode(*p) == "f(pow(2.0, x))");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "f(powf(2.0f, x))");

    p = abs(x);
    REQUIRE(ccode(*p) == "fabs(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "fabsf(x)");
    REQUIRE(cudacode(*p) == "fabs(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "fabsf(x)");
    p = sin(x);
    REQUIRE(ccode(*p) == "sin(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "sinf(x)");
    REQUIRE(cudacode(*p) == "sin(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "sinf(x)");
    p = cos(x);
    REQUIRE(ccode(*p) == "cos(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "cosf(x)");
    REQUIRE(cudacode(*p) == "cos(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "cosf(x)");
    p = tan(x);
    REQUIRE(ccode(*p) == "tan(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "tanf(x)");
    REQUIRE(cudacode(*p) == "tan(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "tanf(x)");
    p = cot(x);
    REQUIRE(ccode(*p) == "1/tan(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "1.0f/tanf(x)");
    REQUIRE(cudacode(*p) == "1.0/tan(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "1.0f/tanf(x)");
    REQUIRE(metalcode(*p) == "1.0f/tan(x)");
    p = csc(x);
    REQUIRE(ccode(*p) == "1/sin(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "1.0f/sinf(x)");
    REQUIRE(cudacode(*p) == "1.0/sin(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "1.0f/sinf(x)");
    REQUIRE(metalcode(*p) == "1.0f/sin(x)");
    p = sec(x);
    REQUIRE(ccode(*p) == "1/cos(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "1.0f/cosf(x)");
    REQUIRE(cudacode(*p) == "1.0/cos(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "1.0f/cosf(x)");
    REQUIRE(metalcode(*p) == "1.0f/cos(x)");
    p = atan2(x, y);
    REQUIRE(ccode(*p) == "atan2(x, y)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "atan2f(x, y)");
    REQUIRE(cudacode(*p) == "atan2(x, y)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "atan2f(x, y)");
    p = acot(x);
    REQUIRE(ccode(*p) == "atan(1/x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "atanf(1.0f/x)");
    REQUIRE(cudacode(*p) == "atan(1.0/x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "atanf(1.0f/x)");
    REQUIRE(metalcode(*p) == "atan(1.0f/x)");
    p = acsc(x);
    REQUIRE(ccode(*p) == "asin(1/x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "asinf(1.0f/x)");
    REQUIRE(cudacode(*p) == "asin(1.0/x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "asinf(1.0f/x)");
    REQUIRE(metalcode(*p) == "asin(1.0f/x)");
    p = asec(x);
    REQUIRE(ccode(*p) == "acos(1/x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "acosf(1.0f/x)");
    REQUIRE(cudacode(*p) == "acos(1.0/x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "acosf(1.0f/x)");
    REQUIRE(metalcode(*p) == "acos(1.0f/x)");
    p = exp(x);
    REQUIRE(ccode(*p) == "exp(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "expf(x)");
    REQUIRE(cudacode(*p) == "exp(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "expf(x)");
    p = log(x);
    REQUIRE(ccode(*p) == "log(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "logf(x)");
    REQUIRE(cudacode(*p) == "log(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "logf(x)");
    p = sinh(x);
    REQUIRE(ccode(*p) == "sinh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "sinhf(x)");
    REQUIRE(cudacode(*p) == "sinh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "sinhf(x)");
    p = cosh(x);
    REQUIRE(ccode(*p) == "cosh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "coshf(x)");
    REQUIRE(cudacode(*p) == "cosh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "coshf(x)");
    p = tanh(x);
    REQUIRE(ccode(*p) == "tanh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "tanhf(x)");
    REQUIRE(cudacode(*p) == "tanh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "tanhf(x)");
    p = coth(x);
    REQUIRE(ccode(*p) == "1/tanh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "1.0f/tanhf(x)");
    REQUIRE(cudacode(*p) == "1.0/tanh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "1.0f/tanhf(x)");
    REQUIRE(metalcode(*p) == "1.0f/tanh(x)");
    p = csch(x);
    REQUIRE(ccode(*p) == "1/sinh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "1.0f/sinhf(x)");
    REQUIRE(cudacode(*p) == "1.0/sinh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "1.0f/sinhf(x)");
    REQUIRE(metalcode(*p) == "1.0f/sinh(x)");
    p = sech(x);
    REQUIRE(ccode(*p) == "1/cosh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "1.0f/coshf(x)");
    REQUIRE(cudacode(*p) == "1.0/cosh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "1.0f/coshf(x)");
    REQUIRE(metalcode(*p) == "1.0f/cosh(x)");
    p = asinh(x);
    REQUIRE(ccode(*p) == "asinh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "asinhf(x)");
    REQUIRE(cudacode(*p) == "asinh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "asinhf(x)");
    p = acosh(x);
    REQUIRE(ccode(*p) == "acosh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "acoshf(x)");
    REQUIRE(cudacode(*p) == "acosh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "acoshf(x)");
    p = atanh(x);
    REQUIRE(ccode(*p) == "atanh(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "atanhf(x)");
    REQUIRE(cudacode(*p) == "atanh(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "atanhf(x)");
    p = acoth(x);
    REQUIRE(ccode(*p) == "atanh(1/x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "atanhf(1.0f/x)");
    REQUIRE(cudacode(*p) == "atanh(1.0/x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "atanhf(1.0f/x)");
    REQUIRE(metalcode(*p) == "atanh(1.0f/x)");
    p = acsch(x);
    REQUIRE(ccode(*p) == "asinh(1/x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "asinhf(1.0f/x)");
    REQUIRE(cudacode(*p) == "asinh(1.0/x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "asinhf(1.0f/x)");
    REQUIRE(metalcode(*p) == "asinh(1.0f/x)");
    p = asech(x);
    REQUIRE(ccode(*p) == "acosh(1/x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "acoshf(1.0f/x)");
    REQUIRE(cudacode(*p) == "acosh(1.0/x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "acoshf(1.0f/x)");
    REQUIRE(metalcode(*p) == "acosh(1.0f/x)");
    p = floor(x);
    REQUIRE(ccode(*p) == "floor(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "floorf(x)");
    REQUIRE(cudacode(*p) == "floor(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "floorf(x)");
    p = ceiling(x);
    REQUIRE(ccode(*p) == "ceil(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "ceilf(x)");
    REQUIRE(cudacode(*p) == "ceil(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "ceilf(x)");
    p = truncate(x);
    REQUIRE(ccode(*p) == "trunc(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "truncf(x)");
    REQUIRE(cudacode(*p) == "trunc(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "truncf(x)");
    p = erf(x);
    REQUIRE(ccode(*p) == "erf(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "erff(x)");
    REQUIRE(cudacode(*p) == "erf(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "erff(x)");
    p = erfc(x);
    REQUIRE(ccode(*p) == "erfc(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "erfcf(x)");
    REQUIRE(cudacode(*p) == "erfc(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "erfcf(x)");
    p = gamma(x);
    REQUIRE(ccode(*p) == "tgamma(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "tgammaf(x)");
    REQUIRE(cudacode(*p) == "tgamma(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "tgammaf(x)");
    p = loggamma(x);
    REQUIRE(ccode(*p) == "lgamma(x)");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "lgammaf(x)");
    REQUIRE(cudacode(*p) == "lgamma(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "lgammaf(x)");
    p = max({x, y, z});
    REQUIRE(ccode(*p) == "fmax(x, fmax(y, z))");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "fmaxf(x, fmaxf(y, z))");
    REQUIRE(cudacode(*p) == "fmax(x, fmax(y, z))");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float)
            == "fmaxf(x, fmaxf(y, z))");
    p = max({a, b, c, x, y, z});
    REQUIRE(ccode(*p) == "fmax(fmax(a, fmax(b, c)), fmax(x, fmax(y, z)))");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float)
            == "fmaxf(fmaxf(a, fmaxf(b, c)), fmaxf(x, fmaxf(y, z)))");
    REQUIRE(cudacode(*p) == "fmax(fmax(a, fmax(b, c)), fmax(x, fmax(y, z)))");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float)
            == "fmaxf(fmaxf(a, fmaxf(b, c)), fmaxf(x, fmaxf(y, z)))");
    p = min({x, y, z});
    REQUIRE(ccode(*p) == "fmin(x, fmin(y, z))");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "fminf(x, fminf(y, z))");
    REQUIRE(cudacode(*p) == "fmin(x, fmin(y, z))");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float)
            == "fminf(x, fminf(y, z))");
    p = min({a, b, c, x, y, z});
    REQUIRE(ccode(*p) == "fmin(fmin(a, fmin(b, c)), fmin(x, fmin(y, z)))");
    REQUIRE(ccode(*p, CodePrinterPrecision::Float)
            == "fminf(fminf(a, fminf(b, c)), fminf(x, fminf(y, z)))");
    REQUIRE(cudacode(*p) == "fmin(fmin(a, fmin(b, c)), fmin(x, fmin(y, z)))");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float)
            == "fminf(fminf(a, fminf(b, c)), fminf(x, fminf(y, z)))");
}

TEST_CASE("Configurable precision", "[ccode][cudacode]")
{
    auto x = symbol("x");
    auto p = sin(x);

    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "sinf(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "sinf(x)");

    p = abs(x);
    REQUIRE(ccode(*p, CodePrinterPrecision::Float) == "fabsf(x)");
    REQUIRE(cudacode(*p, CodePrinterPrecision::Float) == "fabsf(x)");

    auto q = pow(integer(2), x);
    REQUIRE(ccode(*q, CodePrinterPrecision::Float) == "powf(2.0f, x)");
    REQUIRE(cudacode(*q, CodePrinterPrecision::Float) == "powf(2.0f, x)");
}

TEST_CASE("Relationals", "[ccode][cudacode]")
{
    auto x = symbol("x");
    auto y = symbol("y");

    auto p = Eq(x, y);
    CHECK(ccode(*p) == "x == y");
    CHECK(ccode(*p, CodePrinterPrecision::Float) == "x == y");
    CHECK(cudacode(*p) == "x == y");
    CHECK(cudacode(*p, CodePrinterPrecision::Float) == "x == y");

    p = Ne(x, y);
    CHECK(ccode(*p) == "x != y");
    CHECK(ccode(*p, CodePrinterPrecision::Float) == "x != y");
    CHECK(cudacode(*p) == "x != y");
    CHECK(cudacode(*p, CodePrinterPrecision::Float) == "x != y");

    p = Le(x, y);
    CHECK(ccode(*p) == "x <= y");
    CHECK(ccode(*p, CodePrinterPrecision::Float) == "x <= y");
    CHECK(cudacode(*p) == "x <= y");
    CHECK(cudacode(*p, CodePrinterPrecision::Float) == "x <= y");

    p = Lt(x, y);
    CHECK(ccode(*p) == "x < y");
    CHECK(ccode(*p, CodePrinterPrecision::Float) == "x < y");
    CHECK(cudacode(*p) == "x < y");
    CHECK(cudacode(*p, CodePrinterPrecision::Float) == "x < y");
}

TEST_CASE("Piecewise", "[ccode][cudacode]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto int1 = interval(NegInf, integer(2), true, false);
    auto int2 = interval(integer(2), integer(5), true, false);
    auto p = piecewise({{x, contains(x, int1)},
                        {y, contains(x, int2)},
                        {add(x, y), boolTrue}});

    REQUIRE(ccode(*p)
            == "((x <= 2) ? (\n   x\n)\n: ((x > 2 && x <= 5) ? (\n   "
               "y\n)\n: (\n   x + y\n)))");

    REQUIRE(ccode(*p, CodePrinterPrecision::Float)
            == "((x <= 2.0f) ? (\n   x\n)\n: ((x > 2.0f && x <= 5.0f) ? (\n   "
               "y\n)\n: (\n   x + y\n)))");

    REQUIRE(cudacode(*p)
            == "((x <= 2.0) ? (\n   x\n)\n: ((x > 2.0 && x <= 5.0) ? (\n   "
               "y\n)\n: (\n   x + y\n)))");

    REQUIRE(cudacode(*p, CodePrinterPrecision::Float)
            == "((x <= 2.0f) ? (\n   x\n)\n: ((x > 2.0f && x <= 5.0f) ? (\n   "
               "y\n)\n: (\n   x + y\n)))");
}

TEST_CASE("JavaScript math functions", "[jscode]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");
    auto p = function_symbol("f", x);

    REQUIRE(jscode(*p) == "f(x)");

    p = function_symbol("f", pow(integer(2), x));
    REQUIRE(jscode(*p) == "f(Math.pow(2, x))");

    p = sqrt(x);
    REQUIRE(jscode(*p) == "Math.sqrt(x)");
    p = cbrt(x);
    REQUIRE(jscode(*p) == "Math.cbrt(x)");
    p = abs(x);
    REQUIRE(jscode(*p) == "Math.abs(x)");
    p = sin(x);
    REQUIRE(jscode(*p) == "Math.sin(x)");
    p = cos(x);
    REQUIRE(jscode(*p) == "Math.cos(x)");
    p = cot(x);
    REQUIRE(jscode(*p) == "1/Math.tan(x)");
    p = asec(x);
    REQUIRE(jscode(*p) == "Math.acos(1/x)");
    p = sech(x);
    REQUIRE(jscode(*p) == "1/Math.cosh(x)");
    p = acoth(x);
    REQUIRE(jscode(*p) == "Math.atanh(1/x)");
    p = max({x, y, z});
    REQUIRE(jscode(*p) == "Math.max(x, y, z)");
    p = min({x, y, z});
    REQUIRE(jscode(*p) == "Math.min(x, y, z)");
    p = exp(x);
    REQUIRE(jscode(*p) == "Math.exp(x)");
    JSCodePrinter JS;
    REQUIRE(JS.apply(cot(x)) == "1/Math.tan(x)");
    REQUIRE(JS.apply(SymEngine::vec_basic{cot(x), acoth(x)})
            == "1/Math.tan(x), Math.atanh(1/x)");
    REQUIRE(JS.apply(pi) == "Math.PI");
}
