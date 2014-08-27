/**
 *  \file ntheory.h
 *  Basic Ntheory Functions
 *
 **/
// Following number theoretic functions just make use of GMP's number theoretic
// function implementations. Please refer the following manual:
// https://gmplib.org/manual/Number-Theoretic-Functions.html

#ifndef CSYMPY_NTHEORY_H
#define CSYMPY_NTHEORY_H

#include "integer.h"

namespace CSymPy {

// Prime Functions
//! Probabilistic Prime
int probab_prime_p(const Integer &a, int reps = 25);
//! \return next prime after `a`
RCP<const Integer> nextprime(const Integer &a);

// Basic Number-theoretic functions
//! Greatest Common Divisor
RCP<const Integer> gcd(const Integer &a, const Integer &b);
//! Least Common Multiple
RCP<const Integer> lcm(const Integer &a, const Integer &b);
//! Extended GCD
void gcd_ext(const Ptr<RCP<const Integer>> &g, const Ptr<RCP<const Integer>> &s,
        const Ptr<RCP<const Integer>> &t, const Integer &a, const Integer &b);
//! modulo
void mod(const Ptr<RCP<const Number>> &r, const Integer &n, const Integer &d);
//! \return floor of quotient when `n` is divided by `d`
void fdiv_q(const Ptr<RCP<const Integer>> &q, const Integer &n, const Integer &d);
//! inverse modulo
int mod_inverse(const Ptr<RCP<const Integer>> &b, const Integer &a,
        const Integer &m);

//! Chinese remainder function. Return true when a solution exists.
bool crt(const Ptr<RCP<const Integer>> &R, std::vector<RCP<const Integer>> &rem,
       std::vector<RCP<const Integer>> &mod);

//! Fibonacci number
RCP<const Integer> fibonacci(unsigned long n);

//! Fibonacci n and n-1
void fibonacci2(const Ptr<RCP<const Integer>> &g, const Ptr<RCP<const Integer>> &s,
        unsigned long n);

//! Lucas number
RCP<const Integer> lucas(unsigned long n);

//! Lucas number n and n-1
void lucas2(const Ptr<RCP<const Integer>> &g, const Ptr<RCP<const Integer>> &s,
        unsigned long n);

//! Binomial Coefficient
RCP<const Integer> binomial(const Integer &n,unsigned long k);

//! Factorial
RCP<const Integer> factorial(unsigned long n);

//! \return true of `a` divides `b`
bool divides(const RCP<const Integer> &a, const RCP<const Integer> &b);

//! Factorization
//! \param B1 is only used when `n` is factored using gmp-ecm
int factor(const Ptr<RCP<const Integer>> &f, const Integer &n, double B1 = 1.0);

//! Factor using trial division. 
//! \return 1 if a non-trivial factor is found, otherwise 0.
int factor_trial_division(const Ptr<RCP<const Integer>> &f, const Integer &n);

//! Factor using lehman's methods
int factor_lehman_method(const Ptr<RCP<const Integer>> &f, const Integer &n);

//! Factor using Pollard's p-1 method
int factor_pollard_pm1_method(const Ptr<RCP<const Integer>> &f, const Integer &n, 
        unsigned B = 10, unsigned retries = 5);

//! Factor using Pollard's rho methods
int factor_pollard_rho_method(const Ptr<RCP<const Integer>> &f, const Integer &n, 
        unsigned retries = 5);

//! Find prime factors of `n`
void prime_factors(const RCP<const Integer> &n,
        std::vector<RCP<const Integer>> &primes);
//! Find multiplicities of prime factors of `n`
void prime_factor_multiplicities(const RCP<const Integer> &n,
        map_integer_uint &primes);
// Sieve class stores all the primes upto a limit. When a prime or a list of prime 
// is requested, if the prime is not there in the sieve, it is extended to hold that 
// prime. The implementation is a very basic Eratosthenes sieve, but the code should
// be quite optimized. For limit=1e8, it is about 20x slower than the
// `primesieve` library (1206ms vs 55.63ms).
class Sieve {

private:
    static std::vector<unsigned> _primes;
    static void _extend(unsigned limit);
    static unsigned _sieve_size;

public:
    // Returns all primes up to the `limit` (including). The vector `primes` should
    // be empty on input and it will be filled with the primes.
    //! \param primes: holds all primes up to the `limit` (including).
    static void generate_primes(unsigned limit, std::vector<unsigned> &primes);
    //Clear the array of primes stored if the variable set_clear is set to true
    static void clear();
    //Set the sieve size in kilobytes. Set it to L1d cache size for best performance.
    //Default value is 32.
    static void set_sieve_size(unsigned size);
    //Variable to set whether the sieve is cleared when clear() is called
    static bool set_clear;

    class iterator {

    private:
        unsigned _index;
        unsigned _limit;

    public:
        //Iterator that generates primes upto limit
        iterator(unsigned limit);
        //Iterator that generates primes with no limit.
        iterator();
        //Destructor
        ~iterator();
        //Next prime
        unsigned next_prime();
    };
};

//! Computes the Bernoulli number Bn as an exact fraction, for an isolated integer n
RCP<const Number> bernoulli(ulong n);
//! Computes a primitive root. Returns false if no primitive root exists. 
//Primitive root calculated is the smallest when n is prime.
bool primitive_root(const Ptr<RCP<const Integer>> &g, const Integer &n);
//! Computes all primitive roots less than n. Returns false if no primitive root exists.
bool primitive_root_list(std::vector<RCP<const Integer>> &roots, const Integer &n);
}
#endif

