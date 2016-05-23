#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/series_generic_multivariate.h>

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
using SymEngine::set_sym;
using SymEngine::vec_int;
using SymEngine::MultivariateSeries;
using SymEngine::MultivariateExprPolynomial;
using SymEngine::MultivariatePolynomial;
using SymEngine::map_sym_uint;
using SymEngine::vec_sym;

using namespace SymEngine::literals;

TEST_CASE("Create MultivariateSeries", "[MultivariateSeries]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression num1(integer(2));                             // 2
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp2(integer(2) - Expression(symbol("d")));  //(2 - d)
    Expression comp3(integer(-3) + Expression(symbol("e"))); //(-3 + e)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)
    vec_sym s;
    vec_int v;

    MultivariateExprPolynomial p1(
        MultivariatePolynomial::multivariate_polynomial(
            {x, y},
            {{{1, 1}, a}, {{1, 2}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}}));
    MultivariateExprPolynomial p2(
        MultivariatePolynomial::multivariate_polynomial({x, y},
                                                        {{{1, 0}, comp1},
                                                         {{0, 0}, comp2},
                                                         {{2, 2}, comp3},
                                                         {{3, 4}, comp4}}));
    MultivariateExprPolynomial p3(
        MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, Expression(integer(0))}}));
    MultivariateExprPolynomial p4(
        MultivariatePolynomial::multivariate_polynomial(s,
                                                        {{v, Expression(0)}}));
    MultivariateExprPolynomial p5(
        MultivariatePolynomial::multivariate_polynomial(s, {{v, comp1}}));

    // REQUIRE(p1->__str__() == "2*x**2 y - b*x y**2 + a*x y - 3*y");
    // REQUIRE(p2->__str__()
    //         == "(-4 - f)*x**3 y**4 + (-3 + e)*x**2 y**2 + (1 + c)*x + (2 -
    //         d)");
    // REQUIRE(p3->__str__() == "0");
    // REQUIRE(p4->__str__() == "0");
    // REQUIRE(p5->__str__() == "(1 + c)");
    // RCP<const MultivariateSeries> ms1 = make_rcp<const MultivariateSeries>(
    // p1, "x", 5, 0, {{x, 5}, {y, 3}});
    MultivariateSeries ms1(p1, "x", 5, {{x, 5}, {y, 3}});
    // RCP<const MultivariateSeries> ms2 = make_rcp<const MultivariateSeries>({
    // p2, "x", 5, 0, {{x, 5}, {y, 3}} });
    MultivariateSeries ms2(p2, "x", 4, {{x, 4}, {y, 4}});
    // RCP<const MultivariateSeries> ms3 = make_rcp<const MultivariateSeries>({
    // p3, "x", 5, 0, {{x, 5}, {y, 3}} });
    MultivariateSeries ms3(p3, "x", 3, {{x, 3}, {y, 5}});
    // RCP<const MultivariateSeries> ms4 = make_rcp<const MultivariateSeries>({
    // p4, "", 0, 0, {{x, 5}, {y, 3}} });
    MultivariateSeries ms4(p4, "", 0, {{x, 2}, {y, 4}});
    // RCP<const MultivariateSeries> ms5 = make_rcp<const MultivariateSeries>({
    // p5, "", 0, 0, {{x, 5}, {y, 3}} });
    MultivariateSeries ms5(p5, "", 0, {{x, 1}, {y, 1}});

    REQUIRE(ms1.__str__()
            == "2*x**2 y - b*x y**2 + a*x y - 3*y + O(|x|**5 + |y|**3)");
    REQUIRE(ms2.__str__() == "(-4 - f)*x**3 y**4 + (-3 + e)*x**2 y**2 + (1 + "
                             "c)*x + (2 - d) + O(|x|**4 + |y|**4)");
    REQUIRE(ms3.__str__() == "0 + O(|x|**3 + |y|**5)");
    REQUIRE(ms4.__str__() == "0 + O(|x|**2 + |y|**4)");
    REQUIRE(ms5.__str__() == "(1 + c) + O(|x|**1 + |y|**1)");
}

