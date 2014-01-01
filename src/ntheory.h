// Following number theoretic functions just make use of GMP's number theoretic
// function implementations. Please refer the following manual:
// https://gmplib.org/manual/Number-Theoretic-Functions.html

#ifndef CSYMPY_NTHEORY_H
#define CSYMPY_NTHEORY_H

#include <gmpxx.h>

#include "number.h"
#include "integer.h"

namespace CSymPy {

inline RCP<Integer> gcd(const Integer &a, const Integer &b)
{
    mpz_class g;
    
    mpz_gcd(g.get_mpz_t(), static_cast<Integer>(a).as_mpz().get_mpz_t(), 
                                  static_cast<Integer>(b).as_mpz().get_mpz_t());

    return rcp(new Integer(g));
}

inline RCP<Integer> lcm(const Integer &a, const Integer &b)
{
    mpz_class c;

    mpz_lcm(c.get_mpz_t(), static_cast<Integer>(a).as_mpz().get_mpz_t(),
                                 static_cast<Integer>(b).as_mpz().get_mpz_t());

    return rcp(new Integer(c));
}

inline RCP<Integer> probab_prime_p(const Integer &a, int reps = 25)
{
    return rcp(new Integer(mpz_probab_prime_p(
                         static_cast<Integer>(a).as_mpz().get_mpz_t(), reps)));
}

inline RCP<Integer> nextprime(const Integer &a)
{
    mpz_class c;

    mpz_nextprime(c.get_mpz_t(), static_cast<Integer>(a).as_mpz().get_mpz_t());

    return rcp(new Integer(c));
}

}
#endif
