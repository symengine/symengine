#include "catch.hpp"
#include <chrono>

#include <symengine/ntheory.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/dict.h>

using SymEngine::Basic;
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
using SymEngine::mod_f;
using SymEngine::quotient;
using SymEngine::quotient_f;
using SymEngine::quotient_mod;
using SymEngine::quotient_mod_f;
using SymEngine::Number;
using SymEngine::bernoulli;
using SymEngine::crt;
using SymEngine::primitive_root;
using SymEngine::primitive_root_list;
using SymEngine::multiplicative_order;
using SymEngine::totient;
using SymEngine::carmichael;

TEST_CASE("test_gcd_lcm(): ntheory", "[ntheory]")
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i4 = integer(4);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> g = integer(2);
    RCP<const Integer> s = integer(2);
    RCP<const Integer> t = integer(3);

    REQUIRE(eq(*gcd(*i2, *i4), *integer(2)));
    REQUIRE(eq(*gcd(*i2, *i3), *integer(1)));
    REQUIRE(eq(*gcd(*i2, *i6), *integer(2)));
    REQUIRE(eq(*gcd(*i3, *i6), *integer(3)));

    REQUIRE(eq(*lcm(*i2, *i4), *integer(4)));
    REQUIRE(eq(*lcm(*i2, *i3), *integer(6)));
    REQUIRE(eq(*lcm(*i2, *i6), *integer(6)));
    REQUIRE(eq(*lcm(*i3, *i6), *integer(6)));

    gcd_ext(outArg(g), outArg(s), outArg(t), *i2, *i3);
    REQUIRE(eq(*g, *integer(1)));
    REQUIRE(eq(*g, *add(mul(i2, s), mul(i3, t)))); // check if g = i2*s + i3*t

    gcd_ext(outArg(g), outArg(s), outArg(t), *i3, *i6);
    REQUIRE(eq(*g, *integer(3)));
    REQUIRE(eq(*g, *add(mul(i3, s), mul(i6, t)))); // check if g = i3*s + i6*t
}

TEST_CASE("test_nextprime(): ntheory", "[ntheory]")
{
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i6 = integer(6);

    REQUIRE(eq(*nextprime(*i1), *integer(2)));
    REQUIRE(eq(*nextprime(*i5), *integer(7)));
    REQUIRE(eq(*nextprime(*i6), *integer(7)));
}

TEST_CASE("test_probab_prime_p(): ntheory", "[ntheory]")
{
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i6 = integer(6);

    REQUIRE(probab_prime_p(*i1) == 0);
    REQUIRE(probab_prime_p(*i5) == 2);
    REQUIRE(probab_prime_p(*i6) == 0);
}

TEST_CASE("test_modular_inverse(): ntheory", "[ntheory]")
{
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i11 = integer(11);
    RCP<const Integer> b;

    REQUIRE(mod_inverse(outArg(b), *i3, *i5) != 0);
    REQUIRE(eq(*b, *integer(2)));

    REQUIRE(mod_inverse(outArg(b), *i3, *i8) != 0);
    REQUIRE(eq(*b, *integer(3)));

    REQUIRE(mod_inverse(outArg(b), *i3, *i11) != 0);
    REQUIRE(eq(*b, *integer(4)));
}

TEST_CASE("test_modulo(): ntheory", "[ntheory]")
{
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> im11 = integer(-11);
    RCP<const Integer> b, c;

    b = mod(*i5, *i3);
    REQUIRE(eq(*b, *integer(2)));

    b = mod(*im11, *i8);
    REQUIRE(eq(*b, *integer(-3)));

    b = mod_f(*im11, *i8);
    REQUIRE(eq(*b, *i5));

    b = quotient(*im11, *i5);
    REQUIRE(eq(*b, *integer(-2)));

    b = quotient_f(*im11, *i5);
    REQUIRE(eq(*b, *integer(-3)));

    quotient_mod(outArg(b), outArg(c), *im11, *i5);
    REQUIRE(eq(*b, *integer(-2)));
    REQUIRE(eq(*c, *integer(-1)));

    quotient_mod_f(outArg(b), outArg(c), *im11, *i5);
    REQUIRE(eq(*b, *integer(-3)));
    REQUIRE(eq(*c, *integer(4)));
}

