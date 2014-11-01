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
#include "eval_arb.h"


using CSymPy::RCP;
using CSymPy::Basic;
using CSymPy::integer;
using CSymPy::div;
using CSymPy::rcp_static_cast;
using CSymPy::Rational;
using CSymPy::add;
using CSymPy::sqrt;
using CSymPy::mul;
using CSymPy::pow;
using CSymPy::sin;
using CSymPy::cos;
using CSymPy::tan;
using CSymPy::eval_arb;
using CSymPy::print_stack_on_segfault;

void test_Integer()
{
    arb_t a, b;
    arb_init(a);
    arb_init(b);

    eval_arb(a, *integer(3));
    arb_set_ui(b, 3);

    assert(arb_equal(a, b) && arb_is_exact(a));

    eval_arb(a, *integer(-45));
    arb_set_si(b, -45);

    assert(arb_equal(a, b) && arb_is_exact(a));

    srand(time(NULL));
    unsigned int ui = rand();

    RCP<const Basic> i = integer(ui);
    eval_arb(a, *i);
    arb_set_ui(b, ui);

    assert(arb_equal(a, b) && arb_is_exact(a));
}

void test_Rational()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> q = div(integer(22), integer(7));
    eval_arb(a, *q, 4);

    assert(arb_contains_si(a, 3) != 0);

    mpfr_t f;
    mpfr_init2(f, 17);
    mpfr_set_d(f, (double)22/7, MPFR_RNDN);

    eval_arb(a, *q, 10);

    assert(arb_contains_mpfr(a, f));

    srand(time(NULL));
    int x_ = rand();
    srand(time(NULL) + 1000);
    int y_ = rand() - RAND_MAX / 2;
    if (y_ == 0) {
        y_ = RAND_MAX / 2;
    }

    RCP<const Basic> x = integer(x_);
    RCP<const Basic> y = integer(y_);
    q = div(x, y);
    eval_arb(a, *q, 10);

    mpfr_set_d(f, (double)x_/y_, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));
}

void test_Add()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> q = div(integer(223), integer(71));
    RCP<const Basic> r1 = add(sqrt(integer(3)), q);
    eval_arb(a, *r1, 12);

    mpfr_t f;
    mpfr_init2(f, 17);
    mpfr_set_d(f, sqrt(3) + (double)223/71, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));

    q = div(integer(34), integer(7100));
    r1 = add(sqrt(integer(7)), q);
    eval_arb(a, *r1, 10);

    mpfr_set_d(f, sqrt(7) + (double)34/7100, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));
}

void test_Mul()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> q = div(integer(377), integer(120));
    RCP<const Basic> r1 = mul(sqrt(integer(15)), q);
    eval_arb(a, *r1, 11);

    mpfr_t f;
    mpfr_init2(f, 17);
    mpfr_set_d(f, sqrt(15)*(double)377/120, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));

    q = div(integer(9001), integer(71));
    r1 = mul(sqrt(integer(2317)), q);
    eval_arb(a, *r1, 8);

    mpfr_set_d(f, sqrt(2317)*(double)9001/71, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));
}

void test_Pow()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> b = div(integer(377), integer(120));
    RCP<const Basic> e = div(integer(34), integer(1257));
    RCP<const Basic> r1 = pow(b, e);
    eval_arb(a, *r1, 8);

    mpfr_t f;
    mpfr_init2(f, 17);
    mpfr_set_d(f, pow((double)377/120, (double)34/1257), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));

    b = div(integer(120), integer(377));
    e = div(integer(97), integer(34));
    r1 = pow(b, e);
    eval_arb(a, *r1, 10);

    mpfr_set_d(f, pow((double)120/377, (double)97/34), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));
}

void test_Sin()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> r1 = sin(integer(2));
    eval_arb(a, *r1, 10);

    mpfr_t f;
    mpfr_init2(f, 17);
    mpfr_set_d(f, sin(2), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));

    RCP<const Basic> q = div(integer(3), integer(5));
    r1 = add(sin(integer(2)), sin(q));  // r1 = sin(2) + sin(3/5)
    eval_arb(a, *r1, 10);

    mpfr_set_d(f, sin(2) + sin((double)3/5), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));
}

void test_Cos()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> r1 = cos(integer(-1));
    eval_arb(a, *r1, 13);

    mpfr_t f;
    mpfr_init2(f, 17);
    mpfr_set_d(f, cos(-1), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));

    RCP<const Basic> q = div(integer(7), integer(3));
    r1 = mul(cos(integer(-1)), cos(q));  // r1 = cos(-1)*cos(7/3)
    eval_arb(a, *r1, 13);

    mpfr_set_d(f, cos(-1)*cos((double)7/3), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));
}

void test_Tan()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> r1 = tan(integer(1239));
    eval_arb(a, *r1, 10);

    mpfr_t f;
    mpfr_init2(f, 17);
    mpfr_set_d(f, tan(1239), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));

    // r1 = tan(2) + tan(3)*tan(7)
    r1 = add(tan(integer(2)), mul(tan(integer(3)), tan(integer(7))));
    eval_arb(a, *r1, 14);

    mpfr_set_d(f, tan(2) + tan(3)*tan(7), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_Integer();
    test_Rational();
    test_Add();
    test_Mul();
    test_Pow();
    test_Sin();
    test_Cos();
    test_Tan();

    return 0;
}
