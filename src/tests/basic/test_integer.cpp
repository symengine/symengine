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

void test_i_nth_root()
{
    RCP<Integer> i7 = integer(7);
    RCP<Integer> i9 = integer(9);
    RCP<Integer> i10 = integer(10);
    RCP<Integer> r;
    
    assert(i_nth_root(*i7, 2, outArg(r)) == 0);
    assert(eq(r, integer(2)));
    
    assert(i_nth_root(*i9, 2, outArg(r)) != 0);
    assert(eq(r, integer(3)));
    
    assert(i_nth_root(*i9, 3, outArg(r)) == 0);
    assert(eq(r, integer(2)));

    assert(i_nth_root(*i10, 2, outArg(r)) == 0);
    assert(eq(r, integer(3)));
}

void test_perfect_power_square()
{
    RCP<Integer> i7 = integer(7);
    RCP<Integer> i8 = integer(8);
    RCP<Integer> i9 = integer(9);
    RCP<Integer> i10 = integer(10);
    
    assert(perfect_square(*i7) == 0);
    assert(perfect_power(*i7) == 0);
    assert(perfect_square(*i8) == 0);
    assert(perfect_power(*i8) != 0);
    assert(perfect_square(*i9) != 0);
    assert(perfect_power(*i9) != 0);
    assert(perfect_square(*i10) == 0);
    assert(perfect_power(*i10) == 0);
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
    test_i_nth_root();
    test_perfect_power_square();
    test_iabs();
    
    return 0;
}
