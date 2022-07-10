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

TEST_CASE("Test is_real", "[is_real]")
{
    auto n5 = integer(5);
    auto I5 = identity_matrix(n5);

    REQUIRE(is_true(is_real(*I5)));
}

TEST_CASE("Test is_symmetric", "[is_symmetric]")
{
    auto n5 = integer(5);
    auto I5 = identity_matrix(n5);

    REQUIRE(is_true(is_symmetric(*I5)));
}
