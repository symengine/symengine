#include <chrono>

#include "matrix.h"
#include "integer.h"
#include "symbol.h"
#include "add.h"
#include "mul.h"
#include "pow.h"

using CSymPy::print_stack_on_segfault;
using CSymPy::RCP;
using CSymPy::integer;
using CSymPy::DenseMatrix;
using CSymPy::Basic;
using CSymPy::symbol;
using CSymPy::is_a;
using CSymPy::Add;
using CSymPy::minus_one;

void test_dense_dense_addition()
{
    DenseMatrix C = DenseMatrix(2, 2);
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    add_dense_dense(A, B, C);

    assert(C == DenseMatrix(2, 2, {integer(2), integer(4), integer(6), integer(8)}));

    A = DenseMatrix(2, 2, {integer(1), integer(-1), integer(0), integer(2)});
    B = DenseMatrix(2, 2, {integer(1), integer(2), integer(-3), integer(0)});
    add_dense_dense(A, B, C);

    assert(C == DenseMatrix(2, 2, {integer(2), integer(1), integer(-3), integer(2)}));

    A = DenseMatrix(2, 2, {integer(1), symbol("a"), integer(0), symbol("b")});
    B = DenseMatrix(2, 2, {symbol("c"), integer(2), integer(-3), integer(0)});
    add_dense_dense(A, B, C);

    assert(C == DenseMatrix(2, 2, {add(integer(1), symbol("c")), add(symbol("a"),
        integer(2)), integer(-3), symbol("b")}));

    C = DenseMatrix(1, 4);
    A = DenseMatrix(1, 4, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    B = DenseMatrix(1, 4, {integer(3), integer(2), integer(-3), integer(0)});
    add_dense_dense(A, B, C);

    assert(C == DenseMatrix(1, 4, {add(integer(3), symbol("a")), add(symbol("b"),
        integer(2)), add(symbol("c"), integer(-3)), symbol("d")}));

    C = DenseMatrix(2, 3);
    A = DenseMatrix(2, 3, {integer(7), integer(4), integer(-3), integer(-5),
        symbol("a"), symbol("b")});
    B = DenseMatrix(2, 3, {integer(10), integer(13), integer(5), integer(-7),
        symbol("c"), symbol("d")});
    add_dense_dense(A, B, C);

    assert(C == DenseMatrix(2, 3, {integer(17), integer(17), integer(2), integer(-12),
        add(symbol("a"), symbol("c")), add(symbol("b"), symbol("d"))}));
}

void test_add_dense_scalar()
{
    // More tests should be added
    RCP<const Basic> k = integer(2);

    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    add_dense_scalar(A, k, B);

    assert(B == DenseMatrix(2, 2, {integer(3), integer(4), integer(5), integer(6)}));
}

void test_dense_dense_multiplication()
{
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    DenseMatrix B = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix C = DenseMatrix(2, 2);
    mul_dense_dense(A, B, C);

    assert(C == DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)}));

    A = DenseMatrix(1, 4, {integer(1), integer(3), integer(7), integer(-5)});
    B = DenseMatrix(4, 1, {integer(1), integer(2), integer(3), integer(4)});
    C = DenseMatrix(1, 1);
    mul_dense_dense(A, B, C);

    assert(C == DenseMatrix(1, 1, {integer(8)}));

    A = DenseMatrix(4, 1, {integer(10), integer(-3), integer(7), integer(-5)});
    B = DenseMatrix(1, 3, {integer(11), integer(20), integer(12)});
    C = DenseMatrix(4, 3);
    mul_dense_dense(A, B, C);

    assert(C == DenseMatrix(4, 3, {integer(110), integer(200), integer(120),
        integer(-33), integer(-60), integer(-36), integer(77), integer(140),
        integer(84), integer(-55), integer(-100), integer(-60)}));

    A = DenseMatrix(3, 3, {symbol("a"), symbol("b"), symbol("c"), symbol("p"),
        symbol("q"), symbol("r"), symbol("u"), symbol("v"), symbol("w")});
    B = DenseMatrix(3, 1, {symbol("x"), symbol("y"), symbol("z")});
    C = DenseMatrix(3, 1);
    mul_dense_dense(A, B, C);

    assert(C == DenseMatrix(3, 1, {add(add(mul(symbol("a"), symbol("x")),
        mul(symbol("b"), symbol("y"))), mul(symbol("c"), symbol("z"))),
        add(add(mul(symbol("p"), symbol("x")), mul(symbol("q"), symbol("y"))),
        mul(symbol("r"), symbol("z"))), add(add(mul(symbol("u"), symbol("x")),
        mul(symbol("v"), symbol("y"))), mul(symbol("w"), symbol("z")))}));
}

