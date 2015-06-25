#include "catch.hpp"
#include <symengine/integer.h>
#include <symengine/real_mpfr.h>
#include <symengine/complex_mpc.h>

using SymEngine::print_stack_on_segfault;
using SymEngine::RCP;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Number;
using SymEngine::Rational;
using SymEngine::Complex;
using SymEngine::eq;
using SymEngine::is_a;
#ifdef HAVE_SYMENGINE_MPFR
using SymEngine::mpfr_class;
using SymEngine::real_mpfr;
#endif
#ifdef HAVE_SYMENGINE_MPC
using SymEngine::mpc_class;
using SymEngine::complex_mpc;
using SymEngine::ComplexMPC;
#endif

TEST_CASE("RealMPFR: arithmetic", "[number]")
{
#ifdef HAVE_SYMENGINE_MPFR
    mpfr_class a(100), b(100), d(100), c(100);
    mpfr_set_ui(a.get_mpfr_t(), 10, MPFR_RNDN);
    mpfr_set_ui(b.get_mpfr_t(), 20, MPFR_RNDN);
    mpfr_set_ui(c.get_mpfr_t(), 100, MPFR_RNDN);
    mpfr_set_ui(d.get_mpfr_t(), 1024, MPFR_RNDN);
    RCP<const Number> r1 = real_mpfr(a);
    RCP<const Number> r2 = real_mpfr(b);
    RCP<const Number> r3 = real_mpfr(c);
    RCP<const Number> r4 = real_mpfr(d);
    RCP<const Number> r5 = subnum(integer(0), r1);
    RCP<const Number> i1 = integer(1);
    RCP<const Number> i2 = integer(2);
    RCP<const Number> half = integer(1)->div(*integer(2));
    RCP<const Number> c1 = Complex::from_two_nums(*i1, *i1);

    assert(eq(r2, addnum(r1, r1)));
    assert(eq(r2, mulnum(r1, i2)));
    assert(eq(r2, mulnum(i2, r1)));
    assert(eq(r1, divnum(r2, i2)));
    assert(eq(divnum(i1, r1), divnum(i2, r2)));
    assert(eq(r1, subnum(r2, r1)));
    assert(eq(r1, subnum(integer(20), r1)));
    assert(eq(r1, mulnum(r2, half)));
    assert(eq(r3, pownum(r1, i2)));
    assert(eq(r4, pownum(i2, r1)));
    assert(eq(r1, pownum(r3, half)));
    assert(eq(divnum(i1, r4), half->pow(*r1)));
#ifdef HAVE_SYMENGINE_MPC
    assert(is_a<ComplexMPC>(*addnum(r1, c1)));
    assert(is_a<ComplexMPC>(*mulnum(c1, r2)));
    assert(is_a<ComplexMPC>(*pownum(r5, half)));
    assert(is_a<ComplexMPC>(*pownum(integer(-2), r5)));
#else
    SYMENGINE_CHECK_THROW(addnum(r1, c1), std::runtime_error);
    SYMENGINE_CHECK_THROW(pownum(r5, half), std::runtime_error);
    SYMENGINE_CHECK_THROW(pownum(integer(-2), r1), std::runtime_error);
#endif //HAVE_SYMENGINE_MPC
#endif //HAVE_SYMENGINE_MPFR
}