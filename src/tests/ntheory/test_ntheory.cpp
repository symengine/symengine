#include <iostream>

//#include "basic.h"
#include "ntheory.h"
#include "integer.h"

using CSymPy::Integer;
using CSymPy::print_stack_on_segfault;
using CSymPy::rcp;

void test_gcd()
{
    Integer i2 (2);
    Integer i4 (4);

    assert(eq(gcd(i2, i4), rcp(new Integer(2))));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_gcd();

    return 0;
}
