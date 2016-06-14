#include <symengine/eval.h>

RCP<const Number> eval(const Basic &b, unsigned long bits = 53,
                       bool real = false)
{
    if (bits <= 53 && real) { // double
        double d = eval_double(b);
    } else if (bits <= 53 && !real) { // complex double
        std::complex<double> d = eval_complex_double(b);

    } else if (bits > 53 && real) { // mpfr
#ifdef HAVE_SYMENGINE_MPFR
        mpfr_class mc = mpfr_class(bits);
        mpfr_ptr result = mc.get_mpfr_t();
        eval_mpfr(result, b, MPFR_RNDN);
        return rcp(new RealMPFR(std::move(mc)));
#endif       // HAVE_SYMENGINE_MPFR
    } else { // mpc
#ifdef HAVE_SYMENGINE_MPC
        mpc_class mc = mpc_class(bits);
        mpc_ptr result = mc.get_mpc_t();
        eval_mpc(result, b, MPFR_RNDN);
        return rcp(new RealMPC(std::move(mc)));
#endif // HAVE_SYMENGINE_MPC
    }
}
