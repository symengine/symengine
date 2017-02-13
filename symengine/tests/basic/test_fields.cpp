#include "catch.hpp"
#include <iostream>

#include <symengine/basic.h>
#include <symengine/fields.h>
#include <symengine/symengine_rcp.h>
#include <symengine/dict.h>
#include <symengine/symbol.h>
#include <symengine/symengine_exception.h>

using SymEngine::SymEngineException;
using SymEngine::RCP;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::gf_poly;
using SymEngine::integer_class;
using SymEngine::map_uint_mpz;
using SymEngine::GaloisField;
using SymEngine::GaloisFieldDict;
using SymEngine::UIntPoly;

using namespace SymEngine::literals;

TEST_CASE("Constructor of GaloisField : Basic", "[basic]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const GaloisField> P = gf_poly(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}}, 2_z);
    CHECK(P->__str__() == "x**2 + 1");

    RCP<const GaloisField> Q
        = GaloisField::from_vec(x, {1_z, 0_z, 2_z, 3_z}, 2_z);
    CHECK(Q->__str__() == "x**3 + 1");

    RCP<const GaloisField> R
        = GaloisField::from_vec(x, {17_z, 0_z, 7_z, 9_z, 7_z, 14_z}, 7_z);
    CHECK(R->__str__() == "2*x**3 + 3");

    RCP<const GaloisField> S = gf_poly(x, {{0, 2_z}}, 7_z);
    CHECK(S->__str__() == "2");

    RCP<const GaloisField> T = gf_poly(x, map_uint_mpz{}, 7_z);
    CHECK(T->__str__() == "0");

    RCP<const GaloisField> U = gf_poly(x, {{0, 2_z}, {1, 0_z}, {2, 0_z}}, 7_z);
    CHECK(U->__str__() == "2");

    RCP<const UIntPoly> UP
        = UIntPoly::from_dict(x, {{0, 3_z}, {1, 4_z}, {2, 5_z}});
    U = GaloisField::from_uintpoly(*UP, 5_z);
    CHECK(U->__str__() == "4*x + 3");

    UP = UIntPoly::from_dict(x, {{0, 10_z}, {1, 7_z}, {2, 9_z}});
    U = GaloisField::from_uintpoly(*UP, 7_z);
    CHECK(U->__str__() == "2*x**2 + 3");
}

