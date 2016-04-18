#include "catch.hpp"
#include <chrono>

#include <symengine/matrix.h>
#include <symengine/integer.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>

using SymEngine::print_stack_on_segfault;
using SymEngine::RCP;
using SymEngine::integer;
using SymEngine::DenseMatrix;
using SymEngine::Basic;
using SymEngine::symbol;
using SymEngine::Symbol;
using SymEngine::is_a;
using SymEngine::Add;
using SymEngine::minus_one;
using SymEngine::CSRMatrix;
using SymEngine::add;
using SymEngine::sub;
using SymEngine::eye;
using SymEngine::diag;
using SymEngine::vec_basic;
using SymEngine::function_symbol;

TEST_CASE("test_get_set(): matrices", "[matrices]")
{
    // Test for DenseMatirx
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(0), integer(-1), integer(-2)});

    REQUIRE(eq(*A.get(0, 0), *integer(1)));
    REQUIRE(eq(*A.get(1, 1), *integer(-2)));

    A.set(1, 0, integer(0));
    REQUIRE(A == DenseMatrix(
                     2, 2, {integer(1), integer(0), integer(0), integer(-2)}));
    REQUIRE(A != DenseMatrix(2, 1, {integer(1), integer(-2)}));

    A.set(0, 1, integer(-2));
    REQUIRE(A == DenseMatrix(
                     2, 2, {integer(1), integer(-2), integer(0), integer(-2)}));

    DenseMatrix C(A);
    C.set(0, 0, integer(0));
    REQUIRE(A == DenseMatrix(
                     2, 2, {integer(1), integer(-2), integer(0), integer(-2)}));
    REQUIRE(C == DenseMatrix(
                     2, 2, {integer(0), integer(-2), integer(0), integer(-2)}));

    // Test for CSRMatrix
    CSRMatrix B = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(1), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});

    REQUIRE(eq(*B.get(0, 0), *integer(1)));
    REQUIRE(eq(*B.get(1, 2), *integer(3)));
    REQUIRE(eq(*B.get(2, 1), *integer(5)));

    B.set(2, 1, integer(6));
    REQUIRE(B == CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                           {integer(1), integer(2), integer(3), integer(4),
                            integer(6), integer(6)}));

    B.set(0, 1, integer(1));
    REQUIRE(B == CSRMatrix(3, 3, {0, 3, 4, 7}, {0, 1, 2, 2, 0, 1, 2},
                           {integer(1), integer(1), integer(2), integer(3),
                            integer(4), integer(6), integer(6)}));

    B.set(1, 2, integer(7));
    REQUIRE(B == CSRMatrix(3, 3, {0, 3, 4, 7}, {0, 1, 2, 2, 0, 1, 2},
                           {integer(1), integer(1), integer(2), integer(7),
                            integer(4), integer(6), integer(6)}));

    B = CSRMatrix(3, 3, {0, 1, 2, 5}, {0, 2, 0, 1, 2},
                  {integer(1), integer(3), integer(4), integer(5), integer(6)});

    B.set(0, 2, integer(2));
    REQUIRE(B == CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                           {integer(1), integer(2), integer(3), integer(4),
                            integer(5), integer(6)}));

    B = CSRMatrix(3, 3); // 3x3 Zero matrix

    REQUIRE(eq(*B.get(0, 0), *integer(0)));
    REQUIRE(eq(*B.get(1, 2), *integer(0)));
    REQUIRE(eq(*B.get(2, 2), *integer(0)));

    B.set(0, 0, integer(1));
    B.set(0, 2, integer(2));
    B.set(1, 2, integer(3));
    B.set(2, 0, integer(4));
    B.set(2, 1, integer(5));
    B.set(2, 2, integer(6));

    REQUIRE(B == CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                           {integer(1), integer(2), integer(3), integer(4),
                            integer(5), integer(6)}));

    B.set(0, 0, integer(0));
    REQUIRE(B == CSRMatrix(3, 3, {0, 1, 2, 5}, {2, 2, 0, 1, 2},
                           {integer(2), integer(3), integer(4), integer(5),
                            integer(6)}));

    B.set(2, 1, integer(0));
    REQUIRE(B == CSRMatrix(3, 3, {0, 1, 2, 4}, {2, 2, 0, 2},
                           {integer(2), integer(3), integer(4), integer(6)}));
}

TEST_CASE("test_dense_dense_addition(): matrices", "[matrices]")
{
    DenseMatrix C = DenseMatrix(2, 2);
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    add_dense_dense(A, B, C);

    REQUIRE(C == DenseMatrix(2, 2,
                             {integer(2), integer(4), integer(6), integer(8)}));

    A = DenseMatrix(2, 2, {integer(1), integer(-1), integer(0), integer(2)});
    B = DenseMatrix(2, 2, {integer(1), integer(2), integer(-3), integer(0)});
    add_dense_dense(A, B, C);

    REQUIRE(C == DenseMatrix(
                     2, 2, {integer(2), integer(1), integer(-3), integer(2)}));

    A = DenseMatrix(2, 2, {integer(1), symbol("a"), integer(0), symbol("b")});
    B = DenseMatrix(2, 2, {symbol("c"), integer(2), integer(-3), integer(0)});
    add_dense_dense(A, B, C);

    REQUIRE(C == DenseMatrix(2, 2, {add(integer(1), symbol("c")),
                                    add(symbol("a"), integer(2)), integer(-3),
                                    symbol("b")}));

    C = DenseMatrix(1, 4);
    A = DenseMatrix(1, 4, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    B = DenseMatrix(1, 4, {integer(3), integer(2), integer(-3), integer(0)});
    add_dense_dense(A, B, C);

    REQUIRE(C
            == DenseMatrix(1, 4, {add(integer(3), symbol("a")),
                                  add(symbol("b"), integer(2)),
                                  add(symbol("c"), integer(-3)), symbol("d")}));

    C = DenseMatrix(2, 3);
    A = DenseMatrix(2, 3, {integer(7), integer(4), integer(-3), integer(-5),
                           symbol("a"), symbol("b")});
    B = DenseMatrix(2, 3, {integer(10), integer(13), integer(5), integer(-7),
                           symbol("c"), symbol("d")});
    add_dense_dense(A, B, C);

    REQUIRE(C == DenseMatrix(2, 3, {integer(17), integer(17), integer(2),
                                    integer(-12), add(symbol("a"), symbol("c")),
                                    add(symbol("b"), symbol("d"))}));
}

TEST_CASE("test_add_dense_scalar(): matrices", "[matrices]")
{
    // More tests should be added
    RCP<const Basic> k = integer(2);

    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    add_dense_scalar(A, k, B);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(3), integer(4), integer(5), integer(6)}));
}

