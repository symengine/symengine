#include "catch.hpp"
#include <chrono>

#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/pow.h>
#include <symengine/symengine_exception.h>

using SymEngine::SymEngineException;
using SymEngine::UIntPoly;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;
using SymEngine::integer_class;
using SymEngine::UIntDict;
using SymEngine::add;
using SymEngine::vec_integer_class;
using SymEngine::RCPBasicKeyLess;

using namespace SymEngine::literals;

TEST_CASE("Constructor of UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> P
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UIntPoly> Q = UIntPoly::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPoly> R = UIntPoly::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(R->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPoly> S = UIntPoly::from_dict(x, {{0, 2_z}});
    REQUIRE(S->__str__() == "2");

    RCP<const UIntPoly> T = UIntPoly::from_dict(x, map_uint_mpz{});
    REQUIRE(T->__str__() == "0");

    RCP<const UIntPoly> U
        = UIntPoly::from_dict(x, {{0, 2_z}, {1, 0_z}, {2, 0_z}});
    REQUIRE(U->__str__() == "2");
}

TEST_CASE("Adding two UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPoly> b
        = UIntPoly::from_dict(x, {{0, 2_z}, {1, 3_z}, {2, 4_z}});

    RCP<const Basic> c = add_upoly(*a, *b);
    REQUIRE(c->__str__() == "5*x**2 + 5*x + 3");

    RCP<const UIntPoly> d = UIntPoly::from_dict(x, {{0, 1_z}});
    RCP<const Basic> e = add_upoly(*a, *d);
    RCP<const Basic> f = add_upoly(*d, *a);
    REQUIRE(e->__str__() == "x**2 + 2*x + 2");
    REQUIRE(f->__str__() == "x**2 + 2*x + 2");

    RCP<const UIntPoly> g
        = UIntPoly::from_dict(y, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    CHECK_THROWS_AS(add_upoly(*a, *g), SymEngineException);
}

TEST_CASE("Negative of a UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPoly> b = UIntPoly::from_dict(x, map_uint_mpz{});

    RCP<const UIntPoly> c = neg_upoly(*a);
    RCP<const UIntPoly> d = neg_upoly(*b);
    REQUIRE(c->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(d->__str__() == "0");
}

TEST_CASE("Subtracting two UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPoly> b
        = UIntPoly::from_dict(x, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    RCP<const UIntPoly> c = UIntPoly::from_dict(x, {{0, 2_z}});
    RCP<const UIntPoly> f = UIntPoly::from_dict(y, {{0, 2_z}});

    RCP<const Basic> d = sub_upoly(*b, *a);
    REQUIRE(d->__str__() == "3*x**2 + x + 1");
    d = sub_upoly(*c, *a);
    REQUIRE(d->__str__() == "-x**2 - 2*x + 1");
    d = sub_upoly(*a, *c);
    REQUIRE(d->__str__() == "x**2 + 2*x - 1");
    CHECK_THROWS_AS(sub_upoly(*a, *f), SymEngineException);
}

TEST_CASE("Multiplication of two UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPoly> b
        = UIntPoly::from_dict(x, {{0, -1_z}, {1, -2_z}, {2, -1_z}});
    RCP<const UIntPoly> e
        = UIntPoly::from_dict(x, {{0, 5_z}, {1, -2_z}, {2, -1_z}});
    RCP<const UIntPoly> f
        = UIntPoly::from_dict(x, {{0, 6_z}, {1, -2_z}, {2, 3_z}});
    RCP<const UIntPoly> k
        = UIntPoly::from_dict(x, {{0, -1_z}, {1, -2_z}, {2, -100_z}});

    RCP<const UIntPoly> c = mul_upoly(*a, *a);
    RCP<const UIntPoly> d = mul_upoly(*a, *b);
    RCP<const UIntPoly> g = mul_upoly(*e, *e);
    RCP<const UIntPoly> h = mul_upoly(*e, *f);
    RCP<const UIntPoly> i = mul_upoly(*f, *f);
    RCP<const UIntPoly> l = mul_upoly(*k, *f);
    RCP<const UIntPoly> m = mul_upoly(*k, *k);

    REQUIRE(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
    REQUIRE(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
    REQUIRE(g->__str__() == "x**4 + 4*x**3 - 6*x**2 - 20*x + 25");
    REQUIRE(h->__str__() == "-3*x**4 - 4*x**3 + 13*x**2 - 22*x + 30");
    REQUIRE(i->__str__() == "9*x**4 - 12*x**3 + 40*x**2 - 24*x + 36");
    REQUIRE(l->__str__() == "-300*x**4 + 194*x**3 - 599*x**2 - 10*x - 6");
    REQUIRE(m->__str__() == "10000*x**4 + 400*x**3 + 204*x**2 + 4*x + 1");

    c = UIntPoly::from_dict(x, {{0, -1_z}});
    REQUIRE(mul_upoly(*a, *c)->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(mul_upoly(*c, *a)->__str__() == "-x**2 - 2*x - 1");

    c = UIntPoly::from_dict(y, {{0, -1_z}});
    CHECK_THROWS_AS(mul_upoly(*a, *c), SymEngineException);
}

TEST_CASE("Comparing two UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UIntPoly> P = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}});
    RCP<const UIntPoly> Q
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(P->compare(*Q) == -1);

    P = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 3_z}, {3, 2_z}});
    REQUIRE(P->compare(*Q) == 1);

    P = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {3, 3_z}});
    REQUIRE(P->compare(*Q) == 1);

    P = UIntPoly::from_dict(y, {{0, 1_z}, {1, 2_z}, {3, 3_z}});
    REQUIRE(P->compare(*Q) == 1);
}

TEST_CASE("UIntPoly as_symbolic", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(eq(*a->as_symbolic(),
               *add({one, mul(integer(2), x), pow(x, integer(2))})));
    REQUIRE(not eq(*a->as_symbolic(),
                   *add({one, mul(integer(3), x), pow(x, integer(2))})));

    RCP<const UIntPoly> b
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 1_z}, {2, 2_z}});
    REQUIRE(eq(*b->as_symbolic(),
               *add({one, x, mul(integer(2), pow(x, integer(2)))})));

    RCP<const UIntPoly> c = UIntPoly::from_dict(x, map_uint_mpz{});
    REQUIRE(eq(*c->as_symbolic(), *zero));
}