TEST_CASE("GaloisField Addition, Subtraction, Multiplication : Basic",
          "[basic]")
{
    RCP<const Symbol> x = symbol("x");
    std::vector<integer_class> mp, a = {2_z, 3_z, 4_z};
    std::vector<integer_class> b = {3_z, 3_z, 6_z, 6_z};
    RCP<const GaloisField> r1 = GaloisField::from_vec(x, a, 5_z);
    RCP<const GaloisField> r2 = GaloisField::from_vec(x, b, 5_z);
    RCP<const GaloisField> r3 = add_upoly(*r1, *r2);
    CHECK(r3->__str__() == "x**3 + x");
    r3 = sub_upoly(*r1, *r2);
    CHECK(r3->__str__() == "4*x**3 + 3*x**2 + 4");
    r3 = mul_upoly(*r1, *r2);
    mp = r3->get_dict();
    CHECK(mp[0] == 1);
    CHECK(mp[2] == 3);
    CHECK(mp[3] == 2);
    CHECK(mp[4] == 2);
    CHECK(mp[5] == 4);

    a = {};
    r1 = GaloisField::from_vec(x, a, 11_z);
    r2 = neg_upoly(*r1);
    mp = r2->get_dict();
    CHECK(mp.empty());
    r3 = GaloisField::from_vec(x, {0_z}, 11_z);
    r2 = add_upoly(*r1, *r3);
    mp = r2->get_dict();
    CHECK(mp.empty());
    r2 = sub_upoly(*r1, *r3);
    mp = r2->get_dict();
    CHECK(mp.empty());
    r2 = mul_upoly(*r1, *r3);
    mp = r2->get_dict();
    CHECK(mp.empty());

    r3 = GaloisField::from_vec(x, {3_z}, 11_z);
    r2 = add_upoly(*r1, *r3);
    CHECK(r2->__str__() == "3");
    r2 = sub_upoly(*r1, *r3);
    CHECK(r2->__str__() == "8");
    r2 = mul_upoly(*r1, *r3);
    mp = r2->get_dict();
    CHECK(mp.empty());
    r2 = GaloisField::from_vec(x, a, 11_z);
    a = {2_z};
    r2 = GaloisField::from_vec(x, a, 11_z);
    r3 = add_upoly(*r1, *r2);
    CHECK(r2->__str__() == "2");
    r3 = sub_upoly(*r1, *r2);
    r3 = sub_upoly(*r1, *r2);
    CHECK(r3->__str__() == "9");
    r2 = quo_upoly(*r1, *GaloisField::from_vec(x, {3_z}, 11_z));
    mp = r2->get_dict();
    CHECK(mp.empty());

    a = {1_z};
    r1 = GaloisField::from_vec(x, a, 11_z);
    r2 = neg_upoly(*r1);
    CHECK(r2->__str__() == "10");
    r2 = add_upoly(*r1, *GaloisField::from_vec(x, {10_z}, 11_z));
    mp = r2->get_dict();
    CHECK(mp.empty());
    r2 = add_upoly(*r1, *GaloisField::from_vec(x, {4_z}, 11_z));
    mp = r2->get_dict();
    CHECK(r2->__str__() == "5");
    r2 = sub_upoly(*r1, *GaloisField::from_vec(x, {1_z}, 11_z));
    mp = r2->get_dict();
    CHECK(mp.empty());
    r2 = sub_upoly(*r1, *GaloisField::from_vec(x, {4_z}, 11_z));
    CHECK(r2->__str__() == "8");
    r2 = mul_upoly(*r1, *GaloisField::from_vec(x, {}, 11_z));
    mp = r2->get_dict();
    CHECK(mp.empty());
    r2 = mul_upoly(*r1, *GaloisField::from_vec(x, {3_z}, 11_z));
    CHECK(r2->__str__() == "3");
    r2 = add_upoly(*r1, *GaloisField::from_vec(x, {3_z}, 11_z));
    CHECK(r2->__str__() == "4");

    a = {1_z, 2_z, 3_z};
    r1 = GaloisField::from_vec(x, a, 11_z);
    r2 = neg_upoly(*r1);
    CHECK(r2->__str__() == "8*x**2 + 9*x + 10");
    r2 = add_upoly(*r1, *GaloisField::from_vec(x, {4_z}, 11_z));
    CHECK(r2->__str__() == "3*x**2 + 2*x + 5");
    r2 = sub_upoly(*r1, *GaloisField::from_vec(x, {1_z}, 11_z));
    CHECK(r2->__str__() == "3*x**2 + 2*x");
    r2 = mul_upoly(*r1, *GaloisField::from_vec(x, {}, 11_z));
    mp = r2->get_dict();
    CHECK(mp.empty());
    r2 = mul_upoly(*r1, *GaloisField::from_vec(x, {7_z}, 11_z));
    mp = r2->get_dict();
    CHECK(mp[0] == 7);
    CHECK(mp[1] == 3);
    CHECK(mp[2] == 10);

    a = {3_z, 2_z, 1_z};
    b = {8_z, 9_z, 10_z};
    r1 = GaloisField::from_vec(x, a, 11_z);
    r2 = GaloisField::from_vec(x, b, 11_z);
    r3 = sub_upoly(*r1, *r2);
    CHECK(r3->__str__() == "2*x**2 + 4*x + 6");
    a = {3_z, 0_z, 0_z, 6_z, 1_z, 2_z};
    b = {4_z, 0_z, 1_z, 0_z};
    r1 = GaloisField::from_vec(x, a, 11_z);
    r2 = GaloisField::from_vec(x, b, 11_z);
    mp = mul_upoly(*r2, *r1)->get_dict();
    CHECK(mp[0] == 1);
    CHECK(mp[1] == 0);
    CHECK(mp[2] == 3);
    CHECK(mp[3] == 2);
    CHECK(mp[4] == 4);
    CHECK(mp[5] == 3);
    CHECK(mp[6] == 1);
    CHECK(mp[7] == 2);
}
TEST_CASE("GaloisFieldDict Division, GCD, LCM, Shifts : Basic", "[basic]")
{
    RCP<const Symbol> x = symbol("x");
    std::vector<integer_class> a, b, mp;
    GaloisFieldDict d1, d2, d3, d4;
    a = {0_z, 1_z, 2_z, 3_z, 4_z, 5_z};
    b = {0_z, 3_z, 2_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 7_z);
    d2 = GaloisFieldDict::from_vec(b, 7_z);
    d1.gf_div(d2, outArg(d3), outArg(d4));
    mp = d3.get_dict();
    CHECK(mp[0] == 0);
    CHECK(mp[1] == 1);
    CHECK(mp[2] == 5);
    mp = d4.get_dict();
    CHECK(mp[0] == 0);
    CHECK(mp[1] == 1);
    CHECK(mp[2] == 6);
    CHECK(d3 == d1 / d2);
    CHECK(d4 == d1 % d2);

    d2 = d1;
    d2 *= 2_z;
    d1 += d1;
    CHECK(d1 == d2);
    d2 = d1;
    d2 = d1.gf_sqr();
    d1 *= d1;
    CHECK(d1 == d2);
    a = {};
    d1 = GaloisFieldDict::from_vec(a, 7_z);
    d2 -= d2;
    CHECK(d2.dict_.empty());
    d1 = GaloisFieldDict::from_vec({1_z}, 7_z);
    d2 = GaloisFieldDict::from_vec(b, 7_z);
    d2 /= d2;
    CHECK(d1 == d2);
    d2 = GaloisFieldDict::from_vec(b, 7_z);
    d2 %= d2;
    CHECK(d2.dict_.empty());
    a = {0_z, 1_z, 2_z, 3_z, 4_z, 5_z};
    b = {3_z, 2_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 7_z);
    d2 = GaloisFieldDict::from_vec(b, 7_z);
    d1.gf_div(d2, outArg(d3), outArg(d4));
    mp = d3.get_dict();
    CHECK(mp[0] == 6);
    CHECK(mp[1] == 0);
    CHECK(mp[2] == 1);
    CHECK(mp[3] == 5);
    mp = d4.get_dict();
    CHECK(mp[0] == 3);
    CHECK(mp[1] == 3);
    CHECK(d3 == d1 / d2);
    CHECK(d4 == d1 % d2);

    a = {1_z};
    b = {3_z, 2_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 7_z);
    d2 = GaloisFieldDict::from_vec(b, 7_z);
    d1.gf_div(d2, outArg(d3), outArg(d4));
    CHECK(d3.get_dict().empty());
    mp = d4.get_dict();
    CHECK(mp[0] == 1);
    CHECK(d3 == d1 / d2);
    CHECK(d4 == d1 % d2);

    a = {};
    d1 = GaloisFieldDict::from_vec(a, 7_z);
    d2 = d1.gf_lshift(5_z);
    CHECK(d2.get_dict().empty());
    d1.gf_rshift(5_z, outArg(d2), outArg(d3));
    CHECK(d2.get_dict().empty());
    CHECK(d2.get_dict().empty());
    a = {5_z, 4_z, 3_z, 2_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 7_z);
    mp = d1.gf_lshift(1_z).get_dict();
    CHECK(mp[0] == 0);
    CHECK(mp[1] == 5);
    CHECK(mp[2] == 4);
    CHECK(mp[3] == 3);
    CHECK(mp[4] == 2);
    CHECK(mp[5] == 1);
    mp = d1.gf_lshift(2_z).get_dict();
    CHECK(mp[0] == 0);
    CHECK(mp[1] == 0);
    CHECK(mp[2] == 5);
    CHECK(mp[3] == 4);
    CHECK(mp[4] == 3);
    CHECK(mp[5] == 2);
    CHECK(mp[6] == 1);
    d1.gf_rshift(0_z, outArg(d2), outArg(d3));
    CHECK(d1 == d2);
    CHECK(d3.get_dict().empty());
    d1.gf_rshift(5_z, outArg(d2), outArg(d3));
    mp = d2.get_dict();
    d1.gf_rshift(1_z, outArg(d2), outArg(d3));
    mp = d2.get_dict();
    CHECK(mp[0] == 4);
    CHECK(mp[1] == 3);
    CHECK(mp[2] == 2);
    CHECK(mp[3] == 1);
    mp = d3.get_dict();
    CHECK(mp[0] == 5);
    d1.gf_rshift(3_z, outArg(d2), outArg(d3));
    mp = d2.get_dict();
    CHECK(mp[0] == 2);
    CHECK(mp[1] == 1);
    mp = d3.get_dict();
    CHECK(mp[0] == 5);
    CHECK(mp[1] == 4);
    CHECK(mp[2] == 3);

    a = {8_z, 1_z, 0_z, 0_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    RCP<const GaloisField> U = gf_poly(x, std::move(d1));
    CHECK(pow_upoly(*U, 0)->__str__() == "1");
    d2 = d1.gf_pow(0);
    mp = d2.get_dict();
    CHECK(mp[0] == 1);
    CHECK(mp.size() == 1);
    d2 = d1.gf_pow(1);
    CHECK(d2 == d1);
    CHECK(pow_upoly(*U, 1)->__str__() == "x**4 + x + 8");
    CHECK(pow_upoly(*U, 2)->__str__()
          == "x**8 + 2*x**5 + 5*x**4 + x**2 + 5*x + 9");
    CHECK(pow_upoly(*U, 5)->__str__()
          == "x**20 + 5*x**17 + 7*x**16 + 10*x**14 + 6*x**13 + 2*x**12 + "
             "10*x**11 + 9*x**10 + 6*x**9 + 10*x**8 + 6*x**7 + 6*x**6 + "
             "5*x**4 + 2*x**3 + 5*x**2 + 9*x + 10");
    d2 = d1.gf_pow(2);
    mp = d2.get_dict();
    CHECK(mp[0] == 9);
    CHECK(mp[1] == 5);
    CHECK(mp[2] == 1);
    CHECK(mp[4] == 5);
    CHECK(mp[5] == 2);
    CHECK(mp[8] == 1);
    d2 = d1.gf_pow(5);
    mp = d2.get_dict();
    CHECK(mp[0] == 10);
    CHECK(mp[1] == 9);
    CHECK(mp[2] == 5);
    CHECK(mp[3] == 2);
    CHECK(mp[4] == 5);
    CHECK(mp[6] == 6);
    CHECK(mp[7] == 6);
    CHECK(mp[8] == 10);
    CHECK(mp[9] == 6);
    CHECK(mp[10] == 9);
    CHECK(mp[11] == 10);
    CHECK(mp[12] == 2);
    CHECK(mp[13] == 6);
    CHECK(mp[14] == 10);
    CHECK(mp[16] == 7);
    CHECK(mp[17] == 5);
    CHECK(mp[20] == 1);
    d2 = d1.gf_pow(8);
    d3 = d1.gf_pow(4);
    CHECK(d2 == d3.gf_sqr());

    integer_class LC;
    a = {};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d1.gf_monic(LC, outArg(d2));
    CHECK(LC == 0_z);
    CHECK(d2 == d1);
    a = {1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d1.gf_monic(LC, outArg(d2));
    CHECK(LC == 1_z);
    CHECK(d2 == d1);
    a = {2_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d1.gf_monic(LC, outArg(d2));
    CHECK(LC == 2_z);
    mp = d2.get_dict();
    CHECK(mp[0] == 1_z);
    a = {4_z, 3_z, 2_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d1.gf_monic(LC, outArg(d2));
    CHECK(LC == 1_z);
    CHECK(d2 == d1);
    a = {5_z, 4_z, 3_z, 2_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d1.gf_monic(LC, outArg(d2));
    CHECK(LC == 2_z);
    mp = d2.get_dict();
    CHECK(mp[0] == 8);
    CHECK(mp[1] == 2);
    CHECK(mp[2] == 7);
    CHECK(mp[3] == 1);

    a = {7_z, 8_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    a = {3_z, 2_z};
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    d1.gf_div(d2, outArg(d3), outArg(d4));
    CHECK(d3 == d1 / d2);
    CHECK(d4 == d1 % d2);

    a = {};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_gcd(d2).get_dict().empty());
    a = {2_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    mp = d2.gf_gcd(d1).get_dict();
    CHECK(mp[0] == 1);
    CHECK(d1.gf_gcd(d2).get_dict() == d2.gf_gcd(d1).get_dict());
    a = {0_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d3 = d1.gf_gcd(d2);
    CHECK(d1.get_dict() == d3.get_dict());
    CHECK(d3.get_dict() == d2.gf_gcd(d1).get_dict());

    a = {0_z, 3_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_gcd(d2).get_dict() == d2.gf_gcd(d1).get_dict());
    mp = d1.gf_gcd(d2).get_dict();
    CHECK(mp[1] == 1);

    a = {7_z, 8_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    a = {7_z, 1_z, 7_z, 1_z};
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    mp = d2.gf_gcd(d1).get_dict();
    CHECK(mp[0] == 7);
    CHECK(mp[1] == 1);
    mp = d1.gf_gcd(d2).get_dict();
    CHECK(mp[0] == 7);
    CHECK(mp[1] == 1);

    a = {};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_lcm(d2).get_dict().empty());
    a = {2_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_lcm(d2).get_dict().empty());
    CHECK(d2.gf_lcm(d1).get_dict().empty());
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    mp = d1.gf_gcd(d2).get_dict();
    CHECK(mp[0] == 1);
    a = {0_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    a = {};
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    d3 = d1.gf_lcm(d2);
    CHECK(d3.get_dict().empty());
    CHECK(d3.get_dict() == d2.gf_lcm(d1).get_dict());
    a = {0_z, 3_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_lcm(d2).get_dict() == d2.gf_lcm(d1).get_dict());
    mp = d1.gf_lcm(d2).get_dict();
    CHECK(mp[1] == 1);
    a = {7_z, 8_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    a = {7_z, 1_z, 7_z, 1_z};
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_lcm(d2).get_dict() == d2.gf_lcm(d1).get_dict());
    mp = d1.gf_lcm(d2).get_dict();
    CHECK(mp[0] == 7);
    CHECK(mp[1] == 8);
    CHECK(mp[2] == 8);
    CHECK(mp[3] == 8);
    CHECK(mp[4] == 1);
}

TEST_CASE("GaloisFieldDict Differentiation, Square Free Algorithms : Basic",
          "[basic]")
{
    RCP<const Symbol> x = symbol("x");
    std::vector<integer_class> a, mp;
    GaloisFieldDict d1, d2, d3, d4;
    a = {};
    d1 = GaloisFieldDict::from_vec(a, 11_z);

    d1 = GaloisFieldDict::from_vec(a, 11_z);
    RCP<const GaloisField> U = gf_poly(x, std::move(d1));
    auto b = U->diff(x);
    CHECK(b->__str__() == "0");
    a = {7_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    U = gf_poly(x, std::move(d1));
    b = U->diff(x);
    CHECK(b->__str__() == "0");
    a = {3_z, 7_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    U = gf_poly(x, std::move(d1));
    b = U->diff(x);
    CHECK(b->__str__() == "7");
    a = {1_z, 3_z, 7_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    U = gf_poly(x, std::move(d1));
    b = U->diff(x);
    CHECK(b->__str__() == "3*x + 3");
    a = {1_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    U = gf_poly(x, std::move(d1));
    b = U->diff(x);
    CHECK(b->__str__() == "0");

    a = {};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_is_sqf() == true);
    a = {1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_is_sqf() == true);
    a = {1_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_is_sqf() == true);
    a = {4_z, 8_z, 5_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_is_sqf() == false);
    a = {1_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_is_sqf() == false);
    a = {};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    auto out = d1.gf_sqf_list();
    CHECK(out.empty());
    a = {1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    out = d1.gf_sqf_list();
    CHECK(out.empty());
    a = {1_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    out = d1.gf_sqf_list();
    CHECK(out[0].first == d1);
    CHECK(out[0].second == 1_z);
    CHECK(out.size() == 1);
    a = {1_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 1_z};
    d2 = GaloisFieldDict::from_vec(a, 11_z);
    out = d2.gf_sqf_list();
    CHECK(out[0].first == d1);
    CHECK(out[0].second == 11_z);
    CHECK(out.size() == 1);

    a = {4_z, 8_z, 5_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 11_z);
    CHECK(d1.gf_is_sqf() == false);
    out = d1.gf_sqf_list();
    CHECK(out[0].first == GaloisFieldDict::from_vec({1_z, 1_z}, 11_z));
    CHECK(out[0].second == 1_z);
    CHECK(out[1].first == GaloisFieldDict::from_vec({2_z, 1_z}, 11_z));
    CHECK(out[1].second == 2_z);
    CHECK(out.size() == 2);
    d2 = d1.gf_sqf_part();
    CHECK(d2 == GaloisFieldDict::from_vec({2_z, 3_z, 1_z}, 11_z));
    a = {0_z, 1_z, 0_z, 0_z, 2_z, 0_z, 0_z, 2_z, 0_z, 0_z, 1_z};
    d1 = GaloisFieldDict::from_vec(a, 3_z);
    CHECK(d1.gf_is_sqf() == false);
    out = d1.gf_sqf_list();
    CHECK(out[0].first == GaloisFieldDict::from_vec({0_z, 1_z}, 3_z));
    CHECK(out[0].second == 1_z);
    CHECK(out[1].first == GaloisFieldDict::from_vec({1_z, 1_z}, 3_z));
    CHECK(out[1].second == 3_z);
    CHECK(out[2].first == GaloisFieldDict::from_vec({2_z, 1_z}, 3_z));
    CHECK(out[2].second == 6_z);
    CHECK(out.size() == 3);
}

TEST_CASE("GaloisFieldDict pow_mod : Basic", "[basic]")
{
    std::vector<integer_class> a, mp;
    GaloisFieldDict d1, d2, d3, d4;
    d2 = GaloisFieldDict::from_vec({8_z, 1_z, 0_z, 0_z, 1_z}, 11_z);
    d1 = GaloisFieldDict::from_vec({7_z, 0_z, 2_z}, 11_z);
    CHECK(d1.gf_pow_mod(d2, 0).is_one());
    d3 = d1.gf_pow_mod(d2, 1);
    CHECK(d3 == GaloisFieldDict::from_vec({1_z, 1_z}, 11_z));
    d3 = d1.gf_pow_mod(d2, 2);
    CHECK(d3 == GaloisFieldDict::from_vec({3_z, 2_z}, 11_z));
    d3 = d1.gf_pow_mod(d2, 5);
    CHECK(d3 == GaloisFieldDict::from_vec({8_z, 7_z}, 11_z));
    d3 = d1.gf_pow_mod(d2, 8);
    CHECK(d3 == GaloisFieldDict::from_vec({5_z, 1_z}, 11_z));
    d3 = d1.gf_pow_mod(d2, 45);
    CHECK(d3 == GaloisFieldDict::from_vec({4_z, 5_z}, 11_z));

    d1 = GaloisFieldDict::from_vec({1_z, 2_z, 0_z, 1_z}, 5_z);
    std::vector<GaloisFieldDict> out = d1.gf_frobenius_monomial_base();
    CHECK(out.size() == 3);
    CHECK(out[0] == GaloisFieldDict::from_vec({1_z}, 5_z));
    CHECK(out[1] == GaloisFieldDict::from_vec({2_z, 4_z, 4_z}, 5_z));
    CHECK(out[2] == GaloisFieldDict::from_vec({2_z, 1_z}, 5_z));

    d1 = GaloisFieldDict::from_vec(
        {2_z, 2_z, 2_z, 0_z, 2_z, 2_z, 0_z, 1_z, 0_z, 2_z}, 3_z);
    d2 = GaloisFieldDict::from_vec(
        {1_z, 0_z, 2_z, 0_z, 1_z, 0_z, 2_z, 0_z, 1_z, 1_z}, 3_z);
    auto b = d2.gf_frobenius_monomial_base();
    GaloisFieldDict h = d1.gf_frobenius_map(d2, b);
    GaloisFieldDict h1 = d2.gf_pow_mod(d1, 3);
    CHECK(h1 == GaloisFieldDict::from_vec(
                    {1_z, 1_z, 1_z, 2_z, 0_z, 0_z, 2_z, 2_z}, 3_z));
    CHECK(h == h1);
    CHECK(h == (d1.gf_pow(3) % d2));
}

TEST_CASE("GaloisFieldDict distinct degree factorization : Basic", "[basic]")
{
    std::vector<integer_class> a, mp;
    GaloisFieldDict d1, d2, d3, d4;
    d1 = GaloisFieldDict({{15, 1_z}, {0, -1_z}}, 11_z);
    auto b = d1.gf_ddf_zassenhaus();
    CHECK(b.size() == 2);
    CHECK(b[0].first
          == GaloisFieldDict::from_vec({10_z, 0_z, 0_z, 0_z, 0_z, 1_z}, 11_z));
    CHECK(b[0].second == 1_z);
    CHECK(b[1].first == GaloisFieldDict::from_vec({1_z, 0_z, 0_z, 0_z, 0_z, 1_z,
                                                   0_z, 0_z, 0_z, 0_z, 1_z},
                                                  11_z));
    CHECK(b[1].second == 2_z);
    auto c = d1.gf_ddf_shoup();
    CHECK(b == c);

    d1 = GaloisFieldDict({{63, 1_z}, {0, 1_z}}, 2_z);
    b = d1.gf_ddf_zassenhaus();
    CHECK(b.size() == 4);
    CHECK(b[0].first == GaloisFieldDict::from_vec({1_z, 1_z}, 2_z));
    CHECK(b[0].second == 1_z);
    CHECK(b[1].first == GaloisFieldDict::from_vec({1_z, 1_z, 1_z}, 2_z));
    CHECK(b[1].second == 2_z);
    CHECK(b[2].first == GaloisFieldDict::from_vec(
                            {1_z, 1_z, 1_z, 1_z, 1_z, 1_z, 1_z}, 2_z));
    CHECK(b[2].second == 3_z);
    CHECK(b[3].first
          == GaloisFieldDict::from_vec(
                 {1_z, 1_z, 0_z, 1_z, 1_z, 0_z, 1_z, 0_z, 1_z, 1_z, 0_z, 1_z,
                  1_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 1_z, 1_z, 0_z,
                  1_z, 1_z, 0_z, 1_z, 0_z, 1_z, 1_z, 0_z, 1_z, 1_z, 0_z, 0_z,
                  0_z, 0_z, 0_z, 0_z, 0_z, 0_z, 1_z, 1_z, 0_z, 1_z, 1_z, 0_z,
                  1_z, 0_z, 1_z, 1_z, 0_z, 1_z, 1_z},
                 2_z));
    CHECK(b[3].second == 6_z);
    c = d1.gf_ddf_shoup();
    CHECK(b == c);

    d1 = GaloisFieldDict({{6, 1_z}, {5, -1_z}, {4, 1_z}, {3, 1_z}, {1, -1_z}},
                         3_z);
    b = d1.gf_ddf_zassenhaus();
    CHECK(b.size() == 2);
    CHECK(b[0].first == GaloisFieldDict::from_vec({0_z, 1_z, 1_z}, 3_z));
    CHECK(b[0].second == 1_z);
    CHECK(b[1].first
          == GaloisFieldDict::from_vec({2_z, 1_z, 0_z, 1_z, 1_z}, 3_z));
    CHECK(b[1].second == 2_z);
    c = d1.gf_ddf_shoup();
    CHECK(b == c);

    d1 = GaloisFieldDict::from_vec(
        {577_z, 24_z, 456_z, 325_z, 791_z, 436_z, 677_z, 26_z, 5_z, 2_z, 1_z},
        809_z);
    b = d1.gf_ddf_zassenhaus();
    CHECK(b.size() == 2);
    CHECK(b[0].first == GaloisFieldDict::from_vec({701_z, 1_z}, 809_z));
    CHECK(b[0].second == 1_z);
    CHECK(b[1].first
          == GaloisFieldDict::from_vec({122_z, 735_z, 70_z, 110_z, 151_z, 694_z,
                                        532_z, 559_z, 110_z, 1_z},
                                       809_z));
    CHECK(b[1].second == 9_z);
    c = d1.gf_ddf_shoup();
    CHECK(b == c);

    d1 = GaloisFieldDict({{15, 1_z}, {1, 1_z}, {0, 1_z}}, 102953_z);
    b = d1.gf_ddf_zassenhaus();
    CHECK(b.size() == 3);
    CHECK(b[0].first
          == GaloisFieldDict::from_vec({68144_z, 22730_z, 1_z}, 102953_z));
    CHECK(b[0].second == 2_z);
    CHECK(b[1].first == GaloisFieldDict::from_vec({84356_z, 88001_z, 52650_z,
                                                   68608_z, 12561_z, 10787_z,
                                                   83977_z, 64876_z, 1_z},
                                                  102953_z));
    CHECK(b[1].second == 4_z);
    CHECK(b[2].first
          == GaloisFieldDict::from_vec(
                 {92335_z, 94508_z, 84569_z, 95022_z, 15347_z, 1_z}, 102953_z));
    CHECK(b[2].second == 5_z);
    c = d1.gf_ddf_shoup();
    CHECK(b == c);

    d1 = GaloisFieldDict::from_vec({}, 11_z);
    b = d1.gf_ddf_zassenhaus();
    CHECK(b.size() == 0);
    c = d1.gf_ddf_shoup();
    CHECK(b == c);

    d1 = GaloisFieldDict::from_vec({1_z}, 11_z);
    b = d1.gf_ddf_zassenhaus();
    CHECK(b.size() == 0);
    c = d1.gf_ddf_shoup();
    CHECK(b == c);
}

TEST_CASE("GaloisFieldDict equal degree factorization : Basic", "[basic]")
{
    std::vector<integer_class> a, mp;
    GaloisFieldDict d1, d2, d3, d4;

    d1 = GaloisFieldDict::from_vec({2_z, 1_z, 0_z, 1_z, 1_z}, 3_z);
    auto f = d1.gf_edf_zassenhaus(2);
    CHECK(f.size() == 2);
    auto it = f.find(GaloisFieldDict::from_vec({1_z, 0_z, 1_z}, 3_z));
    CHECK(it == f.begin());
    CHECK(f.find(GaloisFieldDict::from_vec({2_z, 1_z, 1_z}, 3_z)) == ++it);

    d1 = GaloisFieldDict::from_vec({}, 11_z);
    f = d1.gf_zassenhaus();
    CHECK(f.size() == 0);
    CHECK(f == d1.gf_shoup());

    d1 = GaloisFieldDict::from_vec({1_z}, 11_z);
    f = d1.gf_zassenhaus();
    CHECK(f.size() == 0);
    CHECK(f == d1.gf_shoup());

    d1 = GaloisFieldDict::from_vec({1_z, 1_z}, 11_z);
    f = d1.gf_zassenhaus();
    CHECK(f.size() == 1);
    CHECK(f.find(GaloisFieldDict::from_vec({1_z, 1_z}, 11_z)) != f.end());
    CHECK(f == d1.gf_shoup());

    d1 = GaloisFieldDict::from_vec({0_z, 1_z, 0_z, 0_z, 1_z}, 2_z);
    f = d1.gf_zassenhaus();
    CHECK(f.size() == 3);
    CHECK(f.find(GaloisFieldDict::from_vec({0_z, 1_z}, 2_z)) != f.end());
    CHECK(f.find(GaloisFieldDict::from_vec({1_z, 1_z}, 2_z)) != f.end());
    CHECK(f.find(GaloisFieldDict::from_vec({1_z, 1_z, 1_z}, 2_z)) != f.end());
    CHECK(f == d1.gf_shoup());

    d1 = GaloisFieldDict::from_vec({1_z, -3_z, -1_z, -3_z, 1_z, -3_z, 1_z},
                                   11_z);
    f = d1.gf_zassenhaus();
    CHECK(f.size() == 3);
    it = f.find(GaloisFieldDict::from_vec({1_z, 1_z}, 11_z));
    CHECK(it == f.begin());
    CHECK(f.find(GaloisFieldDict::from_vec({3_z, 5_z, 1_z}, 11_z)) == ++it);
    CHECK(f.find(GaloisFieldDict::from_vec({4_z, 3_z, 2_z, 1_z}, 11_z))
          == ++it);
    CHECK(f == d1.gf_shoup());
}

TEST_CASE("GaloisFieldDict factorization : Basic", "[basic]")
{
    GaloisFieldDict d1;

    d1 = GaloisFieldDict::from_vec({}, 11_z);
    auto f = d1.gf_factor();
    CHECK(f.second.size() == 0);
    CHECK(f.first == 0_z);

    d1 = GaloisFieldDict::from_vec({1_z}, 11_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 0);
    CHECK(f.first == 1_z);

    d1 = GaloisFieldDict::from_vec({1_z, 1_z}, 11_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 1);
    CHECK(f.first == 1_z);
    CHECK(f.second.find({GaloisFieldDict::from_vec({1_z, 1_z}, 11_z), 1})
          != f.second.end());

    d1 = GaloisFieldDict::from_vec({0_z, 1_z, 0_z, 0_z, 1_z}, 2_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 3);
    CHECK(f.first == 1_z);
    auto it = f.second.find({GaloisFieldDict::from_vec({0_z, 1_z}, 2_z), 1});
    CHECK(it == f.second.begin());
    CHECK(f.second.find({GaloisFieldDict::from_vec({1_z, 1_z}, 2_z), 1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec({1_z, 1_z, 1_z}, 2_z), 1})
          == ++it);

    d1 = GaloisFieldDict::from_vec({1_z, -3_z, -1_z, -3_z, 1_z, -3_z, 1_z},
                                   11_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 3);
    CHECK(f.first == 1_z);
    it = f.second.find({GaloisFieldDict::from_vec({1_z, 1_z}, 11_z), 1});
    CHECK(it == f.second.begin());
    CHECK(f.second.find({GaloisFieldDict::from_vec({3_z, 5_z, 1_z}, 11_z), 1})
          == ++it);
    CHECK(f.second.find(
              {GaloisFieldDict::from_vec({4_z, 3_z, 2_z, 1_z}, 11_z), 1})
          == ++it);

    d1 = GaloisFieldDict::from_vec({4_z, 8_z, 5_z, 1_z}, 11_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 2);
    CHECK(f.first == 1_z);
    CHECK(f.second.find({GaloisFieldDict::from_vec({1_z, 1_z}, 11_z), 1})
          == f.second.begin());
    CHECK(f.second.find({GaloisFieldDict::from_vec({2_z, 1_z}, 11_z), 2})
          != f.second.end());

    d1 = GaloisFieldDict::from_vec(
        {0_z, 0_z, 10_z, 10_z, 10_z, 0_z, 1_z, 10_z, 1_z, 1_z}, 11_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 3);
    CHECK(f.first == 1_z);
    it = f.second.find({GaloisFieldDict::from_vec({0_z, 1_z}, 11_z), 2});
    CHECK(it == f.second.begin());
    CHECK(f.second.find({GaloisFieldDict::from_vec({5_z, 9_z, 1_z}, 11_z), 1})
          == ++it);
    CHECK(f.second.find(
              {GaloisFieldDict::from_vec({2_z, 5_z, 8_z, 0_z, 3_z, 1_z}, 11_z),
               1})
          == ++it);

    d1 = GaloisFieldDict({{32, 1_z}, {0, 1_z}}, 11_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 2);
    CHECK(f.first == 1_z);
    CHECK(f.second.find(
              {GaloisFieldDict({{0, 10_z}, {8, 3_z}, {16, 1_z}}, 11_z), 1})
          == f.second.begin());
    CHECK(f.second.find(
              {GaloisFieldDict({{0, 10_z}, {8, 8_z}, {16, 1_z}}, 11_z), 1})
          != f.second.end());

    d1 = GaloisFieldDict({{32, 8_z}, {0, 5_z}}, 11_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 9);
    CHECK(f.first == 8_z);
    it = f.second.find({GaloisFieldDict::from_vec({3_z, 1_z}, 11_z), 2});
    CHECK(it == f.second.begin());
    CHECK(f.second.find({GaloisFieldDict::from_vec({8_z, 1_z}, 11_z), 1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec({2_z, 2_z, 1_z}, 11_z), 1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec({2_z, 9_z, 1_z}, 11_z), 2})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec({9_z, 0_z, 1_z}, 11_z), 1})
          == ++it);
    CHECK(f.second.find(
              {GaloisFieldDict::from_vec({7_z, 0_z, 5_z, 0_z, 1_z}, 11_z), 1})
          == ++it);
    CHECK(f.second.find(
              {GaloisFieldDict::from_vec({7_z, 0_z, 6_z, 0_z, 1_z}, 11_z), 2})
          == ++it);
    CHECK(
        f.second.find({GaloisFieldDict::from_vec(
                           {6_z, 0_z, 0_z, 0_z, 1_z, 0_z, 0_z, 0_z, 1_z}, 11_z),
                       1})
        == ++it);
    CHECK(f.second.find(
              {GaloisFieldDict::from_vec(
                   {6_z, 0_z, 0_z, 0_z, 10_z, 0_z, 0_z, 0_z, 1_z}, 11_z),
               1})
          == ++it);

    d1 = GaloisFieldDict({{63, 8_z}, {0, 5_z}}, 11_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 13);
    CHECK(f.first == 8_z);
    it = f.second.find({GaloisFieldDict::from_vec({7_z, 1_z}, 11_z), 1});
    CHECK(it == f.second.begin());
    CHECK(f.second.find({GaloisFieldDict::from_vec({5_z, 4_z, 1_z}, 11_z), 1})
          == ++it);
    CHECK(f.second.find(
              {GaloisFieldDict::from_vec({2_z, 8_z, 6_z, 1_z}, 11_z), 1})
          == ++it);
    CHECK(f.second.find(
              {GaloisFieldDict::from_vec({2_z, 9_z, 9_z, 1_z}, 11_z), 2})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 0_z, 0_z, 9_z, 0_z, 0_z, 1_z}, 11_z),
                         1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 4_z, 8_z, 0_z, 6_z, 2_z, 1_z}, 11_z),
                         1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 6_z, 0_z, 8_z, 3_z, 2_z, 1_z}, 11_z),
                         2})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 6_z, 4_z, 8_z, 0_z, 2_z, 1_z}, 11_z),
                         1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 6_z, 8_z, 0_z, 6_z, 5_z, 1_z}, 11_z),
                         1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 7_z, 10_z, 7_z, 4_z, 10_z, 1_z}, 11_z),
                         1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 8_z, 6_z, 1_z, 3_z, 3_z, 1_z}, 11_z),
                         2})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 8_z, 9_z, 7_z, 2_z, 6_z, 1_z}, 11_z),
                         1})
          == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {4_z, 9_z, 4_z, 1_z, 10_z, 10_z, 1_z}, 11_z),
                         1})
          == ++it);

    d1 = GaloisFieldDict({{15, 1_z}, {1, 1_z}, {0, 1_z}}, 102953_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 4);
    CHECK(f.first == 1_z);
    it = f.second.find(
        {GaloisFieldDict::from_vec({68144_z, 22730_z, 1_z}, 102953_z), 1});
    CHECK(it == f.second.begin());
    CHECK(
        f.second.find({GaloisFieldDict::from_vec(
                           {4724_z, 86810_z, 77449_z, 81553_z, 1_z}, 102953_z),
                       1})
        == ++it);
    CHECK(
        f.second.find({GaloisFieldDict::from_vec(
                           {31575_z, 14859_z, 56779_z, 86276_z, 1_z}, 102953_z),
                       1})
        == ++it);
    CHECK(f.second.find({GaloisFieldDict::from_vec(
                             {92335_z, 94508_z, 84569_z, 95022_z, 15347_z, 1_z},
                             102953_z),
                         1})
          == ++it);

    d1 = GaloisFieldDict::from_vec({38_z, 39_z, 41_z, 26_z, 5_z, 2_z, 1_z},
                                   53_z);
    f = d1.gf_factor();
    CHECK(f.second.size() == 2);
    CHECK(f.first == 1_z);
    CHECK(f.second.find({GaloisFieldDict::from_vec({26_z, 44_z, 1_z}, 53_z), 1})
          == f.second.begin());
    CHECK(
        f.second.find(
            {GaloisFieldDict::from_vec({30_z, 18_z, 25_z, 11_z, 1_z}, 53_z), 1})
        != f.second.end());
}

