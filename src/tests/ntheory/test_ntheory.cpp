#include <iostream>

//#include "basic.h"
#include "ntheory.h"
#include "integer.h"

using CSymPy::Integer;
using CSymPy::print_stack_on_segfault;
using CSymPy::rcp;

void test_gcd_lcm()
{
    Integer i1 (1);
    Integer i2 (2);
    Integer i3 (3);
    Integer i4 (4);
    Integer i6 (6);

    assert(eq(gcd(i2, i4), rcp(i2)));
    assert(eq(gcd(i2, i3), rcp(i1)));
    assert(eq(gcd(i2, i6), rcp(i2)));
    assert(eq(gcd(i3, i6), rcp(i3)));

    assert(eq(lcm(i2, i4), rcp(i4)));
    assert(eq(lcm(i2, i3), rcp(i6)));
    assert(eq(lcm(i2, i6), rcp(i6)));
    assert(eq(lcm(i3, i6), rcp(i6)));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_gcd_lcm();

    return 0;
}
