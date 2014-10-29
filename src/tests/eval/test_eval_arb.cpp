#include <limits>
#include <cstdlib>
#include <ctime>

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
    mpfr_init2(f, 25);
    mpfr_set_q(f, rcp_static_cast<const Rational>(q)->i.get_mpq_t(), MPFR_RNDN);

    eval_arb(a, *q, 20);

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
    eval_arb(a, *q, 20);

    mpfr_set_q(f, rcp_static_cast<const Rational>(q)->i.get_mpq_t(), MPFR_RNDN);

    assert(arb_contains_mpfr(a, f));
}

void test_Add()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> q = div(integer(223), integer(71));
    RCP<const Basic> r1 = add(sqrt(integer(3)), q);
    eval_arb(a, *r1, 30);

    mpfr_t f1, f2;
    mpfr_init2(f1, 35);
    mpfr_init2(f2, 35);
    mpfr_sqrt_ui(f1, 3, MPFR_RNDN);
    mpfr_set_q(f2, rcp_static_cast<const Rational>(q)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_add(f2, f2, f1, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f2));

    q = div(integer(34), integer(7100));
    r1 = add(sqrt(integer(7)), q);
    eval_arb(a, *r1, 30);

    mpfr_sqrt_ui(f1, 7, MPFR_RNDN);
    mpfr_set_q(f2, rcp_static_cast<const Rational>(q)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_add(f2, f2, f1, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f2));
}

void test_Mul()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> q = div(integer(377), integer(120));
    RCP<const Basic> r1 = mul(sqrt(integer(15)), q);
    eval_arb(a, *r1, 35);

    mpfr_t f1, f2;
    mpfr_init2(f1, 40);
    mpfr_init2(f2, 40);
    mpfr_sqrt_ui(f1, 15, MPFR_RNDN);
    mpfr_set_q(f2, rcp_static_cast<const Rational>(q)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_mul(f2, f2, f1, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f2));

    q = div(integer(9001), integer(71));
    r1 = mul(sqrt(integer(2317)), q);
    eval_arb(a, *r1, 30);

    mpfr_sqrt_ui(f1, 2317, MPFR_RNDN);
    mpfr_set_q(f2, rcp_static_cast<const Rational>(q)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_mul(f2, f2, f1, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f2));
}

void test_Pow()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> b = div(integer(377), integer(120));
    RCP<const Basic> e = div(integer(34), integer(1257));
    RCP<const Basic> r1 = pow(b, e);
    eval_arb(a, *r1, 45);

    mpfr_t f1, f2;
    mpfr_init2(f1, 50);
    mpfr_init2(f2, 50);
    mpfr_set_q(f1, rcp_static_cast<const Rational>(b)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_set_q(f2, rcp_static_cast<const Rational>(e)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_pow(f2, f1, f2, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f2));

    b = div(integer(120), integer(377));
    e = div(integer(97), integer(34));
    r1 = pow(b, e);
    eval_arb(a, *r1, 45);

    mpfr_set_q(f1, rcp_static_cast<const Rational>(b)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_set_q(f2, rcp_static_cast<const Rational>(e)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_pow(f2, f1, f2, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f2));
}

void test_Sin()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> r1 = sin(integer(2));
    eval_arb(a, *r1, 45);

    mpfr_t f1;
    mpfr_init2(f1, 50);
    mpfr_set_ui(f1, 2, MPFR_RNDN);
    mpfr_sin(f1, f1, MPFR_RNDN);    // f1 = sin(2)

    assert(arb_contains_mpfr(a, f1));

    RCP<const Basic> q = div(integer(3), integer(5));
    r1 = add(sin(integer(2)), sin(q));  // r1 = sin(2) + sin(3/5)
    eval_arb(a, *r1, 45);

    mpfr_t f2;
    mpfr_init2(f2, 50);
    mpfr_set_ui(f1, 2, MPFR_RNDN);
    mpfr_sin(f1, f1, MPFR_RNDN);    // f1 = sin(2)
    mpfr_set_q(f2, rcp_static_cast<const Rational>(q)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_sin(f2, f2, MPFR_RNDN);    // f2 = sin(3/5)
    mpfr_add(f2, f2, f1, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f2));
}

void test_Cos()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> r1 = cos(integer(-1));
    eval_arb(a, *r1, 65);

    mpfr_t f1;
    mpfr_init2(f1, 70);
    mpfr_set_si(f1, -1, MPFR_RNDN);
    mpfr_cos(f1, f1, MPFR_RNDN);    // f1 = cos(-1)

    assert(arb_contains_mpfr(a, f1));

    RCP<const Basic> q = div(integer(7), integer(3));
    r1 = mul(cos(integer(-1)), cos(q));  // r1 = cos(-1)*cos(7/3)
    eval_arb(a, *r1, 65);

    mpfr_t f2;
    mpfr_init2(f2, 70);
    mpfr_set_si(f1, -1, MPFR_RNDN);
    mpfr_cos(f1, f1, MPFR_RNDN);    // f1 = cos(-1)
    mpfr_set_q(f2, rcp_static_cast<const Rational>(q)->i.get_mpq_t(), MPFR_RNDN);
    mpfr_cos(f2, f2, MPFR_RNDN);    // f2 = cos(7/3)
    mpfr_mul(f2, f2, f1, MPFR_RNDN);    // f2 = cos(-1)*cos(7/3)

    assert(arb_contains_mpfr(a, f2));
}

void test_Tan()
{
    arb_t a;
    arb_init(a);

    RCP<const Basic> r1 = tan(integer(1239));
    eval_arb(a, *r1, 145);

    mpfr_t f1;
    mpfr_init2(f1, 150);
    mpfr_set_ui(f1, 1239, MPFR_RNDN);
    mpfr_tan(f1, f1, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f1));

    r1 = add(tan(integer(2)), mul(tan(integer(3)), tan(integer(7))));  // r1 = tan(2) + tan(3)*tan(7)
    eval_arb(a, *r1, 45);

    mpfr_t f2;
    mpfr_init2(f2, 150);
    mpfr_set_ui(f1, 3, MPFR_RNDN);
    mpfr_tan(f1, f1, MPFR_RNDN);    // f1 = tan(3)
    mpfr_set_ui(f2, 7, MPFR_RNDN);
    mpfr_tan(f2, f2, MPFR_RNDN);    // f2 = tan(7)
    mpfr_mul(f2, f2, f1, MPFR_RNDN);    // f2 = tan(3)*tan(7)
    mpfr_set_ui(f1, 2, MPFR_RNDN);
    mpfr_tan(f1, f1, MPFR_RNDN);    // f1 = tan(2)
    mpfr_add(f2, f2, f1, MPFR_RNDN);

    assert(arb_contains_mpfr(a, f2));
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