void test_mul_dense_scalar()
{
    // More tests should be added
    RCP<const Basic> k = integer(2);

    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    mul_dense_scalar(A, k, B);

    assert(B == DenseMatrix(2, 2, {integer(2), integer(4), integer(6), integer(8)}));
}

void test_transpose_dense()
{
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    transpose_dense(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(3), integer(2), integer(4)}));

    A = DenseMatrix(3, 3, {symbol("a"), symbol("b"), symbol("c"), symbol("p"),
        symbol("q"), symbol("r"), symbol("u"), symbol("v"), symbol("w")});
    B = DenseMatrix(3, 3);
    transpose_dense(A, B);

    assert(B == DenseMatrix(3, 3, {symbol("a"), symbol("p"), symbol("u"), symbol("b"),
        symbol("q"), symbol("v"), symbol("c"), symbol("r"), symbol("w")}));
}

void test_submatrix_dense()
{
    DenseMatrix A = DenseMatrix(3, 3, {symbol("a"), symbol("b"), symbol("c"),
        symbol("p"), symbol("q"), symbol("r"), symbol("u"), symbol("v"), symbol("w")});
    DenseMatrix B = DenseMatrix(3, 2);
    submatrix_dense(A, 1, 3, 2, 3, B);

    assert(B == DenseMatrix(3, 2, {symbol("b"), symbol("c"), symbol("q"),
        symbol("r"), symbol("v"), symbol("w")}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3),
        integer(4), integer(5), integer(6), integer(7), integer(8), integer(9),
        integer(10), integer(11), integer(12), integer(13), integer(14),
        integer(15), integer(16)});
    B = DenseMatrix(3, 3);
    submatrix_dense(A, 2, 4, 2, 4, B);

    assert(B == DenseMatrix(3, 3, {integer(6), integer(7), integer(8), integer(10),
        integer(11), integer(12), integer(14), integer(15), integer(16)}));

}

void test_pivoted_gaussian_elimination()
{
    auto pivotlist = std::vector<unsigned>(2);
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    pivoted_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(2), integer(0), integer(-2)}));

    A = DenseMatrix(2, 2, {integer(2), integer(3), integer(3), integer(4)});
    B = DenseMatrix(2, 2);
    pivoted_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), div(integer(3), integer(2)),
        integer(0), div(minus_one, integer(2))}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    B = DenseMatrix(2, 2);
    pivoted_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), div(symbol("b"), symbol("a")),
        integer(0), sub(symbol("d"), mul(symbol("c"), div(symbol("b"), symbol("a"))))}));

    pivotlist = std::vector<unsigned>(3);
    A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(2), integer(3), integer(4), integer(3)});
    B = DenseMatrix(3, 3);
    pivoted_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(5), integer(4), integer(6), integer(8)});
    B = DenseMatrix(3, 3);
    pivoted_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(0),
        integer(1), integer(2), integer(0), integer(0), integer(3)}));
}

