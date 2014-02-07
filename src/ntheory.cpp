#include <cmath>

#include "ntheory.h"

#ifdef HAVE_CSYMPY_ECM
#  include <ecm.h>
#endif // HAVE_CSYMPY_ECM

namespace CSymPy {

// Basic number theoretic functions
RCP<const Integer> gcd(const Integer &a, const Integer &b)
{
    mpz_class g;

    mpz_gcd(g.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());

    return integer(g);
}

void gcd_ext(const Integer &a, const Integer &b, const Ptr<RCP<const Integer>> &g,
                         const Ptr<RCP<const Integer>> &s, const Ptr<RCP<const Integer>> &t)
{
    mpz_t g_t;
    mpz_t s_t;
    mpz_t t_t;

    mpz_init(g_t);
    mpz_init(s_t);
    mpz_init(t_t);

    mpz_gcdext(g_t, s_t, t_t, a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());
    *g = integer(mpz_class(g_t));
    *s = integer(mpz_class(s_t));
    *t = integer(mpz_class(t_t));

    mpz_clear(g_t);
    mpz_clear(s_t);
    mpz_clear(t_t);
}

RCP<const Integer> lcm(const Integer &a, const Integer &b)
{
    mpz_class c;

    mpz_lcm(c.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());

    return integer(c);
}

int mod_inverse(const Integer &a, const Integer &m, const Ptr<RCP<const Integer>> &b)
{
    int ret_val;
    mpz_t inv_t;

    mpz_init(inv_t);

    ret_val = mpz_invert(inv_t, a.as_mpz().get_mpz_t(), m.as_mpz().get_mpz_t());
    *b = integer(mpz_class(inv_t));

    mpz_clear(inv_t);

    return ret_val;
}

// Prime functions
int probab_prime_p(const Integer &a, int reps)
{
    return mpz_probab_prime_p(a.as_mpz().get_mpz_t(), reps);
}

RCP<const Integer> nextprime(const Integer &a)
{
    mpz_class c;

    mpz_nextprime(c.get_mpz_t(), a.as_mpz().get_mpz_t());

    return integer(c);
}

// Factoring by Trial division: should not be used, helper function for factor
// Look for factors of `op` below `limit`
int _factor_trial_division(mpz_t rop, const mpz_t op, const mpz_t limit)
{
    int ret_val;
    mpz_t i, l, q, r;

    mpz_init(i);
    mpz_init(l);
    mpz_init(q);
    mpz_init(r);
//    mpz_inits(i, l, q, r);

    mpz_set_ui(i, 2);
    mpz_sqrt(l, op);

    if (mpz_cmp(limit, l) < 0)
        mpz_set(l, limit);

    while(mpz_cmp(i, l) <= 0)
    {
        mpz_tdiv_qr(q, r, op, i);

        if (mpz_cmp_ui(r, 0) == 0) {
            mpz_set(rop, i);
            break;
        }
        mpz_add_ui(i, i, 1);
    }

    mpz_clear(q);
    mpz_clear(r);
//    mpz_clears(q, r);

    if (mpz_cmp(i, l) <= 0)      // We found a factor
        ret_val = 1;
    else
        ret_val = 0;

    mpz_clear(i);
    mpz_clear(l);
//    mpz_clears(i, l);

    return ret_val;
}

// Factoring by Trial division using primes only
int _factor_trial_division_sieve(mpz_class &factor, const mpz_class &N)
{
    mpz_class sqrtN;
    sqrtN = sqrt(N);
    if (!(sqrtN.fits_uint_p()))
        throw std::runtime_error("N too large to factor");
    unsigned limit = sqrtN.get_ui()+1;
    std::vector<unsigned> primes;
    eratosthenes_sieve(limit, primes);
    for (auto &p: primes)
        if (N % p == 0) {
            factor = p;
            return 1;
        }
    return 0;
}

// Factoring by Lehman method, shouldn't be used directly, helper function for
// factor
int _factor_lehman_method(mpz_t rop, const mpz_t n)
{
    int ret_val;
    mpz_t u_bound;
    mpz_init(u_bound);

    mpz_root(u_bound, n, 3);
    mpz_add_ui(u_bound, u_bound, 1);

    ret_val = _factor_trial_division(rop, n, u_bound);

    if (!ret_val){

        mpz_t k, a, b, l, kn;
        mpz_init(k);
        mpz_init(a);
        mpz_init(b);
        mpz_init(l);
        mpz_init(kn);
//        mpz_inits(k, a, b, l, kn);

        mpz_set_ui(k, 1);

        mpf_t t;
        mpf_init(t);

        mpz_set_f(a, t);

        while (mpz_cmp(k, u_bound) <= 0) {

            mpz_mul(kn, k, n);
            mpf_set_z(t, kn);
            mpf_sqrt(t, t);
            mpf_mul_ui(t, t, 2);
            mpz_set_f(a, t);

            // below calculations should be done with mpfr library as gmp
            //doesn't provide floating point root functions
            mpz_root(b, n, 6);
            mpz_root(l, k, 4);
            mpz_tdiv_q(b, b, l);
            mpz_add(b, b, a);

            mpz_mul_ui(kn, kn, 4);

            while (mpz_cmp(a, b) <= 0) {
                mpz_mul(l, a, a);
                mpz_sub(l, l, kn);

                if (mpz_perfect_square_p(l)){
                    mpz_add(a, a, b);
                    mpz_gcd(rop, n, a);
                    ret_val = 1;
                    break;
                }
                mpz_add_ui(a, a, 1);
            }

            if (ret_val){
                mpz_clear(k);
                mpz_clear(a);
                mpz_clear(b);
                mpz_clear(l);
                mpz_clear(kn);
//                mpz_clears(k, a, b, l, kn);

                mpf_clear(t);

                break;
            }
            mpz_add_ui(k, k, 1);
        }
    }

    return ret_val;
}

// Factorization
int factor(const Ptr<RCP<const Integer>> &f, const Integer &n, double B1)
{
    int ret_val = 0;
    mpz_t n_t, f_t;;

    mpz_init(n_t); mpz_init(f_t);
    mpz_set(n_t, n.as_mpz().get_mpz_t());

#ifdef HAVE_CSYMPY_ECM
    if (mpz_perfect_power_p(n_t)) {

        unsigned long int i = 1;
        mpz_t m, rem;
        mpz_init_set_ui(rem, 1); // Any non zero number
        mpz_init_set_ui(m, 2); // set `m` to 2**i, i = 1 at the begining

        // calculate log2n, this can be improved
        for (; mpz_cmp(m, n_t) < 0; i++)
            mpz_mul_ui(m, m, 2);

        // eventually `rem` = 0 zero as `n` is a perfect square. `f_t` will
        // be set to a factor of `n` when that happens
        while(i > 1 && mpz_cmp_ui(rem, 0)) {
            mpz_rootrem(f_t, rem, n_t, i);
            i--;
        }

        ret_val = 1;
    }
    else {

        if (mpz_probab_prime_p(n_t, 25) > 0) { // most probably, n is a prime
            ret_val = 0;
            mpz_set(f_t, n_t);
        }
        else {

            for (int i = 0; i < 10 && !ret_val; i++)
                ret_val = ecm_factor(f_t, n_t, B1, NULL);

            if (!ret_val)
                throw std::runtime_error("ECM failed to factor the given number");
        }
    }
#else
    // B1 is discarded if gmp-ecm is not installed
    if (mpz_cmp_ui(n_t, 21) <= 0)
        ret_val = _factor_trial_division(f_t, n_t, n_t);
    else
        ret_val = _factor_lehman_method(f_t, n_t);
#endif // HAVE_CSYMPY_ECM
    *f = integer(mpz_class(f_t));

    mpz_clear(n_t);
    mpz_clear(f_t);
//    mpz_clears(n_t, f_t);

    return ret_val;
}

int factor_trial_division(const Ptr<RCP<const Integer>> &f, const Integer &n)
{
    int ret_val;
    mpz_class factor;
    ret_val =_factor_trial_division_sieve(factor, n.as_mpz());
    if (ret_val == 1) *f = integer(factor);
    return ret_val;
}

void eratosthenes_sieve(unsigned limit, std::vector<unsigned> &primes)
{
    std::vector<bool> is_prime(limit, true);
    const unsigned sqrt_limit = static_cast<unsigned>(std::sqrt(limit));
    for (unsigned n = 2; n <= sqrt_limit; ++n)
        if (is_prime[n]) {
            primes.push_back(n);
            for (unsigned k = n*n, ulim = limit; k < ulim; k += n)
                is_prime[k] = false;
        }
    for (unsigned n = sqrt_limit + 1; n < limit; ++n)
        if (is_prime[n])
            primes.push_back(n);
}

} // CSymPy