TEST_CASE("test_dense_dense_multiplication(): matrices", "[matrices]")
{
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    DenseMatrix B
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix C = DenseMatrix(2, 2);
    mul_dense_dense(A, B, C);

    REQUIRE(C == DenseMatrix(2, 2,
                             {integer(1), integer(2), integer(3), integer(4)}));

    A = DenseMatrix(1, 4, {integer(1), integer(3), integer(7), integer(-5)});
    B = DenseMatrix(4, 1, {integer(1), integer(2), integer(3), integer(4)});
    C = DenseMatrix(1, 1);
    mul_dense_dense(A, B, C);

    REQUIRE(C == DenseMatrix(1, 1, {integer(8)}));

    A = DenseMatrix(4, 1, {integer(10), integer(-3), integer(7), integer(-5)});
    B = DenseMatrix(1, 3, {integer(11), integer(20), integer(12)});
    C = DenseMatrix(4, 3);
    mul_dense_dense(A, B, C);

    REQUIRE(C
            == DenseMatrix(4, 3, {integer(110), integer(200), integer(120),
                                  integer(-33), integer(-60), integer(-36),
                                  integer(77), integer(140), integer(84),
                                  integer(-55), integer(-100), integer(-60)}));

    A = DenseMatrix(3, 3, {symbol("a"), symbol("b"), symbol("c"), symbol("p"),
                           symbol("q"), symbol("r"), symbol("u"), symbol("v"),
                           symbol("w")});
    B = DenseMatrix(3, 1, {symbol("x"), symbol("y"), symbol("z")});
    C = DenseMatrix(3, 1);
    mul_dense_dense(A, B, C);

    REQUIRE(C == DenseMatrix(3, 1, {add(add(mul(symbol("a"), symbol("x")),
                                            mul(symbol("b"), symbol("y"))),
                                        mul(symbol("c"), symbol("z"))),
                                    add(add(mul(symbol("p"), symbol("x")),
                                            mul(symbol("q"), symbol("y"))),
                                        mul(symbol("r"), symbol("z"))),
                                    add(add(mul(symbol("u"), symbol("x")),
                                            mul(symbol("v"), symbol("y"))),
                                        mul(symbol("w"), symbol("z")))}));
}

TEST_CASE("test_mul_dense_scalar(): matrices", "[matrices]")
{
    // More tests should be added
    RCP<const Basic> k = integer(2);

    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    mul_dense_scalar(A, k, B);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(2), integer(4), integer(6), integer(8)}));
}

TEST_CASE("test_transpose_dense(): matrices", "[matrices]")
{
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    transpose_dense(A, B);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(1), integer(3), integer(2), integer(4)}));

    A = DenseMatrix(3, 3, {symbol("a"), symbol("b"), symbol("c"), symbol("p"),
                           symbol("q"), symbol("r"), symbol("u"), symbol("v"),
                           symbol("w")});
    B = DenseMatrix(3, 3);
    transpose_dense(A, B);

    REQUIRE(B == DenseMatrix(3, 3, {symbol("a"), symbol("p"), symbol("u"),
                                    symbol("b"), symbol("q"), symbol("v"),
                                    symbol("c"), symbol("r"), symbol("w")}));

    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    A = DenseMatrix(1, 3, {x, y, z});
    B = DenseMatrix(3, 1);
    transpose_dense(A, B);

    REQUIRE(B == DenseMatrix(3, 1, {x, y, z}));
}

TEST_CASE("test_submatrix_dense(): matrices", "[matrices]")
{
    DenseMatrix A = DenseMatrix(3, 3, {symbol("a"), symbol("b"), symbol("c"),
                                       symbol("p"), symbol("q"), symbol("r"),
                                       symbol("u"), symbol("v"), symbol("w")});
    DenseMatrix B = DenseMatrix(3, 2);
    submatrix_dense(A, B, 0, 1, 2, 2);

    REQUIRE(B == DenseMatrix(3, 2, {symbol("b"), symbol("c"), symbol("q"),
                                    symbol("r"), symbol("v"), symbol("w")}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(5), integer(6), integer(7), integer(8),
                           integer(9), integer(10), integer(11), integer(12),
                           integer(13), integer(14), integer(15), integer(16)});
    B = DenseMatrix(3, 3);
    submatrix_dense(A, B, 1, 1, 3, 3);

    REQUIRE(B == DenseMatrix(3, 3, {integer(6), integer(7), integer(8),
                                    integer(10), integer(11), integer(12),
                                    integer(14), integer(15), integer(16)}));
}

TEST_CASE("test_pivoted_gaussian_elimination(): matrices", "[matrices]")
{
    auto pivotlist = std::vector<unsigned>(2);
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    pivoted_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(
                     2, 2, {integer(1), integer(2), integer(0), integer(-2)}));

    A = DenseMatrix(2, 2, {integer(2), integer(3), integer(3), integer(4)});
    B = DenseMatrix(2, 2);
    pivoted_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(2, 2, {integer(1), div(integer(3), integer(2)),
                                    integer(0), div(minus_one, integer(2))}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    B = DenseMatrix(2, 2);
    pivoted_gaussian_elimination(A, B, pivotlist);

    REQUIRE(
        B == DenseMatrix(
                 2, 2, {integer(1), div(symbol("b"), symbol("a")), integer(0),
                        sub(symbol("d"),
                            mul(symbol("c"), div(symbol("b"), symbol("a"))))}));

    pivotlist = std::vector<unsigned>(3);
    A = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(2), integer(2),
                     integer(2), integer(3), integer(4), integer(3)});
    B = DenseMatrix(3, 3);
    pivoted_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1),
                                    integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(2), integer(2),
                     integer(5), integer(4), integer(6), integer(8)});
    B = DenseMatrix(3, 3);
    pivoted_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1),
                                    integer(0), integer(1), integer(2),
                                    integer(0), integer(0), integer(3)}));
}

TEST_CASE("test_fraction_free_gaussian_elimination(): matrices", "[matrices]")
{
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    fraction_free_gaussian_elimination(A, B);

    REQUIRE(B == DenseMatrix(
                     2, 2, {integer(1), integer(2), integer(0), integer(-2)}));

    A = DenseMatrix(2, 2, {integer(1), integer(2), integer(2), integer(4)});
    fraction_free_gaussian_elimination(A, B);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(1), integer(2), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});
    fraction_free_gaussian_elimination(A, B);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(1), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    fraction_free_gaussian_elimination(A, B);

    REQUIRE(B == DenseMatrix(2, 2, {symbol("a"), symbol("b"), integer(0),
                                    sub(mul(symbol("a"), symbol("d")),
                                        mul(symbol("b"), symbol("c")))}));

    // Test case taken from :
    // Fraction-Free Algorithms for Linear and Polynomial Equations, George C
    // Nakos,
    // Peter R Turner et. al.
    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(2), integer(2), integer(3), integer(4),
                           integer(3), integer(3), integer(3), integer(4),
                           integer(9), integer(8), integer(7), integer(6)});
    B = DenseMatrix(4, 4);
    fraction_free_gaussian_elimination(A, B);

    REQUIRE(B == DenseMatrix(
                     4, 4, {integer(1), integer(2), integer(3), integer(4),
                            integer(0), integer(-2), integer(-3), integer(-4),
                            integer(0), integer(0), integer(3), integer(4),
                            integer(0), integer(0), integer(0), integer(-10)}));

    // Test case taken from :
    // A SIMPLIFIED FRACTION-FREE INTEGER GAUSS ELIMINATION ALGORITHM
    // Peter R. Turner
    A = DenseMatrix(4, 4, {integer(8), integer(7), integer(4), integer(1),
                           integer(4), integer(6), integer(7), integer(3),
                           integer(6), integer(3), integer(4), integer(6),
                           integer(4), integer(5), integer(8), integer(2)});
    B = DenseMatrix(4, 4);
    fraction_free_gaussian_elimination(A, B);

    REQUIRE(
        B == DenseMatrix(4, 4,
                         {integer(8), integer(7), integer(4), integer(1),
                          integer(0), integer(20), integer(40), integer(20),
                          integer(0), integer(0), integer(110), integer(150),
                          integer(0), integer(0), integer(0), integer(-450)}));

    // Below two test cases are taken from:
    // http://www.mathworks.in/help/symbolic/mupad_ref/linalg-gausselim.html
    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(-1), integer(1),
                           integer(0), integer(1), integer(2), integer(-1),
                           integer(4)});
    B = DenseMatrix(3, 3);
    fraction_free_gaussian_elimination(A, B);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(2), integer(-1),
                                    integer(0), integer(-2), integer(2),
                                    integer(0), integer(0), integer(-2)}));

    A = DenseMatrix(3, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(-1), integer(0), integer(1), integer(0),
                           integer(3), integer(5), integer(6), integer(9)});
    B = DenseMatrix(3, 4);
    fraction_free_gaussian_elimination(A, B);

    REQUIRE(B == DenseMatrix(3, 4, {integer(1), integer(2), integer(3),
                                    integer(4), integer(0), integer(2),
                                    integer(4), integer(4), integer(0),
                                    integer(0), integer(-2), integer(-2)}));
}

