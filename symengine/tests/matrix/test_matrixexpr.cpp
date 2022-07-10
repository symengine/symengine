#include "catch.hpp"
#include <symengine/matrix_expressions.h>
#include <symengine/rational.h>

using SymEngine::DomainError;
using SymEngine::eq;
using SymEngine::identity_matrix;
using SymEngine::integer;
using SymEngine::is_real;
using SymEngine::is_true;
using SymEngine::Rational;
using SymEngine::symbol;

TEST_CASE("Test IdentityMatrix", "[IdentityMatrix]")
{
    auto n1 = integer(1);
    auto n2 = integer(2);
    auto x = symbol("x");

    auto I1 = identity_matrix(n1);
    auto I2 = identity_matrix(n2);
    auto Ix = identity_matrix(x);
    REQUIRE(!eq(*I1, *I2));
    REQUIRE(eq(*I1, *I1));
    REQUIRE(I1->__hash__() != I2->__hash__());
    REQUIRE(I1->compare(*I2) == -1);
    REQUIRE(I2->compare(*I1) == 1);
    REQUIRE(I1->compare(*I1) == 0);
    REQUIRE(I1->get_args().size() == 1);
    CHECK_THROWS_AS(identity_matrix(integer(-1)), DomainError);
    auto rat1 = Rational::from_two_ints(*integer(1), *integer(2));
    CHECK_THROWS_AS(identity_matrix(rat1), DomainError);
}

TEST_CASE("Test ZeroMatrix", "[ZeroMatrix]")
{
    auto n1 = integer(1);
    auto n2 = integer(2);
    auto x = symbol("x");

    auto Z1 = zero_matrix(n1, n1);
    auto Z2 = zero_matrix(n2, n1);
    auto Zx = zero_matrix(x, x);
    REQUIRE(!eq(*Z1, *Z2));
    REQUIRE(eq(*Z1, *Z1));
    REQUIRE(Z1->__hash__() != Z2->__hash__());
    REQUIRE(Z1->compare(*Z2) == -1);
    REQUIRE(Z2->compare(*Z1) == 1);
    REQUIRE(Z1->compare(*Z1) == 0);
    REQUIRE(Z1->get_args().size() == 2);
    CHECK_THROWS_AS(zero_matrix(integer(-1), integer(1)), DomainError);
    CHECK_THROWS_AS(zero_matrix(integer(1), integer(-1)), DomainError);
    auto rat1 = Rational::from_two_ints(*integer(1), *integer(2));
    CHECK_THROWS_AS(zero_matrix(rat1, integer(1)), DomainError);
    CHECK_THROWS_AS(zero_matrix(integer(1), rat1), DomainError);
}

TEST_CASE("Test is_zero", "[is_zero]")
{
    auto n5 = integer(5);
    auto I5 = identity_matrix(n5);
    auto Z5 = zero_matrix(n5, n5);

    REQUIRE(is_false(is_zero(*I5)));
    REQUIRE(is_true(is_zero(*Z5)));
}

TEST_CASE("Test is_real", "[is_real]")
{
    auto n5 = integer(5);
    auto I5 = identity_matrix(n5);
    auto Z5 = zero_matrix(n5, n5);

    REQUIRE(is_true(is_real(*I5)));
    REQUIRE(is_true(is_real(*Z5)));
}

TEST_CASE("Test is_symmetric", "[is_symmetric]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto n2 = integer(2);
    auto n5 = integer(5);
    auto I5 = identity_matrix(n5);
    auto Z52 = zero_matrix(n5, n2);
    auto Z5 = zero_matrix(n5, n5);
    auto Zx = zero_matrix(x, x);
    auto Zxy = zero_matrix(x, y);

    REQUIRE(is_true(is_symmetric(*I5)));
    REQUIRE(is_false(is_symmetric(*Z52)));
    REQUIRE(is_true(is_symmetric(*Z5)));
    REQUIRE(is_true(is_symmetric(*Zx)));
    REQUIRE(is_indeterminate(is_symmetric(*Zxy)));
}

TEST_CASE("Test is_square", "[is_square]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto n2 = integer(2);
    auto n5 = integer(5);
    auto I5 = identity_matrix(n5);
    auto Z52 = zero_matrix(n5, n2);
    auto Z5 = zero_matrix(n5, n5);
    auto Zx = zero_matrix(x, x);
    auto Zxy = zero_matrix(x, y);

    REQUIRE(is_true(is_square(*I5)));
    REQUIRE(is_false(is_square(*Z52)));
    REQUIRE(is_true(is_square(*Z5)));
    REQUIRE(is_true(is_square(*Zx)));
    REQUIRE(is_indeterminate(is_square(*Zxy)));
}
