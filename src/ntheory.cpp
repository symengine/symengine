#include <cmath>
#include <valarray>

#include "ntheory.h"
#include "mul.h"
#ifdef HAVE_CSYMPY_ECM
#  include <ecm.h>
#endif // HAVE_CSYMPY_ECM
#include "dict.h"

namespace CSymPy {

// Basic number theoretic functions
RCP<const Integer> gcd(const Integer &a, const Integer &b)
{
    mpz_class g;

    mpz_gcd(g.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());

    return integer(g);
}

void gcd_ext(const Ptr<RCP<const Integer>> &g, const Ptr<RCP<const Integer>> &s,
        const Ptr<RCP<const Integer>> &t, const Integer &a, const Integer &b)
{
    mpz_t g_t;
    mpz_t s_t;
    mpz_t t_t;

    mpz_init(g_t);
    mpz_init(s_t);
    mpz_init(t_t);

    mpz_gcdext(g_t, s_t, t_t, a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());
    *g = integer(mpz_class(g_t));
    *s = integer(mpz_class(s_t));
    *t = integer(mpz_class(t_t));

    mpz_clear(g_t);
    mpz_clear(s_t);
    mpz_clear(t_t);
}

RCP<const Integer> lcm(const Integer &a, const Integer &b)
{
    mpz_class c;

    mpz_lcm(c.get_mpz_t(), a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t());

    return integer(c);
}

int mod_inverse(const Ptr<RCP<const Integer>> &b, const Integer &a,
        const Integer &m)
{
    int ret_val;
    mpz_t inv_t;

    mpz_init(inv_t);

    ret_val = mpz_invert(inv_t, a.as_mpz().get_mpz_t(), m.as_mpz().get_mpz_t());
    *b = integer(mpz_class(inv_t));

    mpz_clear(inv_t);

    return ret_val;
}

RCP<const Integer> fibonacci(unsigned long n)
{
    mpz_class f;

    mpz_fib_ui(f.get_mpz_t(), n);
    
    return integer(f);
}

void fibonacci2(const Ptr<RCP<const Integer>> &g, const Ptr<RCP<const Integer>> &s,
        unsigned long n)
{
    mpz_t g_t;
    mpz_t s_t;

    mpz_init(g_t);
    mpz_init(s_t);
	
    mpz_fib2_ui(g_t, s_t, n);
    *g = integer(mpz_class(g_t));
    *s = integer(mpz_class(s_t));

    mpz_clear(g_t);
    mpz_clear(s_t);
}

RCP<const Integer> lucas(unsigned long n)
{
    mpz_class f;

    mpz_lucnum_ui(f.get_mpz_t(), n);
    
    return integer(f);
}

void lucas2(const Ptr<RCP<const Integer>> &g, const Ptr<RCP<const Integer>> &s,
        unsigned long n)
{
    mpz_t g_t;
    mpz_t s_t;

    mpz_init(g_t);
    mpz_init(s_t);
	
    mpz_lucnum2_ui(g_t, s_t, n);
    *g = integer(mpz_class(g_t));
    *s = integer(mpz_class(s_t));

    mpz_clear(g_t);
    mpz_clear(s_t);
}

// Binomial Coefficient
RCP<const Integer> binomial(const Integer &n, unsigned long k)
{
    mpz_class f;

    mpz_bin_ui(f.get_mpz_t(), n.as_mpz().get_mpz_t(), k);
    
    return integer(f);
}

// Factorial
RCP<const Integer> factorial(unsigned long n)
{
    mpz_class f;

    mpz_fac_ui(f.get_mpz_t(), n);
    
    return integer(f);
}

// Returns true if `b` divides `a` without reminder
bool divides(const RCP<const Integer> &a, const RCP<const Integer> &b)
{
    return is_a<Integer>(*div(a, b));
}

// Prime functions
int probab_prime_p(const Integer &a, int reps)
{
    return mpz_probab_prime_p(a.as_mpz().get_mpz_t(), reps);
}

RCP<const Integer> nextprime(const Integer &a)
{
    mpz_class c;

    mpz_nextprime(c.get_mpz_t(), a.as_mpz().get_mpz_t());

    return integer(c);
}

// Factoring by Trial division using primes only
int _factor_trial_division_sieve(mpz_class &factor, const mpz_class &N)
{
    mpz_class sqrtN;
    sqrtN = sqrt(N);
    if (!(sqrtN.fits_uint_p()))
        throw std::runtime_error("N too large to factor");
    unsigned limit = sqrtN.get_ui()+1;
    std::vector<unsigned> primes;
    eratosthenes_sieve(limit, primes);
    for (auto &p: primes)
        if (N % p == 0) {
            factor = p;
            return 1;
        }
    return 0;
}

// Factor using lehman method.
int _factor_lehman_method(mpz_class &rop, const mpz_class &n)
{
    if (n < 21)
        throw std::runtime_error("Require n >= 21 to use lehman method");

    int ret_val = 0;
    mpz_class u_bound;

    mpz_root(u_bound.get_mpz_t(), n.get_mpz_t(), 3);
    u_bound = u_bound + 1;

    for (mpz_class i = 2; i <= u_bound; i++)
        if (n % i == 0) {
            rop = n / i;
            ret_val = 1;
            break;
        }

    if (!ret_val){

        mpz_class k, a, b, l;
        mpf_class t;

        k = 1;

        while (k <= u_bound) {
            t = 2 * sqrt(k * n);
            mpz_set_f(a.get_mpz_t(), t.get_mpf_t());
            mpz_root(b.get_mpz_t(), n.get_mpz_t(), 6);
            mpz_root(l.get_mpz_t(), k.get_mpz_t(), 2);
            b = b / (4 * l);
            b = b + a;
            
            while (a <= b) {
                l = a * a - 4 * k * n;
                if (mpz_perfect_square_p(l.get_mpz_t())) {
                    mpz_sqrt(b.get_mpz_t(), l.get_mpz_t());
                    b = a + b;
                    mpz_gcd(rop.get_mpz_t(), n.get_mpz_t(), b.get_mpz_t());
                    ret_val = 1;
                    break;
                }
                a = a + 1;
            }
            if (ret_val)
                break;
            k = k + 1;
        }
    }
    
    return ret_val;
}

int factor_lehman_method(const Ptr<RCP<const Integer>> &f, const Integer &n)
{
    int ret_val;
    mpz_class rop;

    ret_val = _factor_lehman_method(rop, n.as_mpz());
    *f = integer(rop);
    return ret_val;
}

// Factor using Pollard's p-1 method
int _factor_pollard_pm1_method(mpz_class &rop, const mpz_class &n, 
        const mpz_class &c, unsigned B)
{
    if (n < 4 || B < 3)
        throw std::runtime_error("Require n > 3 and B > 2 to use Pollard's p-1 method");
    
    std::vector<unsigned> primes;
    eratosthenes_sieve(B + 1, primes);
    mpz_class m, g, _c;
    _c = c;

    for (auto &p: primes){
        m = 1;
        // calculate log(m, B), this can be improved
        while(m <= B / p){
            m = m * p;
        }
        mpz_powm(g.get_mpz_t(), _c.get_mpz_t(), m.get_mpz_t(), n.get_mpz_t());
        mpz_set(_c.get_mpz_t(), g.get_mpz_t());
    }
    _c = _c - 1;
    mpz_gcd(rop.get_mpz_t(), _c.get_mpz_t(), n.get_mpz_t());
    
    if(rop == 1 || rop == n)
        return 0;
    else
        return 1;
}

int factor_pollard_pm1_method(const Ptr<RCP<const Integer>> &f, const Integer &n, 
        unsigned B, unsigned retries)
{
    int ret_val = 0;
    mpz_class rop, nm4, c;
    gmp_randstate_t state;
    
    gmp_randinit_default(state);
    gmp_randseed_ui(state, retries);
    nm4 = n.as_mpz() - 4;

    for (unsigned i = 0; i < retries && ret_val == 0; i++) {
        mpz_urandomm(c.get_mpz_t(), state, nm4.get_mpz_t());
        c = c + 2;
        ret_val = _factor_pollard_pm1_method(rop, n.as_mpz(), c, B);
    }
    
    if (ret_val != 0)
        *f = integer(rop);
    
    return ret_val;
}

// Factor using Pollard's rho method
int _factor_pollard_rho_method(mpz_class &rop, const mpz_class &n, 
        const mpz_class &a, const mpz_class &s, unsigned steps = 10000)
{
    if (n < 5)
        throw std::runtime_error("Require n > 4 to use pollard's-rho method");

    mpz_class u, v, g, m;
    u = s;
    v = s;

    for (unsigned i = 0; i < steps; i++) {
        u = (u*u + a) % n;
        v = (v*v + a) % n;
        v = (v*v + a) % n;
        m = u - v;
        mpz_gcd(g.get_mpz_t(), m.get_mpz_t(), n.get_mpz_t());

        if (g == n)
            return 0;
        if (g == 1)
            continue;
        rop = g;
        return 1;
    }
    return 0;
}

int factor_pollard_rho_method(const Ptr<RCP<const Integer>> &f, 
        const Integer &n, unsigned retries)
{
    int ret_val = 0;
    mpz_class rop, nm1, nm4, a, s;
    gmp_randstate_t state;
    
    gmp_randinit_default(state);
    gmp_randseed_ui(state, retries);
    nm1 = n.as_mpz() - 1;
    nm4 = n.as_mpz() - 4;

    for (unsigned i = 0; i < retries && ret_val == 0; i++) {
        mpz_urandomm(a.get_mpz_t(), state, nm1.get_mpz_t());
        mpz_urandomm(s.get_mpz_t(), state, nm4.get_mpz_t());
        s = s + 1;
        ret_val = _factor_pollard_rho_method(rop, n.as_mpz(), a, s);
    }
    
    if (ret_val != 0)
        *f = integer(rop);
    return ret_val;
}

// Factorization
int factor(const Ptr<RCP<const Integer>> &f, const Integer &n, double B1)
{
    int ret_val = 0;
    mpz_class _n, _f;

    _n = n.as_mpz();

#ifdef HAVE_CSYMPY_ECM
    if (mpz_perfect_power_p(_n.get_mpz_t())) {

        unsigned long int i = 1;
        mpz_class m, rem;
        rem = 1; // Any non zero number
        m = 2; // set `m` to 2**i, i = 1 at the begining

        // calculate log2n, this can be improved
        for (; m < _n; i++)
            m = m * 2;

        // eventually `rem` = 0 zero as `n` is a perfect power. `f_t` will
        // be set to a factor of `n` when that happens
        while(i > 1 && rem != 0) {
            mpz_rootrem(_f.get_mpz_t(), rem.get_mpz_t(), _n.get_mpz_t(), i);
            i--;
        }

        ret_val = 1;
    }
    else {

        if (mpz_probab_prime_p(_n.get_mpz_t(), 25) > 0) { // most probably, n is a prime
            ret_val = 0;
            _f = _n;
        }
        else {

            for (int i = 0; i < 10 && !ret_val; i++)
                ret_val = ecm_factor(_f.get_mpz_t(), _n.get_mpz_t(), B1, NULL);
            if (!ret_val)
                throw std::runtime_error("ECM failed to factor the given number");
        }
    }
#else
    // B1 is discarded if gmp-ecm is not installed
    ret_val = _factor_trial_division_sieve(_f, _n);
#endif // HAVE_CSYMPY_ECM
    *f = integer(_f);

    return ret_val;
}

int factor_trial_division(const Ptr<RCP<const Integer>> &f, const Integer &n)
{
    int ret_val;
    mpz_class factor;
    ret_val =_factor_trial_division_sieve(factor, n.as_mpz());
    if (ret_val == 1) *f = integer(factor);
    return ret_val;
}

void eratosthenes_sieve(unsigned limit, std::vector<unsigned> &primes)
{
    std::valarray<bool> is_prime(true, limit / 2); 
    if (limit > 2)
        primes.push_back(2);
    //considering only odd integers. An odd number n corresponds to n/2 in the array.
    const unsigned sqrt_limit = static_cast<unsigned>(std::sqrt(limit - 1));
    for (unsigned n = 3; n <= sqrt_limit; n += 2) {
        if (is_prime[n / 2]) {
            std::slice sl = std::slice((n * n )/ 2, 1 + (limit - 1 - n * n) / (2 * n), n); 
            //starting from n*n, all the odd multiples of n are marked not prime. 
            is_prime[sl] = false;
        }
    }
    for (unsigned n = 1; n < limit / 2; n++) {
        if (is_prime[n])
	    primes.push_back(2 * n + 1);
    }
}

void prime_factors(const RCP<const Integer> &n,
        std::vector<RCP<const Integer>> &prime_list)
{
    mpz_class sqrtN;
    mpz_class _n = (*n).as_mpz();
    if (_n == 0) return;
    sqrtN = sqrt(_n);
    if (!sqrtN.fits_uint_p())
        throw std::runtime_error("N too large to factor");
    unsigned limit = sqrtN.get_ui() + 1;
    std::vector<unsigned> primes;
    eratosthenes_sieve(limit, primes);
    
    for (auto &p: primes)
    {
        while (_n % p == 0) {
            prime_list.push_back(integer(p)); 
            _n = _n / p;
        }
        if (_n == 1) break;
    }
    if (!(_n == 1))
        prime_list.push_back(integer(_n));
}

void prime_factor_multiplicities(const RCP<const Integer> &n,
        map_integer_uint &primes_mul)
{
    mpz_class sqrtN;
    mpz_class _n = (*n).as_mpz();
    unsigned count;
    if (_n == 0) return;
    sqrtN = sqrt(_n);
    if (!sqrtN.fits_uint_p())
        throw std::runtime_error("N too large to factor");
    unsigned limit = sqrtN.get_ui() + 1;
    std::vector<unsigned> primes;
    eratosthenes_sieve(limit, primes);
    
    for (auto &p: primes)
    {
        count = 0;
        while (_n % p == 0) { // when a prime factor is found, we divide
            count++;                     // _n by that prime as much as we can
            _n = _n / p;
        }
        if (count > 0) {
            insert(primes_mul, integer(p), count);
            if (_n == 1) break;
        }
    }
    if (!(_n == 1))
        insert(primes_mul, integer(_n), 1);
}

} // CSymPy

