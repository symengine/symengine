#include "integer.h"

using CSymPy::print_stack_on_segfault;
using CSymPy::RCP;
using CSymPy::Integer;
using CSymPy::integer;
using CSymPy::isqrt;

void test_isqrt()
{
    RCP<Integer> i10 = integer(10);
    RCP<Integer> i19 = integer(19);
    RCP<Integer> i25 = integer(25);
    
    assert(eq(isqrt(*i10), integer(3)));
    assert(eq(isqrt(*i19), integer(4)));
    assert(eq(isqrt(*i25), integer(5)));
}

void test_iabs()
{
    RCP<Integer> _i5 = integer(-5);
    RCP<Integer> _i9 = integer(-9);
    RCP<Integer> i12 =  integer(12);
    
    assert(eq(iabs(*_i5), integer(5)));
    assert(eq(iabs(*_i9), integer(9)));
    assert(eq(iabs(*i12), integer(12)));
}

int main(int argc, char *argv[])
{
    print_stack_on_segfault();
    
    test_isqrt();
    test_iabs();
    
    return 0;
}
