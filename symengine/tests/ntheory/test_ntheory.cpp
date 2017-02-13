#include "catch.hpp"
#include <chrono>

#include <symengine/ntheory.h>
#include <symengine/rational.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/real_double.h>

using SymEngine::Basic;
using SymEngine::Integer;
using SymEngine::Rational;
using SymEngine::rational;
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
using SymEngine::mertens;
using SymEngine::integer_class;
using SymEngine::harmonic;
using SymEngine::vec_integer_class;
using SymEngine::zero;
using SymEngine::one;
using SymEngine::minus_one;
using SymEngine::RealDouble;
using SymEngine::real_double;
using SymEngine::SymEngineException;

TEST_CASE("test_gcd_lcm(): ntheory", "[ntheory]")
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i4 = integer(4);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> g = integer(2);
    RCP<const Integer> s = integer(2);
    RCP<const Integer> t = integer(3);

    CHECK(eq(*gcd(*i2, *i4), *integer(2)));
    CHECK(eq(*gcd(*i2, *i3), *integer(1)));
    CHECK(eq(*gcd(*i2, *i6), *integer(2)));
    CHECK(eq(*gcd(*i3, *i6), *integer(3)));

    CHECK(eq(*lcm(*i2, *i4), *integer(4)));
    CHECK(eq(*lcm(*i2, *i3), *integer(6)));
    CHECK(eq(*lcm(*i2, *i6), *integer(6)));
    CHECK(eq(*lcm(*i3, *i6), *integer(6)));

    gcd_ext(outArg(g), outArg(s), outArg(t), *i2, *i3);
    CHECK(eq(*g, *integer(1)));
    CHECK(eq(*g, *add(mul(i2, s), mul(i3, t)))); // check if g = i2*s + i3*t

    gcd_ext(outArg(g), outArg(s), outArg(t), *i3, *i6);
    CHECK(eq(*g, *integer(3)));
    CHECK(eq(*g, *add(mul(i3, s), mul(i6, t)))); // check if g = i3*s + i6*t
}

TEST_CASE("test_nextprime(): ntheory", "[ntheory]")
{
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i6 = integer(6);

    CHECK(eq(*nextprime(*i1), *integer(2)));
    CHECK(eq(*nextprime(*i5), *integer(7)));
    CHECK(eq(*nextprime(*i6), *integer(7)));
}

TEST_CASE("test_probab_prime_p(): ntheory", "[ntheory]")
{
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i6 = integer(6);

    CHECK(probab_prime_p(*i1) == 0);
    // CHECK(probab_prime_p(*i5) == 2); //should only CHECK nonzero
    CHECK(probab_prime_p(*i5) != 0);
    CHECK(probab_prime_p(*i6) == 0);
}

TEST_CASE("test_modular_inverse(): ntheory", "[ntheory]")
{
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i11 = integer(11);
    RCP<const Integer> b;

    CHECK(mod_inverse(outArg(b), *i3, *i5) != 0);
    CHECK(eq(*b, *integer(2)));

    CHECK(mod_inverse(outArg(b), *i3, *i8) != 0);
    CHECK(eq(*b, *integer(3)));

    CHECK(mod_inverse(outArg(b), *i3, *i11) != 0);
    CHECK(eq(*b, *integer(4)));
}

TEST_CASE("test_modulo(): ntheory", "[ntheory]")
{
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> im11 = integer(-11);
    RCP<const Integer> b, c;

    b = mod(*i5, *i3);
    CHECK(eq(*b, *integer(2)));

    b = mod(*im11, *i8);
    CHECK(eq(*b, *integer(-3)));

    b = mod_f(*im11, *i8);
    CHECK(eq(*b, *i5));

    b = quotient(*im11, *i5);
    CHECK(eq(*b, *integer(-2)));

    b = quotient_f(*im11, *i5);
    CHECK(eq(*b, *integer(-3)));

    quotient_mod(outArg(b), outArg(c), *im11, *i5);
    CHECK(eq(*b, *integer(-2)));
    CHECK(eq(*c, *integer(-1)));

    quotient_mod_f(outArg(b), outArg(c), *im11, *i5);
    CHECK(eq(*b, *integer(-3)));
    CHECK(eq(*c, *integer(4)));
}