void test_fraction_free_gaussian_elimination()
{
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(2), integer(0), integer(-2)}));

    A = DenseMatrix(2, 2, {integer(1), integer(2), integer(2), integer(4)});
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(2), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {symbol("a"), symbol("b"), integer(0),
        sub(mul(symbol("a"), symbol("d")), mul(symbol("b"), symbol("c")))}));

    // Test case taken from :
    // Fraction-Free Algorithms for Linear and Polynomial Equations, George C Nakos,
    // Peter R Turner et. al.
    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    B = DenseMatrix(4, 4);
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(0), integer(-2), integer(-3), integer(-4), integer(0), integer(0),
        integer(3), integer(4), integer(0), integer(0), integer(0), integer(-10)}));

    // Test case taken from :
    // A SIMPLIFIED FRACTION-FREE INTEGER GAUSS ELIMINATION ALGORITHM
    // Peter R. Turner
    A = DenseMatrix(4, 4, {integer(8), integer(7), integer(4), integer(1),
        integer(4), integer(6), integer(7), integer(3), integer(6), integer(3),
        integer(4), integer(6), integer(4), integer(5), integer(8), integer(2)});
    B = DenseMatrix(4, 4);
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(4, 4, {integer(8), integer(7), integer(4), integer(1),
    integer(0), integer(20), integer(40), integer(20), integer(0), integer(0),
    integer(110), integer(150), integer(0), integer(0), integer(0), integer(-450)}));

    // Below two test cases are taken from:
    // http://www.mathworks.in/help/symbolic/mupad_ref/linalg-gausselim.html
    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(-1), integer(1),
        integer(0), integer(1), integer(2), integer(-1), integer(4)});
    B = DenseMatrix(3, 3);
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(2), integer(-1),
        integer(0), integer(-2), integer(2), integer(0), integer(0), integer(-2)}));

    A = DenseMatrix(3, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(-1), integer(0), integer(1), integer(0), integer(3), integer(5),
        integer(6), integer(9)});
    B = DenseMatrix(3, 4);
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(3, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(0), integer(2), integer(4), integer(4), integer(0), integer(0),
        integer(-2), integer(-2)}));
}

void test_pivoted_fraction_free_gaussian_elimination()
{
    auto pivotlist = std::vector<unsigned>(2);
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(2), integer(0), integer(-2)}));

    pivotlist = std::vector<unsigned>(4);
    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    B = DenseMatrix(4, 4);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(0), integer(-2), integer(-3), integer(-4), integer(0), integer(0),
        integer(3), integer(4), integer(0), integer(0), integer(0), integer(-10)}));

    pivotlist = std::vector<unsigned>(3);
    A = DenseMatrix(3, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(-1), integer(0), integer(1), integer(0), integer(3), integer(5),
        integer(6), integer(9)});
    B = DenseMatrix(3, 4);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(0), integer(2), integer(4), integer(4), integer(0), integer(0),
        integer(-2), integer(-2)}));

    A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(2), integer(3), integer(3), integer(3)});
    B = DenseMatrix(3, 3);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(0),
        integer(0), integer(0), integer(0), integer(0), integer(0)}));

    // These tests won't work with fraction_free_gaussian_elimination
    A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(2), integer(3), integer(4), integer(3)});
    B = DenseMatrix(3, 3);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(5), integer(4), integer(6), integer(8)});
    B = DenseMatrix(3, 3);
    pivoted_fraction_free_gaussian_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(0),
        integer(2), integer(4), integer(0), integer(0), integer(6)}));
}

void test_pivoted_gauss_jordan_elimination()
{
    // These test cases are verified with SymPy
    auto pivotlist = std::vector<unsigned>(2);
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(2, 2, {integer(1), integer(2), integer(2), integer(4)});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(2), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(0), integer(0), integer(0), integer(0)});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(0), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(2, 2, {symbol("a"), integer(0), symbol("c"), integer(0)});
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)}));

    pivotlist = std::vector<unsigned>(3);
    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(-1),
        integer(7), integer(6), integer(4), integer(5), integer(2)});
    B = DenseMatrix(3, 3);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(3, 2, {integer(-9), integer(4), integer(3), integer(-1),
        integer(7), integer(6)});
    B = DenseMatrix(3, 2);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 2, {integer(1), integer(0), integer(0), integer(1),
    integer(0), integer(0)}));

    // These tests won't work with gauss_jordan_elimination
    A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(2), integer(3), integer(4), integer(3)});
    B = DenseMatrix(3, 3);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(1), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(5), integer(4), integer(6), integer(8)});
    B = DenseMatrix(3, 3);
    pivoted_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(1)}));
}

void test_fraction_free_gauss_jordan_elimination()
{
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    fraction_free_gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(-2), integer(0), integer(0), integer(-2)}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    B = DenseMatrix(4, 4);
    fraction_free_gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(4, 4, {integer(-10), integer(0), integer(0), integer(0),
        integer(0), integer(-10), integer(0), integer(0), integer(0), integer(0),
        integer(-10), integer(0), integer(0), integer(0), integer(0), integer(-10)}));

    A = DenseMatrix(4, 4, {integer(1), integer(7), integer(5), integer(4),
        integer(7), integer(2), integer(2), integer(4), integer(3), integer(6),
        integer(3), integer(4), integer(9), integer(5), integer(7), integer(5)});
    fraction_free_gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(4, 4, {integer(-139), integer(0), integer(0), integer(0),
        integer(0), integer(-139), integer(0), integer(0), integer(0), integer(0),
        integer(-139), integer(0), integer(0), integer(0), integer(0), integer(-139)}));
}

