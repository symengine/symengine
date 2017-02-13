#include "catch.hpp"

#include <symengine/mp_class.h>
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/complex.h>
#include <symengine/complex_double.h>
#include <symengine/constants.h>
#include <iostream>

using std::cout;
using std::endl;

using SymEngine::integer_class;
using SymEngine::rational_class;
using SymEngine::mp_get_ui;
using SymEngine::mp_sign;
using SymEngine::mp_abs;

using namespace SymEngine;

TEST_CASE("operators: integer_class", "[integer_class]")
{
    integer_class i, j;
    rational_class r, p;

    // shift operators
    // Shift operators of negatives are unnecessary and
    // undefined behaviour for C++ standard
    /*CHECK((integer_class(-1024) << 3) == -8192);
    CHECK((integer_class(-1024) >> 3) == -128);
    CHECK((integer_class(-768) << 5) == -24576);
    CHECK((integer_class(-768) >> 5) == -24);
    CHECK((integer_class(-500) << 3) == -4000);
    CHECK((integer_class(-500) >> 3) == -63);
    CHECK((integer_class(-5) << 1) == -10);
    CHECK((integer_class(-5) >> 1) == -3);
    CHECK((integer_class(-4) << 0) == -4);
    CHECK((integer_class(-4) >> 0) == -4);
    CHECK((integer_class(-2) << 10) == -2048);
    CHECK((integer_class(-2) >> 10) == -1);
    CHECK((integer_class(-1) << 4) == -16);
    CHECK((integer_class(-1) >> 4) == -1);*/
    CHECK((integer_class(0) << 5) == 0);
    CHECK((integer_class(0) >> 5) == 0);
    CHECK((integer_class(1) << 2) == 4);
    CHECK((integer_class(1) >> 2) == 0);
    CHECK((integer_class(2) << 4) == 32);
    CHECK((integer_class(2) >> 4) == 0);
    CHECK((integer_class(4) << 1) == 8);
    CHECK((integer_class(4) >> 1) == 2);
    CHECK((integer_class(5) << 6) == 320);
    CHECK((integer_class(5) >> 6) == 0);
    CHECK((integer_class(500) << 1) == 1000);
    CHECK((integer_class(500) >> 1) == 250);
    CHECK((integer_class(768) << 2) == 3072);
    CHECK((integer_class(768) >> 2) == 192);
    CHECK((integer_class(1024) << 3) == 8192);
    CHECK((integer_class(1024) >> 3) == 128);

    // bitwise operators
    i = 9;                // i == 1001
    j = 12;               // j == 1100
    CHECK((i & j) == 8);  // bitwise and
    CHECK((i | j) == 13); // bitwise or
    CHECK((i ^ j) == 5);  // bitwise exclusive or

    // modulus operator (truncated)
    CHECK((integer_class(-1) % integer_class(1)) == 0);
    CHECK((integer_class(0) % integer_class(1)) == 0);
    CHECK((integer_class(1) % integer_class(1)) == 0);
    CHECK((integer_class(-1) % integer_class(2)) == -1);
    CHECK((integer_class(0) % integer_class(2)) == 0);
    CHECK((integer_class(1) % integer_class(2)) == 1);
    CHECK((integer_class(-1) % integer_class(-1)) == 0);
    CHECK((integer_class(0) % integer_class(-1)) == 0);
    CHECK((integer_class(1) % integer_class(-1)) == 0);
    CHECK((integer_class(-1) % integer_class(-2)) == -1);
    CHECK((integer_class(0) % integer_class(-2)) == 0);
    CHECK((integer_class(1) % integer_class(-2)) == 1);
    CHECK((integer_class(2) % integer_class(1)) == 0);
    CHECK((integer_class(2) % integer_class(-1)) == 0);
    CHECK((integer_class(-2) % integer_class(1)) == 0);
    CHECK((integer_class(-2) % integer_class(-1)) == 0);
    CHECK((integer_class(-8) % integer_class(3)) == -2);
    CHECK((integer_class(-8) % integer_class(-3)) == -2);
    CHECK((integer_class(8) % integer_class(-3)) == 2);
    CHECK((integer_class(8) % integer_class(-3)) == 2);
    CHECK((integer_class(8) % integer_class(3)) == 2);
    CHECK((integer_class(100) % integer_class(17)) == 15);

    // compound modulus operator
    j = -9;
    i = 7;
    j %= i;
    CHECK(j == -2);

    // move constructor and move assignment
    integer_class n = 5;
    integer_class m(std::move(n));
    i = std::move(m);

    // construction of rational_class
    // r = rational_class(integer_class(2),3);  fails!
    r = rational_class(integer_class(2), integer_class(3));

    // truncated division
    j = integer_class(12) / integer_class(5);
    CHECK(j == 2);
    j = integer_class(-12) / integer_class(5);
    CHECK(j == -2);
}

