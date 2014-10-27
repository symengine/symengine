#include "eval.h"
#include "pow.h"
#include "rational.h"
#include "integer.h"
#include "basic.h"
#include "complex.h"

#ifdef HAVE_CSYMPY_ARB
#  include "arb.h"
#endif // HAVE_CSYMPY_ARB

namespace CSymPy {

inline void mpz_to_arb(arb_t &a, const mpz_t z)
{
    fmpz_t z_;
    fmpz_init(z_);
    fmpz_set_mpz(z_, z);
    arb_set_fmpz(a, z_);
    fmpz_clear(z_);
}

inline void mpq_to_arb(arb_t &a, const mpq_t q, long prec)
{
    fmpq_t q_;
    fmpq_init(q_);
    fmpq_set_mpq(q_, q);
    arb_set_fmpq(a, q_, prec);
    fmpq_clear(q_);
}

RCP<const Basic> eval_pow(const RCP<const Basic> &x, long prec)
{
    RCP<const Pow> x_ = rcp_static_cast<const Pow>(x);
    arb_t e, b, y;
    arb_init(e);
    arb_init(b);
    arb_init(y);

    RCP<const Basic> base = eval(x_->get_base(), prec);
    if (is_a<Rational>(*base)) {
        mpq_to_arb(b, rcp_static_cast<const Rational>(base)->i.get_mpq_t(), prec);
    } else if (is_a<Integer>(*base)) {
        mpz_to_arb(b, rcp_static_cast<const Integer>(base)->i.get_mpz_t());
    }

    RCP<const Basic> exp = eval(x_->get_exp(), prec);
    if (is_a<Rational>(*exp)) {
        mpq_to_arb(e, rcp_static_cast<const Rational>(exp)->i.get_mpq_t(), prec);
    }

    arb_pow(y, b, e, prec);
}

RCP<const Basic> eval(const RCP<const Basic> &x, long prec)
{
    if (is_a_Number(*x)) {
        return x;
    } else if (is_a<Pow>(*x)) {
        return eval_pow(x, prec);
    }

    return x;
}

} // CSymPy
