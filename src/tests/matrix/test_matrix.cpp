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

void test_gaussian_elimination()
{
    // These test cases are verified with SymPy
    DenseMatrix A = DenseMatrix(2, 2, {integer(1), integer(2), integer(3), integer(4)});
    DenseMatrix B = DenseMatrix(2, 2);
    gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(2, 2, {integer(1), integer(2), integer(2), integer(4)});
    gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(2), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)});
    gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {integer(0), integer(0), integer(0), integer(0)});
    gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(0), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(2, 2, {symbol("a"), symbol("b"), symbol("c"), symbol("d")});
    gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(2, 2, {symbol("a"), integer(0), symbol("c"), integer(0)});
    gaussian_elimination(A, B);

    assert(B == DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(0)}));

    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(-1),
        integer(7), integer(6), integer(4), integer(5), integer(2)});
    B = DenseMatrix(3, 3);
    gaussian_elimination(A, B);

    assert(B == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(0),
        integer(1), integer(0), integer(0), integer(0), integer(1)}));

    A = DenseMatrix(3, 2, {integer(-9), integer(4), integer(3), integer(-1),
        integer(7), integer(6)});
    B = DenseMatrix(3, 2);
    gaussian_elimination(A, B);

    assert(B == DenseMatrix(3, 2, {integer(1), integer(0), integer(0), integer(1),
    integer(0), integer(0)}));
}

void test_fraction_free_LU()
{
    // Example 3, page 14, Nakos, G. C., Turner, P. R., Williams, R. M. (1997).
    // Fraction-free algorithms for linear and polynomial equations.
    // ACM SIGSAM Bulletin, 31(3), 11–19. doi:10.1145/271130.271133.
    DenseMatrix A = DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(2), integer(2), integer(3), integer(4), integer(3), integer(3),
        integer(3), integer(4), integer(9), integer(8), integer(7), integer(6)});
    DenseMatrix L = DenseMatrix(4, 4);
    DenseMatrix U = DenseMatrix(4, 4);
    fraction_free_LU(A, L, U);

    assert(L == DenseMatrix(4, 4, {integer(1), integer(0), integer(0), integer(0),
        integer(2), integer(-2), integer(0), integer(0), integer(3), integer(-3),
        integer(3), integer(0), integer(9), integer(-10), integer(10), integer(-10)}));

    assert(U == DenseMatrix(4, 4, {integer(1), integer(2), integer(3), integer(4),
        integer(0), integer(-2), integer(-3), integer(-4), integer(0), integer(0),
        integer(3), integer(4), integer(0), integer(0), integer(0), integer(-10)}));
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

void test_fraction_free_LU_SymPy()
{
    DenseMatrix A = DenseMatrix(3, 3);
    DenseMatrix L = DenseMatrix(3, 3);
    DenseMatrix D = DenseMatrix(3, 3);
    DenseMatrix U = DenseMatrix(3, 3);

    A = DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(5),
        integer(-3), integer(2), integer(6), integer(2), integer(1)});
    fraction_free_LU(A, L, D, U);

    assert(L == DenseMatrix(3, 3, {integer(1), integer(0), integer(0),
        integer(5), integer(-13), integer(0), integer(6), integer(-10), integer(1)}));
    assert(D == DenseMatrix(3, 3, {integer(1), integer(0), integer(0), integer(0),
        integer(-13), integer(0), integer(0), integer(0), integer(-13)}));
    assert(U == DenseMatrix(3, 3, {integer(1), integer(2), integer(3), integer(0),
        integer(-13), integer(-13), integer(0), integer(0), integer(91)}));

    A = DenseMatrix(3, 3, {integer(1), integer(2), mul(integer(3), symbol("a")),
        integer(5), mul(integer(-3), symbol("a")), mul(integer(2), symbol("a")),
        mul(integer(6), symbol("a")), mul(integer(2), symbol("b")), integer(1)});
    fraction_free_LU(A, L, D, U);

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
    fraction_free_LU(A, L, D, U);

    assert(L == DenseMatrix(3, 3, {integer(5), integer(0), integer(0), integer(-1),
        integer(23), integer(0), integer(-10), integer(20), integer(1)}));
    assert(D == DenseMatrix(3, 3, {integer(5), integer(0), integer(0), integer(0),
        integer(115), integer(0), integer(0), integer(0), integer(23)}));
    assert(U == DenseMatrix(3, 3, {integer(5), integer(3), integer(1), integer(0),
        integer(23), integer(31), integer(0), integer(0), integer(129)}));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_dense_dense_addition();
    test_add_dense_scalar();

    test_dense_dense_multiplication();
    test_mul_dense_scalar();

    test_gaussian_elimination();

    test_fraction_free_LU();

    test_LU();

    test_fraction_free_LU_SymPy();

    return 0;
}
