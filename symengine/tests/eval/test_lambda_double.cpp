#include "catch.hpp"
#include <chrono>

#include <symengine/lambda_double.h>
#include <symengine/symengine_exception.h>

#ifdef HAVE_SYMENGINE_LLVM
#include <symengine/llvm_double.h>
using SymEngine::LLVMDoubleVisitor;
#endif

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::real_double;
using SymEngine::symbol;
using SymEngine::add;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::integer;
using SymEngine::vec_basic;
using SymEngine::complex_double;
using SymEngine::LambdaRealDoubleVisitor;
using SymEngine::LambdaComplexDoubleVisitor;
using SymEngine::max;
using SymEngine::sin;
using SymEngine::cos;
using SymEngine::tan;
using SymEngine::cot;
using SymEngine::csc;
using SymEngine::sec;
using SymEngine::asin;
using SymEngine::acos;
using SymEngine::atan;
using SymEngine::acot;
using SymEngine::acsc;
using SymEngine::asec;
using SymEngine::sinh;
using SymEngine::cosh;
using SymEngine::tanh;
using SymEngine::coth;
using SymEngine::csch;
using SymEngine::sech;
using SymEngine::asinh;
using SymEngine::acosh;
using SymEngine::atanh;
using SymEngine::acoth;
using SymEngine::acsch;
using SymEngine::asech;
using SymEngine::atan2;
using SymEngine::log;
using SymEngine::E;
using SymEngine::Catalan;
using SymEngine::gamma;
using SymEngine::loggamma;
using SymEngine::min;
using SymEngine::Eq;
using SymEngine::Ne;
using SymEngine::Lt;
using SymEngine::Le;
using SymEngine::NotImplementedError;
using SymEngine::SymEngineException;

TEST_CASE("Evaluate to double", "[lambda_double]")
{
    RCP<const Basic> x, y, z, r;
    double d;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");

    r = add(x, add(mul(y, z), pow(x, integer(2))));

    LambdaRealDoubleVisitor v;
    v.init({x, y, z}, *r);

    d = v.call({1.5, 2.0, 3.0});
    REQUIRE(::fabs(d - 9.75) < 1e-12);

    d = v.call({1.5, -1.0, 2.0});
    REQUIRE(::fabs(d - 1.75) < 1e-12);

    r = max({x, add(mul(y, z), integer(3))});
    v.init({x, y, z}, *r);

    d = v.call({4.0, 1.0, 2.5});
    REQUIRE(::fabs(d - 5.5) < 1e-12);

    r = min({pow(x, y), add(mul(y, z), integer(3))});
    v.init({x, y, z}, *r);

    d = v.call({4.0, 2.0, 2.5});
    REQUIRE(::fabs(d - 8.0) < 1e-12);

    // Evaluating to double when there are complex doubles raise an exception
    CHECK_THROWS_AS(
        v.init({x}, *add(complex_double(std::complex<double>(1, 2)), x)),
        NotImplementedError);

    // Undefined symbols raise an exception
    CHECK_THROWS_AS(v.init({x}, *r), SymEngineException);
}

TEST_CASE("Evaluate double cse", "[lambda_double_cse]")
{
    RCP<const Basic> x, y, z, r, s;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");

    r = add(x, add(mul(y, z), pow(mul(y, z), integer(2))));
    s = add(mul(integer(2), x), add(mul(y, z), pow(mul(y, z), integer(2))));

    LambdaRealDoubleVisitor v;
    v.init({x, y, z}, {r, s}, true);

    double d[2];
    double inps[] = {1.5, 2.0, 3.0};
    v.call(d, inps);
    REQUIRE(::fabs(d[0] - 43.5) < 1e-12);
    REQUIRE(::fabs(d[1] - 45.0) < 1e-12);
}

TEST_CASE("Evaluate to std::complex<double>", "[lambda_complex_double]")
{
    RCP<const Basic> x, y, z, r;
    std::complex<double> d;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");

    r = add(x,
            add(mul(y, z), pow(x, complex_double(std::complex<double>(3, 4)))));

    LambdaComplexDoubleVisitor v;
    v.init({x, y, z}, *r);

    d = v.call({std::complex<double>(1.5, 1.0), std::complex<double>(2.5, 4.0),
                std::complex<double>(-8.3, 3.2)});
    REQUIRE(::fabs(d.real() + 32.360749607381) < 1e-12);
    REQUIRE(::fabs(d.imag() + 24.6630395370884) < 1e-12);

    v.init({x, y, z}, *add(x, add(mul(y, z), pow(x, integer(2)))));
    d = v.call({std::complex<double>(1.5, 0.0), std::complex<double>(-1.0, 0.0),
                std::complex<double>(2.0, 0.0)});
    REQUIRE(::fabs(d.real() - 1.75) < 1e-12);
    REQUIRE(::fabs(d.imag() - 0.0) < 1e-12);

    // Undefined symbols raise an exception
    CHECK_THROWS_AS(v.init({x}, *r), SymEngineException);
}