TEST_CASE("test_pivoted_fraction_free_gaussian_elimination(): matrices",
          "[matrices]")
{
    auto pivotlist = std::vector<unsigned>(2);
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(
                     2, 2, {integer(1), integer(2), integer(0), integer(-2)}));

    pivotlist = std::vector<unsigned>(4);
    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(2), integer(2), integer(3), integer(4),
                           integer(3), integer(3), integer(3), integer(4),
                           integer(9), integer(8), integer(7), integer(6)});
    B = DenseMatrix(4, 4);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(
                     4, 4, {integer(1), integer(2), integer(3), integer(4),
                            integer(0), integer(-2), integer(-3), integer(-4),
                            integer(0), integer(0), integer(3), integer(4),
                            integer(0), integer(0), integer(0), integer(-10)}));

    pivotlist = std::vector<unsigned>(3);
    A = DenseMatrix(3, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(-1), integer(0), integer(1), integer(0),
                           integer(3), integer(5), integer(6), integer(9)});
    B = DenseMatrix(3, 4);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 4, {integer(1), integer(2), integer(3),
                                    integer(4), integer(0), integer(2),
                                    integer(4), integer(4), integer(0),
                                    integer(0), integer(-2), integer(-2)}));

    A = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(2), integer(2),
                     integer(2), integer(3), integer(3), integer(3)});
    B = DenseMatrix(3, 3);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1),
                                    integer(0), integer(0), integer(0),
                                    integer(0), integer(0), integer(0)}));

    // These tests won't work with fraction_free_gaussian_elimination
    A = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(2), integer(2),
                     integer(2), integer(3), integer(4), integer(3)});
    B = DenseMatrix(3, 3);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1),
                                    integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(2), integer(2),
                     integer(5), integer(4), integer(6), integer(8)});
    B = DenseMatrix(3, 3);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1),
                                    integer(0), integer(2), integer(4),
                                    integer(0), integer(0), integer(6)}));
}

TEST_CASE("test_pivoted_gauss_jordan_elimination(): matrices", "[matrices]")
{
    // These test cases are verified with SymPy
    auto pivotlist = std::vector<unsigned>(2);
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(1), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(2, 2, {integer(1), integer(2), integer(2), integer(4)});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(1), integer(2), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(1), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(0), integer(0), integer(0), integer(0)});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(0), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(1), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(2, 2, {symbol("a"), integer(0), symbol("c"), integer(0)});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(2, 2,
                             {integer(1), integer(0), integer(0), integer(0)}));

    pivotlist = std::vector<unsigned>(3);
    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(-1),
                           integer(7), integer(6), integer(4), integer(5),
                           integer(2)});
    B = DenseMatrix(3, 3);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(1)}));

    A = DenseMatrix(3, 2, {integer(-9), integer(4), integer(3), integer(-1),
                           integer(7), integer(6)});
    B = DenseMatrix(3, 2);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 2, {integer(1), integer(0), integer(0),
                                    integer(1), integer(0), integer(0)}));

    // These tests won't work with gauss_jordan_elimination
    A = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(2), integer(2),
                     integer(2), integer(3), integer(4), integer(3)});
    B = DenseMatrix(3, 3);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(1),
                                    integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(2), integer(2),
                     integer(5), integer(4), integer(6), integer(8)});
    B = DenseMatrix(3, 3);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(1)}));
}

TEST_CASE("test_fraction_free_gauss_jordan_elimination(): matrices",
          "[matrices]")
{
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    fraction_free_gauss_jordan_elimination(A, B);

    REQUIRE(B == DenseMatrix(
                     2, 2, {integer(-2), integer(0), integer(0), integer(-2)}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(2), integer(2), integer(3), integer(4),
                           integer(3), integer(3), integer(3), integer(4),
                           integer(9), integer(8), integer(7), integer(6)});
    B = DenseMatrix(4, 4);
    fraction_free_gauss_jordan_elimination(A, B);

    REQUIRE(B == DenseMatrix(
                     4, 4, {integer(-10), integer(0), integer(0), integer(0),
                            integer(0), integer(-10), integer(0), integer(0),
                            integer(0), integer(0), integer(-10), integer(0),
                            integer(0), integer(0), integer(0), integer(-10)}));

    A = DenseMatrix(4, 4, {integer(1), integer(7), integer(5), integer(4),
                           integer(7), integer(2), integer(2), integer(4),
                           integer(3), integer(6), integer(3), integer(4),
                           integer(9), integer(5), integer(7), integer(5)});
    fraction_free_gauss_jordan_elimination(A, B);

    REQUIRE(
        B == DenseMatrix(4, 4,
                         {integer(-139), integer(0), integer(0), integer(0),
                          integer(0), integer(-139), integer(0), integer(0),
                          integer(0), integer(0), integer(-139), integer(0),
                          integer(0), integer(0), integer(0), integer(-139)}));
}

TEST_CASE("test_pivoted_fraction_free_gauss_jordan_elimination(): matrices",
          "[matrices]")
{
    // These tests won't work with fraction_free_gauss_jordan_elimination
    auto pivotlist = std::vector<unsigned>(3);
    DenseMatrix A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1),
                                       integer(2), integer(2), integer(2),
                                       integer(3), integer(4), integer(3)});
    DenseMatrix B = DenseMatrix(3, 3);
    pivoted_fraction_free_gauss_jordan_elimination(A, B, pivotlist);

    REQUIRE(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(1),
                                    integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(2), integer(2),
                     integer(5), integer(4), integer(6), integer(8)});
    B = DenseMatrix(3, 3);
    pivoted_fraction_free_gauss_jordan_elimination(A, B, pivotlist);

    // Here the diagonal entry is 6 although the det(A) = -6, this because we
    // have interchanged two rows
    REQUIRE(B == DenseMatrix(3, 3, {integer(6), integer(0), integer(0),
                                    integer(0), integer(6), integer(0),
                                    integer(0), integer(0), integer(6)}));
}