TEST_CASE("powers and roots: integer_class", "[integer_class]")
{
    integer_class res, i;

    // mp_pow_ui
    mp_pow_ui(res, -1, 0);
    CHECK(res == 1);
    mp_pow_ui(res, 0, 0);
    CHECK(res == 1); // gmp doc says 0**0 == 1
    mp_pow_ui(res, 1, 0);
    CHECK(res == 1);
    mp_pow_ui(res, -1, 1);
    CHECK(res == -1);
    mp_pow_ui(res, 0, 1);
    CHECK(res == 0);
    mp_pow_ui(res, 1, 1);
    CHECK(res == 1);
    mp_pow_ui(res, -1, 2);
    CHECK(res == 1);
    mp_pow_ui(res, 0, 2);
    CHECK(res == 0);
    mp_pow_ui(res, 1, 2);
    CHECK(res == 1);
    mp_pow_ui(res, -2, 0);
    CHECK(res == 1);
    mp_pow_ui(res, -1, 3);
    CHECK(res == -1);
    mp_pow_ui(res, -2, 1);
    CHECK(res == -2);
    mp_pow_ui(res, -2, 2);
    CHECK(res == 4);
    mp_pow_ui(res, -2, 3);
    CHECK(res == -8);
    mp_pow_ui(res, 2, 1);
    CHECK(res == 2);
    mp_pow_ui(res, 2, 3);
    CHECK(res == 8);

    // mp_powm
    mp_powm(res, -1, -1, -1);
    CHECK(res == 0);
    mp_powm(res, -1, -1, 1);
    CHECK(res == 0);
    mp_powm(res, -1, 0, -1);
    CHECK(res == 0);
    mp_powm(res, -1, 0, 1);
    CHECK(res == 0);
    mp_powm(res, -1, 1, -1);
    CHECK(res == 0);
    mp_powm(res, -1, 1, 1);
    CHECK(res == 0);
    mp_powm(res, 0, -1, -1);
    CHECK(res == 0);
    mp_powm(res, 0, -1, 1);
    CHECK(res == 0);
    mp_powm(res, 0, 0, -1);
    CHECK(res == 0);
    mp_powm(res, 0, 0, 1);
    CHECK(res == 0);
    mp_powm(res, 0, 1, -1);
    CHECK(res == 0);
    mp_powm(res, 0, 1, 1);
    CHECK(res == 0);
    mp_powm(res, 1, -1, -1);
    CHECK(res == 0);
    mp_powm(res, 1, -1, 1);
    CHECK(res == 0);
    mp_powm(res, 1, 0, -1);
    CHECK(res == 0);
    mp_powm(res, 1, 0, 1);
    CHECK(res == 0);
    mp_powm(res, 1, 1, -1);
    CHECK(res == 0);
    mp_powm(res, 1, 1, 1);
    CHECK(res == 0);
    mp_powm(res, -2, 3, 2);
    CHECK(res == 0);
    mp_powm(res, 3, -1, 5);
    CHECK(res == 2);
    mp_powm(res, -3, -1, 5);
    CHECK(res == 3);
    mp_powm(res, 3, 2, -5);
    CHECK(res == 4);
    mp_powm(res, 4, -2, 9);
    CHECK(res == 4);
    mp_powm(res, -4, 2, 9);
    CHECK(res == 7);
    mp_powm(res, 4, 2, 6);
    CHECK(res == 4);
    mp_powm(res, 9, -2, 4);
    CHECK(res == 1);
    mp_powm(res, 10, 2, 4);
    CHECK(res == 0);
    mp_powm(res, -9, -2, 4);
    CHECK(res == 1);
    mp_powm(res, -12, -3, 35);
    CHECK(res == 8);
    mp_powm(res, -3, -9, 10);
    CHECK(res == 3);
    mp_powm(res, -11, -6, 10);
    CHECK(res == 1);
    mp_powm(res, -4, 1, 5);
    CHECK(res == 1);
    mp_powm(res, -3, 2, 7);
    CHECK(res == 2);
    mp_powm(res, -3, 3, 7);
    CHECK(res == 1);
    mp_powm(res, -3, 1, 7);
    CHECK(res == 4);

    // mp_sqrt
    CHECK(mp_sqrt(0) == 0);
    CHECK(mp_sqrt(1) == 1);
    CHECK(mp_sqrt(2) == 1);
    CHECK(mp_sqrt(3) == 1);
    CHECK(mp_sqrt(4) == 2);
    CHECK(mp_sqrt(5) == 2);
    CHECK(mp_sqrt(9) == 3);
    CHECK(mp_sqrt(10) == 3);
    CHECK(mp_sqrt(24) == 4);
    CHECK(mp_sqrt(25) == 5);
    CHECK(mp_sqrt(26) == 5);
    CHECK(mp_sqrt(35) == 5);
    CHECK(mp_sqrt(36) == 6);
    CHECK(mp_sqrt(37) == 6);
    CHECK(mp_sqrt(288) == 16);
    CHECK(mp_sqrt(289) == 17);
    CHECK(mp_sqrt(290) == 17);
    CHECK(mp_sqrt(14640) == 120);
    CHECK(mp_sqrt(14641) == 121);
    CHECK(mp_sqrt(14642) == 121);

    // mp_root
    mp_root(res, 64, 3);
    CHECK(res == 4);
    mp_root(res, integer_class(1234567890123456789), 3);
    CHECK(res == 1072765);
    bool is_perfect_root = mp_root(res, -64, 3);
    CHECK(is_perfect_root);
    CHECK(res == -4);

    // mp_rootrem
    integer_class rem;
    mp_rootrem(res, rem, 50, 2);
    CHECK(res == 7);
    CHECK(rem == 1);

    // mp_perfect_power_p
    CHECK(mp_perfect_power_p(integer_class(1024)));
    CHECK(!mp_perfect_power_p(integer_class(1025)));
    mp_pow_ui(res, integer_class(6), 7);
    CHECK(mp_perfect_power_p(res));
    CHECK(mp_perfect_power_p(integer_class(9)));
    CHECK(mp_perfect_power_p(integer_class(1)));
    CHECK(mp_perfect_power_p(27));
    CHECK(mp_perfect_power_p(-27));
    CHECK(mp_perfect_power_p(-64));
    CHECK(mp_perfect_power_p(-32));

    // mp_perfect_square_p
    CHECK(mp_perfect_square_p(49));
    CHECK(!mp_perfect_square_p(50));
}

