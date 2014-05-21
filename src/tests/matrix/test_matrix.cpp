#include <chrono>

#include "matrix.h"
#include "integer.h"
#include "symbol.h"
#include "add.h"

using CSymPy::print_stack_on_segfault;
using CSymPy::RCP;
using CSymPy::integer;
using CSymPy::DenseMatrix;
using CSymPy::densematrix;
using CSymPy::Basic;
using CSymPy::symbol;

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
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_dense_dense_addition();

    return 0;
}