void test_pivoted_fraction_free_gauss_jordan_elimination()
{
    // These tests won't work with fraction_free_gauss_jordan_elimination
    auto pivotlist = std::vector<unsigned>(3);
    DenseMatrix A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(2), integer(3), integer(4), integer(3)});
    DenseMatrix B = DenseMatrix(3, 3);
    pivoted_fraction_free_gauss_jordan_elimination(A, B, pivotlist);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(1), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(2),
        integer(2), integer(5), integer(4), integer(6), integer(8)});
    B = DenseMatrix(3, 3);
    pivoted_fraction_free_gauss_jordan_elimination(A, B, pivotlist);

    // Here the diagonal entry is 6 although the det(A) = -6, this because we
    // have interchanged two rows
    assert(B == DenseMatrix(3, 3, {integer(6), integer(0), integer(0), integer(0),
        integer(6), integer(0), integer(0), integer(0), integer(6)}));
}

void test_fraction_free_gaussian_elimination_solve()
{
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(1), integer(1),
        integer(-1)});
    DenseMatrix b = DenseMatrix(2, 1, {integer(5), integer(3)});
    DenseMatrix x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    assert(x == DenseMatrix(2, 1, {integer(4), integer(1)}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    b = DenseMatrix(4, 1, {integer(10), integer(11), integer(13), integer(30)});
    x = DenseMatrix(4, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    assert(x == DenseMatrix(4, 1, {integer(1), integer(1), integer(1),
        integer(1)}));

    A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    b = DenseMatrix(2, 1, {integer(1), integer(1)});
    x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    assert(x == DenseMatrix(2, 1, {integer(1), integer(1)}));

    A = DenseMatrix(2, 2, {integer(5), integer(-4), integer(8), integer(1)});
    b = DenseMatrix(2, 1, {integer(7), integer(26)});
    x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

    assert(x == DenseMatrix(2, 1, {integer(3), integer(2)}));

    // below two sets produce the correct matrix but the results are not
    // simplified. See: https://github.com/sympy/csympy/issues/183
    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("b"), symbol("a")});
    b = DenseMatrix(2, 1, {add(pow(symbol("a"), integer(2)), pow(symbol("b"), integer(2))),
        mul(integer(2), mul(symbol("a"), symbol("b")))});
    x = DenseMatrix(2, 1);
    fraction_free_gaussian_elimination_solve(A, b, x);

//    assert(x == DenseMatrix(2, 1, {symbol("a"), symbol("b")}));

    A = DenseMatrix(2, 2, {integer(1), integer(1), integer(1), integer(-1)});
    b = DenseMatrix(2, 1, {add(symbol("a"), symbol("b")), sub(symbol("a"), symbol("b"))});
    x = DenseMatrix(2, 1, {symbol("a"), symbol("b")});
    fraction_free_gaussian_elimination_solve(A, b, x);

//    assert(x == DenseMatrix(2, 1, {symbol("a"), symbol("b")}));
}

void test_fraction_free_gauss_jordan_solve()
{
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(1), integer(1),
        integer(-1)});
    DenseMatrix b = DenseMatrix(2, 1, {integer(5), integer(3)});
    DenseMatrix x = DenseMatrix(2, 1);
    fraction_free_gauss_jordan_solve(A, b, x);

    assert(x == DenseMatrix(2, 1, {integer(4), integer(1)}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    b = DenseMatrix(4, 1, {integer(10), integer(11), integer(13), integer(30)});
    x = DenseMatrix(4, 1);
    fraction_free_gauss_jordan_solve(A, b, x);

    assert(x == DenseMatrix(4, 1, {integer(1), integer(1), integer(1),
        integer(1)}));
}

void test_fraction_free_LU()
{
    // Example 3, page 14, Nakos, G. C., Turner, P. R., Williams, R. M. (1997).
    // Fraction-free algorithms for linear and polynomial equations.
    // ACM SIGSAM Bulletin, 31(3), 11–19. doi:10.1145/271130.271133.
    DenseMatrix A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    DenseMatrix LU = DenseMatrix(4, 4);
    DenseMatrix U = DenseMatrix(4, 4);
    fraction_free_LU(A, LU);

    assert(LU == DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(-2), integer(-3), integer(-4), integer(3), integer(-3),
        integer(3), integer(4), integer(9), integer(-10), integer(10), integer(-10)}));

    DenseMatrix b = DenseMatrix(4, 1, {integer(10), integer(11), integer(13),
        integer(30)});
    DenseMatrix x = DenseMatrix(4, 1);

    // To solve the system Ax = b, We call forward substitution algorithm with
    // LU and b. Then we call backward substitution algorithm with with LU and
    // modified b from forward substitution. This will find the solutions.
    forward_substitution(LU, b, x);

    assert(x == DenseMatrix(4, 1, {integer(10), integer(-9), integer(7),
        integer(-10)}));

    back_substitution(LU, x, b);

    assert(b == DenseMatrix(4, 1, {integer(1), integer(1), integer(1),
        integer(1)}));
}

