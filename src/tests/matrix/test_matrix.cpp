#include <chrono>

#include "matrix.h"
#include "integer.h"
#include "symbol.h"
#include "add.h"
#include "pow.h"
#include "mul.h"

using CSymPy::print_stack_on_segfault;
using CSymPy::RCP;
using CSymPy::integer;
using CSymPy::DenseMatrix;
using CSymPy::densematrix;
using CSymPy::Basic;
using CSymPy::symbol;
using CSymPy::is_a;
using CSymPy::Add;
using CSymPy::pow;
using CSymPy::rcp_static_cast;
using CSymPy::Mul;

void test_dense_dense_addition()
{
    RCP<const DenseMatrix> A, B, C;

    A = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    B = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    C = densematrix(2, 2, {integer(2), integer(4), integer(6), integer(8)});

    assert(eq(add_dense_dense(*A, *B), C));

    A = densematrix(2, 2, {integer(1), integer(-1), integer(0), integer(2)});
    B = densematrix(2, 2, {integer(1), integer(2), integer(-3), integer(0)});
    C = densematrix(2, 2, {integer(2), integer(1), integer(-3), integer(2)});

    assert(eq(add_dense_dense(*A, *B), C));

    A = densematrix(2, 2, {integer(1), symbol("a"), integer(0), symbol("b")});
    B = densematrix(2, 2, {symbol("c"), integer(2), integer(-3), integer(0)});
    C = densematrix(2, 2, {add(integer(1), symbol("c")), add(symbol("a"),
        integer(2)), integer(-3), symbol("b")});

    assert(eq(add_dense_dense(*A, *B), C));

    A = densematrix(1, 4, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    B = densematrix(1, 4, {integer(3), integer(2), integer(-3), integer(0)});
    C = densematrix(1, 4, {add(integer(3), symbol("a")), add(symbol("b"),
        integer(2)), add(symbol("c"), integer(-3)), symbol("d")});

    assert(eq(add_dense_dense(*A, *B), C));

    A = densematrix(2, 3, {integer(7), integer(4), integer(-3), integer(-5),
        symbol("a"), symbol("b")});
    B = densematrix(2, 3, {integer(10), integer(13), integer(5), integer(-7),
        symbol("c"), symbol("d")});
    C = densematrix(2, 3, {integer(17), integer(17), integer(2), integer(-12),
        add(symbol("a"), symbol("c")), add(symbol("b"), symbol("d"))});

    assert(eq(add_dense_dense(*A, *B), C));
}

void test_add_matrix()
{
    // Just want to check whether the member method works
    RCP<const DenseMatrix> A, B, C;

    A = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    B = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    C = densematrix(2, 2, {integer(2), integer(4), integer(6), integer(8)});

    assert(eq(A->add_matrix(*B), C));
}

void test_dense_dense_multiplication()
{
    RCP<const DenseMatrix> A, B, C;

    A = densematrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    B = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    C = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});

    assert(eq(mul_dense_dense(*A, *B), C));

    A = densematrix(1, 4, {integer(1), integer(3), integer(7), integer(-5)});
    B = densematrix(4, 1, {integer(1), integer(2), integer(3), integer(4)});
    C = densematrix(1, 1, {integer(8)});

    assert(eq(mul_dense_dense(*A, *B), C));

    A = densematrix(4, 1, {integer(10), integer(-3), integer(7), integer(-5)});
    B = densematrix(1, 3, {integer(11), integer(20), integer(12)});
    C = densematrix(4, 3, {integer(110), integer(200), integer(120),
        integer(-33), integer(-60), integer(-36), integer(77), integer(140),
        integer(84), integer(-55), integer(-100), integer(-60)});

    assert(eq(mul_dense_dense(*A, *B), C));

    A = densematrix(3, 3, {symbol("a"), symbol("b"), symbol("c"), symbol("p"),
        symbol("q"), symbol("r"), symbol("u"), symbol("v"), symbol("w")});
    B = densematrix(3, 1, {symbol("x"), symbol("y"), symbol("z")});
    C = densematrix(3, 1, {add(add(mul(symbol("a"), symbol("x")), mul(symbol("b"), symbol("y"))),
        mul(symbol("c"), symbol("z"))), add(add(mul(symbol("p"), symbol("x")),
        mul(symbol("q"), symbol("y"))), mul(symbol("r"), symbol("z"))),
        add(add(mul(symbol("u"), symbol("x")), mul(symbol("v"), symbol("y"))),
        mul(symbol("w"), symbol("z")))});

    assert(eq(mul_dense_dense(*A, *B), C));
}

