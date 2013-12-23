#ifndef CSYMPY_NTHEORY_H
#define CSYMPY_NTHEORY_H

#include <gmpxx.h>
#include <initializer_list>

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

inline RCP<Integer> nextprime(const Integer &a)
{
    mpz_class c;

    mpz_nextprime(c.get_mpz_t(), static_cast<Integer>(a).as_mpz().get_mpz_t());

    return rcp(new Integer(c));
}

}
#endif