TEST_CASE("factors and multiples: integer_class", "[integer_class]")
{
    integer_class quo, rem, res, gcd;

    // mp_fdiv_qr
    mp_fdiv_qr(quo, rem, -1, 1);
    CHECK(quo == -1);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, 0, 1);
    CHECK(quo == 0);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, 1, 1);
    CHECK(quo == 1);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, -1, 2);
    CHECK(quo == -1);
    CHECK(rem == 1);
    mp_fdiv_qr(quo, rem, 0, 2);
    CHECK(quo == 0);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, 1, 2);
    CHECK(quo == 0);
    CHECK(rem == 1);
    mp_fdiv_qr(quo, rem, -1, -1);
    CHECK(quo == 1);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, 0, -1);
    CHECK(quo == 0);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, 1, -1);
    CHECK(quo == -1);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, -1, -2);
    CHECK(quo == 0);
    CHECK(rem == -1);
    mp_fdiv_qr(quo, rem, 0, -2);
    CHECK(quo == 0);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, 1, -2);
    CHECK(quo == -1);
    CHECK(rem == -1);
    mp_fdiv_qr(quo, rem, 2, 1);
    CHECK(quo == 2);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, 2, -1);
    CHECK(quo == -2);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, -2, 1);
    CHECK(quo == -2);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, -2, -1);
    CHECK(quo == 2);
    CHECK(rem == 0);
    mp_fdiv_qr(quo, rem, -8, 3);
    CHECK(quo == -3);
    CHECK(rem == 1);
    mp_fdiv_qr(quo, rem, -8, -3);
    CHECK(quo == 2);
    CHECK(rem == -2);
    mp_fdiv_qr(quo, rem, 8, -3);
    CHECK(quo == -3);
    CHECK(rem == -1);
    mp_fdiv_qr(quo, rem, 8, -3);
    CHECK(quo == -3);
    CHECK(rem == -1);
    mp_fdiv_qr(quo, rem, 8, 3);
    CHECK(quo == 2);
    CHECK(rem == 2);
    mp_fdiv_qr(quo, rem, -100, -17);
    CHECK(quo == 5);
    CHECK(rem == -15);
    mp_fdiv_qr(quo, rem, -100, 17);
    CHECK(quo == -6);
    CHECK(rem == 2);
    mp_fdiv_qr(quo, rem, 100, -17);
    CHECK(quo == -6);
    CHECK(rem == -2);
    mp_fdiv_qr(quo, rem, 100, 17);
    CHECK(quo == 5);
    CHECK(rem == 15);
    mp_fdiv_qr(quo, rem, 369, 12);
    CHECK(quo == 30);
    CHECK(rem == 9);
    mp_fdiv_qr(quo, rem, 123456789, 12345);
    CHECK(quo == 10000);
    CHECK(rem == 6789);

    // mp_fdiv_r
    mp_fdiv_r(rem, 15, 7);
    CHECK(rem == 1);
    mp_fdiv_r(rem, -15, 7);
    CHECK(rem == 6);
    mp_fdiv_r(rem, 15, -7);
    CHECK(rem == -6);
    mp_fdiv_r(rem, -15, -7);
    CHECK(rem == -1);
    mp_fdiv_r(rem, -1, 5);
    CHECK(rem == 4);

    mp_fdiv_q(quo, 15, 7);
    CHECK(quo == 2);
    mp_fdiv_q(quo, -15, 7);
    CHECK(quo == -3);
    mp_fdiv_q(quo, 15, -7);
    CHECK(quo == -3);
    mp_fdiv_q(quo, -15, -7);
    CHECK(quo == 2);

    mp_tdiv_qr(quo, rem, 15, 7);
    CHECK(quo == 2);
    CHECK(rem == 1);
    mp_tdiv_qr(quo, rem, -15, 7);
    CHECK(quo == -2);
    CHECK(rem == -1);
    mp_tdiv_qr(quo, rem, 15, -7);
    CHECK(quo == -2);
    CHECK(rem == 1);
    mp_tdiv_qr(quo, rem, -15, -7);
    CHECK(quo == 2);
    CHECK(rem == -1);

    // mp_divexact
    mp_divexact(res, -2, -1);
    CHECK(res == 2);
    mp_divexact(res, -1, -1);
    CHECK(res == 1);
    mp_divexact(res, 1, -1);
    CHECK(res == -1);
    mp_divexact(res, 2, -1);
    CHECK(res == -2);
    mp_divexact(res, -2, 1);
    CHECK(res == -2);
    mp_divexact(res, -1, 1);
    CHECK(res == -1);
    mp_divexact(res, 1, 1);
    CHECK(res == 1);
    mp_divexact(res, 2, 1);
    CHECK(res == 2);
    mp_divexact(res, -6, -3);
    CHECK(res == 2);
    mp_divexact(res, -6, -2);
    CHECK(res == 3);
    mp_divexact(res, -6, 2);
    CHECK(res == -3);
    mp_divexact(res, -6, 3);
    CHECK(res == -2);
    mp_divexact(res, 109187, -227);
    CHECK(res == -481);
    mp_divexact(res, -109187, 227);
    CHECK(res == -481);

    // mp_gcd
    mp_gcd(gcd, -1, -1);
    CHECK(gcd == 1);
    mp_gcd(gcd, -1, 0);
    CHECK(gcd == 1);
    mp_gcd(gcd, -1, 1);
    CHECK(gcd == 1);
    mp_gcd(gcd, 0, -1);
    CHECK(gcd == 1);
    mp_gcd(gcd, 0, 0);
    CHECK(gcd == 0);
    mp_gcd(gcd, 0, 1);
    CHECK(gcd == 1);
    mp_gcd(gcd, 1, -1);
    CHECK(gcd == 1);
    mp_gcd(gcd, 1, 0);
    CHECK(gcd == 1);
    mp_gcd(gcd, 1, -1);
    CHECK(gcd == 1);
    mp_gcd(gcd, 2, -1);
    CHECK(gcd == 1);
    mp_gcd(gcd, 2, 0);
    CHECK(gcd == 2);
    mp_gcd(gcd, 2, 1);
    CHECK(gcd == 1);
    mp_gcd(gcd, 2, 2);
    CHECK(gcd == 2);
    mp_gcd(gcd, -1, 2);
    CHECK(gcd == 1);
    mp_gcd(gcd, 0, 2);
    CHECK(gcd == 2);
    mp_gcd(gcd, 1, 2);
    CHECK(gcd == 1);
    mp_gcd(gcd, 2, 2);
    CHECK(gcd == 2);
    mp_gcd(gcd, 2, 6);
    CHECK(gcd == 2);
    mp_gcd(gcd, 6, 2);
    CHECK(gcd == 2);
    mp_gcd(gcd, -2, 6);
    CHECK(gcd == 2);
    mp_gcd(gcd, 6, -2);
    CHECK(gcd == 2);
    mp_gcd(gcd, 2, -6);
    CHECK(gcd == 2);
    mp_gcd(gcd, -6, 2);
    CHECK(gcd == 2);
    mp_gcd(gcd, -2, -6);
    CHECK(gcd == 2);
    mp_gcd(gcd, -6, -2);
    CHECK(gcd == 2);
    mp_gcd(gcd, 5, 6);
    CHECK(gcd == 1);
    mp_gcd(gcd, 6, 5);
    CHECK(gcd == 1);
    mp_gcd(gcd, -5, 6);
    CHECK(gcd == 1);
    mp_gcd(gcd, 6, -5);
    CHECK(gcd == 1);
    mp_gcd(gcd, 5, -6);
    CHECK(gcd == 1);
    mp_gcd(gcd, -6, 5);
    CHECK(gcd == 1);
    mp_gcd(gcd, -5, -6);
    CHECK(gcd == 1);
    mp_gcd(gcd, -6, -5);
    CHECK(gcd == 1);
    mp_gcd(gcd, 8, 12);
    CHECK(gcd == 4);
    mp_gcd(gcd, 12, 8);
    CHECK(gcd == 4);
    mp_gcd(gcd, -8, 12);
    CHECK(gcd == 4);
    mp_gcd(gcd, 12, -8);
    CHECK(gcd == 4);
    mp_gcd(gcd, 8, -12);
    CHECK(gcd == 4);
    mp_gcd(gcd, -12, 8);
    CHECK(gcd == 4);
    mp_gcd(gcd, -8, -12);
    CHECK(gcd == 4);
    mp_gcd(gcd, -12, -8);
    CHECK(gcd == 4);

    integer_class s, t;
    mp_gcdext(gcd, s, t, -1, -1);
    CHECK(gcd == 1);
    CHECK(s == 0);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -1, 0);
    CHECK(gcd == 1);
    CHECK(s == -1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, -1, 1);
    CHECK(gcd == 1);
    CHECK(s == 0);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 0, -1);
    CHECK(gcd == 1);
    CHECK(s == 0);
    CHECK(t == -1);
    // mp_gcdext(gcd, s, t, 0, 0);
    // CHECK(gcd == 0); CHECK(s == 0); CHECK(t == 0); //fails for boostmp
    // but not gmp
    mp_gcdext(gcd, s, t, 0, 1);
    CHECK(gcd == 1);
    CHECK(s == 0);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 1, -1);
    CHECK(gcd == 1);
    CHECK(s == 0);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, 1, 0);
    CHECK(gcd == 1);
    CHECK(s == 1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, 1, -1);
    CHECK(gcd == 1);
    CHECK(s == 0);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, 2, -1);
    CHECK(gcd == 1);
    CHECK(s == 0);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, 2, 0);
    CHECK(gcd == 2);
    CHECK(s == 1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, 2, 1);
    CHECK(gcd == 1);
    CHECK(s == 0);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 2, 2);
    CHECK(gcd == 2);
    CHECK(s == 0);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, -1, 2);
    CHECK(gcd == 1);
    CHECK(s == -1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, 0, 2);
    CHECK(gcd == 2);
    CHECK(s == 0);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 1, 2);
    CHECK(gcd == 1);
    CHECK(s == 1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, 2, 2);
    CHECK(gcd == 2);
    CHECK(s == 0);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 2, 6);
    CHECK(gcd == 2);
    CHECK(s == 1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, 6, 2);
    CHECK(gcd == 2);
    CHECK(s == 0);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, -2, 6);
    CHECK(gcd == 2);
    CHECK(s == -1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, 6, -2);
    CHECK(gcd == 2);
    CHECK(s == 0);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, 2, -6);
    CHECK(gcd == 2);
    CHECK(s == 1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, -6, 2);
    CHECK(gcd == 2);
    CHECK(s == 0);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, -2, -6);
    CHECK(gcd == 2);
    CHECK(s == -1);
    CHECK(t == 0);
    mp_gcdext(gcd, s, t, -6, -2);
    CHECK(gcd == 2);
    CHECK(s == 0);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, 5, 6);
    CHECK(gcd == 1);
    CHECK(s == -1);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 6, 5);
    CHECK(gcd == 1);
    CHECK(s == 1);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -5, 6);
    CHECK(gcd == 1);
    CHECK(s == 1);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 6, -5);
    CHECK(gcd == 1);
    CHECK(s == 1);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 5, -6);
    CHECK(gcd == 1);
    CHECK(s == -1);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -6, 5);
    CHECK(gcd == 1);
    CHECK(s == -1);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -5, -6);
    CHECK(gcd == 1);
    CHECK(s == 1);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -6, -5);
    CHECK(gcd == 1);
    CHECK(s == -1);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 8, 12);
    CHECK(gcd == 4);
    CHECK(s == -1);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 12, 8);
    CHECK(gcd == 4);
    CHECK(s == 1);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -8, 12);
    CHECK(gcd == 4);
    CHECK(s == 1);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 12, -8);
    CHECK(gcd == 4);
    CHECK(s == 1);
    CHECK(t == 1);
    mp_gcdext(gcd, s, t, 8, -12);
    CHECK(gcd == 4);
    CHECK(s == -1);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -12, 8);
    CHECK(gcd == 4);
    CHECK(s == -1);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -8, -12);
    CHECK(gcd == 4);
    CHECK(s == 1);
    CHECK(t == -1);
    mp_gcdext(gcd, s, t, -12, -8);
    CHECK(gcd == 4);
    CHECK(s == -1);
    CHECK(t == 1);

    // mp_invert
    // behavior undefined when m == 0, so don't check those cases
    bool is_invertible;
    is_invertible = mp_invert(res, -1, -1);
    CHECK(is_invertible == 1);
    CHECK(res == 0);
    is_invertible = mp_invert(res, -1, 1);
    CHECK(is_invertible == 1);
    CHECK(res == 0);
    is_invertible = mp_invert(res, -1, 1);
    CHECK(is_invertible == 1);
    CHECK(res == 0);
    is_invertible = mp_invert(res, 2, 1);
    CHECK(is_invertible == 1);
    CHECK(res == 0);
    is_invertible = mp_invert(res, -1, 2);
    CHECK(is_invertible == 1);
    CHECK(res == 1);
    is_invertible = mp_invert(res, 3, 4);
    CHECK(is_invertible == 1);
    CHECK(res == 3);
    is_invertible = mp_invert(res, 2, 4);
    CHECK(is_invertible == 0);
    is_invertible = mp_invert(res, -14, 15);
    CHECK(is_invertible == 1);
    CHECK(res == 1);
    is_invertible = mp_invert(res, 39, -10);
    CHECK(is_invertible == 1);
    CHECK(res == 9);
    is_invertible = mp_invert(res, -10, 77);
    CHECK(is_invertible == 1);
    CHECK(res == 23);
    is_invertible = mp_invert(res, -77, -10);
    CHECK(is_invertible == 1);
    CHECK(res == 7);

    // mp_lcm
    mp_lcm(res, 18, 24);
    CHECK(res == 72);
}

