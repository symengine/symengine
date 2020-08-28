#define NONIUS_RUNNER
#include "nonius.h++"

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/lambda_double.h>

using SymEngine::Basic;
using SymEngine::symbol;
using SymEngine::integer;
using SymEngine::LambdaRealDoubleVisitor;
using SymEngine::RCP;

LambdaRealDoubleVisitor get_real_double_visitor(bool cse)
{
    LambdaRealDoubleVisitor v;
    RCP<const Basic> x, y, z, r;
    x = symbol("x");
    y = symbol("y");
    z = symbol("z");
    r = sin(add(x, cos(add(mul(y, z), pow(x, integer(2))))));
    r = mul(add(integer(3), r), add(integer(2), r));
    r = pow(add(integer(5), r), add(integer(-2), r));
    v.init({x, y, z}, *r, cse);
    return v;
}

NONIUS_BENCHMARK("lambda_real_double_visitor_call",
                 [](nonius::chronometer meter) {
                     double d;
                     auto v = get_real_double_visitor(false);
                     meter.measure([&](int i) {
                         d = v.call({1.0 + i, 4.4365 + 2 * i, 12.8 + 3 * i});
                     });
                 })

NONIUS_BENCHMARK("lambda_real_double_visitor_cse_call",
                 [](nonius::chronometer meter) {
                     double d;
                     auto v = get_real_double_visitor(true);
                     meter.measure([&](int i) {
                         d = v.call({1.0 + i, 4.4365 + 2 * i, 12.8 + 3 * i});
                     });
                 })