void test_LU()
{
    DenseMatrix A = DenseMatrix(3, 3, {integer(1), integer(3), integer(5),
        integer(2), integer(5), integer(6), integer(8), integer(3), integer(1)});
    DenseMatrix L = DenseMatrix(3, 3);
    DenseMatrix U = DenseMatrix(3, 3);
    LU(A, L, U);

    assert(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
        integer(2), integer(1), integer(0), integer(8), integer(21), integer(1)}));
    assert(U == DenseMatrix(3, 3, {integer(1), integer(3), integer(5),
        integer(0), integer(-1), integer(-4), integer(0), integer(0), integer(45)}));

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(6), integer(3),
        integer(3), integer(5), integer(6), integer(-5), integer(2), integer(4),
        integer(5), integer(6), integer(6), integer(-10), integer(2),
        integer(-30)});
    L = DenseMatrix(4, 4);
    U = DenseMatrix(4, 4);
    LU(A, L, U);

    assert(L == DenseMatrix(4, 4, {integer(1), integer(0), integer(0), integer(0),
        integer(3), integer(1), integer(0), integer(0), integer(2), integer(0),
        integer(1), integer(0), integer(6), integer(22), div(integer(-230),
        integer(7)), integer(1)}));
    assert(U == DenseMatrix(4, 4, {integer(1), integer(2), integer(6),
        integer(3), integer(0), integer(-1), integer(-12), integer(-14),
        integer(0), integer(0), integer(-7), integer(0), integer(0), integer(0),
        integer(0), integer(260)}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    L = DenseMatrix(2, 2);
    U = DenseMatrix(2, 2);
    DenseMatrix B = DenseMatrix(2, 2);
    LU(A, L, U);
    mul_dense_dense(L, U, B);

    assert(A == B);
}

void test_fraction_free_LDU()
{
    DenseMatrix A = DenseMatrix(3, 3);
    DenseMatrix L = DenseMatrix(3, 3);
    DenseMatrix D = DenseMatrix(3, 3);
    DenseMatrix U = DenseMatrix(3, 3);

    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(5),
        integer(-3), integer(2), integer(6), integer(2), integer(1)});
    fraction_free_LDU(A, L, D, U);

    assert(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
        integer(5), integer(-13), integer(0), integer(6), integer(-10), integer(1)}));
    assert(D == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(0),
        integer(-13), integer(0), integer(0), integer(0), integer(-13)}));
    assert(U == DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(0),
        integer(-13), integer(-13), integer(0), integer(0), integer(91)}));

    A = DenseMatrix(3, 3, {integer(1), integer(2), mul(integer(3), symbol("a")),
        integer(5), mul(integer(-3), symbol("a")), mul(integer(2), symbol("a")),
        mul(integer(6), symbol("a")), mul(integer(2), symbol("b")), integer(1)});
    fraction_free_LDU(A, L, D, U);

    assert(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(5),
        sub(mul(integer(-3), symbol("a")), integer(10)), integer(0),
        mul(integer(6), symbol("a")), add(mul(integer(-12), symbol("a")),
        mul(integer(2), symbol("b"))), integer(1)}));
    assert(D == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(0),
        sub(mul(integer(-3), symbol("a")), integer(10)), integer(0), integer(0),
        integer(0), sub(mul(integer(-3), symbol("a")), integer(10))}));
    assert(U == DenseMatrix(3, 3, {integer(1), integer(2),
        mul(integer(3), symbol("a")), integer(0),
        sub(mul(integer(-3), symbol("a")), integer(10)),
        mul(integer(-13), symbol("a")), integer(0), integer(0),
        add(
            mul(mul(integer(13), symbol("a")),
                add(mul(integer(-12), symbol("a")), mul(integer(2), symbol("b")))),
            mul(sub(mul(integer(-3), symbol("a")), integer(10)),
                add(mul(integer(-18), pow(symbol("a"), integer(2))), integer(1)))
            )
        }));

    A = DenseMatrix(3, 3, {integer(5), integer(3), integer(1), integer(-1),
        integer(4), integer(6), integer(-10), integer(-2), integer(9)});
    fraction_free_LDU(A, L, D, U);

    assert(L == DenseMatrix(3, 3, {integer(5), integer(0), integer(0), integer(-1),
        integer(23), integer(0), integer(-10), integer(20), integer(1)}));
    assert(D == DenseMatrix(3, 3, {integer(5), integer(0), integer(0), integer(0),
        integer(115), integer(0), integer(0), integer(0), integer(23)}));
    assert(U == DenseMatrix(3, 3, {integer(5), integer(3), integer(1), integer(0),
        integer(23), integer(31), integer(0), integer(0), integer(129)}));
}

