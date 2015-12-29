#include <symengine/symengine_config.h>

#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/functions.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/pow.h>

using SymEngine::Basic;
using SymEngine::Integer;
using SymEngine::Symbol;
using SymEngine::Add;
using SymEngine::sin;
using SymEngine::cos;

namespace SymEngine {

#ifdef HAVE_SYMENGINE_PIRANHA
static bool eq(const umap_short_basic& v1, const umap_short_basic& v2)
{
    for (auto it : v2) {
        //std::cerr << it.first << ", " << *(it.second) << "::" << *(v1.at(it.first)) << std::endl;
        if (not it.second->__eq__(*(v1.at(it.first))))
            return false;
        }
    return true;
}

TEST_CASE("Expression series expansion initialization", "[Expansion init]")
{
    RCP<const Symbol> x = symbol("x"), y = symbol("y");
    auto z = sin(add(integer(1), cos(x)));

    REQUIRE(series(z, y, 20)[0]->__eq__(*z));
    REQUIRE(series(z, x, 0)[0]->__eq__(*integer(0)));
}

TEST_CASE("Expression series expansion: Add ", "[Expansion of Add]")
{
    RCP<const Symbol> x = symbol("x"), y = symbol("y");
    auto z = add(integer(1), x);
    z = sub(z, pow(x, integer(2)));
    z = add(z, pow(x, integer(4)));

    auto vb = umap_short_basic{{0, integer(1)}, {1, integer(1)}, {2, integer(-1)}, {4, integer(1)}};
    REQUIRE(eq(series(z, x, 5), vb));
    auto vb1 = umap_short_basic{{0, integer(1)}, {1, integer(1)}, {2, integer(-1)}};
    REQUIRE(eq(series(z, x, 3), vb1));
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

    REQUIRE(series(z1, x, 10)[9]->__eq__(*rational(1, 362880)));
    auto res = umap_short_basic{{0, integer(1)}, {2, rational(-1, 2)}};
    REQUIRE(eq(series(z2, x, 3), res));
    REQUIRE(series(z3, x, 9)[8]->__eq__(*rational(1, 40320)));
    REQUIRE(series(z4, x, 12)[11]->__eq__(*rational(-4, 155925)));
    REQUIRE(series(z5, x, 30)[27]->__eq__(*rational(-1300075, 8388608)));
    REQUIRE(series(z6, x, 15)[11]->__eq__(*rational(-125929, 362880)));
}

TEST_CASE("Expression series expansion: division, inversion ", "[Expansion of 1/ex]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Integer> one = integer(1);
    RCP<const Integer> two = integer(2);
    RCP<const Integer> three = integer(3);
    auto ex1 = div(one, sub(one, x));                   // 1/(1-x)
    auto ex2 = div(x, sub(sub(one, x), pow(x, two)));   // x/(1-x-x^2)
    auto ex3 = div(pow(x, three), sub(one, mul(pow(x, two), two))); // x^3/(1-2x^2)
    auto ex4 = div(one, sub(one, sin(x)));              // 1/(1-sin(x))
    auto ex5 = div(one, x);
    auto ex6 = div(one, mul(x, sub(one, x)));
    auto res1 = umap_short_basic{{-1, integer(1)}};
    auto res2 = umap_short_basic{{-1, integer(1)}, {0, integer(1)}};

    REQUIRE(series(ex1, x, 100)[99]->__eq__(*integer(1)));
    REQUIRE(series(ex2, x, 100)[35]->__eq__(*integer(9227465)));
    REQUIRE(series(ex3, x, 100)[49]->__eq__(*integer(8388608)));
    REQUIRE(series(ex4, x, 20)[10]->__eq__(*rational(1382, 14175)));
    REQUIRE(eq(series(ex5, x, 8), res1));
    REQUIRE(eq(series(ex6, x, 8), res2));
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

    REQUIRE(series(ex1, x, 8)[6]->__eq__(*rational(-21, 2097152)));
    REQUIRE(series(ex2, x, 12)[10]->__eq__(*rational(1621477, 4782969)));
    REQUIRE(series(ex3, x, 12)[10]->__eq__(*rational(-2431, 262144)));
    REQUIRE(series(ex4, x, 100)[8]->__eq__(*rational(-559, 645120)));
    REQUIRE(series(ex5, x, 20)[10]->__eq__(*rational(701, 127575)));
    REQUIRE(series(ex6, x, 10)[8]->__eq__(*rational(-559, 645120)));
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

    REQUIRE(series(ex1, x, 100)[98]->__eq__(*rational(-1, 98)));
    REQUIRE(series(ex2, x, 20)[12]->__eq__(*rational(-691, 935550)));
    REQUIRE(series(ex3, x, 100)[48]->__eq__(*rational(1, 48)));
    REQUIRE(series(ex4, x, 20)[9]->__eq__(*rational(1, 362880)));
    auto res1 = umap_short_basic{{0, integer(1)}, {1, integer(1)}};
    auto res2 = umap_short_basic{{1, integer(1)}};
    REQUIRE(eq(series(ex5, x, 20), res1));
    REQUIRE(eq(series(ex6, x, 20), res2));
    REQUIRE(series(ex7, x, 20)[10]->__eq__(*rational(-2951, 3628800)));
    REQUIRE(series(ex8, x, 20)[16]->__eq__(*rational(1381, 2661120)));
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

    REQUIRE(series_invfunc(ex1, x, 20)[15]->__eq__(*integer(2674440)));
    REQUIRE(series_invfunc(ex2, x, 20)[15]->__eq__(*integer(7752)));
    REQUIRE(series_invfunc(ex3, x, 20)[15]->__eq__(*rational(143, 10240)));
    REQUIRE(series_invfunc(ex4, x, 20)[10]->__eq__(*rational(-156250, 567)));
}

TEST_CASE("Expression series expansion: atan, tan, asin, cot, sec, csc", "[Expansion of tan, atan, asin, cot, sec, csc]")
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
    auto res2 = umap_short_basic{{-1, integer(1)}, {7, rational(-1051, 1814400)}};
    auto ex9 = sec(x);
    auto ex10 = csc(x);
    
