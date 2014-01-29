#include "integer.h"
#include "ntheory.h"
#include "add.h"
#include "mul.h"

#ifdef HAVE_CSYMPY_ECM
#  include <ecm.h>
#endif // HAVE_CSYMPY_ECM

namespace CSymPy {

// Basic number theoretic functions
RCP<Integer> gcd(const Integer &a, const Integer &b)
{
    mpz_class g;

    mpz_gcd(g.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());

    return integer(g);
}

void gcd_ext(const Integer &a, const Integer &b, const Ptr<RCP<Integer>> &g,
                         const Ptr<RCP<Integer>> &s, const Ptr<RCP<Integer>> &t)
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

RCP<Integer> lcm(const Integer &a, const Integer &b)
{
    mpz_class c;

    mpz_lcm(c.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());

    return integer(c);
}

int mod_inverse(const Integer &a, const Integer &m, const Ptr<RCP<Integer>> &b)
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

RCP<Integer> nextprime(const Integer &a)
{
    mpz_class c;

    mpz_nextprime(c.get_mpz_t(), a.as_mpz().get_mpz_t());

    return integer(c);
}

// Factorization
int factor(const Ptr<RCP<Integer>> &f, const Integer &n, double B1)
{
    int ret_val = 0;
    mpz_t n_t, f_t;;

    mpz_init(n_t); mpz_init(f_t);
    mpz_set(n_t, n.as_mpz().get_mpz_t());
        
#ifdef HAVE_CSYMPY_ECM       
    ret_val = ecm_factor(f_t, n_t, B1, NULL);  
#else
    // B1 is discarded if gmp-ecm is not installed
    ret_val = _factor_trial_division(f_t, n_t);
#endif // HAVE_CSYMPY_ECM
    *f = integer(mpz_class(f_t));
    
    mpz_clear(n_t); mpz_clear(f_t);  
    
    return ret_val;
}

// Factoring by Trial division: should not be used, helper function for factor
int _factor_trial_division(mpz_t rop, const mpz_t op)
{
    mpz_t i, limit, q, r;
    
    mpz_init(i); mpz_init(limit); mpz_init(q); mpz_init(r);
    
    mpz_set_ui(i, 2);
    mpz_sqrt(limit, op);
    
    while(mpz_cmp(i, limit) <= 0)
    {
        mpz_tdiv_qr(q, r, op, i);
        
        if (mpz_cmp_ui(r, 0) == 0) {
            mpz_set(rop, i);
            break;
        }
        mpz_add_ui(i, i, 1);
    }
    // 'op' is prime
    if (mpz_cmp(i, limit) > 0)
        mpz_set(rop, op);

    mpz_clear(i); mpz_clear(limit); mpz_clear(q); mpz_clear(r);
    
    return 1; // Return 1 since this method always find a factor
}

} // CSymPy