TEST_CASE("test_fraction_free_gaussian_elimination_solve(): matrices",
          "[matrices]")
{
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(1), integer(1), integer(-1)});
    DenseMatrix b = DenseMatrix(2, 1, {integer(5), integer(3)});
    DenseMatrix x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    REQUIRE(x == DenseMatrix(2, 1, {integer(4), integer(1)}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(2), integer(2), integer(3), integer(4),
                           integer(3), integer(3), integer(3), integer(4),
                           integer(9), integer(8), integer(7), integer(6)});
    b = DenseMatrix(4, 1, {integer(10), integer(11), integer(13), integer(30)});
    x = DenseMatrix(4, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    REQUIRE(x == DenseMatrix(4, 1,
                             {integer(1), integer(1), integer(1), integer(1)}));

    A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    b = DenseMatrix(2, 1, {integer(1), integer(1)});
    x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    REQUIRE(x == DenseMatrix(2, 1, {integer(1), integer(1)}));

    A = DenseMatrix(2, 2, {integer(5), integer(-4), integer(8), integer(1)});
    b = DenseMatrix(2, 1, {integer(7), integer(26)});
    x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    REQUIRE(x == DenseMatrix(2, 1, {integer(3), integer(2)}));

    // below two sets produce the correct matrix but the results are not
    // simplified. See: https://github.com/sympy/symengine/issues/183
    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("b"), symbol("a")});
    b = DenseMatrix(
        2, 1, {add(pow(symbol("a"), integer(2)), pow(symbol("b"), integer(2))),
               mul(integer(2), mul(symbol("a"), symbol("b")))});
    x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    // REQUIRE(x == DenseMatrix(2, 1, {symbol("a"), symbol("b")}));

    A = DenseMatrix(2, 2, {integer(1), integer(1), integer(1), integer(-1)});
    b = DenseMatrix(
        2, 1, {add(symbol("a"), symbol("b")), sub(symbol("a"), symbol("b"))});
    x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    // REQUIRE(x == DenseMatrix(2, 1, {symbol("a"), symbol("b")}));

    // Solve two systems at once, Ax = transpose([7, 3]) and Ax = transpose([4,
    // 6])
    A = DenseMatrix(2, 2, {integer(1), integer(1), integer(1), integer(-1)});
    b = DenseMatrix(2, 2, {integer(7), integer(4), integer(3), integer(6)});
    x = DenseMatrix(2, 2);
    fraction_free_gaussian_elimination_solve(A, b, x);

    REQUIRE(x == DenseMatrix(
                     2, 2, {integer(5), integer(5), integer(2), integer(-1)}));
}

TEST_CASE("test_fraction_free_gauss_jordan_solve(): matrices", "[matrices]")
{
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(1), integer(1), integer(-1)});
    DenseMatrix b = DenseMatrix(2, 1, {integer(5), integer(3)});
    DenseMatrix x = DenseMatrix(2, 1);
    fraction_free_gauss_jordan_solve(A, b, x);

    REQUIRE(x == DenseMatrix(2, 1, {integer(4), integer(1)}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(2), integer(2), integer(3), integer(4),
                           integer(3), integer(3), integer(3), integer(4),
                           integer(9), integer(8), integer(7), integer(6)});
    b = DenseMatrix(4, 1, {integer(10), integer(11), integer(13), integer(30)});
    x = DenseMatrix(4, 1);
    fraction_free_gauss_jordan_solve(A, b, x);

    REQUIRE(x == DenseMatrix(4, 1,
                             {integer(1), integer(1), integer(1), integer(1)}));

    // Solve two systems at once, Ax = transpose([7, 3]) and Ax = transpose([4,
    // 6])
    A = DenseMatrix(2, 2, {integer(1), integer(1), integer(1), integer(-1)});
    b = DenseMatrix(2, 2, {integer(7), integer(4), integer(3), integer(6)});
    x = DenseMatrix(2, 2);
    fraction_free_gauss_jordan_solve(A, b, x);

    REQUIRE(x == DenseMatrix(
                     2, 2, {integer(5), integer(5), integer(2), integer(-1)}));
}

TEST_CASE("test_fraction_free_LU(): matrices", "[matrices]")
{
    // Example 3, page 14, Nakos, G. C., Turner, P. R., Williams, R. M. (1997).
    // Fraction-free algorithms for linear and polynomial equations.
    // ACM SIGSAM Bulletin, 31(3), 11–19. doi:10.1145/271130.271133.
    DenseMatrix A
        = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                             integer(2), integer(2), integer(3), integer(4),
                             integer(3), integer(3), integer(3), integer(4),
                             integer(9), integer(8), integer(7), integer(6)});
    DenseMatrix LU = DenseMatrix(4, 4);
    DenseMatrix U = DenseMatrix(4, 4);
    fraction_free_LU(A, LU);

    REQUIRE(
        LU == DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                                 integer(2), integer(-2), integer(-3),
                                 integer(-4), integer(3), integer(-3),
                                 integer(3), integer(4), integer(9),
                                 integer(-10), integer(10), integer(-10)}));

    DenseMatrix b = DenseMatrix(
        4, 1, {integer(10), integer(11), integer(13), integer(30)});
    DenseMatrix x = DenseMatrix(4, 1);

    // To solve the system Ax = b, We call forward substitution algorithm with
    // LU and b. Then we call backward substitution algorithm with with LU and
    // modified b from forward substitution. This will find the solutions.
    forward_substitution(LU, b, x);

    REQUIRE(x == DenseMatrix(4, 1, {integer(10), integer(-9), integer(7),
                                    integer(-10)}));

    back_substitution(LU, x, b);

    REQUIRE(b == DenseMatrix(4, 1,
                             {integer(1), integer(1), integer(1), integer(1)}));
}

