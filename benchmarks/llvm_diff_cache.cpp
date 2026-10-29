#include <iostream>
#include <chrono>
#include <iomanip>

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/llvm_double.h>
#include <symengine/eval_mpfr.h>
using SymEngine::LLVMDoubleVisitor;

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::add;
using SymEngine::symbol;
using SymEngine::integer;
using SymEngine::log;
using SymEngine::sin;
using SymEngine::pow;
using SymEngine::sqrt;
using SymEngine::Symbol;
using SymEngine::rcp_static_cast;
using SymEngine::free_symbols;
using SymEngine::DenseMatrix;

double CommonSubexprLLVM()
{
    RCP<const Basic> e;

    std::string tmp_str = "a";
    std::vector<RCP<const Basic>> v;
    for (int i = 0; i < 1000; ++i) {
        v.push_back(symbol(tmp_str));
        tmp_str += "a";
    }

    e = integer(23);
    for (unsigned int i = 0; i < v.size(); ++i) {
        RCP<const Basic> z = symbol(tmp_str);
        e = pow(e, add(cos(sqrt(log(sin(pow(v[v.size() - i - 1], v[i]))))), e));
    }
    e = expand(e);

    DenseMatrix M, J, S;
    M = DenseMatrix(1, 1, {e});
    S = DenseMatrix(10, 1, v);
    J = DenseMatrix(1, 10);

    auto t1 = std::chrono::high_resolution_clock::now();
    jacobian(M, S, J);
    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout
        << "Time for jacobian with common subexpressions (with cache) : \t "
        << std::setw(15) << std::setprecision(9) << std::fixed
        << std::chrono::duration<double>(t2 - t1).count() << std::endl;

    t1 = std::chrono::high_resolution_clock::now();

    LLVMDoubleVisitor v3;
    bool symbolic_cse = true;
    int opt_level = 3;
    v3.init(v,
            {J.get(0, 0), J.get(0, 1), J.get(0, 2), J.get(0, 3), J.get(0, 4),
             J.get(0, 5), J.get(0, 6), J.get(0, 7), J.get(0, 8), J.get(0, 9)},
            symbolic_cse, opt_level);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout
        << "Time for llvm init with common subexpressions (with cache) : \t "
        << std::setw(15) << std::setprecision(9) << std::fixed
        << std::chrono::duration<double>(t2 - t1).count() << std::endl;
    return 0;
}

double NoCommonSubexprLLVM()
{
    RCP<const Basic> e;

    std::string tmp_str = "a";
    std::vector<RCP<const Basic>> v;
    for (int i = 0; i < 2000; ++i) {
        v.push_back(symbol(tmp_str));
        tmp_str += "a";
    }

    e = integer(23);
    for (unsigned int i = 0; i < v.size(); ++i) {
        RCP<const Basic> z = symbol(tmp_str);
        e = pow(e, cos(sqrt(log(sin(pow(v[v.size() - i - 1], v[i]))))));
    }
    e = expand(e);

    DenseMatrix M, J, S;
    M = DenseMatrix(1, 1, {e});
    S = DenseMatrix(10, 1, v);
    J = DenseMatrix(1, 10);

    auto t1 = std::chrono::high_resolution_clock::now();
    jacobian(M, S, J, false);
    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout << "Time for jacobian without common subexpressions (without "
                 "cache) : \t "
              << std::setw(15) << std::setprecision(9) << std::fixed
              << std::chrono::duration<double>(t2 - t1).count() << std::endl;

    t1 = std::chrono::high_resolution_clock::now();

    LLVMDoubleVisitor v3;
    bool symbolic_cse = true;
    int opt_level = 3;
    v3.init(v,
            {J.get(0, 0), J.get(0, 1), J.get(0, 2), J.get(0, 3), J.get(0, 4),
             J.get(0, 5), J.get(0, 6), J.get(0, 7), J.get(0, 8), J.get(0, 9)},
            symbolic_cse, opt_level);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Time for llvm init without common subexpressions (without "
                 "cache) : \t "
              << std::setw(15) << std::setprecision(9) << std::fixed
              << std::chrono::duration<double>(t2 - t1).count() << std::endl;
    return 0;
}

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();
    CommonSubexprLLVM();
    NoCommonSubexprLLVM();
    return 0;
}
