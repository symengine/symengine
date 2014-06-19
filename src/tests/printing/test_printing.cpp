#include <chrono>

#include "basic.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"

using CSymPy::RCP;
using CSymPy::Basic;
using CSymPy::div;
using CSymPy::pow;
using CSymPy::mul;
using CSymPy::integer;
using CSymPy::print_stack_on_segfault;

void test_printing()
{
    RCP<const Basic> r;
    
    r = div(integer(12), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "12*196^-1/2");

    r = mul(integer(12), pow(integer(196), div(integer(1), integer(2))));
    assert(r->__str__() == "12*196^1/2");

    r = mul(integer(23), mul(pow(integer(5), div(integer(1), integer(2))),
        pow(integer(7), div(integer(1), integer(2)))));
    assert(r->__str__() == "23*5^1/2*7^1/2");
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_printing();

    return 0;
}