TEST_CASE("test_LU(): matrices", "[matrices]")
{
    DenseMatrix A = DenseMatrix(3, 3, {integer(1), integer(3), integer(5),
                                       integer(2), integer(5), integer(6),
                                       integer(8), integer(3), integer(1)});
    DenseMatrix L = DenseMatrix(3, 3);
    DenseMatrix U = DenseMatrix(3, 3);
    LU(A, L, U);

    REQUIRE(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    integer(2), integer(1), integer(0),
                                    integer(8), integer(21), integer(1)}));
    REQUIRE(U == DenseMatrix(3, 3, {integer(1), integer(3), integer(5),
                                    integer(0), integer(-1), integer(-4),
                                    integer(0), integer(0), integer(45)}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(6), integer(3),
                           integer(3), integer(5), integer(6), integer(-5),
                           integer(2), integer(4), integer(5), integer(6),
                           integer(6), integer(-10), integer(2), integer(-30)});
    L = DenseMatrix(4, 4);
    U = DenseMatrix(4, 4);
    LU(A, L, U);

    REQUIRE(
        L == DenseMatrix(4, 4, {integer(1), integer(0), integer(0), integer(0),
                                integer(3), integer(1), integer(0), integer(0),
                                integer(2), integer(0), integer(1), integer(0),
                                integer(6), integer(22),
                                div(integer(-230), integer(7)), integer(1)}));
    REQUIRE(U == DenseMatrix(
                     4, 4, {integer(1), integer(2), integer(6), integer(3),
                            integer(0), integer(-1), integer(-12), integer(-14),
                            integer(0), integer(0), integer(-7), integer(0),
                            integer(0), integer(0), integer(0), integer(260)}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    L = DenseMatrix(2, 2);
    U = DenseMatrix(2, 2);
    DenseMatrix B = DenseMatrix(2, 2);
    LU(A, L, U);
    mul_dense_dense(L, U, B);

    REQUIRE(A == B);
}

TEST_CASE("test_fraction_free_LDU(): matrices", "[matrices]")
{
    DenseMatrix A = DenseMatrix(3, 3);
    DenseMatrix L = DenseMatrix(3, 3);
    DenseMatrix D = DenseMatrix(3, 3);
    DenseMatrix U = DenseMatrix(3, 3);

    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(5),
                           integer(-3), integer(2), integer(6), integer(2),
                           integer(1)});
    fraction_free_LDU(A, L, D, U);

    REQUIRE(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    integer(5), integer(-13), integer(0),
                                    integer(6), integer(-10), integer(1)}));
    REQUIRE(D == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    integer(0), integer(-13), integer(0),
                                    integer(0), integer(0), integer(-13)}));
    REQUIRE(U == DenseMatrix(3, 3, {integer(1), integer(2), integer(3),
                                    integer(0), integer(-13), integer(-13),
                                    integer(0), integer(0), integer(91)}));

    A = DenseMatrix(3, 3,
                    {integer(1), integer(2), mul(integer(3), symbol("a")),
                     integer(5), mul(integer(-3), symbol("a")),
                     mul(integer(2), symbol("a")), mul(integer(6), symbol("a")),
                     mul(integer(2), symbol("b")), integer(1)});
    fraction_free_LDU(A, L, D, U);

    REQUIRE(
        L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(5),
                                sub(mul(integer(-3), symbol("a")), integer(10)),
                                integer(0), mul(integer(6), symbol("a")),
                                add(mul(integer(-12), symbol("a")),
                                    mul(integer(2), symbol("b"))),
                                integer(1)}));
    REQUIRE(D == DenseMatrix(
                     3, 3, {integer(1), integer(0), integer(0), integer(0),
                            sub(mul(integer(-3), symbol("a")), integer(10)),
                            integer(0), integer(0), integer(0),
                            sub(mul(integer(-3), symbol("a")), integer(10))}));
    REQUIRE(U
            == DenseMatrix(
                   3, 3,
                   {integer(1), integer(2), mul(integer(3), symbol("a")),
                    integer(0), sub(mul(integer(-3), symbol("a")), integer(10)),
                    mul(integer(-13), symbol("a")), integer(0), integer(0),
                    add(mul(mul(integer(13), symbol("a")),
                            add(mul(integer(-12), symbol("a")),
                                mul(integer(2), symbol("b")))),
                        mul(sub(mul(integer(-3), symbol("a")), integer(10)),
                            add(mul(integer(-18), pow(symbol("a"), integer(2))),
                                integer(1))))}));

    A = DenseMatrix(3, 3, {integer(5), integer(3), integer(1), integer(-1),
                           integer(4), integer(6), integer(-10), integer(-2),
                           integer(9)});
    fraction_free_LDU(A, L, D, U);

    REQUIRE(L == DenseMatrix(3, 3, {integer(5), integer(0), integer(0),
                                    integer(-1), integer(23), integer(0),
                                    integer(-10), integer(20), integer(1)}));
    REQUIRE(D == DenseMatrix(3, 3, {integer(5), integer(0), integer(0),
                                    integer(0), integer(115), integer(0),
                                    integer(0), integer(0), integer(23)}));
    REQUIRE(U == DenseMatrix(3, 3, {integer(5), integer(3), integer(1),
                                    integer(0), integer(23), integer(31),
                                    integer(0), integer(0), integer(129)}));
}

TEST_CASE("test_QR(): matrices", "[matrices]")
{
    DenseMatrix A = DenseMatrix(3, 3);
    DenseMatrix Q = DenseMatrix(3, 3);
    DenseMatrix R = DenseMatrix(3, 3);

    A = DenseMatrix(3, 3, {integer(12), integer(-51), integer(4), integer(6),
                           integer(167), integer(-68), integer(-4), integer(24),
                           integer(-41)});
    QR(A, Q, R);
}

TEST_CASE("test_LDL(): matrices", "[matrices]")
{
    DenseMatrix A = DenseMatrix(
        3, 3, {integer(4), integer(12), integer(-16), integer(12), integer(37),
               integer(-43), integer(-16), integer(-43), integer(98)});
    DenseMatrix L = DenseMatrix(3, 3);
    DenseMatrix D = DenseMatrix(3, 3);

    LDL(A, L, D);

    REQUIRE(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    integer(3), integer(1), integer(0),
                                    integer(-4), integer(5), integer(1)}));
    REQUIRE(D == DenseMatrix(3, 3, {integer(4), integer(0), integer(0),
                                    integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(9)}));

    A = DenseMatrix(3, 3, {integer(25), integer(15), integer(-5), integer(15),
                           integer(18), integer(0), integer(-5), integer(0),
                           integer(11)});
    LDL(A, L, D);

    REQUIRE(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    div(integer(3), integer(5)), integer(1),
                                    integer(0), div(integer(-1), integer(5)),
                                    div(integer(1), integer(3)), integer(1)}));
    REQUIRE(D == DenseMatrix(3, 3, {integer(25), integer(0), integer(0),
                                    integer(0), integer(9), integer(0),
                                    integer(0), integer(0), integer(9)}));
}

/*TEST_CASE("test_cholesky(): matrices", "[matrices]")
{
    DenseMatrix A = DenseMatrix(3, 3, {integer(4), integer(12), integer(-16),
                                       integer(12), integer(37), integer(-43),
                                       integer(-16), integer(-43),
integer(98)});
    DenseMatrix L = DenseMatrix(3, 3);

    cholesky(A, L);

    REQUIRE(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                   integer(3), integer(1), integer(0),
                                   integer(-4), integer(5), integer(1)}));
}*/

