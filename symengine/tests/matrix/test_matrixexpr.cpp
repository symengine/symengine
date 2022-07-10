#include "catch.hpp"
#include <symengine/matrix_expressions.h>
#include <symengine/rational.h>
#include <symengine/complex.h>

using SymEngine::Complex;
using SymEngine::diagonal_matrix;
using SymEngine::DomainError;
using SymEngine::eq;
using SymEngine::identity_matrix;
using SymEngine::integer;
using SymEngine::is_a;
using SymEngine::is_real;
using SymEngine::is_true;
using SymEngine::one;
using SymEngine::Rational;
using SymEngine::symbol;
using SymEngine::Trace;
using SymEngine::zero;

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

TEST_CASE("Test DiagonalMatrix", "[DiagonalMatrix]")
{
    auto i1 = integer(1);
    auto i2 = integer(23);
    auto diag1 = diagonal_matrix({i1, i2});
    auto diag2 = diagonal_matrix({i2, i1});
    REQUIRE(!eq(*diag1, *diag2));
    REQUIRE(eq(*diag1, *diag1));
    REQUIRE(diag1->__hash__() != diag2->__hash__());
    REQUIRE(diag1->compare(*diag2) == -1);
    REQUIRE(diag2->compare(*diag1) == 1);
    REQUIRE(diag2->compare(*diag2) == 0);
    REQUIRE(diag1->get_args().size() == 2);
}

TEST_CASE("Test Trace", "[Trace]")
{
    auto n1 = integer(1);
    auto n2 = integer(2);
    auto x = symbol("x");

    auto Z1 = zero_matrix(n1, n1);
    auto Z2 = zero_matrix(n2, n2);
    auto Z3 = zero_matrix(n2, n1);
    auto Z4 = zero_matrix(x, n2);
    auto Z5 = zero_matrix(x, n1);
    REQUIRE(eq(*trace(Z1), *zero));
    REQUIRE(eq(*trace(Z2), *zero));
    CHECK_THROWS_AS(trace(Z3), DomainError);
    auto tr1 = trace(Z4);
    auto tr2 = trace(Z5);
    REQUIRE(is_a<Trace>(*tr1));
    REQUIRE(tr1->get_args().size() == 1);

    auto I1 = identity_matrix(x);
    REQUIRE(eq(*trace(I1), *x));

    REQUIRE(tr1->compare(*tr1) == 0);
    REQUIRE(eq(*tr1, *tr1));
    REQUIRE(!eq(*tr1, *tr2));
    REQUIRE(tr1->__hash__() != tr2->__hash__());
    REQUIRE(tr1->compare(*tr2) == 1);
    REQUIRE(tr2->compare(*tr1) == -1);

    auto D1 = diagonal_matrix({integer(2), integer(23)});
    REQUIRE(eq(*trace(D1), *integer(25)));
}

TEST_CASE("Test is_zero", "[is_zero]")
{
    auto n5 = integer(5);
    auto I5 = identity_matrix(n5);
    auto Z5 = zero_matrix(n5, n5);
    auto D1 = diagonal_matrix({integer(0), integer(23)});
    auto D2 = diagonal_matrix({integer(0), integer(0)});
    auto D3 = diagonal_matrix({integer(0), integer(0), symbol("x")});

    REQUIRE(is_false(is_zero(*I5)));
    REQUIRE(is_true(is_zero(*Z5)));
    REQUIRE(is_false(is_zero(*D1)));
    REQUIRE(is_true(is_zero(*D2)));
    REQUIRE(is_indeterminate(is_zero(*D3)));
}

TEST_CASE("Test is_real", "[is_real]")
{
    auto c1 = Complex::from_two_nums(*one, *one);
    auto n5 = integer(5);
    auto I5 = identity_matrix(n5);
    auto Z5 = zero_matrix(n5, n5);
    auto D1 = diagonal_matrix({integer(0), integer(0), symbol("x")});
    auto D2 = diagonal_matrix({integer(23), integer(0)});
    auto D3 = diagonal_matrix({integer(23), c1, integer(0)});

    REQUIRE(is_true(is_real(*I5)));
    REQUIRE(is_true(is_real(*Z5)));
    REQUIRE(is_indeterminate(is_real(*D1)));
    REQUIRE(is_true(is_real(*D2)));
    REQUIRE(is_false(is_real(*D3)));
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
    auto D1 = diagonal_matrix({integer(0), integer(23)});

    REQUIRE(is_true(is_symmetric(*I5)));
    REQUIRE(is_false(is_symmetric(*Z52)));
    REQUIRE(is_true(is_symmetric(*Z5)));
    REQUIRE(is_true(is_symmetric(*Zx)));
    REQUIRE(is_indeterminate(is_symmetric(*Zxy)));
    REQUIRE(is_true(is_symmetric(*D1)));
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
    auto D1 = diagonal_matrix({integer(0), integer(23)});

    REQUIRE(is_true(is_square(*I5)));
    REQUIRE(is_false(is_square(*Z52)));
    REQUIRE(is_true(is_square(*Z5)));
    REQUIRE(is_true(is_square(*Zx)));
    REQUIRE(is_indeterminate(is_square(*Zxy)));
    REQUIRE(is_true(is_square(*D1)));
}

TEST_CASE("Test is_diagonal", "[is_diagonal]")
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
    auto D1 = diagonal_matrix({integer(0), integer(23)});

    REQUIRE(is_true(is_diagonal(*I5)));
    REQUIRE(is_false(is_diagonal(*Z52)));
    REQUIRE(is_true(is_diagonal(*Z5)));
    REQUIRE(is_true(is_diagonal(*Zx)));
    REQUIRE(is_indeterminate(is_diagonal(*Zxy)));
    REQUIRE(is_true(is_diagonal(*D1)));
}

TEST_CASE("Test is_lower", "[is_lower]")
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
    auto D1 = diagonal_matrix({integer(0), integer(23)});

    REQUIRE(is_true(is_lower(*I5)));
    REQUIRE(is_false(is_lower(*Z52)));
    REQUIRE(is_true(is_lower(*Z5)));
    REQUIRE(is_true(is_lower(*Zx)));
    REQUIRE(is_indeterminate(is_lower(*Zxy)));
    REQUIRE(is_true(is_lower(*D1)));
}

TEST_CASE("Test is_upper", "[is_upper]")
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
    auto D1 = diagonal_matrix({integer(0), integer(23)});

    REQUIRE(is_true(is_upper(*I5)));
    REQUIRE(is_false(is_upper(*Z52)));
    REQUIRE(is_true(is_upper(*Z5)));
    REQUIRE(is_true(is_upper(*Zx)));
    REQUIRE(is_indeterminate(is_upper(*Zxy)));
    REQUIRE(is_true(is_upper(*D1)));
}