TEST_CASE("Adding two MultivariateSeries", "[MultivariateSeries]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(div(b, a));

    map_sym_uint m1 = {{x, 2}, {y, 3}};
    map_sym_uint m2 = {{y, 4}, {z, 2}};
    map_sym_uint m3 = {{x, 2}, {y, 3}, {z, 2}};

    MultivariateExprPolynomial p1(
        MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, expr1}, {{0, 1}, expr2}, {{2, 3}, expr1}}));
    MultivariateExprPolynomial p2(
        MultivariatePolynomial::multivariate_polynomial(
            {y, z}, {{{0, 0}, expr4}, {{4, 2}, expr1}}));

    MultivariateExprPolynomial q1(
        MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {{{0, 0, 0}, expr1 + expr4},
                        {{0, 1, 0}, expr2},
                        {{2, 3, 0}, expr1}}));
    MultivariateExprPolynomial q2(
        MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, expr1 + 3}, {{0, 1}, expr2}, {{2, 3}, expr1}}));

    RCP<const MultivariateSeries> s1
        = make_rcp<const MultivariateSeries>(p1, "x", 2, m1);
    RCP<const MultivariateSeries> s2
        = make_rcp<const MultivariateSeries>(p2, "y", 4, m2);

    RCP<const MultivariateSeries> r1
        = make_rcp<const MultivariateSeries>(q1, "x", 2, m3);

    RCP<const Number> n = integer(3);

    RCP<const MultivariateSeries> r2
        = make_rcp<const MultivariateSeries>(q2, "x", 2, m1);

    REQUIRE(eq(*s1->add(*s2), *r1));
    REQUIRE(eq(*s1->add(*n), *r2));
}

TEST_CASE("Multiplying two MultivariateSeries", "[MultivariateSeries]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(div(b, a));

    map_sym_uint m1 = {{x, 2}, {y, 3}};
    map_sym_uint m2 = {{y, 4}, {z, 2}};
    map_sym_uint m3 = {{x, 2}, {y, 3}, {z, 2}};

    MultivariateExprPolynomial p1(
        MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, expr1}, {{0, 1}, expr2}, {{2, 3}, expr1}}));
    MultivariateExprPolynomial p2(
        MultivariatePolynomial::multivariate_polynomial(
            {y, z}, {{{0, 0}, expr4}, {{4, 2}, expr1}}));

    MultivariateExprPolynomial q1(
        MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {
                           {{0, 0, 0}, expr1 * expr4},
                           {{0, 1, 0}, expr2 * expr4},
                           {{2, 3, 0}, expr1 * expr4},
                       }));

    MultivariateExprPolynomial q2(
        MultivariatePolynomial::multivariate_polynomial(
            {x, y},
            {{{0, 0}, expr1 * 3}, {{0, 1}, expr2 * 3}, {{2, 3}, expr1 * 3}}));

    RCP<const MultivariateSeries> s1
        = make_rcp<const MultivariateSeries>(p1, "x", 2, m1);
    RCP<const MultivariateSeries> s2
        = make_rcp<const MultivariateSeries>(p2, "y", 4, m2);

    RCP<const MultivariateSeries> r1
        = make_rcp<const MultivariateSeries>(q1, "x", 2, m3);

    RCP<const Number> n = integer(3);

    RCP<const MultivariateSeries> r2
        = make_rcp<const MultivariateSeries>(q2, "x", 2, m1);

    REQUIRE(eq(*s1->mul(*s2), *r1));
    REQUIRE(eq(*s1->mul(*n), *r2));
}

