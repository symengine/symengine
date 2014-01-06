#include "ntheory.h"

namespace CSymPy {

RCP<Integer> gcd(const Integer &a, const Integer &b)
{
    mpz_class g;

    mpz_gcd(g.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());

    return integer(g);
}

void gcd_ext(const Integer &a, const Integer &b, const Ptr<RCP<Integer>> &g,
                         const Ptr<RCP<Integer>> &s, const Ptr<RCP<Integer>> &t)
{
    mpz_t _g;
    mpz_t _s;
    mpz_t _t;
    
    mpz_init(_g);
    mpz_init(_s);
    mpz_init(_t);
    
    mpz_gcdext(_g, _s, _t, a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());
    *g = integer(mpz_class(_g));
    *s = integer(mpz_class(_s));
    *t = integer(mpz_class(_t));
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
    mpz_t inv;
    mpz_init(inv);
    
    ret_val = mpz_invert(inv, a.as_mpz().get_mpz_t(), m.as_mpz().get_mpz_t());
    *b = integer(mpz_class(inv));
    
    return ret_val;
}

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

} // CSymPy
