#include <limits>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "basic.h"
#include "integer.h"
#include "rational.h"
#include "add.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "eval_mpfr.h"
#include "eval_arb.h"
#include "constants.h"
#include "real_double.h"

using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::Rational;
using SymEngine::integer;
using SymEngine::pi;
using SymEngine::mul;
using SymEngine::div;
using SymEngine::eval_mpfr;
using SymEngine::print_stack_on_segfault;

void test_precision()
{
    mpfr_t a, b;
    mpz_t i;
    mpfr_init2(a, 53);
    mpfr_init2(b, 40);
    RCP<const Basic> r = mul(pi, integer(1963319607));
    mpz_init_set_ui(i, 6167950454);

    eval_mpfr(a, *r, MPFR_RNDN);
    mpfr_set_z(b, i, MPFR_RNDN);
    // `a` and `b` are equal as `eval_mpfr` was done with a precision of 53 bits (precision of `a`)
    assert(mpfr_cmp(a, b) == 0);

    mpfr_set_prec(a, 100);
    eval_mpfr(a, *r, MPFR_RNDN);
    // `a` and `b` are not equal as `eval_mpfr` was done with a precision of 100 bits
    assert(mpfr_cmp(a, b) == 1);

    mpz_clear(i);
    mpfr_clear(a);
    mpfr_clear(b);
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_precision();

    return 0;
}