void test_QR()
{
    DenseMatrix A = DenseMatrix(3, 3);
    DenseMatrix Q = DenseMatrix(3, 3);
    DenseMatrix R = DenseMatrix(3, 3);

    A = DenseMatrix(3, 3, {integer(12), integer(-51), integer(4), integer(6),
        integer(167), integer(-68), integer(-4), integer(24), integer(-41)});
    QR(A, Q, R);
}

void test_LDL()
{
    DenseMatrix A = DenseMatrix(3, 3, {integer(4), integer(12), integer(-16),
        integer(12), integer(37), integer(-43), integer(-16), integer(-43),
        integer(98)});
    DenseMatrix L = DenseMatrix(3, 3);
    DenseMatrix D = DenseMatrix(3, 3);

    LDL(A, L, D);

    assert(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(3),
        integer(1), integer(0), integer(-4), integer(5), integer(1)}));
    assert(D == DenseMatrix(3, 3, {integer(4), integer(0), integer(0), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(9)}));

    A = DenseMatrix(3, 3, {integer(25), integer(15), integer(-5), integer(15),
        integer(18), integer(0), integer(-5), integer(0), integer(11)});
    LDL(A, L, D);

    assert(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
        div(integer(3), integer(5)), integer(1), integer(0),
        div(integer(-1), integer(5)), div(integer(1), integer(3)), integer(1)}));
    assert(D == DenseMatrix(3, 3, {integer(25), integer(0), integer(0),
        integer(0), integer(9), integer(0), integer(0), integer(0), integer(9)}));
}

void test_cholesky()
{
    DenseMatrix A = DenseMatrix(3, 3, {integer(4), integer(12), integer(-16),
        integer(12), integer(37), integer(-43), integer(-16), integer(-43),
        integer(98)});
    DenseMatrix L = DenseMatrix(3, 3);

    cholesky(A, L);

    assert(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(3),
        integer(1), integer(0), integer(-4), integer(5), integer(1)}));
}