TEST_CASE("test_determinant(): matrices", "[matrices]")
{
    // Test cases are taken from SymPy
    DenseMatrix M = DenseMatrix(1, 1, {integer(1)});
    REQUIRE(eq(*det_bareis(M), *integer(1)));
    REQUIRE(eq(*det_berkowitz(M), *integer(1)));

    M = DenseMatrix(2, 2, {integer(-3), integer(2), integer(8), integer(-5)});
    REQUIRE(eq(*det_bareis(M), *integer(-1)));
    REQUIRE(eq(*det_berkowitz(M), *integer(-1)));

    M = DenseMatrix(2, 2, {symbol("x"), integer(1), symbol("y"),
                           mul(integer(2), symbol("y"))});
    REQUIRE(
        eq(*det_bareis(M),
           *sub(mul(integer(2), mul(symbol("x"), symbol("y"))), symbol("y"))));

    M = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(1), integer(2),
                     integer(3), integer(1), integer(3), integer(6)});
    REQUIRE(eq(*det_bareis(M), *integer(1)));
    REQUIRE(eq(*det_berkowitz(M), *integer(1)));

    M = DenseMatrix(4, 4, {integer(3), integer(-2), integer(0), integer(5),
                           integer(-2), integer(1), integer(-2), integer(2),
                           integer(0), integer(-2), integer(5), integer(0),
                           integer(5), integer(0), integer(3), integer(4)});
    REQUIRE(eq(*det_bareis(M), *integer(-289)));
    REQUIRE(eq(*det_berkowitz(M), *integer(-289)));

    M = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                           integer(5), integer(6), integer(7), integer(8),
                           integer(9), integer(10), integer(11), integer(12),
                           integer(13), integer(14), integer(15), integer(16)});
    REQUIRE(eq(*det_bareis(M), *integer(0)));
    REQUIRE(eq(*det_berkowitz(M), *integer(0)));

    M = DenseMatrix(
        5, 5, {integer(3), integer(2), integer(0), integer(0), integer(0),
               integer(0), integer(3), integer(2), integer(0), integer(0),
               integer(0), integer(0), integer(3), integer(2), integer(0),
               integer(0), integer(0), integer(0), integer(3), integer(2),
               integer(2), integer(0), integer(0), integer(0), integer(3)});
    REQUIRE(eq(*det_bareis(M), *integer(275)));
    REQUIRE(eq(*det_berkowitz(M), *integer(275)));

    M = DenseMatrix(
        5, 5, {integer(1), integer(0), integer(1), integer(2), integer(12),
               integer(2), integer(0), integer(1), integer(1), integer(4),
               integer(2), integer(1), integer(1), integer(-1), integer(3),
               integer(3), integer(2), integer(-1), integer(1), integer(8),
               integer(1), integer(1), integer(1), integer(0), integer(6)});
    REQUIRE(eq(*det_bareis(M), *integer(-55)));
    REQUIRE(eq(*det_berkowitz(M), *integer(-55)));

    M = DenseMatrix(
        5, 5, {integer(-5), integer(2), integer(3), integer(4), integer(5),
               integer(1), integer(-4), integer(3), integer(4), integer(5),
               integer(1), integer(2), integer(-3), integer(4), integer(5),
               integer(1), integer(2), integer(3), integer(-2), integer(5),
               integer(1), integer(2), integer(3), integer(4), integer(-1)});
    REQUIRE(eq(*det_bareis(M), *integer(11664)));
    REQUIRE(eq(*det_berkowitz(M), *integer(11664)));

    M = DenseMatrix(
        5, 5, {integer(2), integer(7), integer(-1), integer(3), integer(2),
               integer(0), integer(0), integer(1), integer(0), integer(1),
               integer(-2), integer(0), integer(7), integer(0), integer(2),
               integer(-3), integer(-2), integer(4), integer(5), integer(3),
               integer(1), integer(0), integer(0), integer(0), integer(1)});
    REQUIRE(eq(*det_bareis(M), *integer(123)));
    REQUIRE(eq(*det_berkowitz(M), *integer(123)));
}

TEST_CASE("test_berkowitz(): matrices", "[matrices]")
{
    std::vector<DenseMatrix> polys;
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");

    DenseMatrix M
        = DenseMatrix(2, 2, {integer(1), integer(0), integer(-1), integer(1)});
    berkowitz(M, polys);

    REQUIRE(polys[1]
            == DenseMatrix(3, 1, {integer(1), integer(-2), integer(1)}));
    REQUIRE(polys[0] == DenseMatrix(2, 1, {integer(1), integer(-1)}));

    polys.clear();

    M = DenseMatrix(3, 3,
                    {integer(3), integer(4), integer(2), integer(1), integer(6),
                     integer(2), integer(5), integer(9), integer(0)});
    berkowitz(M, polys);

    polys.clear();

    M = DenseMatrix(3, 3,
                    {x, y, z, integer(1), integer(0), integer(0), y, z, x});
    berkowitz(M, polys);

    REQUIRE(polys[2]
            == DenseMatrix(4, 1, {integer(1), mul(integer(-2), x),
                                  sub(sub(pow(x, integer(2)), mul(y, z)), y),
                                  sub(mul(x, y), pow(z, integer(2)))}));
    REQUIRE(polys[1] == DenseMatrix(3, 1, {integer(1), mul(integer(-1), x),
                                           mul(integer(-1), y)}));
    REQUIRE(polys[0] == DenseMatrix(2, 1, {integer(1), mul(integer(-1), x)}));

    polys.clear();

    M = DenseMatrix(3, 3,
                    {integer(1), integer(1), integer(1), integer(1), integer(2),
                     integer(3), integer(1), integer(3), integer(6)});
    berkowitz(M, polys);

    REQUIRE(polys[2] == DenseMatrix(4, 1, {integer(1), integer(-9), integer(9),
                                           integer(-1)}));
    REQUIRE(polys[1]
            == DenseMatrix(3, 1, {integer(1), integer(-3), integer(1)}));
    REQUIRE(polys[0] == DenseMatrix(2, 1, {integer(1), integer(-1)}));
}

TEST_CASE("test_solve_functions(): matrices", "[matrices]")
{
    DenseMatrix A
        = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
                             integer(2), integer(2), integer(3), integer(4),
                             integer(3), integer(3), integer(3), integer(4),
                             integer(9), integer(8), integer(7), integer(6)});
    DenseMatrix b = DenseMatrix(
        4, 1, {integer(10), integer(11), integer(13), integer(30)});
    DenseMatrix x = DenseMatrix(4, 1);

    fraction_free_LU_solve(A, b, x);

    REQUIRE(x == DenseMatrix(4, 1,
                             {integer(1), integer(1), integer(1), integer(1)}));

    x = DenseMatrix(4, 1);
    LU_solve(A, b, x);

    REQUIRE(x == DenseMatrix(4, 1,
                             {integer(1), integer(1), integer(1), integer(1)}));

    A = DenseMatrix(2, 2, {integer(5), integer(-4), integer(8), integer(1)});
    b = DenseMatrix(2, 1, {integer(7), integer(26)});
    x = DenseMatrix(2, 1);
    fraction_free_LU_solve(A, b, x);

    REQUIRE(x == DenseMatrix(2, 1, {integer(3), integer(2)}));

    x = DenseMatrix(2, 1);
    LU_solve(A, b, x);

    REQUIRE(x == DenseMatrix(2, 1, {integer(3), integer(2)}));

    A = DenseMatrix(2, 2, {integer(5), integer(-4), integer(-4), integer(7)});
    b = DenseMatrix(2, 1, {integer(24), integer(-23)});
    x = DenseMatrix(2, 1);
    LDL_solve(A, b, x);

    REQUIRE(x == DenseMatrix(2, 1, {integer(4), integer(-1)}));

    A = DenseMatrix(2, 2, {integer(19), integer(-5), integer(-5), integer(1)});
    b = DenseMatrix(2, 1, {integer(3), integer(-3)});
    x = DenseMatrix(2, 1);
    LDL_solve(A, b, x);

    REQUIRE(x == DenseMatrix(2, 1, {integer(2), integer(7)}));
}