TEST_CASE("primes: integer_class", "[integer_class]")
{
    // mp_probab_prime_p
    CHECK(mp_probab_prime_p(690089, 25));  // 690089 is prime
    CHECK(!mp_probab_prime_p(636751, 25)); // 636751 == 857*743

    // mp_nextprime
    integer_class res;
    mp_nextprime(res, integer_class(6000));
    CHECK(res == 6007);
}

TEST_CASE("fibonacci and lucas: integer_class", "[integer_class]")
{
    integer_class res, res2;
    mp_fib_ui(res, 6);
    CHECK(res == 8);
    mp_fib2_ui(res, res2, 9);
    CHECK(res == 34);
    CHECK(res2 == 21);

    mp_lucnum_ui(res, 0);
    CHECK(res == 2);
    mp_lucnum_ui(res, 1);
    CHECK(res == 1);
    mp_lucnum_ui(res, 6);
    CHECK(res == 18);
    mp_lucnum2_ui(res, res2, 8);
    CHECK(res == 47);
    CHECK(res2 == 29);
}

TEST_CASE("legendre: integer_class", "[integer_class]")
{
    integer_class prime(13619591); // 13619591 is a prime
    integer_class i = prime * integer_class(34714) + integer_class(81);
    CHECK(mp_legendre(i, prime) == 1);

    // mp_jacobi
    CHECK(mp_jacobi(i, prime) == 1);
    CHECK(mp_jacobi(30, 59) == -1);
    CHECK(mp_jacobi(30, 57) == 0);
    CHECK(mp_jacobi(17, 41) == -1);
    CHECK(mp_jacobi(18, 27) == 0);
    CHECK(mp_jacobi(1, 1) == 1);
    CHECK(mp_jacobi(28, 25) == 1);
    CHECK(mp_jacobi(13, 45) == -1);
    CHECK(mp_jacobi(13, 45) == -1);
    CHECK(mp_jacobi(24, 33) == 0);
    CHECK(mp_jacobi(20, 39) == 1);
    CHECK(mp_jacobi(12, 51) == 0);
    CHECK(mp_jacobi(30, 7) == 1);
    CHECK(mp_jacobi(27, 11) == 1);
    CHECK(mp_jacobi(14, 3) == -1);
    CHECK(mp_jacobi(-58, 3) == -1);
    CHECK(mp_jacobi(-58, 9) == 1);
    CHECK(mp_jacobi(-20, 11) == -1);
    CHECK(mp_jacobi(-1, 1) == 1);
    CHECK(mp_jacobi(0, 17) == 0);
    CHECK(mp_jacobi(1, 1) == 1);
    CHECK(mp_jacobi(1, 15) == 1);
    CHECK(mp_jacobi(5, 15) == 0);
    CHECK(mp_jacobi(36, 25) == 1);
    CHECK(mp_jacobi(46, 35) == 1);

    // mp_kronecker
    CHECK(mp_kronecker(-58, -58) == 0);
    CHECK(mp_kronecker(-58, -43) == 1);
    CHECK(mp_kronecker(-20, -35) == 0);
    CHECK(mp_kronecker(-1, -32) == -1);
    CHECK(mp_kronecker(0, -1) == 1);
    CHECK(mp_kronecker(1, 1) == 1);
    CHECK(mp_kronecker(1, 3) == 1);
    CHECK(mp_kronecker(5, 18) == -1);
    CHECK(mp_kronecker(36, 50) == 0);
    CHECK(mp_kronecker(46, 79) == 1);
}