TEST_CASE("Testing MultivariateSeries::__eq__(), __hash__, compare",
          "[MultivariateSeries]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(div(b, a));

    MultivariateExprPolynomial p1(
        MultivariatePolynomial::multivariate_polynomial({x, y, z},
                                                        {{{0, 0, 0}, expr1},
                                                         {{1, 0, 0}, expr2},
                                                         {{0, 1, 1}, expr3},
                                                         {{0, 2, 0}, expr4}}));
    MultivariateExprPolynomial p2(
        MultivariatePolynomial::multivariate_polynomial({x, y, z},
                                                        {{{0, 1, 0}, expr1},
                                                         {{2, 0, 0}, expr2},
                                                         {{3, 1, 1}, expr3},
                                                         {{1, 2, 4}, expr4}}));

    map_sym_uint m1 = {{x, 2}, {y, 2}, {z, 3}};
    map_sym_uint m2 = {{x, 2}, {y, 2}, {z, 10}};
    map_sym_uint m3 = {{x, 3}, {y, 2}, {z, 4}};

    RCP<const MultivariateSeries> s1
        = make_rcp<const MultivariateSeries>(p1, "x", 2, m1);
    RCP<const MultivariateSeries> s2
        = make_rcp<const MultivariateSeries>(p1, "x", 2, m2);
    RCP<const MultivariateSeries> s3
        = make_rcp<const MultivariateSeries>(p2, "z", 4, m3);

    REQUIRE(s1->__eq__(*s1));
    REQUIRE(!s1->__eq__(*s2));
    REQUIRE(!s1->__eq__(*s3));

    REQUIRE(s1->compare(*s1) == 0);
    REQUIRE(s1->compare(*s2) != 0);
    REQUIRE(s1->compare(*s3) != 0);

    REQUIRE(s1->__hash__() == s1->__hash__());
    REQUIRE(s1->__hash__() != s2->__hash__());
    REQUIRE(s1->__hash__() != s3->__hash__());
}

TEST_CASE("Integration of MultivariateSeries", "[MultivariateSeries]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    MultivariateExprPolynomial ex(
        MultivariatePolynomial::multivariate_polynomial(
            {x}, {{{1}, Expression(1)}}));
    MultivariateExprPolynomial why(
        MultivariatePolynomial::multivariate_polynomial(
            {y}, {{{1}, Expression(1)}}));
    MultivariateExprPolynomial zee(
        MultivariatePolynomial::multivariate_polynomial(
            {z}, {{{1}, Expression(1)}}));
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(div(b, a));
    MultivariateExprPolynomial p(
        MultivariatePolynomial::multivariate_polynomial({x, y},
                                                        {{{2, 1}, expr1},
                                                         {{1, 2}, expr2},
                                                         {{2, 0}, expr3},
                                                         {{0, 2}, expr4},
                                                         {{1, 0}, expr1},
                                                         {{0, 1}, expr2},
                                                         {{0, 0}, expr3}}));

    MultivariateExprPolynomial q1(
        MultivariatePolynomial::multivariate_polynomial({x, y},
                                                        {{{3, 1}, expr1 / 3},
                                                         {{2, 2}, expr2 / 2},
                                                         {{3, 0}, expr3 / 3},
                                                         {{1, 2}, expr4},
                                                         {{2, 0}, expr1 / 2},
                                                         {{1, 1}, expr2},
                                                         {{1, 0}, expr3}}));
    MultivariateExprPolynomial q2(
        MultivariatePolynomial::multivariate_polynomial({x, y},
                                                        {{{2, 2}, expr1 / 2},
                                                         {{1, 3}, expr2 / 3},
                                                         {{2, 1}, expr3},
                                                         {{0, 3}, expr4 / 3},
                                                         {{1, 1}, expr1},
                                                         {{0, 2}, expr2 / 2},
                                                         {{0, 1}, expr3}}));
    MultivariateExprPolynomial q3(
        MultivariatePolynomial::multivariate_polynomial({x, y, z},
                                                        {{{2, 1, 1}, expr1},
                                                         {{1, 2, 1}, expr2},
                                                         {{2, 0, 1}, expr3},
                                                         {{0, 2, 1}, expr4},
                                                         {{1, 0, 1}, expr1},
                                                         {{0, 1, 1}, expr2},
                                                         {{0, 0, 1}, expr3}}));

    REQUIRE(MultivariateSeries::integrate(p, ex) == q1);
    REQUIRE(MultivariateSeries::integrate(p, why) == q2);
    REQUIRE(MultivariateSeries::integrate(p, zee) == q3);
}

#define series_coeff(EX, SYM, PREC, COEFF)                                     \
    MultivariateSeries::series(EX, SYM->get_name(), PREC)->get_coeff(COEFF)
#define invseries_coeff(EX, SYM, PREC, COEFF)                                  \
    MultivariateSeries(                                                        \
        MultivariateSeries::series_reverse(                                    \
            MultivariateSeries::series(EX, SYM->get_name(), PREC)->get_poly(), \
            MultivariateSeries::var(SYM->get_name()), PREC),                   \
        SYM->get_name(), PREC)                                                 \
        .get_coeff(COEFF)