    REQUIRE(series(ex1, x, 20)[19]->__eq__(*rational(-1, 19)));
    REQUIRE(series(ex2, x, 40)[33]->__eq__(*rational(65536, 33)));
    REQUIRE(series(ex3, x, 20)[13]->__eq__(*rational(21844, 6081075)));
    REQUIRE(series(ex4, x, 20)[12]->__eq__(*rational(1303712, 14175)));
    REQUIRE(series(ex5, x, 20)[15]->__eq__(*rational(143, 10240)));
    REQUIRE(series(ex6, x, 20)[16]->__eq__(*rational(1259743, 2048)));
    REQUIRE(eq(series(ex7, x, 5), res1));
    REQUIRE(eq(series(ex8, x, 10), res2));
    REQUIRE(series(ex9, x, 20)[8]->__eq__(*rational(277, 8064)));
    REQUIRE(series(ex10, x, 20)[7]->__eq__(*rational(127, 604800)));
}

TEST_CASE("Expression series expansion: sinh, cosh, tanh, asinh, atanh", "[Expansion of sinh, cosh, tanh, asinh, atanh]")
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

    REQUIRE(series(ex1, x, 10)[9]->__eq__(*rational(1, 362880)));
    REQUIRE(series(ex2, x, 20)[10]->__eq__(*rational(325249, 40320)));
    REQUIRE(series(ex3, x, 12)[10]->__eq__(*rational(1, 3628800)));
    REQUIRE(series(ex4, x, 20)[11]->__eq__(*rational(3756889, 362880)));
    REQUIRE(series(ex5, x, 20)[13]->__eq__(*rational(21844, 6081075)));
    REQUIRE(series(ex6, x, 20)[14]->__eq__(*rational(225979, 66825)));
    REQUIRE(series(ex7, x, 100)[99]->__eq__(*rational(1, 99)));
    REQUIRE(series(ex8, x, 20)[16]->__eq__(*integer(2048)));
    REQUIRE(series(ex9, x, 20)[15]->__eq__(*rational(-143, 10240)));
    REQUIRE(series(ex10, x, 20)[16]->__eq__(*rational(-3179, 2048)));
}

TEST_CASE("Expression series expansion: lambertw ", "[Expansion of lambertw]")
{
    RCP<const Symbol> x = symbol("x");
    auto ex1 = lambertw(x);
    auto ex2 = lambertw(sin(x));

    REQUIRE(series(ex1, x, 10)[7]->__eq__(*rational(16807, 720)));
    REQUIRE(series(ex2, x, 12)[10]->__eq__(*rational(-2993294, 14175)));
}

#else
TEST_CASE("Check error when expansion called without Piranha ", "[Expansion without Piranha]")
{
    RCP<const Symbol> x = symbol("x");
    auto ex1 = lambertw(x);
    REQUIRE_THROWS_AS(series(ex1, x, 10), std::runtime_error);
}
#endif


} // SymEngine

