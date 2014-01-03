#include <iostream>

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

    assert(eq(gcd(i2, i4), rcp(new Integer(2))));
    assert(eq(gcd(i2, i3), rcp(new Integer(1))));
    assert(eq(gcd(i2, i6), rcp(new Integer(2))));
    assert(eq(gcd(i3, i6), rcp(new Integer(3))));

    assert(eq(lcm(i2, i4), rcp(new Integer(4))));
    assert(eq(lcm(i2, i3), rcp(new Integer(6))));
    assert(eq(lcm(i2, i6), rcp(new Integer(6))));
    assert(eq(lcm(i3, i6), rcp(new Integer(6))));
}

void test_nextprime()
{
    Integer i1 (1);
    Integer i5 (5);
    Integer i6 (6);

    assert(eq(nextprime(i1), rcp(new Integer(2))));
    assert(eq(nextprime(i5), rcp(new Integer(7))));
    assert(eq(nextprime(i6), rcp(new Integer(7))));
}

void test_probab_prime_p()
{
    Integer i1 (1);
    Integer i5 (5);
    Integer i6 (6);

    assert(probab_prime_p(i1) == 0);
    assert(probab_prime_p(i5) == 2);
    assert(probab_prime_p(i6) == 0);
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_gcd_lcm();
    test_nextprime();
    test_probab_prime_p();

    return 0;
}
