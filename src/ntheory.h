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
    mpz_gcd(c.get_mpz_t(), static_cast<const mpz_class>(a).get_mpz_t(), static_cast<const mpz_class>(b).get_mpz_t());
    return rcp(new Integer(c));
}

}
