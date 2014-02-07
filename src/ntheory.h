// Following number theoretic functions just make use of GMP's number theoretic
// function implementations. Please refer the following manual:
// https://gmplib.org/manual/Number-Theoretic-Functions.html

#ifndef CSYMPY_NTHEORY_H
#define CSYMPY_NTHEORY_H

#include "integer.h"

namespace CSymPy {

// Prime Functions
int probab_prime_p(const Integer &a, int reps = 25);
RCP<const Integer> nextprime(const Integer &a);

// Basic Number-theoretic functions
RCP<const Integer> gcd(const Integer &a, const Integer &b);
RCP<const Integer> lcm(const Integer &a, const Integer &b);
void gcd_ext(const Integer &a, const Integer &b, const Ptr<RCP<const Integer>> &g,
                        const Ptr<RCP<const Integer>> &s, const Ptr<RCP<const Integer>> &t);

int mod_inverse(const Integer &a, const Integer &m, const Ptr<RCP<const Integer>> &b);

// Factorization 
// parameter B1 is only used when `n` is factored using gmp-ecm
int factor(const Ptr<RCP<const Integer>> &f, const Integer &n, double B1 = 1.0);
// Factor using trial division. Returns 1 if a non-trivial factor is found,
// otherwise 0.
int factor_trial_division(const Ptr<RCP<const Integer>> &f, const Integer &n);

// Returns all primes up to the `limit` (excluding). The vector `primes` should
// be empty on input and it will be filled with the primes.
// The implementation is a very basic Eratosthenes sieve, but the code should
// be quite optimized. For limit=1e8, it is about 50x slower than the
// `primesieve` library (1498ms vs 28.29ms).
void eratosthenes_sieve(unsigned limit, std::vector<unsigned> &primes);

}
#endif
