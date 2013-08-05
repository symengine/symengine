#include <iostream>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"

using Teuchos::RCP;
using Teuchos::rcp;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Pow;
using CSymPy::Symbol;
using CSymPy::Dict_int;
using CSymPy::Integer;

void test_add()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1 = x + x;
    RCP<Basic> r2 = i2*x;
    RCP<Basic> r3 = i3*x;
    assert(*r1 == *r2);
    assert(*r1 != *r3);

    r3 = i2*y;
    assert(*r1 != *r3);
    assert(*r2 != *r3);

    r1 = y*x+i2*x*y;
    r2 = i3*x*y;
    assert(*r1 == *r2);

    r1 = x + x + x;
    r2 = i3 * x;
    assert(*r1 == *r2);

    r1 = x + x + x;
    r2 = x * i3;
    assert(*r1 == *r2);
}

void test_pow()
{
    RCP<Basic> x = rcp(new Symbol("x"));
    RCP<Basic> y = rcp(new Symbol("y"));
    RCP<Basic> z = rcp(new Symbol("z"));
    RCP<Basic> i2 = rcp(new Integer(2));
    RCP<Basic> i3 = rcp(new Integer(3));
    RCP<Basic> i4 = rcp(new Integer(4));

    RCP<Basic> r1;
    RCP<Basic> r2;

    r1 = x*x;
    r2 = rcp(new Pow(x, i2));
    assert(*r1 == *r2);
}


int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    test_add();
    test_pow();

    return 0;
}
