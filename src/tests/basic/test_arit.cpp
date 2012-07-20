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
    RCP<Basic> x  = rcp(new Symbol("x"));
    RCP<Basic> y  = rcp(new Symbol("y"));

    RCP<Basic> r = (x + y) + (y + x);
    std::cout << *r << std::endl;

    r = x + x;
    std::cout << *r << std::endl;

    r = x + x + y;
    std::cout << *r << std::endl;
}


int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    test_add();

    return 0;
}
