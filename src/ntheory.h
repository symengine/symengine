// Following number theoretic functions just make use of GMP's number theoretic
// function implementations. Please refer the following manual:
// https://gmplib.org/manual/Number-Theoretic-Functions.html

#ifndef CSYMPY_NTHEORY_H
#define CSYMPY_NTHEORY_H

#include "integer.h"

namespace CSymPy {

// Prime Functions
int probab_prime_p(const Integer &a, int reps = 25);
RCP<Integer> nextprime(const Integer &a);

// Basic Number-theoretic functions
RCP<Integer> gcd(const Integer &a, const Integer &b);
RCP<Integer> lcm(const Integer &a, const Integer &b);
void gcd_ext(const Integer &a, const Integer &b, const Ptr<RCP<Integer>> &g,
                        const Ptr<RCP<Integer>> &s, const Ptr<RCP<Integer>> &t);

int mod_inverse(const Integer &a, const Integer &m, const Ptr<RCP<Integer>> &b);
bool divides(const RCP<Integer> &a, const RCP<Integer> &b);

// Factorization
int factor(const Ptr<RCP<Integer>> &f, const Integer &n, double B1 = 1.0);
// Factor using trial division. Returns 1 if a non-trivial factor is found,
// otherwise 0.
int factor_trial_division(const Ptr<RCP<Integer>> &f, const Integer &n);

// Returns all primes up to the `limit` (excluding). The vector `primes` should
// be empty on input and it will be filled with the primes.
// The implementation is a very basic Eratosthenes sieve, but the code should
// be quite optimized. For limit=1e8, it is about 50x slower than the
// `primesieve` library (1498ms vs 28.29ms).
void eratosthenes_sieve(unsigned limit, std::vector<unsigned> &primes);
// Find prime factors of `n`
void primefactors(const Integer &n, std::vector<Integer> &primes);

}
#endif

