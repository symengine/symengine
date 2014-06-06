#include <iostream>
#include <lela/integer.h>
#include <lela/ring/modular.h>
#include <lela/matrix/dense.h>

using namespace LELA;

int main ()
{
    //Integers Z;
    DenseMatrix<integer> A(4, 4);
    DenseMatrix<integer> B(4, 4);

    for (unsigned i = 1; i < 5; i++)
        for (unsigned j = 1; j < 5; j++){
            A.setEntry(i, j, (i - 1)*4 + j);
            B.setEntry(i, j, (i - 1)*4 + j);
        }

	return 0;
}
