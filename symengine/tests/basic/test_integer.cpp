#include "integer.h"

using SymEngine::print_stack_on_segfault;
using SymEngine::RCP;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::isqrt;

void test_isqrt()
{
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> i19 = integer(19);
    RCP<const Integer> i25 = integer(25);

    assert(eq(isqrt(*i10), integer(3)));
    assert(eq(isqrt(*i19), integer(4)));
    assert(eq(isqrt(*i25), integer(5)));
}

void test_i_nth_root()
{
    RCP<const Integer> i7 = integer(7);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> r;

    assert(i_nth_root(outArg(r), *i7, 2) == 0);
    assert(eq(r, integer(2)));

    assert(i_nth_root(outArg(r), *i9, 2) != 0);
    assert(eq(r, integer(3)));

    assert(i_nth_root(outArg(r), *i9, 3) == 0);
    assert(eq(r, integer(2)));

    assert(i_nth_root(outArg(r), *i10, 2) == 0);
    assert(eq(r, integer(3)));
}

void test_perfect_power_square()
{
    RCP<const Integer> i7 = integer(7);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i10 = integer(10);

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
    RCP<const Integer> _i5 = integer(-5);
    RCP<const Integer> _i9 = integer(-9);
    RCP<const Integer> i12 =  integer(12);

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