TEST_CASE("test_fibonacci_lucas(): ntheory", "[ntheory]")
{
    RCP<const Integer> g;
    RCP<const Integer> s;

    CHECK(eq(*fibonacci(1), *integer(1)));
    CHECK(eq(*fibonacci(2), *integer(1)));
    CHECK(eq(*fibonacci(3), *integer(2)));
    CHECK(eq(*fibonacci(5), *integer(5)));

    CHECK(eq(*lucas(1), *integer(1)));
    CHECK(eq(*lucas(2), *integer(3)));
    CHECK(eq(*lucas(3), *integer(4)));
    CHECK(eq(*lucas(5), *integer(11)));

    fibonacci2(outArg(g), outArg(s), 10);
    CHECK(eq(*g, *integer(55)));
    CHECK(eq(*s, *integer(34)));

    lucas2(outArg(g), outArg(s), 10);
    CHECK(eq(*g, *integer(123)));
    CHECK(eq(*s, *integer(76)));
}

TEST_CASE("test_binomial(): ntheory", "[ntheory]")
{
    RCP<const Integer> i10 = integer(10);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i0 = integer(0);
    RCP<const Integer> m10 = integer(-10);

    CHECK(eq(*binomial(*i10, 1), *i10));
    CHECK(eq(*binomial(*i5, 2), *i10));
    CHECK(eq(*binomial(*i5, 10), *i0));
    CHECK(eq(*binomial(*i10, 11), *i0));
    CHECK(eq(*binomial(*i10, 2), *integer(45)));

    CHECK(eq(*binomial(*m10, 3), *integer(-220)));
    CHECK(eq(*binomial(*m10, 2), *integer(55)));
}

TEST_CASE("test_factorial(): ntheory", "[ntheory]")
{
    RCP<const Integer> i1 = integer(1);

    CHECK(eq(*factorial(1), *i1));
    CHECK(eq(*factorial(0), *i1));
    CHECK(eq(*factorial(5), *integer(120)));
    CHECK(eq(*factorial(9), *integer(362880)));
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

    CHECK(factor(outArg(f), *i2) == 0);
    CHECK(factor(outArg(f), *i3) == 0);
    CHECK(factor(outArg(f), *i17) == 0);
    CHECK(factor(outArg(f), *i31) == 0);

    CHECK(factor(outArg(f), *i6) > 0);
    CHECK(divides(*i6, *f));
    CHECK(factor(outArg(f), *i121) > 0);
    CHECK(divides(*i121, *f));
    CHECK(factor(outArg(f), *i122) > 0);
    CHECK(divides(*i122, *f));
    CHECK(factor(outArg(f), *i1001) > 0);
    CHECK(divides(*i1001, *f));
    CHECK(not divides(*i1001, *i6));
    CHECK(factor(outArg(f), *i900) > 0);
    CHECK(divides(*i900, *f));
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

    CHECK(factor_lehman_method(outArg(f), *i23) == 0);
    CHECK(factor_lehman_method(outArg(f), *i31) == 0);
    CHECK(factor_lehman_method(outArg(f), *i47) == 0);

    CHECK(factor_lehman_method(outArg(f), *i21) > 0);
    CHECK((divides(*i21, *f) and not eq(*f, *i1)
           and not eq(*f, *i21))); // Lehman's method returns only a proper
                                   // divisor when composite
    CHECK(factor_lehman_method(outArg(f), *i121) > 0);
    CHECK((divides(*i121, *f) and not eq(*f, *i1) and not eq(*f, *i121)));
    CHECK(factor_lehman_method(outArg(f), *i122) > 0);
    CHECK((divides(*i122, *f) and not eq(*f, *i1) and not eq(*f, *i122)));
    CHECK(factor_lehman_method(outArg(f), *i900) > 0);
    CHECK((divides(*i900, *f) and not eq(*f, *i1) and not eq(*f, *i900)));
    CHECK(factor_lehman_method(outArg(f), *i1001) > 0);
    CHECK((divides(*i1001, *f) and not eq(*f, *i1) and not eq(*f, *i1001)));

    CHECK_THROWS_AS(factor_lehman_method(outArg(f), *i1), SymEngineException);
}

