#include <chrono>

#include "ntheory.h"
#include "integer.h"
#include "rational.h"
#include "add.h"
#include "mul.h"
#include "dict.h"

using CSymPy::Integer;
using CSymPy::Rational;
using CSymPy::print_stack_on_segfault;
using CSymPy::RCP;
using CSymPy::fibonacci;
using CSymPy::lucas;
using CSymPy::factorial;
using CSymPy::integer;
using CSymPy::is_a;
using CSymPy::map_integer_uint;
using CSymPy::rcp_dynamic_cast;
using CSymPy::mod_inverse;
using CSymPy::mod;
using CSymPy::Number;
using CSymPy::bernoulli;

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

    gcd_ext(outArg(g), outArg(s), outArg(t), *i2, *i3);
    assert(eq(g, integer(1)));
    assert(eq(g, add(mul(i2, s), mul(i3, t)))); // check if g = i2*s + i3*t

    gcd_ext(outArg(g), outArg(s), outArg(t), *i3, *i6);
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

    assert(mod_inverse(outArg(b), *i3, *i5) != 0);
    assert(eq(b, integer(2)));

    assert(mod_inverse(outArg(b), *i3, *i8) != 0);
    assert(eq(b, integer(3)));

    assert(mod_inverse(outArg(b), *i3, *i11) != 0);
    assert(eq(b, integer(4)));
}

void test_modulo()
{
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i11 = integer(11);
    RCP<const Number> b;

    mod(outArg(b), *i5, *i3);
    assert(eq(b, integer(2)));

    mod(outArg(b), *i11, *i8);
    assert(eq(b, integer(3)));

    mod(outArg(b), *i11, *i3);
    assert(eq(b, integer(2)));
}

void test_fibonacci_lucas()
{
    RCP<const Integer> g;
    RCP<const Integer> s;

    assert(eq(fibonacci(1), integer(1)));
    assert(eq(fibonacci(2), integer(1)));
    assert(eq(fibonacci(3), integer(2)));
    assert(eq(fibonacci(5), integer(5)));

    assert(eq(lucas(1), integer(1)));
    assert(eq(lucas(2), integer(3)));
    assert(eq(lucas(3), integer(4)));
    assert(eq(lucas(5), integer(11)));

    fibonacci2(outArg(g), outArg(s), 10);
    assert(eq(g, integer(55)));
    assert(eq(s, integer(34)));

    lucas2(outArg(g), outArg(s), 10);
    assert(eq(g, integer(123)));
    assert(eq(s, integer(76)));
}

void test_binomial()
{
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i0 = integer(0);
    RCP<const Integer> m10 = integer(-10);

    assert(eq(binomial(*i10, 1), i10));
    assert(eq(binomial(*i5, 2), i10));
    assert(eq(binomial(*i5, 10), i0));
    assert(eq(binomial(*i10, 11), i0));
    assert(eq(binomial(*i10, 2), integer(45)));

    assert(eq(binomial(*m10, 3), integer(-220)));
    assert(eq(binomial(*m10, 2), integer(55)));
}