TEST_CASE("test_char_poly(): matrices", "[matrices]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> t = symbol("t");

    DenseMatrix A = DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                       integer(0), integer(1), integer(0),
                                       integer(0), integer(0), integer(1)});
    DenseMatrix B = DenseMatrix(4, 1);
    char_poly(A, B);

    REQUIRE(B == DenseMatrix(
                     4, 1, {integer(1), integer(-3), integer(3), integer(-1)}));

    A = DenseMatrix(2, 2, {integer(1), integer(3), integer(2), integer(0)});
    B = DenseMatrix(3, 1);
    char_poly(A, B);

    REQUIRE(B == DenseMatrix(3, 1, {integer(1), integer(-1), integer(-6)}));

    A = DenseMatrix(2, 2, {integer(2), integer(1), integer(-1), integer(0)});
    B = DenseMatrix(3, 1);
    char_poly(A, B);

    REQUIRE(B == DenseMatrix(3, 1, {integer(1), integer(-2), integer(1)}));

    A = DenseMatrix(2, 2, {x, y, z, t});
    B = DenseMatrix(3, 1);
    char_poly(A, B);

    REQUIRE(
        B == DenseMatrix(3, 1, {integer(1),
                                add(mul(integer(-1), t), mul(integer(-1), x)),
                                add(mul(integer(-1), mul(y, z)), mul(t, x))}));
}

TEST_CASE("test_inverse(): matrices", "[matrices]")
{
    DenseMatrix I3 = DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                        integer(0), integer(1), integer(0),
                                        integer(0), integer(0), integer(1)});

    DenseMatrix A
        = DenseMatrix(4, 4, {integer(1), integer(0), integer(0), integer(0),
                             integer(0), integer(1), integer(0), integer(0),
                             integer(0), integer(0), integer(1), integer(0),
                             integer(0), integer(0), integer(0), integer(1)});
    DenseMatrix B = DenseMatrix(4, 4);

    inverse_fraction_free_LU(A, B);
    REQUIRE(A == B);
    inverse_LU(A, B);
    REQUIRE(A == B);
    inverse_gauss_jordan(A, B);
    REQUIRE(A == B);

    A = DenseMatrix(3, 3,
                    {integer(2), integer(3), integer(5), integer(3), integer(6),
                     integer(2), integer(8), integer(3), integer(6)});
    B = DenseMatrix(3, 3);
    DenseMatrix C = DenseMatrix(3, 3);

    inverse_fraction_free_LU(A, B);
    mul_dense_dense(A, B, C);
    REQUIRE(C == I3);

    inverse_LU(A, B);
    mul_dense_dense(A, B, C);
    REQUIRE(C == I3);

    inverse_gauss_jordan(A, B);
    mul_dense_dense(A, B, C);
    REQUIRE(C == I3);

    A = DenseMatrix(3, 3, {integer(48), integer(49), integer(31), integer(9),
                           integer(71), integer(94), integer(59), integer(28),
                           integer(65)});

    inverse_fraction_free_LU(A, B);
    mul_dense_dense(A, B, C);
    REQUIRE(C == I3);

    inverse_LU(A, B);
    mul_dense_dense(A, B, C);
    REQUIRE(C == I3);

    inverse_gauss_jordan(A, B);
    mul_dense_dense(A, B, C);
    REQUIRE(C == I3);
}

TEST_CASE("test_csr_has_canonical_format(): matrices", "[matrices]")
{
    std::vector<unsigned> p = {0, 2, 3, 6};
    std::vector<unsigned> j = {2, 0, 2, 0, 1, 2};

    REQUIRE(CSRMatrix::csr_has_canonical_format(p, j, 3) == false);

    j = {0, 2, 2, 0, 1, 1};

    REQUIRE(CSRMatrix::csr_has_canonical_format(p, j, 3) == false);
}

TEST_CASE("test_csr_eq(): matrices", "[matrices]")
{
    CSRMatrix A = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(1), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});
    CSRMatrix B = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(1), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});

    REQUIRE(A == B);

    CSRMatrix C = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(0), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});

    REQUIRE(not(A == C));
}

TEST_CASE("test_from_coo(): matrices", "[matrices]")
{
    // Example from:
    // http://docs.scipy.org/doc/scipy/reference/generated/scipy.sparse.csr_matrix.html#scipy.sparse.csr_matrix
    CSRMatrix A = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(1), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});
    CSRMatrix B
        = CSRMatrix::from_coo(3, 3, {0, 0, 1, 2, 2, 2}, {0, 2, 2, 0, 1, 2},
                              {integer(1), integer(2), integer(3), integer(4),
                               integer(5), integer(6)});

    REQUIRE(A == B);

    A = CSRMatrix(4, 6, {0, 2, 4, 7, 8}, {0, 1, 1, 3, 2, 3, 4, 5},
                  {integer(10), integer(20), integer(30), integer(40),
                   integer(50), integer(60), integer(70), integer(80)});
    B = CSRMatrix::from_coo(
        4, 6, {0, 0, 1, 1, 2, 2, 2, 3}, {0, 1, 1, 3, 2, 3, 4, 5},
        {integer(10), integer(20), integer(30), integer(40), integer(50),
         integer(60), integer(70), integer(80)});

    REQUIRE(A == B);

    // Check for duplicate removal
    // Here duplicates are summed to create one element
    A = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                  {integer(0), integer(2), integer(3), integer(4), integer(12),
                   integer(6)});
    B = CSRMatrix::from_coo(3, 3, {0, 0, 0, 1, 2, 2, 2, 2},
                            {0, 2, 0, 2, 0, 1, 2, 1},
                            {integer(1), integer(2), integer(-1), integer(3),
                             integer(4), integer(5), integer(6), integer(7)});

    REQUIRE(A == B);

    A = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                  {integer(-1), integer(3), integer(3), integer(4), integer(5),
                   integer(13)});
    B = CSRMatrix::from_coo(3, 3, {0, 0, 0, 1, 2, 2, 2, 2},
                            {2, 2, 0, 2, 0, 1, 2, 2},
                            {integer(1), integer(2), integer(-1), integer(3),
                             integer(4), integer(5), integer(6), integer(7)});

    REQUIRE(A == B);
}

TEST_CASE("test_csr_diagonal(): matrices", "[matrices]")
{
    CSRMatrix A = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(1), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});
    DenseMatrix D = DenseMatrix(3, 1);
    csr_diagonal(A, D);

    REQUIRE(D == DenseMatrix(3, 1, {integer(1), integer(0), integer(6)}));
}

TEST_CASE("test_csr_scale_rows(): matrices", "[matrices]")
{
    CSRMatrix A = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(1), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});
    DenseMatrix X = DenseMatrix(3, 1, {integer(1), integer(-1), integer(3)});

    csr_scale_rows(A, X);

    REQUIRE(A == CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                           {integer(1), integer(2), integer(-3), integer(12),
                            integer(15), integer(18)}));

    X = DenseMatrix(3, 1, {integer(1), integer(0), integer(-1)});
    CHECK_THROWS_AS(csr_scale_columns(A, X), std::runtime_error);
}