TEST_CASE("test_fibonacci_lucas(): ntheory", "[ntheory]")
{
    RCP<const Integer> g;
    RCP<const Integer> s;

    REQUIRE(eq(*fibonacci(1), *integer(1)));
    REQUIRE(eq(*fibonacci(2), *integer(1)));
    REQUIRE(eq(*fibonacci(3), *integer(2)));
    REQUIRE(eq(*fibonacci(5), *integer(5)));

    REQUIRE(eq(*lucas(1), *integer(1)));
    REQUIRE(eq(*lucas(2), *integer(3)));
    REQUIRE(eq(*lucas(3), *integer(4)));
    REQUIRE(eq(*lucas(5), *integer(11)));

    fibonacci2(outArg(g), outArg(s), 10);
    REQUIRE(eq(*g, *integer(55)));
    REQUIRE(eq(*s, *integer(34)));

    lucas2(outArg(g), outArg(s), 10);
    REQUIRE(eq(*g, *integer(123)));
    REQUIRE(eq(*s, *integer(76)));
}

TEST_CASE("test_binomial(): ntheory", "[ntheory]")
{
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i0 = integer(0);
    RCP<const Integer> m10 = integer(-10);

    REQUIRE(eq(*binomial(*i10, 1), *i10));
    REQUIRE(eq(*binomial(*i5, 2), *i10));
    REQUIRE(eq(*binomial(*i5, 10), *i0));
    REQUIRE(eq(*binomial(*i10, 11), *i0));
    REQUIRE(eq(*binomial(*i10, 2), *integer(45)));

    REQUIRE(eq(*binomial(*m10, 3), *integer(-220)));
    REQUIRE(eq(*binomial(*m10, 2), *integer(55)));
}

TEST_CASE("test_factorial(): ntheory", "[ntheory]")
{
    RCP<const Integer> i1 = integer(1);

    REQUIRE(eq(*factorial(1), *i1));
    REQUIRE(eq(*factorial(0), *i1));
    REQUIRE(eq(*factorial(5), *integer(120)));
    REQUIRE(eq(*factorial(9), *integer(362880)));
}

TEST_CASE("test_factor(): ntheory", "[ntheory]")
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

    REQUIRE(factor(outArg(f), *i2) == 0);
    REQUIRE(factor(outArg(f), *i3) == 0);
    REQUIRE(factor(outArg(f), *i17) == 0);
    REQUIRE(factor(outArg(f), *i31) == 0);

    REQUIRE(factor(outArg(f), *i6) > 0);
    REQUIRE(divides(*i6, *f));
    REQUIRE(factor(outArg(f), *i121) > 0);
    REQUIRE(divides(*i121, *f));
    REQUIRE(factor(outArg(f), *i122) > 0);
    REQUIRE(divides(*i122, *f));
    REQUIRE(factor(outArg(f), *i1001) > 0);
    REQUIRE(divides(*i1001, *f));
    REQUIRE(not divides(*i1001, *i6));
    REQUIRE(factor(outArg(f), *i900) > 0);
    REQUIRE(divides(*i900, *f));
}

TEST_CASE("test_factor_lehman_method(): ntheory", "[ntheory]")
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

    REQUIRE(factor_lehman_method(outArg(f), *i23) == 0);
    REQUIRE(factor_lehman_method(outArg(f), *i31) == 0);
    REQUIRE(factor_lehman_method(outArg(f), *i47) == 0);

    REQUIRE(factor_lehman_method(outArg(f), *i21) > 0);
    REQUIRE((divides(*i21, *f) and not eq(*f, *i1) and not eq(*f, *i21)));   //Lehman's method returns only a proper divisor when composite
    REQUIRE(factor_lehman_method(outArg(f), *i121) > 0);
    REQUIRE((divides(*i121, *f) and not eq(*f, *i1) and not eq(*f, *i121)));
    REQUIRE(factor_lehman_method(outArg(f), *i122) > 0);
    REQUIRE((divides(*i122, *f) and not eq(*f, *i1) and not eq(*f, *i122)));
    REQUIRE(factor_lehman_method(outArg(f), *i900) > 0);
    REQUIRE((divides(*i900, *f) and not eq(*f, *i1) and not eq(*f, *i900)));
    REQUIRE(factor_lehman_method(outArg(f), *i1001) > 0);
    REQUIRE((divides(*i1001, *f) and not eq(*f, *i1) and not eq(*f, *i1001)));
}