void test_factorial()
{
    RCP<const Integer> i1 = integer(1);

    assert(eq(factorial(1), i1));
    assert(eq(factorial(0), i1));
    assert(eq(factorial(5), integer(120)));
    assert(eq(factorial(9), integer(362880)));
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

void test_factor_lehman_method()
{
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i21 = integer(21);
    RCP<const Integer> i23 = integer(23);
    RCP<const Integer> i31 = integer(31);
    RCP<const Integer> i47 = integer(47);
    RCP<const Integer> i121 = integer(121);
    RCP<const Integer> i122 = integer(122);
    RCP<const Integer> i900 = integer(900);
    RCP<const Integer> i1001 = integer(1001);
    RCP<const Integer> f;

    assert(factor_lehman_method(outArg(f), *i23) == 0);
    assert(factor_lehman_method(outArg(f), *i31) == 0);
    assert(factor_lehman_method(outArg(f), *i47) == 0);

    assert(factor_lehman_method(outArg(f), *i21) > 0);
    assert(divides(i21, f) && !eq(f, i1) && !eq(f, i21));   //Lehman's method returns only a proper divisor when composite
    assert(factor_lehman_method(outArg(f), *i121) > 0);
    assert(divides(i121, f) && !eq(f, i1) && !eq(f, i121));
    assert(factor_lehman_method(outArg(f), *i122) > 0);
    assert(divides(i122, f) && !eq(f, i1) && !eq(f, i122));
    assert(factor_lehman_method(outArg(f), *i900) > 0);
    assert(divides(i900, f) && !eq(f, i1) && !eq(f, i900));
    assert(factor_lehman_method(outArg(f), *i1001) > 0);
    assert(divides(i1001, f) && !eq(f, i1) && !eq(f, i1001));
}

void test_factor_pollard_pm1_method()
{
    RCP<const Integer> i23 = integer(23);
    RCP<const Integer> i31 = integer(31);
    RCP<const Integer> i47 = integer(47);
    RCP<const Integer> i121 = integer(121);
    RCP<const Integer> i122 = integer(122);
    RCP<const Integer> i900 = integer(900);
    RCP<const Integer> i1001 = integer(1001);
    RCP<const Integer> i1850 = integer(1850);
    RCP<const Integer> f;

    assert(factor_pollard_pm1_method(outArg(f), *i23) == 0);
    assert(factor_pollard_pm1_method(outArg(f), *i31) == 0);
    assert(factor_pollard_pm1_method(outArg(f), *i47) == 0);

    assert(factor_pollard_pm1_method(outArg(f), *i121) == 0 || divides(i121, f));
    assert(factor_pollard_pm1_method(outArg(f), *i122) == 0 || divides(i122, f));
    assert(factor_pollard_pm1_method(outArg(f), *i900) == 0 || divides(i900, f));
    assert(factor_pollard_pm1_method(outArg(f), *i1001, 20) == 0 || divides(i1001, f));
    assert(factor_pollard_pm1_method(outArg(f), *i1850) == 0 || divides(i1850, f));
}

void test_factor_pollard_rho_method()
{
    RCP<const Integer> i23 = integer(23);
    RCP<const Integer> i31 = integer(31);
    RCP<const Integer> i47 = integer(47);
    RCP<const Integer> i121 = integer(121);
    RCP<const Integer> i122 = integer(122);
    RCP<const Integer> i900 = integer(900);
    RCP<const Integer> i1001 = integer(1001);
    RCP<const Integer> i1850 = integer(1850);
    RCP<const Integer> f;

    assert(factor_pollard_rho_method(outArg(f), *i23) == 0);
    assert(factor_pollard_rho_method(outArg(f), *i31) == 0);
    assert(factor_pollard_rho_method(outArg(f), *i47) == 0);

    assert(factor_pollard_rho_method(outArg(f), *i121) == 0 || divides(i121, f));
    assert(factor_pollard_rho_method(outArg(f), *i122) == 0 || divides(i122, f));
    assert(factor_pollard_rho_method(outArg(f), *i900) == 0 || divides(i900, f));
    assert(factor_pollard_rho_method(outArg(f), *i1001) == 0 || divides(i1001, f));
    assert(factor_pollard_rho_method(outArg(f), *i1850) == 0 || divides(i1850, f));
}

void test_sieve()
{
    const int MAX = 100003;
    std::vector<unsigned> v;
    auto t1 = std::chrono::high_resolution_clock::now();
    CSymPy::Sieve::generate_primes(MAX, v);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()
        << "us" << std::endl;
    std::cout << "Number of primes up to " << MAX << ": " << v.size() << std::endl;
    assert(v.size() == 9593);
}

void test_sieve_iterator()
{
    const int MAX = 100003;
    int count = 0, prime;
    CSymPy::Sieve::iterator pi(MAX);
    auto t1 = std::chrono::high_resolution_clock::now();
    while((prime=pi.next_prime()) <= MAX){
        count++;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()
        << "us" << std::endl;
    std::cout << "Number of primes up to " << MAX << ": " << count << std::endl;
    assert(count == 9593);
}

// helper function for test_primefactors
void _test_primefactors(const RCP<const Integer> &a, unsigned size)
{
    std::vector<RCP<const Integer>> primes;

    prime_factors(a, primes);
    assert(primes.size() == size);

    for (auto &it: primes) {
        assert(divides(a, it) == true);
        assert(probab_prime_p(*it) > 0);
    }
}

void test_prime_factors()
{
    RCP<const Integer> i0 = integer(0);
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> i12 = integer(12);
    RCP<const Integer> i36 = integer(36);
    RCP<const Integer> i125 = integer(125);
    RCP<const Integer> i1001 = integer(1001);

    _test_primefactors(i0, 0);
    _test_primefactors(i1, 0);
    _test_primefactors(i5, 1);
    _test_primefactors(i6, 2);
    _test_primefactors(i12, 3);
    _test_primefactors(i36, 4);
    _test_primefactors(i125, 3);
    _test_primefactors(i1001, 3);
}

void _test_prime_factor_multiplicities(const RCP<const Integer> &a)
{
    unsigned multiplicity;
    RCP<const Integer> _a = a;
    std::vector<RCP<const Integer>> primes;
    map_integer_uint prime_mul;

    prime_factor_multiplicities(a, prime_mul);

    for (auto it : prime_mul) {
        multiplicity = it.second;
        while(multiplicity) {
            _a = rcp_dynamic_cast<const Integer>(div(_a, it.first));
            multiplicity--;
        }
    }

    assert(eq(_a, integer(1)));
}

void test_prime_factor_multiplicities()
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> i12 = integer(12);
    RCP<const Integer> i36 = integer(36);
    RCP<const Integer> i125 = integer(125);
    RCP<const Integer> i2357 = integer(2357);

    _test_prime_factor_multiplicities(i2);
    _test_prime_factor_multiplicities(i3);
    _test_prime_factor_multiplicities(i6);
    _test_prime_factor_multiplicities(i12);
    _test_prime_factor_multiplicities(i36);
    _test_prime_factor_multiplicities(i125);
    _test_prime_factor_multiplicities(i2357);
}

void test_bernoulli()
{
    RCP<const Number> r1;
    RCP<const Number> r2;
    #ifdef HAVE_CSYMPY_ARB
        r1 = bernoulli(12);
        r2 = Rational::from_two_ints(integer(-691), integer(2730));
        assert(eq(r1, r2));
    #else
        CSYMPY_CHECK_THROW(bernoulli(12), std::runtime_error)
    #endif
}

void test_crt() {
    RCP<const Integer> g;
    std::vector<RCP<const Integer>> r = {integer(21), integer(31), integer(6), integer(17), integer(83)};
    std::vector<RCP<const Integer>> m = {integer(30), integer(35), integer(45), integer(77), integer(88)};
    assert(crt(outArg(g), r, m) == true);
    assert(eq(g, integer(9411)));
}

void test_primitive_root()
{
    RCP<const Integer> i15 = integer(15);
    RCP<const Integer> i18 = integer(47);
    RCP<const Integer> i162 = integer(162);
    RCP<const Integer> i100 = integer(100);
    RCP<const Integer> i = integer(40487*40487);
    RCP<const Integer> g;

    assert(primitive_root(outArg(g), *i15) == false);
    assert(primitive_root(outArg(g), *i100) == false);
    assert(primitive_root(outArg(g), *i18) == true);
    assert(primitive_root(outArg(g), *i) == true);

    std::vector<RCP<const Integer>> roots;
    primitive_root_list(roots, 162);
    assert(roots.size() == 18);
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_gcd_lcm();
    test_nextprime();
    test_probab_prime_p();
    test_modular_inverse();
    test_fibonacci_lucas();
    test_binomial();
    test_factorial();
    test_factor();
    test_factor_lehman_method();
    test_factor_pollard_pm1_method();
    test_factor_pollard_rho_method();
    test_sieve();
    test_sieve_iterator();
    test_prime_factors();
    test_prime_factor_multiplicities();
    test_modulo();
    test_bernoulli();
    test_crt();
    test_primitive_root();

    return 0;
}