void test_determinant()
{
    // Test cases are taken from SymPy
    DenseMatrix M = DenseMatrix(1, 1, {integer(1)});
    assert(eq(det_bareis(M), integer(1)));
    assert(eq(det_berkowitz(M), integer(1)));

    M = DenseMatrix(2, 2, {integer(-3), integer(2), integer(8), integer(-5)});
    assert(eq(det_bareis(M), integer(-1)));
    assert(eq(det_berkowitz(M), integer(-1)));

    M = DenseMatrix(2, 2, {symbol("x"), integer(1), symbol("y"),
            mul(integer(2), symbol("y"))});
    assert(eq(det_bareis(M), sub(mul(integer(2), mul(symbol("x"), symbol("y"))),
            symbol("y"))));

    M = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(1),
            integer(2), integer(3), integer(1), integer(3), integer(6)});
    assert(eq(det_bareis(M), integer(1)));
    assert(eq(det_berkowitz(M), integer(1)));

    M = DenseMatrix(4, 4, {integer(3), integer(-2), integer(0), integer(5),
            integer(-2), integer(1), integer(-2), integer(2), integer(0),
            integer(-2), integer(5), integer(0), integer(5),  integer(0),
            integer(3), integer(4)});
    assert(eq(det_bareis(M), integer(-289)));
    assert(eq(det_berkowitz(M), integer(-289)));

    M = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
            integer(5), integer(6), integer(7), integer(8), integer(9),
            integer(10), integer(11), integer(12), integer(13), integer(14),
            integer(15), integer(16)});
    assert(eq(det_bareis(M), integer(0)));
    assert(eq(det_berkowitz(M), integer(0)));

    M = DenseMatrix(5, 5, {integer(3), integer(2), integer(0), integer(0), integer(0),
                 integer(0), integer(3), integer(2), integer(0), integer(0),
                 integer(0), integer(0), integer(3), integer(2), integer(0),
                 integer(0), integer(0), integer(0), integer(3), integer(2),
                 integer(2), integer(0), integer(0), integer(0), integer(3)});
    assert(eq(det_bareis(M), integer(275)));
    assert(eq(det_berkowitz(M), integer(275)));

    M = DenseMatrix(5, 5, {integer(1), integer(0), integer(1), integer(2),
            integer(12), integer(2), integer(0), integer(1), integer(1),
            integer(4), integer(2), integer(1), integer(1), integer(-1),
            integer(3), integer(3), integer(2), integer(-1), integer(1),
            integer(8), integer(1), integer(1),  integer(1), integer(0),
            integer(6)});
    assert(eq(det_bareis(M), integer(-55)));
    assert(eq(det_berkowitz(M), integer(-55)));

    M = DenseMatrix(5, 5, {integer(-5), integer(2), integer(3), integer(4),
            integer(5), integer(1), integer(-4), integer(3), integer(4),
            integer(5), integer(1), integer(2), integer(-3), integer(4),
            integer(5), integer(1), integer(2), integer(3), integer(-2),
            integer(5), integer(1), integer(2), integer(3), integer(4),
            integer(-1)});
    assert(eq(det_bareis(M), integer(11664)));
    assert(eq(det_berkowitz(M), integer(11664)));

    M = DenseMatrix(5, 5, {integer(2), integer(7), integer(-1), integer(3),
            integer(2), integer(0), integer(0), integer(1), integer(0),
            integer(1), integer(-2), integer(0), integer(7), integer(0),
            integer(2), integer(-3), integer(-2), integer(4), integer(5),
            integer(3), integer(1), integer(0), integer(0), integer(0),
            integer(1)});
    assert(eq(det_bareis(M), integer(123)));
    assert(eq(det_berkowitz(M), integer(123)));
}

void test_berkowitz()
{
    std::vector<DenseMatrix> polys;
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");

    DenseMatrix M = DenseMatrix(2, 2, {integer(1), integer(0),
        integer(-1), integer(1)});
    berkowitz(M, polys);

    assert(polys[1] == DenseMatrix(3, 1, {integer(1), integer(-2),
        integer(1)}));
    assert(polys[0] == DenseMatrix(2, 1, {integer(1), integer(-1)}));

    polys.clear();

    M = DenseMatrix(3, 3, {integer(3), integer(4), integer(2), integer(1),
        integer(6), integer(2), integer(5), integer(9), integer(0)});
    berkowitz(M, polys);

    polys.clear();

    M = DenseMatrix(3, 3, {x, y, z, integer(1), integer(0), integer(0), y, z, x});
    berkowitz(M, polys);

    assert(polys[2] ==
        DenseMatrix(4, 1, {integer(1), mul(integer(-2), x),
            sub(sub(pow(x, integer(2)), mul(y, z)), y),
            sub(mul(x, y), pow(z, integer(2)))}));
    assert(polys[1] == DenseMatrix(3, 1, {integer(1), mul(integer(-1), x),
        mul(integer(-1), y)}));
    assert(polys[0] == DenseMatrix(2, 1, {integer(1), mul(integer(-1), x)}));

    polys.clear();

    M = DenseMatrix(3, 3, {integer(1), integer(1), integer(1), integer(1),
            integer(2), integer(3), integer(1), integer(3), integer(6)});
    berkowitz(M, polys);

    assert(polys[2] ==
        DenseMatrix(4, 1, {integer(1), integer(-9), integer(9), integer(-1)}));
    assert(polys[1] == DenseMatrix(3, 1, {integer(1), integer(-3), integer(1)}));
    assert(polys[0] == DenseMatrix(2, 1, {integer(1), integer(-1)}));
}