static bool expand_check_pairs(const RCP<const Basic> &ex,
                               const RCP<const Symbol> &x, int prec,
                               const umap_short_basic &pairs)
{
    auto ser = SymEngine::MultivariateSeries::series(ex, x->get_name(), prec);
    for (auto it : pairs) {
        if (not it.second->__eq__(*(ser->get_coeff(it.first))))
            return false;
    }
    return true;
}

TEST_CASE("Expression series expansion: Add ", "[Expansion of Add]")
{
    RCP<const Symbol> x = symbol("x"), y = symbol("y");
    auto z = add(integer(1), x);
    z = sub(z, pow(x, integer(2)));
    z = add(z, pow(x, integer(4)));
    auto a = sub(integer(1), pow(symbol("x"), integer(2)));
    a = add(symbol("a"), a);

    auto vb = umap_short_basic{
        {0, integer(1)}, {1, integer(1)}, {2, integer(-1)}, {4, integer(1)}};
    // REQUIRE(expand_check_pairs(z, x, 5, vb));
    auto vb1
        = umap_short_basic{{0, integer(1)}, {1, integer(1)}, {2, integer(-1)}};
    REQUIRE(expand_check_pairs(z, x, 3, vb1));
    auto vc = umap_short_basic{
        {0, add(integer(1), symbol("a"))}, {1, integer(0)}, {2, integer(-1)}};
    // REQUIRE(expand_check_pairs(a, x, 5, vc));
}

TEST_CASE("Expression series expansion: sin, cos", "[Expansion of sin, cos]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Integer> one = integer(1);
    auto z1 = sin(x);
    auto z2 = cos(x);
    auto z3 = add(sin(x), cos(x));
    auto z4 = mul(sin(x), cos(x));
    auto z5 = sin(atan(x));
    auto z6 = cos(div(x, sub(one, x)));
    auto z7 = sin(mul(symbol("a"), x));

    REQUIRE(series_coeff(z1, x, 10, 9)->__eq__(*rational(1, 362880)));
    auto res = umap_short_basic{{0, integer(1)}, {2, rational(-1, 2)}};
    REQUIRE(expand_check_pairs(z2, x, 3, res));
    REQUIRE(series_coeff(z3, x, 9, 8)->__eq__(*rational(1, 40320)));
    REQUIRE(series_coeff(z4, x, 12, 11)->__eq__(*rational(-4, 155925)));
    REQUIRE(series_coeff(z5, x, 30, 27)->__eq__(*rational(-1300075, 8388608)));
    REQUIRE(series_coeff(z6, x, 15, 11)->__eq__(*rational(-125929, 362880)));
    REQUIRE(series_coeff(z7, x, 10, 9)
                ->__eq__(
                    *mul((pow(symbol("a"), integer(9))), rational(1, 362880))));
}

TEST_CASE("Expression series expansion: division, inversion ",
          "[Expansion of 1/ex]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Integer> one = integer(1);
    RCP<const Integer> two = integer(2);
    RCP<const Integer> three = integer(3);
    auto ex1 = div(one, sub(one, x));                 // 1/(1-x)
    auto ex2 = div(x, sub(sub(one, x), pow(x, two))); // x/(1-x-x^2)
    auto ex3
        = div(pow(x, three), sub(one, mul(pow(x, two), two))); // x^3/(1-2x^2)
    auto ex4 = div(one, sub(one, sin(x)));                     // 1/(1-sin(x))
    auto ex5 = div(one, x);
    auto ex6 = div(one, mul(x, sub(one, x)));
    auto ex7 = div(one, mul(symbol("a"), x));
    auto res1 = umap_short_basic{{-1, integer(1)}};
    auto res2 = umap_short_basic{{-1, integer(1)}, {0, integer(1)}};
    auto res3 = umap_short_basic{{-1, div(integer(1), symbol("a"))}};

    REQUIRE(series_coeff(ex1, x, 100, 99)->__eq__(*integer(1)));
    REQUIRE(series_coeff(ex2, x, 100, 35)->__eq__(*integer(9227465)));
    REQUIRE(series_coeff(ex3, x, 100, 49)->__eq__(*integer(8388608)));
    REQUIRE(series_coeff(ex4, x, 20, 10)->__eq__(*rational(1382, 14175)));
    REQUIRE(expand_check_pairs(ex5, x, 8, res1));
    REQUIRE(expand_check_pairs(ex6, x, 8, res2));
    REQUIRE(expand_check_pairs(ex7, x, 8, res3));
}

