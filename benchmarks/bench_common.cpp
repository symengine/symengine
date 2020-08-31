#ifndef SYMENGINE_BENCH_EXPRESSIONS_H
#define SYMENGINE_BENCH_EXPRESSIONS_H

#include "bench_common.h"
#include <cmath>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>

using SymEngine::add;
using SymEngine::Basic;
using SymEngine::cos;
using SymEngine::integer;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::RCP;
using SymEngine::sin;
using SymEngine::symbol;
using SymEngine::vec_basic;

vec_basic get_vec()
{
    return {symbol("x"), symbol("y"), symbol("z")};
}

vec_basic get_expression_1(const vec_basic &v)
{
    vec_basic r{symbol("r0")};
    r[0] = sin(add(v[0], cos(add(mul(v[1], v[2]), pow(v[0], integer(2))))));
    r[0] = mul(add(integer(3), r[0]), add(integer(2), r[0]));
    r[0] = pow(add(integer(5), r[0]), add(integer(-2), r[0]));
    return r;
}

void call_compiled_expression_1(double *d, const double *v)
{
    double r1 = std::sin(v[0] + std::cos((v[1] * v[2]) + std::pow(v[0], 2)));
    double r2 = (3 + r1) * (2 + r1);
    *d = std::pow((5 + r2), (r2 - 2));
}

vec_basic get_expression_2(const vec_basic &v)
{
    vec_basic r{symbol("r0"), symbol("r1"), symbol("r2")};
    r[0] = mul(integer(2), add(v[0], add(v[0], mul(v[1], v[2]))));
    r[1] = add(v[0], add(v[0], mul(v[2], v[1])));
    r[2] = mul(integer(-2), add(v[0], add(v[0], mul(v[1], v[2]))));
    return r;
}

void call_compiled_expression_2(double *d, const double *v)
{
    d[0] = 2.0 * (v[0] + v[0] + (v[1] * v[2]));
    d[1] = v[0] + v[0] + (v[2] * v[1]);
    d[2] = -2.0 * (v[0] + v[0] + (v[1] * v[2]));
}

#endif