TEST_CASE("Evaluation of UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPoly> b
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 0_z}, {2, -1_z}});

    REQUIRE(a->eval(2_z) == 9);
    REQUIRE(a->eval(10_z) == 121);
    REQUIRE(b->eval(-1_z) == 0);
    REQUIRE(b->eval(0_z) == 1);

    vec_integer_class resa = {9_z, 121_z, 0_z, 1_z};
    vec_integer_class resb = {-3_z, -99_z, 0_z, 1_z};
    REQUIRE(a->multieval({2_z, 10_z, -1_z, 0_z}) == resa);
    REQUIRE(b->multieval({2_z, 10_z, -1_z, 0_z}) == resb);
}

TEST_CASE("Derivative of UIntPoly", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPoly> b = UIntPoly::from_dict(y, {{2, 4_z}});

    REQUIRE(a->diff(x)->__str__() == "2*x + 2");
    REQUIRE(a->diff(y)->__str__() == "0");
    REQUIRE(b->diff(y)->__str__() == "8*y");
}

TEST_CASE("UIntPoly expand", "[UIntPoly][expand]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a
        = UIntPoly::from_dict(x, {{1, 1_z}, {2, 1_z}, {3, 1_z}});
    RCP<const Basic> b = make_rcp<const Pow>(a, integer(3));
    RCP<const Basic> c = expand(b);

    REQUIRE(b->__str__() == "(x**3 + x**2 + x)**3");
    REQUIRE(c->__str__()
            == "x**9 + 3*x**8 + 6*x**7 + 7*x**6 + 6*x**5 + 3*x**4 + x**3");
}

TEST_CASE("UIntPoly pow", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a = UIntPoly::from_dict(x, {{0, 1_z}, {1, 1_z}});
    RCP<const UIntPoly> b = UIntPoly::from_dict(x, {{0, 3_z}, {2, 1_z}});

    RCP<const UIntPoly> aaa = pow_upoly(*a, 3);
    RCP<const UIntPoly> bb = pow_upoly(*b, 2);

    REQUIRE(aaa->__str__() == "x**3 + 3*x**2 + 3*x + 1");
    REQUIRE(bb->__str__() == "x**4 + 6*x**2 + 9");
}

