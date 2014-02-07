#include <chrono>

#include "ntheory.h"
#include "integer.h"
#include "add.h"
#include "mul.h"

using CSymPy::Integer;
using CSymPy::print_stack_on_segfault;
using CSymPy::RCP;
using CSymPy::integer;
using CSymPy::is_a;

void test_gcd_lcm()
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i4 = integer(4);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> g = integer(2);
    RCP<const Integer> s = integer(2);
    RCP<const Integer> t = integer(3);

    assert(eq(gcd(*i2, *i4), integer(2)));
    assert(eq(gcd(*i2, *i3), integer(1)));
    assert(eq(gcd(*i2, *i6), integer(2)));
    assert(eq(gcd(*i3, *i6), integer(3)));

    assert(eq(lcm(*i2, *i4), integer(4)));
    assert(eq(lcm(*i2, *i3), integer(6)));
    assert(eq(lcm(*i2, *i6), integer(6)));
    assert(eq(lcm(*i3, *i6), integer(6)));

    gcd_ext(*i2, *i3, outArg(g), outArg(s), outArg(t));
    assert(eq(g, integer(1)));
    assert(eq(g, add(mul(i2, s), mul(i3, t)))); // check if g = i2*s + i3*t

    gcd_ext(*i3, *i6, outArg(g), outArg(s), outArg(t));
    assert(eq(g, integer(3)));
    assert(eq(g, add(mul(i3, s), mul(i6, t)))); // check if g = i3*s + i6*t
}

void test_nextprime()
{
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i6 = integer(6);

    assert(eq(nextprime(*i1), integer(2)));
    assert(eq(nextprime(*i5), integer(7)));
    assert(eq(nextprime(*i6), integer(7)));
}

void test_probab_prime_p()
{
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i6 = integer(6);

    assert(probab_prime_p(*i1) == 0);
    assert(probab_prime_p(*i5) == 2);
    assert(probab_prime_p(*i6) == 0);
}

void test_modular_inverse()
{
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i11 = integer(11);
    RCP<const Integer> b;

    assert(mod_inverse(*i3, *i5, outArg(b)) != 0);
    assert(eq(b, integer(2)));

    assert(mod_inverse(*i3, *i8, outArg(b)) != 0);
    assert(eq(b, integer(3)));

    assert(mod_inverse(*i3, *i11, outArg(b)) != 0);
    assert(eq(b, integer(4)));
}

// Returns true if `b` divides `a` without reminder
bool divides(const RCP<const Integer> &a, const RCP<const Integer> &b)
{
    return is_a<Integer>(*div(a, b));
}

void test_factor()
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> i17 = integer(17);
    RCP<const Integer> i31 = integer(31);
    RCP<const Integer> i121 = integer(121);
    RCP<const Integer> i122 = integer(122);
    RCP<const Integer> i1001 = integer(1001);
    RCP<const Integer> i900 = integer(900);
    RCP<const Integer> f;

    assert(factor(outArg(f), *i2) == 0);
    assert(factor(outArg(f), *i3) == 0);
    assert(factor(outArg(f), *i17) == 0);
    assert(factor(outArg(f), *i31) == 0);

    assert(factor(outArg(f), *i6) > 0);
    assert(divides(i6, f));
    assert(factor(outArg(f), *i121) > 0);
    assert(divides(i121, f));
    assert(factor(outArg(f), *i122) > 0);
    assert(divides(i122, f));
    assert(factor(outArg(f), *i1001) > 0);
    assert(divides(i1001, f));
    assert(!divides(i1001, i6));
    assert(factor(outArg(f), *i900) > 0);
    assert(divides(i900, f));
}

void test_factor_trial_division()
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> i17 = integer(17);
    RCP<const Integer> i31 = integer(31);
    RCP<const Integer> i121 = integer(121);
    RCP<const Integer> i122 = integer(122);
    RCP<const Integer> i1001 = integer(1001);
    RCP<const Integer> i900 = integer(900);
    RCP<const Integer> f;

    assert(factor_trial_division(outArg(f), *i2) == 0);
    assert(factor_trial_division(outArg(f), *i3) == 0);
    assert(factor_trial_division(outArg(f), *i17) == 0);
    assert(factor_trial_division(outArg(f), *i31) == 0);

    assert(factor_trial_division(outArg(f), *i6) > 0);
    assert(divides(i6, f));
    assert(factor_trial_division(outArg(f), *i121) > 0);
    assert(divides(i121, f));
    assert(factor_trial_division(outArg(f), *i122) > 0);
    assert(divides(i122, f));
    assert(factor_trial_division(outArg(f), *i1001) > 0);
    assert(divides(i1001, f));
    assert(!divides(i1001, i6));
    assert(factor_trial_division(outArg(f), *i900) > 0);
    assert(divides(i900, f));
}

void test_sieve()
{
    const int MAX=100000;
    std::vector<unsigned> v;
    auto t1 = std::chrono::high_resolution_clock::now();
    CSymPy::eratosthenes_sieve(MAX, v);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
    std::cout << "Number of primes up to " << MAX << ": " << v.size() << std::endl;
    assert(v.size() == 9592);
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_gcd_lcm();
    test_nextprime();
    test_probab_prime_p();
    test_modular_inverse();
    test_factor();
    test_factor_trial_division();
    test_sieve();

    return 0;
}
