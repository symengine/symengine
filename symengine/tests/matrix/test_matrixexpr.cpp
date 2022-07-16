#include "catch.hpp"
#include <symengine/matrix_expressions.h>
#include <symengine/rational.h>
#include <symengine/complex.h>

using SymEngine::Complex;
using SymEngine::diagonal_matrix;
using SymEngine::DiagonalMatrix;
using SymEngine::DomainError;
using SymEngine::down_cast;
using SymEngine::eq;
using SymEngine::hadamard_product;
using SymEngine::HadamardProduct;
using SymEngine::identity_matrix;
using SymEngine::IdentityMatrix;
using SymEngine::integer;
using SymEngine::is_a;
using SymEngine::is_real;
using SymEngine::is_true;
using SymEngine::make_rcp;
using SymEngine::matrix_add;
using SymEngine::matrix_symbol;
using SymEngine::MatrixAdd;
using SymEngine::one;
using SymEngine::Rational;
using SymEngine::symbol;
using SymEngine::Trace;
using SymEngine::vec_basic;
using SymEngine::zero;
using SymEngine::ZeroMatrix;

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

    REQUIRE(I1->__str__() == "I");
    REQUIRE(!down_cast<const IdentityMatrix &>(*I1).is_canonical(integer(-1)));
    REQUIRE(!down_cast<const IdentityMatrix &>(*I1).is_canonical(rat1));
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

    REQUIRE(Z1->__str__() == "0");
    REQUIRE(!down_cast<const ZeroMatrix &>(*Z1).is_canonical(integer(2),
                                                             integer(-1)));
    REQUIRE(!down_cast<const ZeroMatrix &>(*Z1).is_canonical(integer(-1),
                                                             integer(1)));
    REQUIRE(!down_cast<const ZeroMatrix &>(*Z1).is_canonical(rat1, integer(2)));
    REQUIRE(!down_cast<const ZeroMatrix &>(*Z1).is_canonical(integer(2), rat1));
}

TEST_CASE("Test MatrixSymbol", "[MatrixSymbol]")
{
    auto n1 = integer(1);
    auto n2 = integer(2);
    auto x = symbol("x");

    auto A = matrix_symbol("A");
    auto B = matrix_symbol("B");
    REQUIRE(!eq(*A, *B));
    REQUIRE(eq(*A, *A));
    REQUIRE(A->__hash__() != B->__hash__());
    REQUIRE(A->compare(*B) == -1);
    REQUIRE(B->compare(*A) == 1);
    REQUIRE(A->compare(*A) == 0);
    REQUIRE(A->get_args().size() == 0);
}