TEST_CASE("Expression series expansion: roots", "[Expansion of root(ex)]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Number> q12 = rational(1, 2);
    RCP<const Number> qm23 = rational(-2, 3);
    RCP<const Integer> one = integer(1);
    RCP<const Integer> four = integer(4);
    auto ex1 = pow(sub(four, x), q12);
    auto ex2 = pow(sub(one, x), qm23);
    auto ex3 = sqrt(sub(one, x));
    auto ex4 = pow(cos(x), q12);
    auto ex5 = pow(cos(x), qm23);
    auto ex6 = sqrt(cos(x));

    REQUIRE(series_coeff(ex1, x, 8, 6)->__eq__(*rational(-21, 2097152)));
    REQUIRE(series_coeff(ex2, x, 12, 10)->__eq__(*rational(1621477, 4782969)));
    REQUIRE(series_coeff(ex3, x, 12, 10)->__eq__(*rational(-2431, 262144)));
    REQUIRE(series_coeff(ex4, x, 100, 8)->__eq__(*rational(-559, 645120)));
    REQUIRE(series_coeff(ex5, x, 20, 10)->__eq__(*rational(701, 127575)));
    REQUIRE(series_coeff(ex6, x, 10, 8)->__eq__(*rational(-559, 645120)));
}

TEST_CASE("Expression series expansion: log, exp ", "[Expansion of log, exp]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Integer> one = integer(1);
    RCP<const Integer> two = integer(2);
    RCP<const Integer> three = integer(3);
    auto ex1 = log(add(one, x));
    auto ex2 = log(cos(x));
    auto ex3 = log(div(one, sub(one, x)));
    auto ex4 = exp(x);
    auto ex5 = exp(log(add(x, one)));
    auto ex6 = log(exp(x));
    auto ex7 = exp(sin(x));
    auto ex8 = pow(cos(x), sin(x));

    REQUIRE(series_coeff(ex1, x, 100, 98)->__eq__(*rational(-1, 98)));
    REQUIRE(series_coeff(ex2, x, 20, 12)->__eq__(*rational(-691, 935550)));
    REQUIRE(series_coeff(ex3, x, 100, 48)->__eq__(*rational(1, 48)));
    REQUIRE(series_coeff(ex4, x, 20, 9)->__eq__(*rational(1, 362880)));
    auto res1 = umap_short_basic{{0, integer(1)}, {1, integer(1)}};
    auto res2 = umap_short_basic{{1, integer(1)}};
    REQUIRE(expand_check_pairs(ex5, x, 20, res1));
    REQUIRE(expand_check_pairs(ex6, x, 20, res2));
    REQUIRE(series_coeff(ex7, x, 20, 10)->__eq__(*rational(-2951, 3628800)));
    REQUIRE(series_coeff(ex8, x, 20, 16)->__eq__(*rational(1381, 2661120)));
}

TEST_CASE("Expression series expansion: reversion ", "[Expansion of f^-1]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Integer> two = integer(2);
    RCP<const Integer> three = integer(3);
    auto ex1 = sub(x, pow(x, two));
    auto ex2 = sub(x, pow(x, three));
    auto ex3 = sin(x);
    auto ex4 = mul(x, exp(x));

    REQUIRE(invseries_coeff(ex1, x, 20, 15)->__eq__(*integer(2674440)));
    REQUIRE(invseries_coeff(ex2, x, 20, 15)->__eq__(*integer(7752)));
    REQUIRE(invseries_coeff(ex3, x, 20, 15)->__eq__(*rational(143, 10240)));
    REQUIRE(invseries_coeff(ex4, x, 20, 10)->__eq__(*rational(-156250, 567)));
}

