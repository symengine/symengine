#include "catch.hpp"

#include <symengine/integer.h>
#include <symengine/symengine_exception.h>

using SymEngine::SymEngineException;
using SymEngine::print_stack_on_segfault;
using SymEngine::RCP;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::integer_class;
using SymEngine::isqrt;

TEST_CASE("isqrt: integer", "[integer]")
{
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> i19 = integer(19);
    RCP<const Integer> i25 = integer(25);

    CHECK(eq(*isqrt(*i10), *integer(3)));
    CHECK(eq(*isqrt(*i19), *integer(4)));
    CHECK(eq(*isqrt(*i25), *integer(5)));
}

TEST_CASE("i_nth_root: integer", "[integer]")
{
    RCP<const Integer> i7 = integer(7);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> r;

    CHECK(i_nth_root(outArg(r), *i7, 2) == 0);
    CHECK(eq(*r, *integer(2)));

    CHECK(i_nth_root(outArg(r), *i9, 2) != 0);
    CHECK(eq(*r, *integer(3)));

    CHECK(i_nth_root(outArg(r), *i9, 3) == 0);
    CHECK(eq(*r, *integer(2)));

    CHECK(i_nth_root(outArg(r), *i10, 2) == 0);
    CHECK(eq(*r, *integer(3)));
}

TEST_CASE("perfect_power_square: integer", "[integer]")
{
    RCP<const Integer> i7 = integer(7);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i10 = integer(10);

    CHECK(perfect_square(*i7) == 0);
    CHECK(perfect_power(*i7) == 0);
    CHECK(perfect_square(*i8) == 0);
    CHECK(perfect_power(*i8) != 0);
    CHECK(perfect_square(*i9) != 0);
    CHECK(perfect_power(*i9) != 0);
    CHECK(perfect_square(*i10) == 0);
    CHECK(perfect_power(*i10) == 0);
}

TEST_CASE("iabs: integer", "[integer]")
{
    RCP<const Integer> _i5 = integer(-5);
    RCP<const Integer> _i9 = integer(-9);
    RCP<const Integer> i12 = integer(12);

    CHECK(eq(*iabs(*_i5), *integer(5)));
    CHECK(eq(*iabs(*_i9), *integer(9)));
    CHECK(eq(*iabs(*i12), *integer(12)));
}

TEST_CASE("fix#461: integer", "[integer]")
{
    RCP<const Integer> ir;

    long lmax = std::numeric_limits<long>::max();
    ir = integer(lmax);
    CHECK(integer_class(lmax) == ir->as_integer_class());

    unsigned long ulmax = std::numeric_limits<unsigned long>::max();
    ir = integer(ulmax);
    CHECK(integer_class(ulmax) == ir->as_integer_class());

    int imax = std::numeric_limits<int>::max();
    ir = integer(imax);
    CHECK(integer_class(imax) == ir->as_integer_class());

    integer_class val(12345);
    ir = integer(val);
    CHECK(val == ir->as_integer_class());
}