TEST_CASE("GaloisFieldDict gf_compose_mod, gf_trace_map : Basic", "[basic]")
{
    GaloisFieldDict d1, d2, d3;

    d1 = GaloisFieldDict::from_vec({1_z, 9_z, 4_z, 1_z, 1_z}, 11_z);
    d2 = GaloisFieldDict::from_vec({1_z, 1_z, 1_z}, 11_z);
    d3 = GaloisFieldDict::from_vec({2_z, 0_z, 0_z, 1_z}, 11_z);
    CHECK(d1.gf_compose_mod(d2, d3)
          == GaloisFieldDict::from_vec({10_z, 6_z, 9_z, 3_z}, 11_z));

    d2.dict_.clear();
    CHECK(d1.gf_compose_mod(d2, d3) == GaloisFieldDict::from_vec({}, 11_z));

    d2 = GaloisFieldDict::from_vec({1_z, 1_z, 1_z}, 11_z);
    auto d4 = GaloisFieldDict::from_vec({0_z, 1_z}, 11_z);
    d3 = d1.gf_pow_mod(d4, 11);
    CHECK(d1.gf_trace_map(d2, d3, d4, 0).first
          == GaloisFieldDict::from_vec({1_z, 1_z, 1_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 0).second
          == GaloisFieldDict::from_vec({1_z, 1_z, 1_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 1).first
          == GaloisFieldDict::from_vec({3_z, 10_z, 2_z, 5_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 1).second
          == GaloisFieldDict::from_vec({4_z, 0_z, 3_z, 5_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 2).first
          == GaloisFieldDict::from_vec({3_z, 5_z, 9_z, 5_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 2).second
          == GaloisFieldDict::from_vec({7_z, 5_z, 1_z, 10_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 3).first
          == GaloisFieldDict::from_vec({0_z, 6_z, 10_z, 1_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 3).second
          == GaloisFieldDict::from_vec({7_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 4).first
          == GaloisFieldDict::from_vec({1_z, 1_z, 1_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 4).second
          == GaloisFieldDict::from_vec({8_z, 1_z, 1_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 5).first
          == GaloisFieldDict::from_vec({3_z, 10_z, 2_z, 5_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 5).second
          == GaloisFieldDict::from_vec({0_z, 0_z, 3_z, 5_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 11).first
          == GaloisFieldDict::from_vec({0_z, 6_z, 10_z, 1_z}, 11_z));
    CHECK(d1.gf_trace_map(d2, d3, d4, 11).second
          == GaloisFieldDict::from_vec({10_z}, 11_z));
}

TEST_CASE("GaloisFieldDict eval : Basic", "[basic]")
{
    GaloisFieldDict d1;
    d1 = GaloisFieldDict::from_vec({}, 11_z);
    CHECK(d1.gf_eval(4_z) == 0_z);

    d1 = GaloisFieldDict::from_vec({7_z}, 11_z);
    CHECK(d1.gf_eval(4_z) == 7_z);
    CHECK(d1.gf_eval(15_z) == 7_z);

    d1 = GaloisFieldDict::from_vec(
        {0_z, 2_z, 1_z, 3_z, 4_z, 2_z, 3_z, 0_z, 1_z}, 11_z);
    CHECK(d1.gf_eval(0_z) == 0_z);

    d1 = GaloisFieldDict::from_vec(
        {0_z, 2_z, 1_z, 3_z, 4_z, 2_z, 3_z, 0_z, 1_z}, 11_z);
    CHECK(d1.gf_eval(4_z) == 9_z);

    d1 = GaloisFieldDict::from_vec(
        {0_z, 2_z, 1_z, 3_z, 4_z, 2_z, 3_z, 0_z, 1_z}, 11_z);
    CHECK(d1.gf_eval(27_z) == 5_z);

    d1 = GaloisFieldDict::from_vec(
        {5_z, 3_z, 1_z, 0_z, 6_z, 4_z, 0_z, 0_z, 4_z}, 11_z);
    CHECK(d1.gf_eval(0_z) == 5_z);

    d1 = GaloisFieldDict::from_vec(
        {5_z, 3_z, 1_z, 0_z, 6_z, 4_z, 0_z, 0_z, 4_z}, 11_z);
    CHECK(d1.gf_eval(4_z) == 3_z);

    d1 = GaloisFieldDict::from_vec(
        {5_z, 3_z, 1_z, 0_z, 6_z, 4_z, 0_z, 0_z, 4_z}, 11_z);
    CHECK(d1.gf_eval(27_z) == 9_z);

    d1 = GaloisFieldDict::from_vec({1_z, 2_z, 3_z}, 11_z);
    std::vector<integer_class> resa = {1_z, 6_z, 6_z, 1_z};
    CHECK(d1.gf_multi_eval({0_z, 1_z, 2_z, 3_z}) == resa);
}
