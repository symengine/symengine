#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"

using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Pow;
using CSymPy::Symbol;
using CSymPy::symbol;
using CSymPy::umap_basic_int;
using CSymPy::map_vec_int;
using CSymPy::Integer;
using CSymPy::integer;
using CSymPy::multinomial_coefficients;
using CSymPy::one;
using CSymPy::zero;
using CSymPy::sin;

void test_sin()
{
    RCP<Basic> x = symbol("x");
    RCP<Basic> y = symbol("x");
    RCP<Basic> z = symbol("x");
    RCP<Basic> im1 = integer(-1);
    RCP<Basic> i2 = integer(2);

    RCP<Basic> r1;
    RCP<Basic> r2;

    r1 = sin(x);
    r2 = sin(x);
    std::cout << *r1 << std::endl;

    assert(eq(r1, r2));
    assert(neq(r1, zero));

    r1 = sin(zero);
    r2 = zero;
    assert(eq(r1, r2));
}


int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    test_sin();

    return 0;
}
