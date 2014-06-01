#include <chrono>

#include "matrix.h"
#include "integer.h"
#include "symbol.h"
#include "add.h"
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

void test_dense_dense_addition()
{
    std::vector<RCP<const Basic>> l, m, n;
    RCP<const DenseMatrix> A, B, C;

    l = {integer(1), integer(2), integer(3), integer(4)};
    A = densematrix(2, 2, l);

    m = {integer(1), integer(2), integer(3), integer(4)};
    B = densematrix(2, 2, m);

    n = {integer(2), integer(4), integer(6), integer(8)};
    C = densematrix(2, 2, n);

    assert(eq(add_dense_dense(*A, *B), C));

    l = {integer(1), integer(-1), integer(0), integer(2)};
    A = densematrix(2, 2, l);

    m = {integer(1), integer(2), integer(-3), integer(0)};
    B = densematrix(2, 2, m);

    n = {integer(2), integer(1), integer(-3), integer(2)};
    C = densematrix(2, 2, n);

    assert(eq(add_dense_dense(*A, *B), C));


    l = {integer(1), symbol("a"), integer(0), symbol("b")};
    A = densematrix(2, 2, l);

    m = {symbol("c"), integer(2), integer(-3), integer(0)};
    B = densematrix(2, 2, m);

    n = {add(integer(1), symbol("c")), add(symbol("a"), integer(2)),
        integer(-3), symbol("b")};
    C = densematrix(2, 2, n);

    assert(eq(add_dense_dense(*A, *B), C));

    l = {symbol("a"), symbol("b"), symbol("c"), symbol("d")};
    A = densematrix(1, 4, l);

    m = {integer(3), integer(2), integer(-3), integer(0)};
    B = densematrix(1, 4, m);

    n = {add(integer(3), symbol("a")), add(symbol("b"), integer(2)),
        add(symbol("c"), integer(-3)), symbol("d")};
    C = densematrix(1, 4, n);

    assert(eq(add_dense_dense(*A, *B), C));

    l = {integer(7), integer(4), integer(-3), integer(-5), symbol("a"), symbol("b")};
    A = densematrix(2, 3, l);

    m = {integer(10), integer(13), integer(5), integer(-7), symbol("c"), symbol("d")};
    B = densematrix(2, 3, m);

    n = {integer(17), integer(17), integer(2), integer(-12),
        add(symbol("a"), symbol("c")), add(symbol("b"), symbol("d"))};
    C = densematrix(2, 3, n);
}

void test_add_matrix()
{
    // Just want to check whether the member method works
    std::vector<RCP<const Basic>> l, m, n;
    RCP<const DenseMatrix> A, B, C;

    l = {integer(1), integer(2), integer(3), integer(4)};
    A = densematrix(2, 2, l);

    m = {integer(1), integer(2), integer(3), integer(4)};
    B = densematrix(2, 2, m);

    n = {integer(2), integer(4), integer(6), integer(8)};
    C = densematrix(2, 2, n);

    assert(eq(A->add_matrix(*B), C));
}

void test_dense_dense_multiplication()
{
    std::vector<RCP<const Basic>> l, m, n;
    RCP<const DenseMatrix> A, B, C;

    l = {integer(1), integer(0), integer(0), integer(1)};
    A = densematrix(2, 2, l);

    m = {integer(1), integer(2), integer(3), integer(4)};
    B = densematrix(2, 2, m);

    n = {integer(1), integer(2), integer(3), integer(4)};
    C = densematrix(2, 2, n);

    assert(eq(mul_dense_dense(*A, *B), C));

    l = {integer(1), integer(3), integer(7), integer(-5)};
    A = densematrix(1, 4, l);

    m = {integer(1), integer(2), integer(3), integer(4)};
    B = densematrix(4, 1, m);

    n = {integer(8)};
    C = densematrix(1, 1, n);

    assert(eq(mul_dense_dense(*A, *B), C));

    l = {integer(10), integer(-3), integer(7), integer(-5)};
    A = densematrix(4, 1, l);

    m = {integer(11), integer(20), integer(12)};
    B = densematrix(1, 3, m);

    n = {integer(110), integer(200), integer(120), integer(-33), integer(-60),
        integer(-36), integer(77), integer(140), integer(84), integer(-55),
        integer(-100), integer(-60)};
    C = densematrix(4, 3, n);

    assert(eq(mul_dense_dense(*A, *B), C));

    l = {symbol("a"), symbol("b"), symbol("c"), symbol("p"), symbol("q"),
        symbol("r"), symbol("u"), symbol("v"), symbol("w")};
    A = densematrix(3, 3, l);

    m = {symbol("x"), symbol("y"), symbol("z")};
    B = densematrix(3, 1, m);

    n = {add(add(mul(symbol("a"), symbol("x")), mul(symbol("b"), symbol("y"))), mul(symbol("c"), symbol("z"))),
    add(add(mul(symbol("p"), symbol("x")), mul(symbol("q"), symbol("y"))), mul(symbol("r"), symbol("z"))),
    add(add(mul(symbol("u"), symbol("x")), mul(symbol("v"), symbol("y"))), mul(symbol("w"), symbol("z")))
    };
    C = densematrix(3, 1, n);

    assert(eq(mul_dense_dense(*A, *B), C));
}

