#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/series_generic_multivariate.h>
#include <symengine/series_generic.h>

using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_int_Expr;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::integer_class;
using SymEngine::rational;
using SymEngine::vec_basic_eq_perm;
using SymEngine::Expression;
using SymEngine::umap_short_basic;
using SymEngine::EulerGamma;
using SymEngine::Number;
using SymEngine::umap_int_basic;
using SymEngine::vec_int;
using SymEngine::MultivariateSeries;
using SymEngine::map_sym_uint;
using SymEngine::mult_series;
using SymEngine::mult_series1;
using SymEngine::map_basic_uint;
using SymEngine::UnivariateSeries;
using SymEngine::E;
using SymEngine::I;

using namespace SymEngine::literals;

template <typename Series>
bool check_series_term(RCP<const Basic> f, vec_int v, map_basic_uint &&m,
                       Expression e)
{
    RCP<const MultivariateSeries> s = mult_series<Series>(f, std::move(m));
    auto ptr = s->get_poly().dict_.find(v);
    if (ptr == s->get_poly().dict_.end()) {
        if (e == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return (e == expand(ptr->second));
    }
}

bool check_series_term_mult_series1(RCP<const Basic> f, vec_int v,
                                    map_sym_uint &&m, Expression e)
{
    RCP<const MultivariateSeries> s = mult_series1(f, std::move(m));
    auto ptr = s->get_poly().dict_.find(v);
    if (ptr == s->get_poly().dict_.end()) {
        if (e == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return (e == expand(ptr->second));
    }
}

TEST_CASE("Multivarable Expansion", "[MultivariateSeries][UnivariateSeries]")
{
    RCP<const Basic> f[18];
    RCP<const Integer> one = make_rcp<Integer>(1);
    RCP<const Integer> two = make_rcp<Integer>(2);
    RCP<const Integer> three = make_rcp<Integer>(3);
    RCP<const Symbol> w = symbol("w");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");

    f[0] = div(one, add(one, x));
    f[1] = cos(x);
    f[2] = sin(x);
    f[3] = pow(integer(2), x);
    f[4] = mul(sin(x), cos(x));
    f[5] = sin(atan(x));
    f[6] = sin(mul(a, x));
    f[7] = atan(div(x, sub(one, x)));
    f[8] = tan(x);
    f[9] = tan(div(x, sub(one, x)));
    f[10] = asin(x);
    f[11] = asin(div(x, sub(one, x)));
    f[12] = cot(x);

    for (unsigned int i = 0; i < 13; i++)
        f[i] = f[i]->subs({{{x, add(add(y, add(z, w)), integer(5))}}});

    Expression ans[18];

    ans[0] = Expression(rational(-5, 7776));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[0], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[0]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[0], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[0]));
    REQUIRE(check_series_term_mult_series1(f[0], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[0]));

    ans[1] = Expression(mul(rational(-1, 4), sin(integer(5))));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[1], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[1]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[1], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[1]));
    REQUIRE(check_series_term_mult_series1(f[1], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[1]));

    ans[2] = Expression(mul(rational(1, 4), cos(integer(5))));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[2], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[2]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[2], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[2]));
    REQUIRE(check_series_term_mult_series1(f[2], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[2]));

    ans[3] = Expression(
        mul(rational(1, 4), mul(pow(log(integer(2)), integer(5)),
                                pow(E, mul(integer(5), log(integer(2)))))));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[3], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[3]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[3], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}},
        ans[3])); // (1/4)*log(2)**5*E**(5*log(2))
    REQUIRE(check_series_term_mult_series1(
        f[3], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}},
        Expression(mul(integer(8), pow(log(integer(2)), integer(5))))));

    ans[4] = Expression(add(mul(integer(-4), pow(sin(integer(5)), integer(2))),
                            mul(integer(4), pow(cos(integer(5)), integer(2)))));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[4], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[4]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[4], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[4]));
    REQUIRE(check_series_term_mult_series1(f[4], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[4]));

    ans[5] = Expression(
        add(mul(rational(7275, 11881376), sin(atan(integer(5)))),
            mul(rational(66045, 47525504), cos(atan(integer(5))))));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[5], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[5]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[5], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[5]));
    REQUIRE(check_series_term_mult_series1(f[5], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[5]));

    ans[6] = Expression(
        mul(rational(1, 4), mul(pow(a, integer(5)), cos(mul(integer(5), a)))));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[6], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[6]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[6], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[6]));
    REQUIRE(check_series_term_mult_series1(f[6], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[6]));

    ans[7] = Expression(rational(191976, 115856201));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[7], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[7]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[7], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[7]));
    REQUIRE(check_series_term_mult_series1(f[7], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[7]));

    ans[8] = Expression(add(
        integer(4),                                             // 4
        add(mul(integer(34), pow(tan(integer(5)), integer(2))), //+ 34*tan(5)**2
            add(mul(integer(60),
                    pow(tan(integer(5)), integer(4))), //+ 60*tan(5)**4
                mul(integer(30),
                    pow(tan(integer(5)), integer(6))))))); //+ 30*tan(5)**6
    REQUIRE(check_series_term<MultivariateSeries>(
        f[8], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[8]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[8], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[8]));
    REQUIRE(check_series_term_mult_series1(f[8], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[8]));

    ans[9] = Expression(add(
        rational(2161, 262144), // 2161/262144
        add(mul(rational(5777, 524288),
                pow(tan(rational(5, 4)),
                    integer(2))), //+ (5777/524288)*tan(5/4)**2
            add(mul(rational(265, 32768),
                    pow(tan(rational(5, 4)),
                        integer(3))), //+ (265/32768)*tan(5/4)**3
                add(mul(rational(735, 262144),
                        pow(tan(rational(5, 4)),
                            integer(4))), //+ (735/262144)*tan(5/4)**4
                    add(mul(rational(15, 32768),
                            pow(tan(rational(5, 4)),
                                integer(5))), //+ (15/32768)*tan(5/4)**5
                        add(mul(rational(15, 524288),
                                pow(tan(rational(5, 4)),
                                    integer(6))), //+ (15/524288)*tan(5/4)**6
                            mul(rational(125, 16384),
                                tan(rational(
                                    5, 4)))))))))); //+ (125/16384)*tan(5/4)
    REQUIRE(check_series_term<MultivariateSeries>(
        f[9], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[9]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[9], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[9]));
    REQUIRE(check_series_term_mult_series1(f[9], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[9]));

    ans[10]
        = Expression(mul(rational(-5603, 10616832), mul(I, sqrt(integer(24)))));

    REQUIRE(check_series_term<MultivariateSeries>(
        f[10], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[10]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[10], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[10]));
    REQUIRE(check_series_term_mult_series1(f[10], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[10]));

    ans[11] = Expression(mul(rational(-13981, 3359232), I));
    REQUIRE(check_series_term<MultivariateSeries>(
        f[11], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[11]));
    REQUIRE(check_series_term<UnivariateSeries>(
        f[11], {2, 1, 2}, {{w, 3}, {y, 3}, {z, 3}}, ans[11]));
    REQUIRE(check_series_term_mult_series1(f[11], {2, 1, 2},
                                           {{w, 3}, {y, 3}, {z, 3}}, ans[11]));
}