TEST_CASE("test_factor_pollard_pm1_method(): ntheory", "[ntheory]")
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

    REQUIRE(factor_pollard_pm1_method(outArg(f), *i23) == 0);
    REQUIRE(factor_pollard_pm1_method(outArg(f), *i31) == 0);
    REQUIRE(factor_pollard_pm1_method(outArg(f), *i47) == 0);

    REQUIRE((factor_pollard_pm1_method(outArg(f), *i121) == 0 or divides(*i121, *f)));
    REQUIRE((factor_pollard_pm1_method(outArg(f), *i122) == 0 or divides(*i122, *f)));
    REQUIRE((factor_pollard_pm1_method(outArg(f), *i900) == 0 or divides(*i900, *f)));
    REQUIRE((factor_pollard_pm1_method(outArg(f), *i1001, 20) == 0 or divides(*i1001, *f)));
    REQUIRE((factor_pollard_pm1_method(outArg(f), *i1850) == 0 or divides(*i1850, *f)));
}

TEST_CASE("test_factor_pollard_rho_method(): ntheory", "[ntheory]")
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

    REQUIRE(factor_pollard_rho_method(outArg(f), *i23) == 0);
    REQUIRE(factor_pollard_rho_method(outArg(f), *i31) == 0);
    REQUIRE(factor_pollard_rho_method(outArg(f), *i47) == 0);

    REQUIRE((factor_pollard_rho_method(outArg(f), *i121) == 0 or divides(*i121, *f)));
    REQUIRE((factor_pollard_rho_method(outArg(f), *i122) == 0 or divides(*i122, *f)));
    REQUIRE((factor_pollard_rho_method(outArg(f), *i900) == 0 or divides(*i900, *f)));
    REQUIRE((factor_pollard_rho_method(outArg(f), *i1001) == 0 or divides(*i1001, *f)));
    REQUIRE((factor_pollard_rho_method(outArg(f), *i1850) == 0 or divides(*i1850, *f)));
}

TEST_CASE("test_sieve(): ntheory", "[ntheory]")
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
    REQUIRE(v.size() == 9593);
}

TEST_CASE("test_sieve_iterator(): ntheory", "[ntheory]")
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
    REQUIRE(count == 9593);
}

// helper function for test_primefactors
void _test_primefactors(const RCP<const Integer> &a, unsigned size)
{
    std::vector<RCP<const Integer>> primes;

    prime_factors(primes, *a);
    REQUIRE(primes.size() == size);

    for (auto &it: primes) {
        REQUIRE(divides(*a, *it) == true);
        REQUIRE(probab_prime_p(*it) > 0);
    }
}

TEST_CASE("test_prime_factors(): ntheory", "[ntheory]")
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

    REQUIRE(eq(*_a, *integer(1)));
}

TEST_CASE("test_prime_factor_multiplicities(): ntheory", "[ntheory]")
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

TEST_CASE("test_bernoulli(): ntheory", "[ntheory]")
{
    RCP<const Number> r1;
    RCP<const Number> r2;
    #ifdef HAVE_SYMENGINE_ARB
        r1 = bernoulli(12);
        r2 = Rational::from_two_ints(*integer(-691), *integer(2730));
        REQUIRE(eq(*r1, *r2));
    #else
        CHECK_THROWS_AS(bernoulli(12), std::runtime_error);
    #endif
}