void test_solve_functions()
{
    DenseMatrix A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    DenseMatrix b = DenseMatrix(4, 1, {integer(10), integer(11), integer(13),
        integer(30)});
    DenseMatrix x = DenseMatrix(4, 1);

    fraction_free_LU_solve(A, b, x);

    assert(x == DenseMatrix(4, 1, {integer(1), integer(1), integer(1),
        integer(1)}));

    x = DenseMatrix(4, 1);
    LU_solve(A, b, x);

    assert(x == DenseMatrix(4, 1, {integer(1), integer(1), integer(1),
        integer(1)}));

    A = DenseMatrix(2, 2, {integer(5), integer(-4), integer(8), integer(1)});
    b = DenseMatrix(2, 1, {integer(7), integer(26)});
    x = DenseMatrix(2, 1);
    fraction_free_LU_solve(A, b, x);

    assert(x == DenseMatrix(2, 1, {integer(3), integer(2)}));

    x = DenseMatrix(2, 1);
    LU_solve(A, b, x);

    assert(x == DenseMatrix(2, 1, {integer(3), integer(2)}));

    A = DenseMatrix(2, 2, {integer(5), integer(-4), integer(-4), integer(7)});
    b = DenseMatrix(2, 1, {integer(24), integer(-23)});
    x = DenseMatrix(2, 1);
    LDL_solve(A, b, x);

    assert(x == DenseMatrix(2, 1, {integer(4), integer(-1)}));

    A = DenseMatrix(2, 2, {integer(19), integer(-5), integer(-5), integer(1)});
    b = DenseMatrix(2, 1, {integer(3), integer(-3)});
    x = DenseMatrix(2, 1);
    LDL_solve(A, b, x);

    assert(x == DenseMatrix(2, 1, {integer(2), integer(7)}));
}

void test_inverse()
{
    DenseMatrix I3 = DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
            integer(0), integer(1), integer(0),
            integer(0), integer(0), integer(1)});

    DenseMatrix A =
        DenseMatrix(4, 4, {integer(1), integer(0), integer(0), integer(0),
            integer(0), integer(1), integer(0), integer(0),
            integer(0), integer(0), integer(1), integer(0),
            integer(0), integer(0), integer(0), integer(1)});
    DenseMatrix B = DenseMatrix(4, 4);

    inverse_fraction_free_LU(A, B);
    assert(A == B);
    inverse_LU(A, B);
    assert(A == B);

    A = DenseMatrix(3, 3, {integer(2), integer(3), integer(5),
        integer(3), integer(6), integer(2),
        integer(8), integer(3), integer(6)});
    B = DenseMatrix(3, 3);
    DenseMatrix C = DenseMatrix(3, 3);

    inverse_fraction_free_LU(A, B);
    mul_dense_dense(A, B, C);
    assert(C == I3);

    inverse_LU(A, B);
    mul_dense_dense(A, B, C);
    assert(C == I3);

    A = DenseMatrix(3, 3, {integer(48), integer(49), integer(31),
        integer(9), integer(71), integer(94),
        integer(59), integer(28), integer(65)});

    inverse_fraction_free_LU(A, B);
    mul_dense_dense(A, B, C);
    assert(C == I3);

    inverse_LU(A, B);
    mul_dense_dense(A, B, C);
    assert(C == I3);
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_dense_dense_addition();

    test_add_dense_scalar();

    test_dense_dense_multiplication();

    test_mul_dense_scalar();

    test_transpose_dense();

    test_submatrix_dense();

    test_pivoted_gaussian_elimination();

    test_fraction_free_gaussian_elimination();

    test_pivoted_fraction_free_gaussian_elimination();

    test_pivoted_gauss_jordan_elimination();

    test_fraction_free_gauss_jordan_elimination();

    test_pivoted_fraction_free_gauss_jordan_elimination();

    test_fraction_free_gaussian_elimination_solve();

    test_fraction_free_gauss_jordan_solve();

    test_fraction_free_LU();

    test_LU();

    test_fraction_free_LDU();

    // test_QR();

    test_LDL();

    // test_cholesky();

    test_determinant();

    test_berkowitz();

    test_solve_functions();

    test_inverse();

    return 0;
}

