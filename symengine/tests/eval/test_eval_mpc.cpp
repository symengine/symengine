#include "catch.hpp"
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/eval_mpc.h>
#include <symengine/eval_mpfr.h>
#include <symengine/constants.h>
#include <symengine/functions.h>

using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::integer;
using SymEngine::pi;
using SymEngine::I;
using SymEngine::E;
using SymEngine::EulerGamma;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::one;
using SymEngine::sub;
using SymEngine::sin;
using SymEngine::cos;
using SymEngine::eval_mpc;
using SymEngine::print_stack_on_segfault;

TEST_CASE("eval: eval_mpc", "[eval_mpc]")
{
    mpc_t a, b;
    mpfr_t real, imag;
    mpc_init2(a, 100);
    mpc_init2(b, 100);
    mpfr_init2(real, 100);
    mpfr_init2(imag, 100);
    RCP<const Basic> s = add(one, cos(integer(2)));
    RCP<const Basic> t = sin(integer(2));
    RCP<const Basic> r = add(pow(E, mul(integer(2), I)), one);

    eval_mpc(a, *r, MPFR_RNDN);
    eval_mpfr(real, *s, MPFR_RNDN);
    eval_mpfr(imag, *t, MPFR_RNDN);

    mpc_set_fr_fr(b, real, imag, MPFR_RNDN);

    REQUIRE(mpc_cmp(a, b) == 0);

    r = add(one, mul(EulerGamma, I));
    s = one;
    t = EulerGamma;

    eval_mpc(a, *r, MPFR_RNDN);
    eval_mpfr(real, *s, MPFR_RNDN);
    eval_mpfr(imag, *t, MPFR_RNDN);

    mpc_set_fr_fr(b, real, imag, MPFR_RNDN);

    REQUIRE(mpc_cmp(a, b) == 0);

    r = add(one, mul(E, I));
    s = one;
    t = E;

    eval_mpc(a, *r, MPFR_RNDN);
    eval_mpfr(real, *s, MPFR_RNDN);
    eval_mpfr(imag, *t, MPFR_RNDN);

    mpc_set_fr_fr(b, real, imag, MPFR_RNDN);

    REQUIRE(mpc_cmp(a, b) == 0);

    mpfr_clear(real);
    mpfr_clear(imag);
    mpc_clear(a);
    mpc_clear(b);
}
