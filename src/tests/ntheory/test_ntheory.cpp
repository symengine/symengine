#include <chrono>

#include "ntheory.h"
#include "integer.h"
#include "rational.h"
#include "add.h"
#include "mul.h"
#include "dict.h"

using SymEngine::Integer;
using SymEngine::Rational;
using SymEngine::print_stack_on_segfault;
using SymEngine::RCP;
using SymEngine::fibonacci;
using SymEngine::lucas;
using SymEngine::factorial;
using SymEngine::integer;
using SymEngine::is_a;
using SymEngine::map_integer_uint;
using SymEngine::rcp_dynamic_cast;
using SymEngine::rcp_static_cast;
using SymEngine::mod_inverse;
using SymEngine::mod;
using SymEngine::Number;
using SymEngine::bernoulli;
using SymEngine::crt;
using SymEngine::primitive_root;
using SymEngine::primitive_root_list;
using SymEngine::multiplicative_order;
using SymEngine::totient;
using SymEngine::carmichael;

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

    b = mod(*i5, *i3);
    assert(eq(b, integer(2)));

    b = mod(*i11, *i8);
    assert(eq(b, integer(3)));

    b = mod(*i11, *i3);
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
    assert(divides(*i6, *f));
    assert(factor(outArg(f), *i121) > 0);
    assert(divides(*i121, *f));
    assert(factor(outArg(f), *i122) > 0);
    assert(divides(*i122, *f));
    assert(factor(outArg(f), *i1001) > 0);
    assert(divides(*i1001, *f));
    assert(!divides(*i1001, *i6));
    assert(factor(outArg(f), *i900) > 0);
    assert(divides(*i900, *f));
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
    assert(divides(*i21, *f) && !eq(f, i1) && !eq(f, i21));   //Lehman's method returns only a proper divisor when composite
    assert(factor_lehman_method(outArg(f), *i121) > 0);
    assert(divides(*i121, *f) && !eq(f, i1) && !eq(f, i121));
    assert(factor_lehman_method(outArg(f), *i122) > 0);
    assert(divides(*i122, *f) && !eq(f, i1) && !eq(f, i122));
    assert(factor_lehman_method(outArg(f), *i900) > 0);
    assert(divides(*i900, *f) && !eq(f, i1) && !eq(f, i900));
    assert(factor_lehman_method(outArg(f), *i1001) > 0);
    assert(divides(*i1001, *f) && !eq(f, i1) && !eq(f, i1001));
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

    assert(factor_pollard_pm1_method(outArg(f), *i121) == 0 || divides(*i121, *f));
    assert(factor_pollard_pm1_method(outArg(f), *i122) == 0 || divides(*i122, *f));
    assert(factor_pollard_pm1_method(outArg(f), *i900) == 0 || divides(*i900, *f));
    assert(factor_pollard_pm1_method(outArg(f), *i1001, 20) == 0 || divides(*i1001, *f));
    assert(factor_pollard_pm1_method(outArg(f), *i1850) == 0 || divides(*i1850, *f));
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

    assert(factor_pollard_rho_method(outArg(f), *i121) == 0 || divides(*i121, *f));
    assert(factor_pollard_rho_method(outArg(f), *i122) == 0 || divides(*i122, *f));
    assert(factor_pollard_rho_method(outArg(f), *i900) == 0 || divides(*i900, *f));
    assert(factor_pollard_rho_method(outArg(f), *i1001) == 0 || divides(*i1001, *f));
    assert(factor_pollard_rho_method(outArg(f), *i1850) == 0 || divides(*i1850, *f));
}

