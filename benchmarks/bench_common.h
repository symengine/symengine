#ifndef SYMENGINE_BENCH_EXPRESSIONS_H
#define SYMENGINE_BENCH_EXPRESSIONS_H

#include <symengine/basic.h>
#include <symengine/dict.h>

const std::size_t call_iterations{1000000};

SymEngine::vec_basic get_vec();

SymEngine::vec_basic get_expression_1(const SymEngine::vec_basic &v);

void call_compiled_expression_1(double *d, const double *v);

SymEngine::vec_basic get_expression_2(const SymEngine::vec_basic &v);

void call_compiled_expression_2(double *d, const double *v);

#endif
