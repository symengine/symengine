/**
 *  \file ntheory.h
 *  Basic number theory functions
 *
 **/

#ifndef SYMENGINE_NTHEORY_H
#define SYMENGINE_NTHEORY_H

#include <symengine/integer.h>

namespace SymEngine
{

// Prime Functions
//! Probabilistic Prime
SYMENGINE_EXPORT int probab_prime_p(const Integer &a, unsigned reps = 25);
//! \return next prime after `a`
SYMENGINE_EXPORT RCP<const Integer> nextprime(const Integer &a);

// Basic Number-theoretic functions
//! Greatest Common Divisor
SYMENGINE_EXPORT RCP<const Integer> gcd(const Integer &a, const Integer &b);
//! Least Common Multiple
SYMENGINE_EXPORT RCP<const Integer> lcm(const Integer &a, const Integer &b);
//! Extended GCD
SYMENGINE_EXPORT
void gcd_ext(const Ptr<RCP<const Integer>> &g, const Ptr<RCP<const Integer>> &s,
             const Ptr<RCP<const Integer>> &t, const Integer &a,
             const Integer &b);
//! modulo round toward zero
SYMENGINE_EXPORT RCP<const Integer> mod(const Integer &n, const Integer &d);
//! \return quotient round toward zero when `n` is divided by `d`
SYMENGINE_EXPORT RCP<const Integer> quotient(const Integer &n,
                                             const Integer &d);
//! \return modulo and quotient round toward zero
SYMENGINE_EXPORT
void quotient_mod(const Ptr<RCP<const Integer>> &q,
                  const Ptr<RCP<const Integer>> &r, const Integer &a,
                  const Integer &b);
//! modulo round toward -inf
SYMENGINE_EXPORT RCP<const Integer> mod_f(const Integer &n, const Integer &d);
//! \return quotient round toward -inf when `n` is divided by `d`
SYMENGINE_EXPORT RCP<const Integer> quotient_f(const Integer &n,
                                               const Integer &d);
//! \return modulo and quotient round toward -inf
SYMENGINE_EXPORT
void quotient_mod_f(const Ptr<RCP<const Integer>> &q,
                    const Ptr<RCP<const Integer>> &r, const Integer &a,
                    const Integer &b);
//! inverse modulo
SYMENGINE_EXPORT
int mod_inverse(const Ptr<RCP<const Integer>> &b, const Integer &a,
                const Integer &m);

//! Chinese remainder function. Return true when a solution exists.
SYMENGINE_EXPORT
bool crt(const Ptr<RCP<const Integer>> &R,
         const std::vector<RCP<const Integer>> &rem,
         const std::vector<RCP<const Integer>> &mod);

//! Fibonacci number
SYMENGINE_EXPORT RCP<const Integer> fibonacci(unsigned long n);

//! Fibonacci n and n-1
SYMENGINE_EXPORT
void fibonacci2(const Ptr<RCP<const Integer>> &g,
                const Ptr<RCP<const Integer>> &s, unsigned long n);

//! Lucas number
SYMENGINE_EXPORT RCP<const Integer> lucas(unsigned long n);

//! Lucas number n and n-1
SYMENGINE_EXPORT
void lucas2(const Ptr<RCP<const Integer>> &g, const Ptr<RCP<const Integer>> &s,
            unsigned long n);

//! Binomial Coefficient
SYMENGINE_EXPORT RCP<const Integer> binomial(const Integer &n, unsigned long k);

//! Factorial
SYMENGINE_EXPORT RCP<const Integer> factorial(unsigned long n);

//! \return true if `b` divides `a`
SYMENGINE_EXPORT bool divides(const Integer &a, const Integer &b);

//! Factorization
//! \param B1 is only used when `n` is factored using gmp-ecm
SYMENGINE_EXPORT int factor(const Ptr<RCP<const Integer>> &f, const Integer &n,
                            double B1 = 1.0);

//! Factor using trial division.
//! \return 1 if a non-trivial factor is found, otherwise 0.
SYMENGINE_EXPORT int factor_trial_division(const Ptr<RCP<const Integer>> &f,
                                           const Integer &n);

//! Factor using lehman's methods
SYMENGINE_EXPORT int factor_lehman_method(const Ptr<RCP<const Integer>> &f,
                                          const Integer &n);

//! Factor using Pollard's p-1 method
SYMENGINE_EXPORT int factor_pollard_pm1_method(const Ptr<RCP<const Integer>> &f,
                                               const Integer &n,
                                               unsigned B = 10,
                                               unsigned retries = 5);

//! Factor using Pollard's rho methods
SYMENGINE_EXPORT int factor_pollard_rho_method(const Ptr<RCP<const Integer>> &f,
                                               const Integer &n,
                                               unsigned retries = 5);

//! Find prime factors of `n`
SYMENGINE_EXPORT void prime_factors(std::vector<RCP<const Integer>> &primes,
                                    const Integer &n);
//! Find multiplicities of prime factors of `n`
SYMENGINE_EXPORT void prime_factor_multiplicities(map_integer_uint &primes,
                                                  const Integer &n);

//! Computes the Bernoulli number Bn as an exact fraction, for an isolated
//! integer n
SYMENGINE_EXPORT RCP<const Number> bernoulli(unsigned long n);
//! Computes the sum of the inverses of the first perfect mth powers
SYMENGINE_EXPORT RCP<const Number> harmonic(unsigned long n, long m = 1);
//! Computes a primitive root. Returns false if no primitive root exists.
// Primitive root calculated is the smallest when n is prime.
SYMENGINE_EXPORT bool primitive_root(const Ptr<RCP<const Integer>> &g,
                                     const Integer &n);
//! Computes all primitive roots less than n. Returns false if no primitive root
//! exists.
SYMENGINE_EXPORT void
primitive_root_list(std::vector<RCP<const Integer>> &roots, const Integer &n);
//! Euler's totient function
SYMENGINE_EXPORT RCP<const Integer> totient(const RCP<const Integer> &n);
//! Carmichael function
SYMENGINE_EXPORT
RCP<const Integer> carmichael(const RCP<const Integer> &n);
//! Multiplicative order. Return false if order does not exist
SYMENGINE_EXPORT
bool multiplicative_order(const Ptr<RCP<const Integer>> &o,
                          const RCP<const Integer> &a,
                          const RCP<const Integer> &n);
//! Legendre Function
SYMENGINE_EXPORT int legendre(const Integer &a, const Integer &n);
//! Jacobi Function
SYMENGINE_EXPORT int jacobi(const Integer &a, const Integer &n);
//! Kronecker Function
SYMENGINE_EXPORT int kronecker(const Integer &a, const Integer &n);
//! All Solutions to x**n == a mod m. Return false if none exists.
SYMENGINE_EXPORT
void nthroot_mod_list(std::vector<RCP<const Integer>> &roots,
                      const RCP<const Integer> &a, const RCP<const Integer> &n,
                      const RCP<const Integer> &m);
//! A solution to x**n == a mod m. Return false if none exists.
SYMENGINE_EXPORT
bool nthroot_mod(const Ptr<RCP<const Integer>> &root,
                 const RCP<const Integer> &a, const RCP<const Integer> &n,
                 const RCP<const Integer> &m);
//! A solution to x**s == a**r mod m where b = r / s. Return false if none
//! exists.
SYMENGINE_EXPORT
bool powermod(const Ptr<RCP<const Integer>> &powm, const RCP<const Integer> &a,
              const RCP<const Number> &b, const RCP<const Integer> &m);
//! All solutions to x**s == a**r mod m where b = r / s. Return false if none
//! exists.
SYMENGINE_EXPORT
void powermod_list(std::vector<RCP<const Integer>> &pows,
                   const RCP<const Integer> &a, const RCP<const Number> &b,
                   const RCP<const Integer> &m);

//! Finds all Quadratic Residues of a Positive Integer
SYMENGINE_EXPORT vec_integer_class quadratic_residues(const Integer &a);

//! Returns true if 'a' is a quadratic residue of 'p'
SYMENGINE_EXPORT bool is_quad_residue(const Integer &a, const Integer &p);
//! Returns true if 'a' is a nth power residue of 'mod'
SYMENGINE_EXPORT bool is_nth_residue(const Integer &a, const Integer &n,
                                     const Integer &mod);
//! Mobius Function
// mu(n) = 1 if n is a square-free positive integer with an even number of prime
// factors
// mu(n) = −1 if n is a square-free positive integer with an odd number of prime
// factors
// mu(n) = 0 if n has a squared prime factor
SYMENGINE_EXPORT int mobius(const Integer &a);
// Mertens Function
// mertens(n) -> Sum of mobius(i) for i from 1 to n
SYMENGINE_EXPORT long mertens(const unsigned long a);

SYMENGINE_EXPORT
integer_class mp_polygonal_number(const integer_class &s,
                                  const integer_class &n);
SYMENGINE_EXPORT
integer_class mp_principal_polygonal_root(const integer_class &s,
                                          const integer_class &x);

/**
 * @brief Decompose a positive integer into perfect powers
 * @param n Integer to decompose
 * @param lowest_exponent Can be set to find the perfect power with the
 *        lowest exponent. Default is to find the highest exponent.
 * @returns The base and exponent as a pair of integers.
 *          (n, 1) if no perfect power exists.
 *
 * See https://en.wikipedia.org/wiki/Perfect_power
 */
SYMENGINE_EXPORT
std::pair<integer_class, integer_class>
mp_perfect_power_decomposition(const integer_class &n,
                               bool lowest_exponent = false);

} // namespace SymEngine

#endif
