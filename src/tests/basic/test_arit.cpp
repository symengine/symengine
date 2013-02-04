#include <iostream>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "mul.h"

using Teuchos::RCP;
using Teuchos::rcp;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Symbol;
using CSymPy::Dict_int;
using CSymPy::Integer;

void test_add()
{
    Dict_int m;
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> i = rcp(new Integer(2));

    RCP<Basic> r1 = x + x;
    RCP<Basic> r2 = i*x;
    std::cout << r1 << std::endl;
    std::cout << r2 << std::endl;
    // FIXME: This fails so far:
    //assert(r1 == r2);
}


int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    test_add();

    return 0;
}