TEST_CASE("misc: integer_class", "[integer_class]")
{
    // mp_abs
    CHECK(mp_abs(integer_class(2)) == 2);
    CHECK(mp_abs(integer_class(-2)) == 2);
    CHECK(mp_abs(integer_class(0)) == 0);

    // mp_sign
    CHECK(mp_sign(integer_class(-2)) < 0);
    CHECK(mp_sign(integer_class(2)) > 0);
    CHECK(mp_sign(integer_class(0)) == 0);

    // mp_get_ui
    CHECK(mp_get_ui(integer_class("-18446744073709551616")) == 0u);
    CHECK(mp_get_ui(integer_class("-18446744073709551615"))
          == 18446744073709551615u);
    CHECK(mp_get_ui(integer_class("-9223372036854775809"))
          == 9223372036854775809u);
    CHECK(mp_get_ui(integer_class("-9223372036854775808"))
          == 9223372036854775808u);
    CHECK(mp_get_ui(integer_class("-2")) == 2u);
    CHECK(mp_get_ui(integer_class("-1")) == 1u);
    CHECK(mp_get_ui(integer_class("0")) == 0u);
    CHECK(mp_get_ui(integer_class("1")) == 1u);
    CHECK(mp_get_ui(integer_class("2")) == 2u);
    CHECK(mp_get_ui(integer_class("9223372036854775807"))
          == 9223372036854775807u);
    CHECK(mp_get_ui(integer_class("-9223372036854775807"))
          == 9223372036854775807u);
    CHECK(mp_get_ui(integer_class("18446744073709551615"))
          == 18446744073709551615u);
    CHECK(mp_get_ui(integer_class("18446744073709551615"))
          == 18446744073709551615u);
    CHECK(mp_get_ui(integer_class("18446744073709551616")) == 0u);

    // mp_scan1
    CHECK(mp_scan1(LONG_MIN) == 63);
    CHECK(mp_scan1(-1024) == 10);
    CHECK(mp_scan1(-768) == 8);
    CHECK(mp_scan1(-500) == 2);
    CHECK(mp_scan1(-5) == 0);
    CHECK(mp_scan1(-4) == 2);
    CHECK(mp_scan1(-2) == 1);
    CHECK(mp_scan1(-1) == 0);
    CHECK(mp_scan1(0) == ULONG_MAX);
    CHECK(mp_scan1(1) == 0);
    CHECK(mp_scan1(2) == 1);
    CHECK(mp_scan1(4) == 2);
    CHECK(mp_scan1(5) == 0);
    CHECK(mp_scan1(500) == 2);
    CHECK(mp_scan1(768) == 8);
    CHECK(mp_scan1(1024) == 10);
    CHECK(mp_scan1(LONG_MAX) == 0);

    // mp_and
    integer_class res;
    mp_and(res, 9, 12);
    CHECK(res == 8);

    // fits
    integer_class long_max(LONG_MAX);
    integer_class past_long_max(long_max + 1);
    integer_class ulong_max(ULONG_MAX);
    integer_class past_ulong_max(ulong_max + 1);
    CHECK(mp_fits_slong_p(long_max));
    CHECK(!mp_fits_slong_p(past_long_max));
    CHECK(mp_fits_ulong_p(long_max));
    CHECK(!mp_fits_ulong_p(past_ulong_max));

    // mp_fac_ui
    mp_fac_ui(res, 5);
    CHECK(res == 120);

    // mp_bin_ui
    mp_bin_ui(res, 5, 2);
    CHECK(res == 10);
    mp_bin_ui(res, integer_class(30), 8);
    CHECK(res == 5852925);

    // add_mul
    integer_class i = 3;
    mp_addmul(i, 4, 5);
    CHECK(i == 23);

    // rational_class
    auto r = rational_class(2, 3);
    CHECK(get_num(r) == 2);
    CHECK(get_den(r) == 3);
    r = rational_class(2, 3);
    CHECK(mp_sign(r) > 0);
    r = rational_class(-2, 3);
    CHECK(mp_sign(r) < 0);
    r = rational_class(0, 1);
    CHECK(mp_sign(r) == 0);

    r = rational_class(-1, 2);
    CHECK(mp_abs(r) == .5);

    r = rational_class(2, 3);
    rational_class p;
    mp_pow_ui(p, r, 4);
    CHECK(get_num(p) == 16);
    CHECK(get_den(p) == 81);
}
