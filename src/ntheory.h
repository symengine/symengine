#ifndef CSYMPY_NTHEORY_H
#define CSYMPY_NTHEORY_H

#include <gmpxx.h>

#include "basic.h"
#include "number.h"
#include "integer.h"

namespace CSymPy {

inline RCP<Integer> gcd(Integer a, Integer b)
{
    mpz_class c;

    mpz_gcd(c.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());
    return rcp(new Integer(c));
}

}
#endif