TEST_CASE("test_factor_pollard_pm1_method(): ntheory", "[ntheory]")
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i23 = integer(23);
    RCP<const Integer> i31 = integer(31);
    RCP<const Integer> i47 = integer(47);
    RCP<const Integer> i121 = integer(121);
    RCP<const Integer> i122 = integer(122);
    RCP<const Integer> i900 = integer(900);
    RCP<const Integer> i1001 = integer(1001);
    RCP<const Integer> i1850 = integer(1850);
    RCP<const Integer> f;

    CHECK(factor_pollard_pm1_method(outArg(f), *i23) == 0);
    CHECK(factor_pollard_pm1_method(outArg(f), *i31) == 0);
    CHECK(factor_pollard_pm1_method(outArg(f), *i47) == 0);

    CHECK((factor_pollard_pm1_method(outArg(f), *i121) == 0
           or divides(*i121, *f)));
    CHECK((factor_pollard_pm1_method(outArg(f), *i122) == 0
           or divides(*i122, *f)));
    CHECK((factor_pollard_pm1_method(outArg(f), *i900) == 0
           or divides(*i900, *f)));
    CHECK((factor_pollard_pm1_method(outArg(f), *i1001, 20) == 0
           or divides(*i1001, *f)));
    CHECK((factor_pollard_pm1_method(outArg(f), *i1850) == 0
           or divides(*i1850, *f)));

#if SYMENGINE_INTEGER_CLASS != SYMENGINE_BOOSTMP
    CHECK_THROWS_AS(factor_pollard_pm1_method(outArg(f), *i2),
                    SymEngineException);
#endif
}

TEST_CASE("test_factor_pollard_rho_method(): ntheory", "[ntheory]")
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i23 = integer(23);
    RCP<const Integer> i31 = integer(31);
    RCP<const Integer> i47 = integer(47);
    RCP<const Integer> i121 = integer(121);
    RCP<const Integer> i122 = integer(122);
    RCP<const Integer> i900 = integer(900);
    RCP<const Integer> i1001 = integer(1001);
    RCP<const Integer> i1850 = integer(1850);
    RCP<const Integer> f;

    CHECK(factor_pollard_rho_method(outArg(f), *i23) == 0);
    CHECK(factor_pollard_rho_method(outArg(f), *i31) == 0);
    CHECK(factor_pollard_rho_method(outArg(f), *i47) == 0);

    CHECK((factor_pollard_rho_method(outArg(f), *i121) == 0
           or divides(*i121, *f)));
    CHECK((factor_pollard_rho_method(outArg(f), *i122) == 0
           or divides(*i122, *f)));
    CHECK((factor_pollard_rho_method(outArg(f), *i900) == 0
           or divides(*i900, *f)));
    CHECK((factor_pollard_rho_method(outArg(f), *i1001) == 0
           or divides(*i1001, *f)));
    CHECK((factor_pollard_rho_method(outArg(f), *i1850) == 0
           or divides(*i1850, *f)));

#if SYMENGINE_INTEGER_CLASS != SYMENGINE_BOOSTMP
    CHECK_THROWS_AS(factor_pollard_rho_method(outArg(f), *i2),
                    SymEngineException);
#endif
}

TEST_CASE("test_sieve(): ntheory", "[ntheory]")
{
    const int MAX = 100003;
    std::vector<unsigned> v;
    auto t1 = std::chrono::high_resolution_clock::now();
    SymEngine::Sieve::generate_primes(v, MAX);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;
    std::cout << "Number of primes up to " << MAX << ": " << v.size()
              << std::endl;
    CHECK(v.size() == 9593);
}