TEST_CASE("Evaluate functions", "[lambda_gamma]")
{
    RCP<const Basic> x, y, z, r;
    double d;
    x = symbol("x");

    LambdaRealDoubleVisitor v;

    std::vector<std::tuple<RCP<const Basic>, double, double>> testvec = {
        std::make_tuple(pow(E, cos(x)), 1.3, 1.30669209920819),
        std::make_tuple(add(sin(x), cos(x)), 1.2, 1.29439684044390),
        std::make_tuple(mul(tan(x), sec(x)), 2.2, 2.33444426269917),
        std::make_tuple(add(csc(x), cot(x)), 0.5, 3.91631736464594),
        std::make_tuple(asin(x), 0.5, 0.523598775598299),
        std::make_tuple(acos(x), 0.9, 0.451026811796262),
        std::make_tuple(add(atan(x), asec(x)), 1.1, 1.26268093282586),
        std::make_tuple(add(acot(x), acsc(x)), 3, 0.661587463850764),
        std::make_tuple(add(sinh(x), csch(x)), 2.2, 4.68146604193974),
        std::make_tuple(mul(tanh(x), cosh(x)), 0.2, 0.201336002541094),
        std::make_tuple(sech(x), 0.5, 0.886818883970074),
        std::make_tuple(coth(x), 0.9, 1.39606725303001),
        std::make_tuple(mul(asinh(x), acosh(x)), 1.2, 0.632303583495024),
        std::make_tuple(mul(acsch(x), asech(x)), 0.3, 3.59566705273267),
        std::make_tuple(acoth(x), 3.3, 0.312852949882206),
        std::make_tuple(atanh(x), 0.7, 0.867300527694053),
        std::make_tuple(Eq(real_double(2.0), x), 2.0, 1.000000000000),
        std::make_tuple(Eq(real_double(3.0), x), 2.0, 0.000000000000),
        std::make_tuple(Ne(real_double(2.0), x), 2.0, 0.000000000000),
        std::make_tuple(Ne(real_double(3.0), x), 2.0, 1.000000000000),
        std::make_tuple(Le(real_double(2.0), x), 2.0, 1.000000000000),
        std::make_tuple(Le(real_double(3.0), x), 2.0, 0.000000000000),
        std::make_tuple(Le(real_double(2.0), x), 2.0000000000001,
                        1.000000000000),
        std::make_tuple(Lt(real_double(3.0), x), 2.0, 0.000000000000),
        std::make_tuple(Lt(real_double(2.0), x), 2.0000000000001,
                        1.000000000000),
        std::make_tuple(atan2(x, add(x, integer(1))), 2.1, 0.595409875478733),
        std::make_tuple(log(x), 1.2, 0.182321556793955),
        std::make_tuple(gamma(x), 1.1, 0.9513507698668),
        std::make_tuple(add(Catalan, x), 1.1, 2.01596559417722),
        std::make_tuple(loggamma(x), 1.3, -0.10817480950786047),
        std::make_tuple(add(gamma(x), loggamma(x)), 1.1, 0.901478328607033459),
        std::make_tuple(abs(x), -1.112111321, 1.112111321),
        std::make_tuple(erf(x), 1.1, 0.88020506957408169),
        std::make_tuple(erfc(x), 2.1, 0.00297946665633298),
    };

    for (unsigned i = 0; i < testvec.size(); i++) {
        v.init({x}, *std::get<0>(testvec[i]));
        d = v.call({std::get<1>(testvec[i])});
        REQUIRE(::fabs(d - std::get<2>(testvec[i])) < 1e-12);
    }
}

#ifdef HAVE_SYMENGINE_LLVM

TEST_CASE("Check llvm and lambda are equal", "[llvm_double]")
{
    RCP<const Basic> x, y, z, r;
    double d, d2, d3;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");

    vec_basic vec = {log(x),   abs(x),      tan(x),   sinh(x), cosh(x), tanh(x),
                     asinh(y), acosh(y),    atanh(x), asin(x), acos(x), atan(x),
                     gamma(x), loggamma(x), erf(x),   erfc(x)};

    r = mul(add(sin(x), add(mul(pow(y, integer(4)), mul(z, integer(2))),
                            pow(sin(x), integer(2)))),
            add(vec));
    for (int i = 0; i < 4; ++i) {
        r = mul(add(pow(integer(2), E), add(r, pow(x, pow(E, cos(x))))), r);
    }

    // r = add(add(x, y), pow(add(x, y), integer(2)));

    LambdaRealDoubleVisitor v;
    v.init({x, y, z}, *r);

    LLVMDoubleVisitor v2;
    v2.init({x, y, z}, *r);

    LLVMDoubleVisitor v3;
    v3.init({x, y, z}, *r, true);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 500; i++) {
        d = v.call({0.4, 2.0, 3.0});
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 500; i++) {
        d2 = v2.call({0.4, 2.0, 3.0});
    }
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 500; i++) {
        d3 = v3.call({0.4, 2.0, 3.0});
    }
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;

    std::cout << d << " " << d2 << " " << d3 << std::endl;
    REQUIRE(::fabs((d - d2) / d) < 1e-12);
    REQUIRE(::fabs((d - d3) / d) < 1e-12);
}

TEST_CASE("Check llvm save and load", "[llvm_double]")
{
    RCP<const Basic> x, y, z, r;
    double d, d2, d3;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");

    r = add(sin(x), add(mul(pow(y, integer(4)), mul(z, integer(2))),
                        pow(sin(x), integer(2))));

    for (int i = 0; i < 4; ++i) {
        r = mul(add(pow(integer(2), E), add(r, pow(x, pow(E, cos(x))))), r);
    }

    LLVMDoubleVisitor v;
    auto t1 = std::chrono::high_resolution_clock::now();
    v.init({x, y, z}, *r);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Initializing "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;

    LLVMDoubleVisitor v2;

    t1 = std::chrono::high_resolution_clock::now();
    auto &s = v.dumps();
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Saving "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    v2.loads(s);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Loading "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;

    d = v.call({0.4, 2.0, 3.0});
    d2 = v2.call({0.4, 2.0, 3.0});
    REQUIRE(::fabs((d - d2) / d) < 1e-12);
}
#endif