TEST_CASE("test_crt(): ntheory", "[ntheory]")
{
    RCP<const Integer> g;
    std::vector<RCP<const Integer>> r, m;
    r = {integer(21), integer(31), integer(6), integer(17), integer(83)};
    m = {integer(30), integer(35), integer(45), integer(77), integer(88)};
    REQUIRE(crt(outArg(g), r, m) == true);
    REQUIRE(eq(*g, *integer(9411)));

    r = {integer(3), integer(2), integer(1)};
    m = {integer(20), integer(14), integer(11)};
    REQUIRE(crt(outArg(g), r, m) == false);

    r = {integer(3), integer(21), integer(23), integer(9), integer(45)};
    m = {integer(15), integer(48), integer(55), integer(61), integer(66)};
    REQUIRE(crt(outArg(g), r, m) == true);
    REQUIRE(eq(*g, *integer(12453)));

    r = {integer(-1), integer(-1), integer(-1), integer(-1), integer(-1)};
    m = {integer(2), integer(3), integer(4), integer(5), integer(6)};
    REQUIRE(crt(outArg(g), r, m) == true);
    REQUIRE(eq(*g, *integer(59)));
}

TEST_CASE("test_primitive_root(): ntheory", "[ntheory]")
{
    RCP<const Integer> i15 = integer(15);
    RCP<const Integer> im18 = integer(-18);
    RCP<const Integer> im22 = integer(-22);
    RCP<const Integer> i162 = integer(162);
    RCP<const Integer> i100 = integer(100);
    RCP<const Integer> i = integer(40487*40487);
    RCP<const Integer> g, p;

    REQUIRE(primitive_root(outArg(g), *i15) == false);
    REQUIRE(primitive_root(outArg(g), *i100) == false);

    REQUIRE(primitive_root(outArg(g), *im18) == true);
    REQUIRE(multiplicative_order(outArg(p), g, im18) == true );
    REQUIRE(eq(*p, *totient(im18)));

    REQUIRE(primitive_root(outArg(g), *i) == true);
    REQUIRE(multiplicative_order(outArg(p), g, i) == true );
    REQUIRE(eq(*p, *totient(i)));

    std::vector<RCP<const Integer>> roots, v;
    primitive_root_list(roots, *i162);
    REQUIRE(roots.size() == 18);

    roots.clear();
    primitive_root_list(roots, *i100);
    REQUIRE(roots.size() == 0);

    roots.clear();
    primitive_root_list(roots, *im22);
    REQUIRE(roots.size() == 4);
    v = {integer(7), integer(13), integer(17), integer(19)};
    bool same = std::equal(v.begin(), v.end(), roots.begin(),
            SymEngine::RCPBasicKeyEq());
    REQUIRE(same == true);
}

TEST_CASE("test_totient_carmichael(): ntheory", "[ntheory]")
{
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i30 = integer(30);
    RCP<const Integer> im1729 = integer(-1729);

    REQUIRE(eq(*totient(i8), *integer(4)));
    REQUIRE(eq(*totient(i9), *integer(6)));
    REQUIRE(eq(*totient(i30), *integer(8)));
    REQUIRE(eq(*totient(im1729), *integer(1296)));

    REQUIRE(eq(*carmichael(i8), *integer(2)));
    REQUIRE(eq(*carmichael(i9), *integer(6)));
    REQUIRE(eq(*carmichael(i30), *integer(4)));
    REQUIRE(eq(*carmichael(im1729), *integer(36)));
}

TEST_CASE("test_multiplicative_order(): ntheory", "[ntheory]")
{
    RCP<const Integer> i0 = integer(0);
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i13 = integer(13);
    RCP<const Integer> g;

    REQUIRE(multiplicative_order(outArg(g), i0, i9) == false);
    REQUIRE(multiplicative_order(outArg(g), i2, i6) == false);
    REQUIRE(multiplicative_order(outArg(g), i1, i13) == true);
    REQUIRE(eq(*g, *i1));
    REQUIRE(multiplicative_order(outArg(g), i13, i9) == true);
    REQUIRE(eq(*g, *i3));
    REQUIRE(multiplicative_order(outArg(g), i2, i9) == true);
    REQUIRE(eq(*g, *i6));
    REQUIRE(multiplicative_order(outArg(g), i3, i13) == true);
    REQUIRE(eq(*g, *i3));
}

