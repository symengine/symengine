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
using CSymPy::add_dense_dense;
using CSymPy::pow;

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

void test_gauss_jordan_elimination()
{
    // These test cases are verified with SymPy
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(2, 2, {integer(1), integer(2), integer(2), integer(4)});
    gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(2), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});
    gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(0), integer(0), integer(0), integer(0)});
    gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(0), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(2, 2, {symbol("a"), integer(0), symbol("c"), integer(0)});
    gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(-1),
        integer(7), integer(6), integer(4), integer(5), integer(2)});
    B = DenseMatrix(3, 3);
    gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(3, 2, {integer(-9), integer(4), integer(3), integer(-1),
        integer(7), integer(6)});
    B = DenseMatrix(3, 2);
    gauss_jordan_elimination(A, B);

    assert(B == DenseMatrix(3, 2, {integer(1), integer(0), integer(0), integer(1),
    integer(0), integer(0)}));
}

void test_diagonal_solve()
{
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    DenseMatrix b = DenseMatrix(2, 1, {integer(1), integer(1)});
    DenseMatrix C = DenseMatrix(2, 1);
    diagonal_solve(A, b, C);

    assert(C == DenseMatrix(2, 1, {integer(1), integer(1)}));

    A = DenseMatrix(2, 2, {integer(5), integer(-4), integer(8), integer(1)});
    b = DenseMatrix(2, 1, {integer(7), integer(26)});
    C = DenseMatrix(2, 1);
    diagonal_solve(A, b, C);

    assert(C == DenseMatrix(2, 1, {integer(3), integer(2)}));

    // below two sets produce the correct matrix but the results are not
    // simplified. See: https://github.com/sympy/csympy/issues/183
    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("b"), symbol("a")});
    b = DenseMatrix(2, 1, {add(pow(symbol("a"), integer(2)), pow(symbol("b"), integer(2))),
        mul(integer(2), mul(symbol("a"), symbol("b")))});
    C = DenseMatrix(2, 1);
    diagonal_solve(A, b, C);

//    assert(C == DenseMatrix(2, 1, {symbol("a"), symbol("b")}));

    A = DenseMatrix(2, 2, {integer(1), integer(1), integer(1), integer(-1)});
    b = DenseMatrix(2, 1, {add(symbol("a"), symbol("b")), sub(symbol("a"), symbol("b"))});
    C = DenseMatrix(2, 1, {symbol("a"), symbol("b")});
    diagonal_solve(A, b, C);

//    assert(C == DenseMatrix(2, 1, {symbol("a"), symbol("b")}));
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

    A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    B = DenseMatrix(4, 4);
    fraction_free_gaussian_elimination(A, B);

    assert(B == DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(0), integer(-2), integer(-3), integer(-4), integer(0), integer(0),
        integer(3), integer(4), integer(0), integer(0), integer(0), integer(20)}));
}


int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_dense_dense_addition();

    test_add_dense_scalar();

    test_dense_dense_multiplication();

    test_mul_dense_scalar();

    test_gauss_jordan_elimination();

    test_diagonal_solve();

    test_transpose_dense();

    test_submatrix_dense();

    test_fraction_free_gaussian_elimination();

    return 0;
}