TEST_CASE("test_csr_scale_columns(): matrices", "[matrices]")
{
    CSRMatrix A = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(1), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});
    DenseMatrix X = DenseMatrix(3, 1, {integer(1), integer(-1), integer(3)});

    csr_scale_columns(A, X);

    REQUIRE(A == CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                           {integer(1), integer(6), integer(9), integer(4),
                            integer(-5), integer(18)}));

    X = DenseMatrix(3, 1, {integer(0), integer(1), integer(-1)});
    CHECK_THROWS_AS(csr_scale_columns(A, X), std::runtime_error);
}

TEST_CASE("test_csr_binop_csr_canonical(): matrices", "[matrices]")
{
    CSRMatrix A = CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                            {integer(1), integer(2), integer(3), integer(4),
                             integer(5), integer(6)});
    CSRMatrix B = CSRMatrix(3, 3);

    csr_binop_csr_canonical(A, A, B, add);
    REQUIRE(B == CSRMatrix(3, 3, {0, 2, 3, 6}, {0, 2, 2, 0, 1, 2},
                           {integer(2), integer(4), integer(6), integer(8),
                            integer(10), integer(12)}));

    csr_binop_csr_canonical(A, A, B, sub);
    REQUIRE(B == CSRMatrix(3, 3));

    CSRMatrix C = CSRMatrix(3, 3, {0, 1, 3, 3}, {1, 0, 1},
                            {integer(7), integer(8), integer(9)});

    csr_binop_csr_canonical(A, C, B, add);
    REQUIRE(B == CSRMatrix(3, 3, {0, 3, 6, 9}, {0, 1, 2, 0, 1, 2, 0, 1, 2},
                           {integer(1), integer(7), integer(2), integer(8),
                            integer(9), integer(3), integer(4), integer(5),
                            integer(6)}));
}

TEST_CASE("test_eye(): matrices", "[matrices]")
{
    DenseMatrix A;
    eye(A, 3);

    REQUIRE(A == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(1)}));

    eye(A, 3, 3, 1);
    REQUIRE(A == DenseMatrix(3, 3, {integer(0), integer(1), integer(0),
                                    integer(0), integer(0), integer(1),
                                    integer(0), integer(0), integer(0)}));

    eye(A, 3, 3, -1);
    REQUIRE(A == DenseMatrix(3, 3, {integer(0), integer(0), integer(0),
                                    integer(1), integer(0), integer(0),
                                    integer(0), integer(1), integer(0)}));

    eye(A, 3, 3, -2);
    REQUIRE(A == DenseMatrix(3, 3, {integer(0), integer(0), integer(0),
                                    integer(0), integer(0), integer(0),
                                    integer(1), integer(0), integer(0)}));
}

TEST_CASE("test_diag(): matrices", "[matrices]")
{
    DenseMatrix A;
    vec_basic d{integer(1), integer(2), integer(3)};

    diag(A, d);
    REQUIRE(A == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
                                    integer(0), integer(2), integer(0),
                                    integer(0), integer(0), integer(3)}));

    diag(A, d, 1);
    REQUIRE(A == DenseMatrix(4, 4,
                             {integer(0), integer(1), integer(0), integer(0),
                              integer(0), integer(0), integer(2), integer(0),
                              integer(0), integer(0), integer(0), integer(3),
                              integer(0), integer(0), integer(0), integer(0)}));

    diag(A, d, -2);
    REQUIRE(
        A == DenseMatrix(5, 5, {integer(0), integer(0), integer(0), integer(0),
                                integer(0), integer(0), integer(0), integer(0),
                                integer(0), integer(0), integer(1), integer(0),
                                integer(0), integer(0), integer(0), integer(0),
                                integer(2), integer(0), integer(0), integer(0),
                                integer(0), integer(0), integer(3), integer(0),
                                integer(0)}));
}

TEST_CASE("test_ones_zeros(): matrices", "[matrices]")
{
    DenseMatrix A;

    ones(A, 1, 5);
    REQUIRE(A == DenseMatrix(1, 5, {integer(1), integer(1), integer(1),
                                    integer(1), integer(1)}));

    ones(A, 2, 3);
    REQUIRE(A == DenseMatrix(2, 3, {integer(1), integer(1), integer(1),
                                    integer(1), integer(1), integer(1)}));

    zeros(A, 3, 2);
    REQUIRE(A == DenseMatrix(3, 2, {integer(0), integer(0), integer(0),
                                    integer(0), integer(0), integer(0)}));
}

TEST_CASE("Test Jacobian", "[matrices]")
{
    DenseMatrix A, X, J;
    RCP<const Basic> x = symbol("x"), y = symbol("y"), z = symbol("z"),
                     t = symbol("t"), f = function_symbol("f", x);
    A = DenseMatrix(
        4, 1, {add(x, z), mul(y, z), add(mul(z, x), add(y, t)), add(x, y)});
    X = DenseMatrix(4, 1, {x, y, z, t});
    J = DenseMatrix(4, 4);
    jacobian(A, X, J);
    REQUIRE(J == DenseMatrix(4, 4, {integer(1), integer(0), integer(1),
                                    integer(0), integer(0), z, y, integer(0), z,
                                    integer(1), x, integer(1), integer(1),
                                    integer(1), integer(0), integer(0)}));

    X = DenseMatrix(4, 1, {f, y, z, t});
    CHECK_THROWS_AS(jacobian(A, X, J), std::runtime_error);

    A = DenseMatrix(
        4, 1, {add(x, z), mul(y, z), add(mul(z, x), add(y, t)), add(x, y)});
    X = DenseMatrix(3, 1, {x, y, z});
    J = DenseMatrix(4, 3);
    jacobian(A, X, J);
    REQUIRE(J == DenseMatrix(4, 3, {integer(1), integer(0), integer(1),
                                    integer(0), z, y, z, integer(1), x,
                                    integer(1), integer(1), integer(0)}));

    A = DenseMatrix(2, 1, {mul(f, y), pow(y, integer(2))});
    X = DenseMatrix(2, 1, {f, y});
    J = DenseMatrix(2, 2);
    sjacobian(A, X, J);
    REQUIRE(J == DenseMatrix(2, 2, {y, f, integer(0), mul(integer(2), y)}));
}

TEST_CASE("Test Diff", "[matrices]")
{
    DenseMatrix A, J;
    RCP<const Symbol> x = symbol("x"), y = symbol("y"), z = symbol("z"),
                      t = symbol("t");
    auto f = function_symbol("f", x);
    A = DenseMatrix(
        2, 2, {add(x, z), mul(y, z), add(mul(z, x), add(y, t)), add(x, y)});
    J = DenseMatrix(2, 2);
    diff(A, x, J);
    REQUIRE(J == DenseMatrix(2, 2, {integer(1), integer(0), z, integer(1)}));

    A = DenseMatrix(
        2, 2, {add(f, z), mul(f, x), add(mul(z, f), add(y, t)), add(f, y)});
    sdiff(A, f, J);
    REQUIRE(J == DenseMatrix(2, 2, {integer(1), x, z, integer(1)}));
}