void test_mul_matrix()
{
    // Just want to check whether the member method works
    RCP<const DenseMatrix> A, B, C;

    A = densematrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    B = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    C = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});

    assert(eq(A->mul_matrix(*B), C));
}

void test_mul_dense_scalar()
{
    // More tests should be added
    RCP<const DenseMatrix> A, B;
    RCP<const Basic> k = integer(2);

    A = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    B = densematrix(2, 2, {integer(2), integer(4), integer(6), integer(8)});

    assert(eq(mul_dense_scalar(*A, k), B));
}

void test_add_dense_scalar()
{
    // More tests should be added
    RCP<const DenseMatrix> A, B;
    RCP<const Basic> k = integer(2);

    A = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    B = densematrix(2, 2, {integer(3), integer(4), integer(5), integer(6)});

    assert(eq(add_dense_scalar(*A, k), B));
}

void test_gaussian_elimination()
{
    // These test cases are verified with SymPy
    std::vector<RCP<const Basic>> l, m;
    RCP<const DenseMatrix> A, B;

    A = densematrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    B = densematrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});

    assert(eq(gaussian_elimination(*A), B));

    A = densematrix(2, 2, {integer(1), integer(2), integer(2), integer(4)});
    B = densematrix(2, 2, {integer(1), integer(2), integer(0), integer(0)});

    assert(eq(gaussian_elimination(*A), B));

    A = densematrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});
    B = densematrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});

    assert(eq(gaussian_elimination(*A), B));

    A = densematrix(2, 2, {integer(0), integer(0), integer(0), integer(0)});
    B = densematrix(2, 2, {integer(0), integer(0), integer(0), integer(0)});

    assert(eq(gaussian_elimination(*A), B));

    A = densematrix(3, 3, {integer(1), integer(2), integer(3), integer(-1),
        integer(7), integer(6), integer(4), integer(5), integer(2)});
    B = densematrix(3, 3, {integer(1), integer(0), integer(0), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(1)});

    assert(eq(gaussian_elimination(*A), B));

    A = densematrix(3, 2, {integer(-9), integer(4), integer(3), integer(-1),
        integer(7), integer(6)});
    B = densematrix(3, 2, {integer(1), integer(0), integer(0), integer(1),
    integer(0), integer(0)});

    assert(eq(gaussian_elimination(*A), B));

    A = densematrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    B = densematrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});

    assert(eq(gaussian_elimination(*A), B));

    A = densematrix(2, 2, {symbol("a"), integer(0), symbol("c"), integer(0)});
    B = densematrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});

    assert(eq(gaussian_elimination(*A), B));
}

void test_diagonal_solve()
{
    RCP<const DenseMatrix> A, b, C;

    A = densematrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    b = densematrix(2, 1, {integer(1), integer(1)});
    C = densematrix(2, 1, {integer(1), integer(1)});

    assert(eq(diagonal_solve(*A, *b), C));

    A = densematrix(2, 2, {integer(5), integer(-4), integer(8), integer(1)});
    b = densematrix(2, 1, {integer(7), integer(26)});
    C = densematrix(2, 1, {integer(3), integer(2)});

    assert(eq(diagonal_solve(*A, *b), C));

    // below two sets produce the correct matrix but the results are not
    // simplified. See: https://github.com/sympy/csympy/issues/183
    A = densematrix(2, 2, {symbol("a"), symbol("b"), symbol("b"), symbol("a")});
    b = densematrix(2, 1, {add(pow(symbol("a"), integer(2)), pow(symbol("b"), integer(2))),
        mul(integer(2), mul(symbol("a"), symbol("b")))});
    C = densematrix(2, 1, {symbol("a"), symbol("b")});

//    assert(eq(diagonal_solve(*A, *b), C));

    A = densematrix(2, 2, {integer(1), integer(1), integer(1), integer(-1)});
    b = densematrix(2, 1, {add(symbol("a"), symbol("b")), sub(symbol("a"), symbol("b"))});
    C = densematrix(2, 1, {symbol("a"), symbol("b")});

//    assert(eq(diagonal_solve(*A, *b), C));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_dense_dense_addition();
    test_add_matrix();

    test_dense_dense_multiplication();
    test_mul_matrix();

    test_mul_dense_scalar();
    test_add_dense_scalar();

    test_gaussian_elimination();
    test_diagonal_solve();

    return 0;
}