TEST_CASE("test_sieve_iterator(): ntheory", "[ntheory]")
{
    const int MAX = 100003;
    int count = 0, prime;
    SymEngine::Sieve::iterator pi(MAX);
    auto t1 = std::chrono::high_resolution_clock::now();
    while ((prime = pi.next_prime()) <= MAX) {
        count++;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;
    std::cout << "Number of primes up to " << MAX << ": " << count << std::endl;
    CHECK(count == 9593);
}

// helper function for test_primefactors
void _test_primefactors(const RCP<const Integer> &a, unsigned size)
{
    std::vector<RCP<const Integer>> primes;

    prime_factors(primes, *a);
    CHECK(primes.size() == size);

    for (auto &it : primes) {
        CHECK(divides(*a, *it) == true);
        CHECK(probab_prime_p(*it) > 0);
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
    _test_primefactors(minus_one, 0);
}

void _test_prime_factor_multiplicities(const RCP<const Integer> &a)
{
    unsigned multiplicity;
    RCP<const Integer> _a = a;
    map_integer_uint prime_mul;

    prime_factor_multiplicities(prime_mul, *a);

    for (auto it : prime_mul) {
        multiplicity = it.second;
        while (multiplicity) {
            _a = rcp_dynamic_cast<const Integer>(div(_a, it.first));
            multiplicity--;
        }
    }

    CHECK(eq(*_a, *integer(1)));
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
    r1 = bernoulli(12);
    r2 = Rational::from_two_ints(*integer(-691), *integer(2730));
    CHECK(eq(*r1, *r2));
}

TEST_CASE("test_crt(): ntheory", "[ntheory]")
{
    RCP<const Integer> g;
    std::vector<RCP<const Integer>> r, m;
    r = {integer(21), integer(31), integer(6), integer(17), integer(83)};
    m = {integer(30), integer(35), integer(45), integer(77), integer(88)};
    CHECK(crt(outArg(g), r, m) == true);
    CHECK(eq(*g, *integer(9411)));

    r = {integer(3), integer(2), integer(1)};
    m = {integer(20), integer(14), integer(11)};
    CHECK(crt(outArg(g), r, m) == false);

    r = {integer(3), integer(21), integer(23), integer(9), integer(45)};
    m = {integer(15), integer(48), integer(55), integer(61), integer(66)};
    CHECK(crt(outArg(g), r, m) == true);
    CHECK(eq(*g, *integer(12453)));

    r = {integer(-1), integer(-1), integer(-1), integer(-1), integer(-1)};
    m = {integer(2), integer(3), integer(4), integer(5), integer(6)};
    CHECK(crt(outArg(g), r, m) == true);
    CHECK(eq(*g, *integer(59)));

    r = {integer(21), integer(31), integer(6), integer(17)};
    m = {integer(30), integer(35), integer(45), integer(77), integer(88)};
    CHECK_THROWS_AS(crt(outArg(g), r, m), SymEngineException);

    m = {};
    CHECK_THROWS_AS(crt(outArg(g), r, m), SymEngineException);
}

TEST_CASE("test_primitive_root(): ntheory", "[ntheory]")
{
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i15 = integer(15);
    RCP<const Integer> im18 = integer(-18);
    RCP<const Integer> im22 = integer(-22);
    RCP<const Integer> i162 = integer(162);
    RCP<const Integer> i100 = integer(100);
    RCP<const Integer> i = integer(40487 * 40487);
    RCP<const Integer> g, p;

    CHECK(primitive_root(outArg(g), *i15) == false);
    CHECK(primitive_root(outArg(g), *i100) == false);

    CHECK(primitive_root(outArg(g), *im18) == true);
    CHECK(multiplicative_order(outArg(p), g, im18) == true);
    CHECK(eq(*p, *totient(im18)));

    CHECK(primitive_root(outArg(g), *i) == true);
    CHECK(multiplicative_order(outArg(p), g, i) == true);
    CHECK(eq(*p, *totient(i)));

    std::vector<RCP<const Integer>> roots, v;
    primitive_root_list(roots, *i162);
    CHECK(roots.size() == 18);

    roots.clear();
    primitive_root_list(roots, *i100);
    CHECK(roots.size() == 0);

    roots.clear();
    primitive_root_list(roots, *im22);
    CHECK(roots.size() == 4);
    v = {integer(7), integer(13), integer(17), integer(19)};
    bool same = std::equal(v.begin(), v.end(), roots.begin(),
                           SymEngine::RCPBasicKeyEq());
    CHECK(same == true);

    roots.clear();
    primitive_root_list(roots, *one);
    CHECK(roots.size() == 0);

    roots.clear();
    primitive_root_list(roots, *i2);
    CHECK(roots.size() == 1);

    CHECK(primitive_root(outArg(g), *one) == false);
    CHECK(primitive_root(outArg(g), *i2) == true);
}

TEST_CASE("test_totient_carmichael(): ntheory", "[ntheory]")
{
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i30 = integer(30);
    RCP<const Integer> im1729 = integer(-1729);

    CHECK(eq(*totient(i8), *integer(4)));
    CHECK(eq(*totient(i9), *integer(6)));
    CHECK(eq(*totient(i30), *integer(8)));
    CHECK(eq(*totient(im1729), *integer(1296)));
    CHECK(eq(*totient(zero), *integer(1)));

    CHECK(eq(*carmichael(i8), *integer(2)));
    CHECK(eq(*carmichael(i9), *integer(6)));
    CHECK(eq(*carmichael(i30), *integer(4)));
    CHECK(eq(*carmichael(im1729), *integer(36)));
    CHECK(eq(*carmichael(zero), *integer(1)));
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

    CHECK(multiplicative_order(outArg(g), i0, i9) == false);
    CHECK(multiplicative_order(outArg(g), i2, i6) == false);
    CHECK(multiplicative_order(outArg(g), i1, i13) == true);
    CHECK(eq(*g, *i1));
    CHECK(multiplicative_order(outArg(g), i13, i9) == true);
    CHECK(eq(*g, *i3));
    CHECK(multiplicative_order(outArg(g), i2, i9) == true);
    CHECK(eq(*g, *i6));
    CHECK(multiplicative_order(outArg(g), i3, i13) == true);
    CHECK(eq(*g, *i3));
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

    CHECK(legendre(*im1, *i23) == -1);
    CHECK(legendre(*im1, *i3) == -1);
    CHECK(legendre(*i3, *i3) == 0);
    CHECK(legendre(*i4, *i5) == 1);

    CHECK(jacobi(*im1, *i93) == 1);
    CHECK(jacobi(*i4, *i27) == 1);
    CHECK(jacobi(*i5, *i115) == 0);
    CHECK(jacobi(*i93, *i115) == -1);

    CHECK(kronecker(*i8, *i27) == -1);
    CHECK(kronecker(*i4, *i8) == 0);
    CHECK(kronecker(*i8, *i23) == 1);
}

TEST_CASE("test_nthroot_mod(): ntheory", "[ntheory]")
{
    RCP<const Integer> im1 = integer(-1);
    RCP<const Integer> im4 = integer(-4);
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
    RCP<const Integer> i65 = integer(65);
    RCP<const Integer> i93 = integer(93);
    RCP<const Integer> i100 = integer(100);
    RCP<const Integer> i105 = integer(105);
    RCP<const Integer> nthroot, rem;
    std::vector<RCP<const Integer>> roots, v;

    CHECK(nthroot_mod(outArg(nthroot), im1, i2, i23) == false);
    CHECK(nthroot_mod(outArg(nthroot), im1, i2, zero) == false);
    CHECK(nthroot_mod(outArg(nthroot), im1, i2, i1) == true);
    CHECK(nthroot_mod(outArg(nthroot), im1, i2, i93) == false);
    CHECK(nthroot_mod(outArg(nthroot), i3, i2, i27) == false);
    CHECK(nthroot_mod(outArg(nthroot), i18, i2, i27) == false);
    CHECK(nthroot_mod(outArg(nthroot), i9, i4, i64) == false);
    CHECK(nthroot_mod(outArg(nthroot), im1, i2, i23) == false);
    CHECK(nthroot_mod(outArg(nthroot), i2, i3, i105) == false);

    CHECK(nthroot_mod(outArg(nthroot), i5, i1, i100) == true);
    CHECK(eq(*nthroot, *i5));

    CHECK(nthroot_mod(outArg(nthroot), im1, i2, i41) == true);
    rem = integer(nthroot->as_integer_class() * nthroot->as_integer_class()
                  - im1->as_integer_class());
    CHECK(divides(*rem, *i41));

    CHECK(nthroot_mod(outArg(nthroot), i31, i4, i41) == true);
    nthroot_mod(outArg(nthroot), i32, i10, i41);
    nthroot_mod_list(roots, i1, i10, i100);

    CHECK(nthroot_mod(outArg(nthroot), i4, i2, i64) == true);
    rem = integer(nthroot->as_integer_class() * nthroot->as_integer_class()
                  - i4->as_integer_class());
    CHECK(divides(*rem, *i64));

    CHECK(nthroot_mod(outArg(nthroot), i32, i10, i41) == true);
    rem = rcp_static_cast<const Integer>(nthroot->powint(*i10));
    rem = integer(rem->as_integer_class() - i32->as_integer_class());
    CHECK(divides(*rem, *i41));

    roots.clear();
    nthroot_mod_list(roots, i9, i2, i27);
    CHECK(roots.size() == 6);
    v = {integer(3),  integer(6),  integer(12),
         integer(15), integer(21), integer(24)};
    bool same = std::equal(v.begin(), v.end(), roots.begin(),
                           SymEngine::RCPBasicKeyEq());
    CHECK(same == true);

    roots.clear();
    nthroot_mod_list(roots, i1, i18, i105);
    CHECK(roots.size() == 24);

    roots.clear();
    nthroot_mod_list(roots, im4, i4, i65);
    CHECK(roots.size() == 16);
    v = {integer(4),  integer(6),  integer(7),  integer(9),
         integer(17), integer(19), integer(22), integer(32),
         integer(33), integer(43), integer(46), integer(48),
         integer(56), integer(58), integer(59), integer(61)};
    same = std::equal(v.begin(), v.end(), roots.begin(),
                      SymEngine::RCPBasicKeyEq());
    CHECK(same == true);

    roots.clear();
    nthroot_mod_list(roots, i1, i18, zero);
    CHECK(roots.size() == 0);

    roots.clear();
    nthroot_mod_list(roots, i1, i18, i1);
    CHECK(roots.size() == 1);
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
    RCP<const RealDouble> rd = real_double(0.8);
    RCP<const Integer> r;
    std::vector<RCP<const Integer>> powms;

    CHECK(powermod(outArg(r), i2, im1, i4) == false);
    CHECK(powermod(outArg(r), i4, i3, i41) == true);
    CHECK(eq(*r, *i23));

    CHECK(powermod(outArg(r), i2, i23->divint(*i41), i41) == true);
    CHECK(eq(*r, *integer(8)));

    CHECK(powermod(outArg(r), i2, rd, i41) == false);

    powermod_list(powms, i15, i1->divint(*i18), i105);
    CHECK(powms.size() == 6);
}

TEST_CASE("test_quadratic_residues(): ntheory", "[ntheory]")
{

    vec_integer_class i1 = {integer_class(0)};
    vec_integer_class i2 = {integer_class(0), integer_class(1)};
    vec_integer_class i3 = {integer_class(0), integer_class(1)};
    vec_integer_class i4 = {integer_class(0), integer_class(1)};
    vec_integer_class i5
        = {integer_class(0), integer_class(1), integer_class(4)};
    vec_integer_class i7 = {integer_class(0), integer_class(1),
                            integer_class(2), integer_class(4)};
    vec_integer_class i100
        = {integer_class(0),  integer_class(1),  integer_class(4),
           integer_class(9),  integer_class(16), integer_class(21),
           integer_class(24), integer_class(25), integer_class(29),
           integer_class(36), integer_class(41), integer_class(44),
           integer_class(49), integer_class(56), integer_class(61),
           integer_class(64), integer_class(69), integer_class(76),
           integer_class(81), integer_class(84), integer_class(89),
           integer_class(96)};

    const RCP<const Integer> a1 = integer(1);
    const RCP<const Integer> a2 = integer(2);
    const RCP<const Integer> a3 = integer(3);
    const RCP<const Integer> a4 = integer(4);
    const RCP<const Integer> a5 = integer(5);
    const RCP<const Integer> a7 = integer(7);
    const RCP<const Integer> a100 = integer(100);

    std::cout << "Quadratic Residues:" << std::endl;
    CHECK(quadratic_residues(*a1) == i1);
    CHECK(quadratic_residues(*a2) == i2);
    CHECK(quadratic_residues(*a3) == i3);
    CHECK(quadratic_residues(*a4) == i4);
    CHECK(quadratic_residues(*a5) == i5);
    CHECK(quadratic_residues(*a7) == i7);
    CHECK(quadratic_residues(*a100) == i100);

    CHECK_THROWS_AS(quadratic_residues(*zero), SymEngineException);
}

TEST_CASE("test_is_quad_residue(): ntheory", "[ntheory]")
{
    const RCP<const Integer> a1 = integer(1);
    const RCP<const Integer> ma1 = integer(-1);
    const RCP<const Integer> a2 = integer(2);
    const RCP<const Integer> a3 = integer(3);
    const RCP<const Integer> a4 = integer(4);
    const RCP<const Integer> a7 = integer(7);
    const RCP<const Integer> a9 = integer(9);
    const RCP<const Integer> a100 = integer(100);

    const RCP<const Integer> t0 = integer(0);
    const RCP<const Integer> t1 = integer(1);
    const RCP<const Integer> t3 = integer(3);
    const RCP<const Integer> t4 = integer(4);
    const RCP<const Integer> nt5 = integer(-5);
    const RCP<const Integer> t7 = integer(7);
    const RCP<const Integer> t56 = integer(56);
    const RCP<const Integer> t89 = integer(89);

    std::cout << "Is_Quadratic_Residue:" << std::endl;
    CHECK(is_quad_residue(*t0, *a1) == true);
    CHECK(is_quad_residue(*t0, *ma1) == true);
    CHECK(is_quad_residue(*t1, *a1) == true);
    CHECK(is_quad_residue(*t1, *a2) == true);
    CHECK(is_quad_residue(*t0, *a4) == true);
    CHECK(is_quad_residue(*t1, *a4) == true);
    CHECK(is_quad_residue(*t4, *a4) == true);
    CHECK(is_quad_residue(*nt5, *a3) == true);
    CHECK(is_quad_residue(*t4, *a7) == true);
    CHECK(is_quad_residue(*t4, *a9) == true);
    CHECK(is_quad_residue(*t7, *a9) == true);
    CHECK(is_quad_residue(*t56, *a100) == true);
    CHECK(is_quad_residue(*t7, *a100) == false);
    CHECK(is_quad_residue(*nt5, *a4) == false);
    CHECK(is_quad_residue(*t4, *a100) == true);
    CHECK(is_quad_residue(*t89, *a100) == true);
    CHECK(is_quad_residue(*t3, *a100) == false);

    CHECK_THROWS_AS(is_quad_residue(*t3, *t0), SymEngineException);
}

TEST_CASE("test_is_nth_residue(): ntheory", "[ntheory]")
{
    const RCP<const Integer> im1 = integer(-1);
    const RCP<const Integer> i1 = integer(1);
    const RCP<const Integer> i2 = integer(2);
    const RCP<const Integer> i3 = integer(3);
    const RCP<const Integer> i4 = integer(4);
    const RCP<const Integer> i5 = integer(5);
    const RCP<const Integer> i9 = integer(9);
    const RCP<const Integer> i10 = integer(10);
    const RCP<const Integer> i16 = integer(16);
    const RCP<const Integer> i18 = integer(18);
    const RCP<const Integer> i23 = integer(23);
    const RCP<const Integer> i27 = integer(27);
    const RCP<const Integer> i31 = integer(31);
    const RCP<const Integer> i32 = integer(32);
    const RCP<const Integer> i41 = integer(41);
    const RCP<const Integer> i64 = integer(64);
    const RCP<const Integer> i93 = integer(93);
    const RCP<const Integer> i100 = integer(100);
    const RCP<const Integer> i105 = integer(105);

    CHECK(is_nth_residue(*im1, *i2, *i23) == false);
    CHECK(is_nth_residue(*im1, *i2, *i93) == false);
    CHECK(is_nth_residue(*i3, *i2, *i27) == false);
    CHECK(is_nth_residue(*i18, *i2, *i27) == false);
    CHECK(is_nth_residue(*i9, *i4, *i64) == false);
    CHECK(is_nth_residue(*im1, *i2, *i23) == false);
    CHECK(is_nth_residue(*i2, *i3, *i105) == false);
    CHECK(is_nth_residue(*i5, *i1, *i100) == true);
    CHECK(is_nth_residue(*im1, *i2, *i41) == true);
    CHECK(is_nth_residue(*i31, *i4, *i41) == true);
    CHECK(is_nth_residue(*i4, *i2, *i64) == true);
    CHECK(is_nth_residue(*i32, *i10, *i41) == true);
    CHECK(is_nth_residue(*i32, *i10, *zero) == false);
    CHECK(is_nth_residue(*i32, *i10, *i1) == true);
    CHECK(is_nth_residue(*i32, *i10, *im1) == true);
}

TEST_CASE("test_mobius(): ntheory", "[ntheory]")
{
    RCP<const Integer> i1 = integer(1);
    RCP<const Integer> i2 = integer(2);
    RCP<const Integer> i3 = integer(3);
    RCP<const Integer> i4 = integer(4);
    RCP<const Integer> i5 = integer(5);
    RCP<const Integer> i6 = integer(6);
    RCP<const Integer> i7 = integer(7);
    RCP<const Integer> i8 = integer(8);
    RCP<const Integer> i9 = integer(9);
    RCP<const Integer> i10 = integer(10);

    CHECK(mobius(*i1) == 1);
    CHECK(mobius(*i2) == -1);
    CHECK(mobius(*i3) == -1);
    CHECK(mobius(*i4) == 0);
    CHECK(mobius(*i5) == -1);
    CHECK(mobius(*i6) == 1);
    CHECK(mobius(*i7) == -1);
    CHECK(mobius(*i8) == 0);
    CHECK(mobius(*i9) == 0);
    CHECK(mobius(*i10) == 1);

    CHECK_THROWS_AS(mobius(*minus_one), SymEngineException);
}

TEST_CASE("test_mertens(): ntheory", "[ntheory]")
{
    CHECK(mertens(1) == 1);
    CHECK(mertens(2) == 0);
    CHECK(mertens(4) == -1);
    CHECK(mertens(12) == -2);
    CHECK(mertens(13) == -3);
    CHECK(mertens(22) == -1);
    CHECK(mertens(31) == -4);
    CHECK(mertens(36) == -1);
    CHECK(mertens(39) == 0);
    CHECK(mertens(113) == -5);
}

TEST_CASE("test_harmonic(): ntheory", "[ntheory]")
{
    RCP<const Number> r1, r2;
    r1 = harmonic(4, 1);
    r2 = rational(25, 12);
    CHECK(eq(*r1, *r2));

    r1 = harmonic(3, 2);
    r2 = rational(49, 36);
    CHECK(eq(*r1, *r2));
}

TEST_CASE("test_factor_trial_division(): ntheory", "[ntheory]")
{
    RCP<const Integer> i47 = integer(47);
    RCP<const Integer> f;

    CHECK(factor_trial_division(outArg(f), *i47) == 0);
}