void test_mul_matrix()
{
    // Just want to check whether the member method works
    std::vector<RCP<const Basic>> l, m, n;
    RCP<const DenseMatrix> A, B, C;

    l = {integer(1), integer(0), integer(0), integer(1)};
    A = densematrix(2, 2, l);

    m = {integer(1), integer(2), integer(3), integer(4)};
    B = densematrix(2, 2, m);

    n = {integer(1), integer(2), integer(3), integer(4)};
    C = densematrix(2, 2, n);

    assert(eq(A->mul_matrix(*B), C));
}

void test_mul_dense_scalar()
{
    // More tests should be added
    std::vector<RCP<const Basic>> l, m;
    RCP<const DenseMatrix> A, B;

    l = {integer(1), integer(2), integer(3), integer(4)};
    A = densematrix(2, 2, l);

    m = {integer(2), integer(4), integer(6), integer(8)};
    B = densematrix(2, 2, m);

    RCP<const Basic> k = integer(2);
    assert(eq(mul_dense_scalar(*A, k), B));
}

void test_add_dense_scalar()
{
    // More tests should be added
    std::vector<RCP<const Basic>> l, m;
    RCP<const DenseMatrix> A, B;

    l = {integer(1), integer(2), integer(3), integer(4)};
    A = densematrix(2, 2, l);

    m = {integer(3), integer(4), integer(5), integer(6)};
    B = densematrix(2, 2, m);

    RCP<const Basic> k = integer(2);
    assert(eq(add_dense_scalar(*A, k), B));
}

void test_gaussian_elimination()
{
    std::vector<RCP<const Basic>> l, m;
    RCP<const DenseMatrix> A, B;

    l = {integer(1), integer(2), integer(3), integer(4)};
    A = densematrix(2, 2, l);

    m = {integer(1), integer(0), integer(0), integer(1)};
    B = densematrix(2, 2, m);

    assert(eq(gaussian_elimination(*A), B));

    l = {integer(1), integer(2), integer(2), integer(4)};
    A = densematrix(2, 2, l);

    m = {integer(1), integer(2), integer(0), integer(0)};
    B = densematrix(2, 2, m);

    assert(eq(gaussian_elimination(*A), B));

    l = {integer(1), integer(0), integer(0), integer(0)};
    A = densematrix(2, 2, l);

    m = {integer(1), integer(0), integer(0), integer(0)};
    B = densematrix(2, 2, m);

    assert(eq(gaussian_elimination(*A), B));

    l = {integer(0), integer(0), integer(0), integer(0)};
    A = densematrix(2, 2, l);

    m = {integer(0), integer(0), integer(0), integer(0)};
    B = densematrix(2, 2, m);

    assert(eq(gaussian_elimination(*A), B));

    l = {integer(1), integer(2), integer(3), integer(-1), integer(7),
        integer(6), integer(4), integer(5), integer(2)};
    A = densematrix(3, 3, l);

    m = {integer(1), integer(0), integer(0), integer(0), integer(1),
        integer(0), integer(0), integer(0), integer(1)};
    B = densematrix(3, 3, m);

    assert(eq(gaussian_elimination(*A), B));

    l = {integer(-9), integer(4), integer(3), integer(-1), integer(7),
        integer(6)};
    A = densematrix(3, 2, l);

    m = {integer(1), integer(0), integer(0), integer(1), integer(0),
        integer(0)};
    B = densematrix(3, 2, m);

    assert(eq(gaussian_elimination(*A), B));
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

    return 0;
}