TEST_CASE("UIntPoly divides", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> a = UIntPoly::from_dict(x, {{0, 1_z}, {1, 1_z}});
    RCP<const UIntPoly> b = UIntPoly::from_dict(x, {{0, 4_z}});
    RCP<const UIntPoly> c = UIntPoly::from_dict(x, {{0, 8_z}, {1, 8_z}});
    RCP<const UIntPoly> res;

    REQUIRE(divides_upoly(*a, *c, outArg(res)));
    REQUIRE(res->__str__() == "8");
    REQUIRE(divides_upoly(*b, *c, outArg(res)));
    REQUIRE(res->__str__() == "2*x + 2");
    REQUIRE(!divides_upoly(*b, *a, outArg(res)));
}

TEST_CASE("UIntDict hensel_lift", "[UIntPoly]")
{
    UIntDict f = UIntDict::from_vec({-1_z, 0_z, 0_z, 0_z, 1_z});
    UIntDict g = UIntDict::from_vec({-1_z, 1_z});
    UIntDict h = UIntDict::from_vec({-2_z, 1_z});
    UIntDict s = UIntDict::from_vec({2_z, 1_z});
    UIntDict t = UIntDict::from_vec({1_z, 1_z});
    std::vector<UIntDict> v = {g, h, s, t};
    auto out = f.zz_hensel_lift(5_z, v, 4);
    REQUIRE(std::find(out.begin(), out.end(), UIntDict::from_vec({-1_z, 1_z}))
            != out.end());
    REQUIRE(std::find(out.begin(), out.end(), UIntDict::from_vec({1_z, 1_z}))
            != out.end());
    REQUIRE(std::find(out.begin(), out.end(), UIntDict::from_vec({-182_z, 1_z}))
            != out.end());
    REQUIRE(std::find(out.begin(), out.end(), UIntDict::from_vec({182_z, 1_z}))
            != out.end());
    REQUIRE(out.size() == 4);

    UIntDict G, H, S, T;
    f = UIntDict::from_vec({-1_z, 0_z, 0_z, 0_z, 1_z});
    g = UIntDict::from_vec({-2_z, -1_z, 2_z, 1_z});
    h = UIntDict::from_vec({-2_z, 1_z});
    s = UIntDict::from_vec({-2_z});
    t = UIntDict::from_vec({-1_z, -2_z, 2_z});
    UIntDict::zz_hensel_step(5_z, f, g, h, s, t, outArg(G), outArg(H),
                             outArg(S), outArg(T));
    REQUIRE(G == UIntDict::from_vec({-7_z, -1_z, 7_z, 1_z}));
    REQUIRE(H == UIntDict::from_vec({-7_z, 1_z}));
    REQUIRE(S == UIntDict::from_vec({8_z}));
    REQUIRE(T == UIntDict::from_vec({-1_z, -12_z, -8_z}));
}
TEST_CASE("UIntPoly zassenhaus", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    auto find_in_set
        = [](RCP<const UIntPoly> to_find,
             std::set<RCP<const UIntPoly>, RCPBasicKeyLess> in_set) {
              for (auto &a : in_set) {
                  if (eq(*a, *to_find))
                      return true;
              }
              return false;
          };

    RCP<const UIntPoly> f = UIntPoly::from_vec(x, {1_z, 0_z, -9_z});
    std::set<RCP<const UIntPoly>, RCPBasicKeyLess> out = f->zz_zassenhaus();
    REQUIRE(find_in_set(UIntPoly::from_vec(x, {1_z, -3_z}), out));
    REQUIRE(find_in_set(UIntPoly::from_vec(x, {-1_z, -3_z}), out));
    REQUIRE(out.size() == 2);

    f = UIntPoly::from_vec(x, {-6_z, 11_z, -6_z, 1_z});
    out = f->zz_zassenhaus();
    REQUIRE(find_in_set(UIntPoly::from_vec(x, {-1_z, 1_z}), out));
    REQUIRE(find_in_set(UIntPoly::from_vec(x, {-2_z, 1_z}), out));
    REQUIRE(find_in_set(UIntPoly::from_vec(x, {-3_z, 1_z}), out));
    REQUIRE(out.size() == 3);

    f = UIntPoly::from_vec(x, {10_z, 13_z, 10_z, 3_z});
    out = f->zz_zassenhaus();
    REQUIRE(find_in_set(UIntPoly::from_vec(x, {2_z, 1_z}), out));
    REQUIRE(find_in_set(UIntPoly::from_vec(x, {5_z, 4_z, 3_z}), out));
    REQUIRE(out.size() == 2);
}