TEST_CASE("Expression series expansion: atan, tan, asin, cot, sec, csc",
          "[Expansion of tan, atan, asin, cot, sec, csc]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Integer> one = integer(1);
    auto ex1 = atan(x);
    auto ex2 = atan(div(x, sub(one, x)));
    auto ex3 = tan(x);
    auto ex4 = tan(div(x, sub(one, x)));
    auto ex5 = asin(x);
    auto ex6 = asin(div(x, sub(one, x)));
    auto ex7 = cot(x);
    auto ex8 = cot(sin(x));
    auto res1 = umap_short_basic{{-1, integer(1)}, {1, rational(-1, 3)}};
    auto res2
        = umap_short_basic{{-1, integer(1)}, {7, rational(-1051, 1814400)}};
    auto ex9 = sec(x);
    auto ex10 = csc(x);

    auto s = MultivariateSeries::series(ex8, "x", 10);

    REQUIRE(series_coeff(ex1, x, 20, 19)->__eq__(*rational(-1, 19)));
    REQUIRE(series_coeff(ex2, x, 40, 33)->__eq__(*rational(65536, 33)));
    REQUIRE(series_coeff(ex3, x, 20, 13)->__eq__(*rational(21844, 6081075)));
    REQUIRE(series_coeff(ex4, x, 20, 12)->__eq__(*rational(1303712, 14175)));
    REQUIRE(series_coeff(ex5, x, 20, 15)->__eq__(*rational(143, 10240)));
    REQUIRE(series_coeff(ex6, x, 20, 16)->__eq__(*rational(1259743, 2048)));
    REQUIRE(expand_check_pairs(ex7, x, 5, res1));
    REQUIRE(expand_check_pairs(ex8, x, 10, res2));
    REQUIRE(series_coeff(ex9, x, 20, 8)->__eq__(*rational(277, 8064)));
    REQUIRE(series_coeff(ex10, x, 20, 7)->__eq__(*rational(127, 604800)));
}

TEST_CASE("Expression series expansion: sinh, cosh, tanh, asinh, atanh",
          "[Expansion of sinh, cosh, tanh, asinh, atanh]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Integer> one = integer(1);
    auto ex1 = sinh(x);
    auto ex2 = sinh(div(x, sub(one, x)));
    auto ex3 = cosh(x);
    auto ex4 = cosh(div(x, sub(one, x)));
    auto ex5 = tanh(x);
    auto ex6 = tanh(div(x, sub(one, x)));
    auto ex7 = atanh(x);
    auto ex8 = atanh(div(x, sub(one, x)));
    auto ex9 = asinh(x);
    auto ex10 = asinh(div(x, sub(one, x)));

    REQUIRE(series_coeff(ex1, x, 10, 9)->__eq__(*rational(1, 362880)));
    REQUIRE(series_coeff(ex2, x, 20, 10)->__eq__(*rational(325249, 40320)));
    REQUIRE(series_coeff(ex3, x, 12, 10)->__eq__(*rational(1, 3628800)));
    REQUIRE(series_coeff(ex4, x, 20, 11)->__eq__(*rational(3756889, 362880)));
    REQUIRE(series_coeff(ex5, x, 20, 13)->__eq__(*rational(21844, 6081075)));
    REQUIRE(series_coeff(ex6, x, 20, 14)->__eq__(*rational(225979, 66825)));
    REQUIRE(series_coeff(ex7, x, 100, 99)->__eq__(*rational(1, 99)));
    REQUIRE(series_coeff(ex8, x, 20, 16)->__eq__(*integer(2048)));
    REQUIRE(series_coeff(ex9, x, 20, 15)->__eq__(*rational(-143, 10240)));
    REQUIRE(series_coeff(ex10, x, 20, 16)->__eq__(*rational(-3179, 2048)));
}

TEST_CASE("Expression series expansion: lambertw ", "[Expansion of lambertw]")
{
    RCP<const Symbol> x = symbol("x");
    auto ex1 = lambertw(x);
    auto ex2 = lambertw(sin(x));

    REQUIRE(series_coeff(ex1, x, 10, 7)->__eq__(*rational(16807, 720)));
    REQUIRE(series_coeff(ex2, x, 12, 10)->__eq__(*rational(-2993294, 14175)));
}

TEST_CASE("Expression series expansion: gamma ", "[Expansion of gamma]")
{
    RCP<const Symbol> x = symbol("x");
    auto ex1 = gamma(x);

    REQUIRE(series_coeff(ex1, x, 10, -1)->__eq__(*one));
    REQUIRE(series_coeff(ex1, x, 10, 0)->__eq__(*neg(EulerGamma)));
}