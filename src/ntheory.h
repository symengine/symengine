// Following number theoretic functions just make use of GMP's number theoretic
// function implementations. Please refer the following manual:
// https://gmplib.org/manual/Number-Theoretic-Functions.html

#ifndef CSYMPY_NTHEORY_H
#define CSYMPY_NTHEORY_H

#include "integer.h"

namespace CSymPy {

RCP<Integer> gcd(const Integer &a, const Integer &b);
RCP<Integer> lcm(const Integer &a, const Integer &b);
int probab_prime_p(const Integer &a, int reps = 25);
RCP<Integer> nextprime(const Integer &a);

}
#endif
