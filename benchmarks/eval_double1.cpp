#include <iostream>
#include <chrono>

#include "Teuchos_stacktrace.hpp"

#include "basic.h"
#include "add.h"
#include "symbol.h"
#include "dict.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "eval_double.h"


using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Mul;
using CSymPy::Pow;
using CSymPy::Symbol;
using CSymPy::umap_basic_num;
using CSymPy::map_vec_int;
using CSymPy::Integer;
using CSymPy::multinomial_coefficients;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::rcp_dynamic_cast;
using CSymPy::integer;
using CSymPy::sin;
using CSymPy::eval_double;

int main(int argc, char* argv[])
{
    Teuchos::print_stack_on_segfault();

    RCP<const Basic> e = sin(integer(1));
    double r, r_exact;

    for(int i = 0; i < 9000; i++)
        e = pow(add(mul(add(e, pow(integer(2), integer(-3))), integer(3)), integer(1)), div(integer(2), integer(3)));

//  Too long:
//  std::cout << "Evaluating: " << *e << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 500; i++)
        r = eval_double(*e);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
/*
In SymPy for few iterations:
In [7]: sympify("(1 + 3*(1/8 + (1 + 3*(1/8 + (1 + 3*(1/8 + (1 + 3*(1/8 + (1 + 3*(1/8 + sin(1)))^(2/3)))^(2/3)))^(2/3)))^(2/3)))^(2/3)").n(20)
Out[7]: 8.0152751504518535013

//    r_exact = 8.0152751504518535013;

Here is code to use SymPy for more iterations:

In [5]: e = sin(1)

In [6]: for i in range(10):
   ...:     e = ((e+2**(-S(3)))*3 + 1)**(S(2)/3)
   ...:

In [7]: e.n(20)
Out[7]: 9.6473976427977306146

But unfortunately SymPy can't do more than perhaps 10 or 20 iterations, while
we need to test ~10000. However, the numbers seem to converge to 9.85647...

*/
//    r_exact = 9.85647;
    r_exact = 9.8564741713701043569;
    std::cout << "r (double) = " << r << std::endl;
    std::cout << "r (exact)  = " << r_exact << std::endl;
    std::cout << "error      = " << std::abs(r-r_exact) << std::endl;

    return 0;
}
