#include <iostream>
#include <chrono>

#include "ntheory.h"
#include "integer.h"

using CSymPy::Integer;
using CSymPy::print_stack_on_segfault;

void test_gcd()
{
    RCP<Basic> I1 = rcp(new Integer(2));
    RCP<Basic> I2 = rcp(new Integer(4));

    assert(eq(gcd(I1, I2), rcp(new Integer(2))));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_gcd();

    return 0;
}