void test_sieve()
{
    const int MAX = 100003;
    std::vector<unsigned> v;
    auto t1 = std::chrono::high_resolution_clock::now();
    SymEngine::Sieve::generate_primes(v, MAX);
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
    SymEngine::Sieve::iterator pi(MAX);
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

    prime_factors(primes, *a);
    assert(primes.size() == size);

    for (auto &it: primes) {
        assert(divides(*a, *it) == true);
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
    map_integer_uint prime_mul;

    prime_factor_multiplicities(prime_mul, *a);

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
    std::vector<RCP<const Integer>> r, m;
    r = {integer(21), integer(31), integer(6), integer(17), integer(83)};
    m = {integer(30), integer(35), integer(45), integer(77), integer(88)};
    assert(crt(outArg(g), r, m) == true);
    assert(eq(g, integer(9411)));

    r = {integer(3), integer(2), integer(1)};
    m = {integer(20), integer(14), integer(11)};
    assert(crt(outArg(g), r, m) == false);

    r = {integer(3), integer(21), integer(23), integer(9), integer(45)};
    m = {integer(15), integer(48), integer(55), integer(61), integer(66)};
    assert(crt(outArg(g), r, m) == true);
    assert(eq(g, integer(12453)));

    r = {integer(-1), integer(-1), integer(-1), integer(-1), integer(-1)};
    m = {integer(2), integer(3), integer(4), integer(5), integer(6)};
    assert(crt(outArg(g), r, m) == true);
    assert(eq(g, integer(59)));
}

void test_primitive_root()
{
    RCP<const Integer> i15 = integer(15);
    RCP<const Integer> im18 = integer(-18);
    RCP<const Integer> im22 = integer(-22);
    RCP<const Integer> i162 = integer(162);
    RCP<const Integer> i100 = integer(100);
    RCP<const Integer> i = integer(40487*40487);
    RCP<const Integer> g, p;

    assert(primitive_root(outArg(g), *i15) == false);
    assert(primitive_root(outArg(g), *i100) == false);

    assert(primitive_root(outArg(g), *im18) == true);
    assert(multiplicative_order(outArg(p), g, im18) == true );
    assert(eq(p, totient(im18)));

    assert(primitive_root(outArg(g), *i) == true);
    assert(multiplicative_order(outArg(p), g, i) == true );
    assert(eq(p, totient(i)));

    std::vector<RCP<const Integer>> roots, v;
    primitive_root_list(roots, *i162);
    assert(roots.size() == 18);

    roots.clear();
    primitive_root_list(roots, *i100);
    assert(roots.size() == 0);
    
    roots.clear();
    primitive_root_list(roots, *im22);
    assert(roots.size() == 4);
    v = {integer(7), integer(13), integer(17), integer(19)};
    bool same = std::equal(v.begin(), v.end(), roots.begin(), SymEngine::eq);
    assert(same == true);
}

void test_totient_carmichael()
{
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i30 = integer(30);
    RCP<const Integer> im1729 = integer(-1729);

    assert(eq(totient(i8), integer(4)));
    assert(eq(totient(i9), integer(6)));
    assert(eq(totient(i30), integer(8)));
    assert(eq(totient(im1729), integer(1296)));

    assert(eq(carmichael(i8), integer(2)));
    assert(eq(carmichael(i9), integer(6)));
    assert(eq(carmichael(i30), integer(4)));
    assert(eq(carmichael(im1729), integer(36)));
}

void test_multiplicative_order()
{
    RCP<const Integer> i0 = integer(0);
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i13 = integer(13);
    RCP<const Integer> g;

    assert(multiplicative_order(outArg(g), i0, i9) == false);
    assert(multiplicative_order(outArg(g), i2, i6) == false);
    assert(multiplicative_order(outArg(g), i1, i13) == true);
    assert(eq(g, i1));
    assert(multiplicative_order(outArg(g), i13, i9) == true);
    assert(eq(g, i3));
    assert(multiplicative_order(outArg(g), i2, i9) == true);
    assert(eq(g, i6));
    assert(multiplicative_order(outArg(g), i3, i13) == true);
    assert(eq(g, i3));
}

void test_legendre_jacobi_kronecker()
{
    RCP<const Integer> im1 = integer(-1);
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i4 = integer(4);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i15 = integer(15);
    RCP<const Integer> i23 = integer(23);
    RCP<const Integer> i27 = integer(27);
    RCP<const Integer> i93 = integer(93);
    RCP<const Integer> i115 = integer(115);

    assert(legendre(*im1, *i23) == -1);
    assert(legendre(*im1, *i3) == -1);
    assert(legendre(*i3, *i3) == 0);
    assert(legendre(*i4, *i5) == 1);

    assert(jacobi(*im1, *i93) == 1);
    assert(jacobi(*i4, *i27) == 1);
    assert(jacobi(*i5, *i115) == 0);
    assert(jacobi(*i93, *i115) == -1);

    assert(kronecker(*i8, *i27) == -1);
    assert(kronecker(*i4, *i8) == 0);
    assert(kronecker(*i8, *i23) == 1);
}

void test_nthroot_mod()
{
    RCP<const Integer> im1 = integer(-1);
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i4 = integer(4);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> i16 = integer(16);
    RCP<const Integer> i18 = integer(18);
    RCP<const Integer> i23 = integer(23);
    RCP<const Integer> i27 = integer(27);
    RCP<const Integer> i31 = integer(31);
    RCP<const Integer> i32 = integer(32);
    RCP<const Integer> i41 = integer(41);
    RCP<const Integer> i64 = integer(64);
    RCP<const Integer> i93 = integer(93);
    RCP<const Integer> i100 = integer(100);
    RCP<const Integer> i105 = integer(105);
    RCP<const Integer> nthroot, rem;
    std::vector<RCP<const Integer>> roots, v;

    assert(nthroot_mod(outArg(nthroot), im1, i2, i23) == false);
    assert(nthroot_mod(outArg(nthroot), im1, i2, i93) == false);
    assert(nthroot_mod(outArg(nthroot), i3, i2, i27) == false);
    assert(nthroot_mod(outArg(nthroot), i18, i2, i27) == false);
    assert(nthroot_mod(outArg(nthroot), i9, i4, i64) == false);
    assert(nthroot_mod(outArg(nthroot), im1, i2, i23) == false);
    assert(nthroot_mod(outArg(nthroot), i2, i3, i105) == false);

    assert(nthroot_mod(outArg(nthroot), i5, i1, i100) == true);
    assert(eq(nthroot, i5));

    assert(nthroot_mod(outArg(nthroot), im1, i2, i41) == true);
    rem = integer(nthroot->as_mpz() * nthroot->as_mpz() - im1->as_mpz());
    assert(divides(*rem, *i41));

    assert(nthroot_mod(outArg(nthroot), i31, i4, i41) == true);
    nthroot_mod(outArg(nthroot), i32, i10, i41);
    nthroot_mod_list(roots, i1, i10, i100);

    assert(nthroot_mod(outArg(nthroot), i4, i2, i64) == true);
    rem = integer(nthroot->as_mpz() * nthroot->as_mpz() - i4->as_mpz());
    assert(divides(*rem, *i64));

    assert(nthroot_mod(outArg(nthroot), i32, i10, i41) == true);
    rem = rcp_static_cast<const Integer>(nthroot->powint(*i10));
    rem = integer(rem->as_mpz() - i32->as_mpz());
    assert(divides(*rem, *i41));

    roots.clear();
    nthroot_mod_list(roots, i9, i2, i27);
    assert(roots.size() == 6);
    v = {integer(3), integer(6), integer(12), integer(15), integer(21), integer(24)};
    bool same = std::equal(v.begin(), v.end(), roots.begin(), SymEngine::eq);
    assert(same == true);

    roots.clear();
    nthroot_mod_list(roots, i1, i18, i105);
    assert(roots.size() == 24);
}

void test_powermod()
{
    RCP<const Integer> im1 = integer(-1);
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i4 = integer(4);
    RCP<const Integer> i15 = integer(15);
    RCP<const Integer> i18 = integer(18);
    RCP<const Integer> i23 = integer(23);
    RCP<const Integer> i41 = integer(41);
    RCP<const Integer> i105 = integer(105);
    RCP<const Integer> r;
    std::vector<RCP<const Integer>> powms;

    assert(powermod(outArg(r), i2, im1, i4) == false);
    assert(powermod(outArg(r), i4, i3, i41) == true);
    assert(eq(r, i23));

    assert(powermod(outArg(r), i2, i23->divint(*i41), i41) == true);
    assert(eq(r, integer(8)));

    powermod_list(powms, i15, i1->divint(*i18), i105);
    assert(powms.size() == 6);
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
    test_totient_carmichael();
    test_multiplicative_order();
    test_primitive_root();
    test_legendre_jacobi_kronecker();
    test_nthroot_mod();
    test_powermod();

    return 0;
}