TEST_CASE("test_legendre_jacobi_kronecker(): ntheory", "[ntheory]")
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

    REQUIRE(legendre(*im1, *i23) == -1);
    REQUIRE(legendre(*im1, *i3) == -1);
    REQUIRE(legendre(*i3, *i3) == 0);
    REQUIRE(legendre(*i4, *i5) == 1);

    REQUIRE(jacobi(*im1, *i93) == 1);
    REQUIRE(jacobi(*i4, *i27) == 1);
    REQUIRE(jacobi(*i5, *i115) == 0);
    REQUIRE(jacobi(*i93, *i115) == -1);

    REQUIRE(kronecker(*i8, *i27) == -1);
    REQUIRE(kronecker(*i4, *i8) == 0);
    REQUIRE(kronecker(*i8, *i23) == 1);
}

TEST_CASE("test_nthroot_mod(): ntheory", "[ntheory]")
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

    REQUIRE(nthroot_mod(outArg(nthroot), im1, i2, i23) == false);
    REQUIRE(nthroot_mod(outArg(nthroot), im1, i2, i93) == false);
    REQUIRE(nthroot_mod(outArg(nthroot), i3, i2, i27) == false);
    REQUIRE(nthroot_mod(outArg(nthroot), i18, i2, i27) == false);
    REQUIRE(nthroot_mod(outArg(nthroot), i9, i4, i64) == false);
    REQUIRE(nthroot_mod(outArg(nthroot), im1, i2, i23) == false);
    REQUIRE(nthroot_mod(outArg(nthroot), i2, i3, i105) == false);

    REQUIRE(nthroot_mod(outArg(nthroot), i5, i1, i100) == true);
    REQUIRE(eq(*nthroot, *i5));

    REQUIRE(nthroot_mod(outArg(nthroot), im1, i2, i41) == true);
    rem = integer(nthroot->as_mpz() * nthroot->as_mpz() - im1->as_mpz());
    REQUIRE(divides(*rem, *i41));

    REQUIRE(nthroot_mod(outArg(nthroot), i31, i4, i41) == true);
    nthroot_mod(outArg(nthroot), i32, i10, i41);
    nthroot_mod_list(roots, i1, i10, i100);

    REQUIRE(nthroot_mod(outArg(nthroot), i4, i2, i64) == true);
    rem = integer(nthroot->as_mpz() * nthroot->as_mpz() - i4->as_mpz());
    REQUIRE(divides(*rem, *i64));

    REQUIRE(nthroot_mod(outArg(nthroot), i32, i10, i41) == true);
    rem = rcp_static_cast<const Integer>(nthroot->powint(*i10));
    rem = integer(rem->as_mpz() - i32->as_mpz());
    REQUIRE(divides(*rem, *i41));

    roots.clear();
    nthroot_mod_list(roots, i9, i2, i27);
    REQUIRE(roots.size() == 6);
    v = {integer(3), integer(6), integer(12), integer(15), integer(21), integer(24)};
    bool same = std::equal(v.begin(), v.end(), roots.begin(),
            SymEngine::RCPBasicKeyEq());
    REQUIRE(same == true);

    roots.clear();
    nthroot_mod_list(roots, i1, i18, i105);
    REQUIRE(roots.size() == 24);
}

TEST_CASE("test_powermod(): ntheory", "[ntheory]")
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

    REQUIRE(powermod(outArg(r), i2, im1, i4) == false);
    REQUIRE(powermod(outArg(r), i4, i3, i41) == true);
    REQUIRE(eq(*r, *i23));

    REQUIRE(powermod(outArg(r), i2, i23->divint(*i41), i41) == true);
    REQUIRE(eq(*r, *integer(8)));

    powermod_list(powms, i15, i1->divint(*i18), i105);
    REQUIRE(powms.size() == 6);
}