TEST_CASE("Test DiagonalMatrix", "[DiagonalMatrix]")
{
    auto i1 = integer(1);
    auto i2 = integer(23);
    auto diag1 = diagonal_matrix({i1, i2});
    auto diag2 = diagonal_matrix({i2, i1});
    REQUIRE(!eq(*diag1, *diag2));
    REQUIRE(eq(*diag1, *diag1));
    REQUIRE(!eq(*diag1, *i1));
    REQUIRE(diag1->__hash__() != diag2->__hash__());
    REQUIRE(diag1->compare(*diag2) == -1);
    REQUIRE(diag2->compare(*diag1) == 1);
    REQUIRE(diag2->compare(*diag2) == 0);
    REQUIRE(diag1->get_args().size() == 2);
    REQUIRE(!down_cast<const DiagonalMatrix &>(*diag1).is_canonical({}));
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

TEST_CASE("Test MatrixAdd", "[MatrixAdd]")
{
    auto i1 = integer(3);
    auto i2 = integer(5);
    auto Z1 = zero_matrix(i1, i1);
    auto Z2 = zero_matrix(i2, i2);
    auto Z3 = zero_matrix(i2, i1);
    auto I1 = identity_matrix(i1);
    auto I2 = identity_matrix(i2);
    auto D1 = diagonal_matrix({integer(2), integer(23), integer(-2)});
    auto D2 = diagonal_matrix({integer(-1), integer(5), integer(0)});
    auto D3 = diagonal_matrix({integer(1), integer(28), integer(-2)});

    auto sum = matrix_add({Z1, I1});
    REQUIRE(eq(*sum, *I1));
    sum = matrix_add({Z1, I1, Z1, Z1});
    REQUIRE(eq(*sum, *I1));
    sum = matrix_add({I1, I1});
    auto vec = vec_basic({I1, I1});
    REQUIRE(eq(*sum, *make_rcp<const MatrixAdd>(vec)));
    sum = matrix_add({Z1, I1, D1, Z1});
    vec = vec_basic({I1, D1});
    REQUIRE(eq(*sum, *make_rcp<const MatrixAdd>(vec)));
    sum = matrix_add({sum, D2});
    vec = vec_basic({I1, D3});
    REQUIRE(eq(*sum, *make_rcp<const MatrixAdd>(vec)));
    REQUIRE(sum->__hash__() == make_rcp<const MatrixAdd>(vec)->__hash__());
    sum = matrix_add({Z1, D1});
    REQUIRE(eq(*sum, *D1));
    sum = matrix_add({D1, D2});
    REQUIRE(eq(*sum, *D3));
    sum = matrix_add({Z1, Z1, Z1});
    REQUIRE(eq(*sum, *Z1));
    sum = matrix_add({I1});
    REQUIRE(eq(*sum, *I1));

    CHECK_THROWS_AS(matrix_add({Z1, Z2}), DomainError);
    CHECK_THROWS_AS(matrix_add({Z2, D1}), DomainError);
    CHECK_THROWS_AS(matrix_add({D1, Z2, D1}), DomainError);
    CHECK_THROWS_AS(matrix_add({D1, I2}), DomainError);
    CHECK_THROWS_AS(matrix_add({Z2, Z3}), DomainError);
    CHECK_THROWS_AS(matrix_add({}), DomainError);
}

TEST_CASE("Test HadamardProduct", "[HadamardProduct]")
{
    auto i1 = integer(3);
    auto i2 = integer(5);
    auto Z1 = zero_matrix(i1, i1);
    auto Z2 = zero_matrix(i2, i2);
    auto Z3 = zero_matrix(i2, i1);
    auto I1 = identity_matrix(i1);
    auto I2 = identity_matrix(i2);
    auto D1 = diagonal_matrix({integer(2), integer(23), integer(-2)});
    auto D2 = diagonal_matrix({integer(-1), integer(5), integer(0)});
    auto D3 = diagonal_matrix({integer(-2), integer(115), integer(0)});

    auto prod = hadamard_product({Z1, I1});
    REQUIRE(eq(*prod, *Z1));
    prod = hadamard_product({I1, Z1, Z1});
    REQUIRE(eq(*prod, *Z1));
    prod = hadamard_product({I1, I1});
    REQUIRE(eq(*prod, *I1));
    prod = hadamard_product({I1, D1});
    auto vec = vec_basic({I1, D1});
    REQUIRE(eq(*prod, *make_rcp<const HadamardProduct>(vec)));
    REQUIRE(prod->__hash__()
            == make_rcp<const HadamardProduct>(vec)->__hash__());
    prod = hadamard_product({D1, D2});
    REQUIRE(eq(*prod, *D3));
    prod = hadamard_product({I1});
    REQUIRE(eq(*prod, *I1));

    CHECK_THROWS_AS(hadamard_product({Z1, Z2}), DomainError);
    CHECK_THROWS_AS(hadamard_product({Z2, D1}), DomainError);
    CHECK_THROWS_AS(hadamard_product({D1, Z2, D1}), DomainError);
    CHECK_THROWS_AS(hadamard_product({D1, I2}), DomainError);
    CHECK_THROWS_AS(hadamard_product({Z2, Z3}), DomainError);
    CHECK_THROWS_AS(hadamard_product({}), DomainError);
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
    auto I1 = identity_matrix(n2);
    auto A1 = matrix_add({D1, I1});

    REQUIRE(is_true(is_symmetric(*I5)));
    REQUIRE(is_false(is_symmetric(*Z52)));
    REQUIRE(is_true(is_symmetric(*Z5)));
    REQUIRE(is_true(is_symmetric(*Zx)));
    REQUIRE(is_indeterminate(is_symmetric(*Zxy)));
    REQUIRE(is_true(is_symmetric(*D1)));
    REQUIRE(is_true(is_symmetric(*A1)));
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
    auto I1 = identity_matrix(n2);
    auto A1 = matrix_add({D1, I1});

    REQUIRE(is_true(is_square(*I5)));
    REQUIRE(is_false(is_square(*Z52)));
    REQUIRE(is_true(is_square(*Z5)));
    REQUIRE(is_true(is_square(*Zx)));
    REQUIRE(is_indeterminate(is_square(*Zxy)));
    REQUIRE(is_true(is_square(*D1)));
    REQUIRE(is_true(is_square(*A1)));
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
    auto I1 = identity_matrix(n2);
    auto A1 = matrix_add({D1, I1});
    auto H1 = hadamard_product({I1, D1});
    auto S1 = matrix_symbol("S1");
    auto H2 = hadamard_product({S1, D1});

    REQUIRE(is_true(is_diagonal(*I5)));
    REQUIRE(is_false(is_diagonal(*Z52)));
    REQUIRE(is_true(is_diagonal(*Z5)));
    REQUIRE(is_true(is_diagonal(*Zx)));
    REQUIRE(is_indeterminate(is_diagonal(*Zxy)));
    REQUIRE(is_true(is_diagonal(*D1)));
    REQUIRE(is_true(is_diagonal(*A1)));
    REQUIRE(is_true(is_diagonal(*H1)));
    REQUIRE(is_true(is_diagonal(*H2)));
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
    auto I1 = identity_matrix(n2);
    auto A1 = matrix_add({D1, I1});

    REQUIRE(is_true(is_lower(*I5)));
    REQUIRE(is_false(is_lower(*Z52)));
    REQUIRE(is_true(is_lower(*Z5)));
    REQUIRE(is_true(is_lower(*Zx)));
    REQUIRE(is_indeterminate(is_lower(*Zxy)));
    REQUIRE(is_true(is_lower(*D1)));
    REQUIRE(is_true(is_lower(*A1)));
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
    auto I1 = identity_matrix(n2);
    auto A1 = matrix_add({D1, I1});

    REQUIRE(is_true(is_upper(*I5)));
    REQUIRE(is_false(is_upper(*Z52)));
    REQUIRE(is_true(is_upper(*Z5)));
    REQUIRE(is_true(is_upper(*Zx)));
    REQUIRE(is_indeterminate(is_upper(*Zxy)));
    REQUIRE(is_true(is_upper(*D1)));
    REQUIRE(is_true(is_upper(*A1)));
}

TEST_CASE("Test is_toeplitz", "[is_toeplitz]")
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
    auto D2 = diagonal_matrix({integer(23), integer(23)});
    auto D3 = diagonal_matrix({x, y, integer(23)});
    auto D4 = diagonal_matrix({x});

    REQUIRE(is_true(is_toeplitz(*I5)));
    REQUIRE(is_true(is_toeplitz(*Z52)));
    REQUIRE(is_true(is_toeplitz(*Z5)));
    REQUIRE(is_true(is_toeplitz(*Zx)));
    REQUIRE(is_true(is_toeplitz(*Zxy)));
    REQUIRE(is_false(is_toeplitz(*D1)));
    REQUIRE(is_true(is_toeplitz(*D2)));
    REQUIRE(is_indeterminate(is_toeplitz(*D3)));
    REQUIRE(is_true(is_toeplitz(*D4)));
}

TEST_CASE("Test size", "[size]")
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

    auto sz = size(*I5);
    REQUIRE(eq(*sz.first, *n5));
    REQUIRE(eq(*sz.second, *n5));
    sz = size(*Z52);
    REQUIRE(eq(*sz.first, *n5));
    REQUIRE(eq(*sz.second, *n2));
    sz = size(*Z5);
    REQUIRE(eq(*sz.first, *n5));
    REQUIRE(eq(*sz.second, *n5));
    sz = size(*Zx);
    REQUIRE(eq(*sz.first, *x));
    REQUIRE(eq(*sz.second, *x));
    sz = size(*Zxy);
    REQUIRE(eq(*sz.first, *x));
    REQUIRE(eq(*sz.second, *y));
    sz = size(*D1);
    REQUIRE(eq(*sz.first, *n2));
    REQUIRE(eq(*sz.second, *n2));
}
