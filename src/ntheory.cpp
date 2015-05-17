#include <cmath>
#include <valarray>

#include "ntheory.h"
#include "rational.h"
#include "mul.h"
#ifdef HAVE_SYMENGINE_ECM
#  include <ecm.h>
#endif // HAVE_SYMENGINE_ECM
#ifdef HAVE_SYMENGINE_PRIMESIEVE
#  include <primesieve.hpp>
#endif // HAVE_SYMENGINE_PRIMESIEVE
#ifdef HAVE_SYMENGINE_ARB
#  include "arb.h"
#  include "bernoulli.h"
#  include "rational.h"
#endif // HAVE_SYMENGINE_ARB
#include "dict.h"

namespace SymEngine {

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

RCP<const Integer> mod(const Integer &n, const Integer &d)
{
    mpz_class q;
    mpz_mod(q.get_mpz_t(), n.as_mpz().get_mpz_t(), d.as_mpz().get_mpz_t());
    return integer(q);
}

RCP<const Integer> quotient_f(const Integer &n, const Integer &d)
{
    mpz_class q;
    mpz_fdiv_q(q.get_mpz_t(), n.as_mpz().get_mpz_t(), d.as_mpz().get_mpz_t());
    return integer(q);
}

void quotient_mod(const Ptr<RCP<const Integer>> &q, const Ptr<RCP<const Integer>> &r, 
        const Integer &n, const Integer &d)
{
    mpz_class _q, _r;
    mpz_fdiv_qr(_q.get_mpz_t(), _r.get_mpz_t(), n.as_mpz().get_mpz_t(), d.as_mpz().get_mpz_t());
    *q = integer(_q);
    *r = integer(_r);
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
bool divides(const Integer &a, const Integer &b)
{
    return mpz_divisible_p(a.as_mpz().get_mpz_t(), b.as_mpz().get_mpz_t()) != 0;
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
    unsigned limit = sqrtN.get_ui();
    Sieve::iterator pi(limit);
    unsigned p;
    while ((p = pi.next_prime()) <= limit) {
        if (N % p == 0) {
            factor = p;
            return 1;
        }
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

    Sieve::iterator pi(u_bound.get_ui());
    unsigned p;
    while ((p = pi.next_prime()) <= u_bound.get_ui()) {
        if (n % p == 0) {
            rop = n / p;
            ret_val = 1;
            break;
        }
    }

    if (!ret_val) {

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

    mpz_class m, g, _c;
    _c = c;

    Sieve::iterator pi(B);
    unsigned p;
    while ((p = pi.next_prime()) <= B) {
        m = 1;
        // calculate log(p, B), this can be improved
        while (m <= B / p) {
            m = m * p;
        }
        mpz_powm(g.get_mpz_t(), _c.get_mpz_t(), m.get_mpz_t(), n.get_mpz_t());
        mpz_set(_c.get_mpz_t(), g.get_mpz_t());
    }
    _c = _c - 1;
    mpz_gcd(rop.get_mpz_t(), _c.get_mpz_t(), n.get_mpz_t());

    if (rop == 1 || rop == n)
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
    gmp_randclear(state);
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
    gmp_randclear(state);
    return ret_val;
}

// Factorization
int factor(const Ptr<RCP<const Integer>> &f, const Integer &n, double B1)
{
    int ret_val = 0;
    mpz_class _n, _f;

    _n = n.as_mpz();

#ifdef HAVE_SYMENGINE_ECM
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
        while (i > 1 && rem != 0) {
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
                ret_val = ecm_factor(_f.get_mpz_t(), _n.get_mpz_t(), B1,
                        nullptr);
            if (!ret_val)
                throw std::runtime_error("ECM failed to factor the given number");
        }
    }
#else
    // B1 is discarded if gmp-ecm is not installed
    ret_val = _factor_trial_division_sieve(_f, _n);
#endif // HAVE_SYMENGINE_ECM
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

void prime_factors(std::vector<RCP<const Integer>> &prime_list, const Integer &n)
{
    mpz_class sqrtN;
    mpz_class _n = n.as_mpz();
    if (_n == 0) return;
    if (_n < 0) _n *= -1;

    sqrtN = sqrt(_n);
    if (!sqrtN.fits_uint_p())
        throw std::runtime_error("N too large to factor");
    unsigned limit = sqrtN.get_ui();
    Sieve::iterator pi(limit);
    unsigned p;
    
    while ((p = pi.next_prime()) <= limit) {
        while (_n % p == 0) {
            prime_list.push_back(integer(p)); 
            _n = _n / p;
        }
        if (_n == 1) break;
    }
    if (!(_n == 1))
        prime_list.push_back(integer(_n));
}

void prime_factor_multiplicities(map_integer_uint &primes_mul, const Integer &n)
{
    mpz_class sqrtN;
    mpz_class _n = n.as_mpz();
    unsigned count;
    if (_n == 0) return;
    if (_n < 0) _n *= -1;

    sqrtN = sqrt(_n);
    if (!sqrtN.fits_uint_p())
        throw std::runtime_error("N too large to factor");
    unsigned limit = sqrtN.get_ui();
    Sieve::iterator pi(limit);

    unsigned p;
    while ((p = pi.next_prime()) <= limit) {
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

std::vector<unsigned> Sieve::_primes = {2,3,5,7,11,13,17,19,23,29};
bool Sieve::_clear = true;
unsigned Sieve::_sieve_size = 32 * 1024 * 8; //32K in bits

void Sieve::set_clear(bool clear)
{
    _clear = clear;
}

void Sieve::clear()
{
    _primes.erase(_primes.begin() + 10, _primes.end());
}

void Sieve::set_sieve_size(unsigned size) {
#ifdef HAVE_SYMENGINE_PRIMESIEVE
    primesieve::set_sieve_size(size);
#else
    _sieve_size = size * 1024 * 8; //size in bits
#endif
}

void Sieve::_extend(unsigned limit)
{
#ifdef HAVE_SYMENGINE_PRIMESIEVE
    if (_primes.back() < limit)
        primesieve::generate_primes(_primes.back() + 1, limit, &_primes);
#else
    const unsigned sqrt_limit = static_cast<unsigned>(std::sqrt(limit));
    unsigned start = _primes.back() + 1;
    if (limit <= start)
        return;
    if (sqrt_limit >= start) {
        _extend(sqrt_limit);
        start = _primes.back() + 1;
    }
    
    unsigned segment = _sieve_size;
    std::valarray<bool> is_prime(segment);
    for (; start <= limit; start += 2 * segment) {
        unsigned finish = std::min(start + segment * 2 + 1, limit);
        is_prime[std::slice(0, segment, 1)] = true;
        //considering only odd integers. An odd number n corresponds to n-start/2 in the array.
        for (unsigned index = 1; index < _primes.size() &&
            _primes[index] * _primes[index] <= finish; ++index) {
            unsigned n = _primes[index];
            unsigned multiple = (start / n + 1) * n;
            if (multiple % 2 == 0)
                multiple += n;
            if (multiple > finish)
                continue;
            std::slice sl = std::slice((multiple-start)/ 2, 1 + (finish - multiple) / (2 * n), n);
            //starting from n*n, all the odd multiples of n are marked not prime. 
            is_prime[sl] = false;
        }
        for (unsigned n = start + 1; n <= finish; n += 2) {
            if (is_prime[(n - start) / 2])
                _primes.push_back(n);
        }
    }
#endif
}

void Sieve::generate_primes(std::vector<unsigned> &primes, unsigned limit)
{
    _extend(limit);
    std::vector<unsigned>::iterator it = std::upper_bound (_primes.begin(), _primes.end(), limit);
    //find the first position greater than limit and reserve space for the primes
    primes.reserve(it - _primes.begin());
    std::copy(_primes.begin(), it, std::back_inserter(primes));
    if (_clear)
        clear();
}

Sieve::iterator::iterator(unsigned max)
{
    _limit = max;
    _index = 0;
}

Sieve::iterator::iterator()
{
    _limit = 0;
    _index = 0;
}

Sieve::iterator::~iterator()
{
    if (_clear)
        Sieve::clear();
}

unsigned Sieve::iterator::next_prime()
{
    if (_index >= _primes.size()) {
        unsigned extend_to = _primes[_index - 1] * 2;
        if (_limit > 0 && _limit < extend_to) {
            extend_to = _limit;
        }
        _extend(extend_to);
        if (_index >= _primes.size()) {     //the next prime is greater than _limit
            return _limit + 1;
        }
    }
    return SymEngine::Sieve::_primes[_index++];
}

RCP<const Number> bernoulli(unsigned long n)
{
#ifdef HAVE_SYMENGINE_ARB
    fmpq_t res;
    fmpq_init(res);
    bernoulli_fmpq_ui(res, n);
    mpq_t a;
    mpq_init(a);
    fmpq_get_mpq(a, res);
    mpq_class b (a);
    fmpq_clear(res);
    mpq_clear(a);
    return Rational::from_mpq(b);
#else
    throw std::runtime_error("Currently supported only if ARB is installed");
#endif
}

// References : Cohen H., A course in computational algebraic number theory (1996), page 21.
bool crt(const Ptr<RCP<const Integer>> &R, const std::vector<RCP<const Integer>> &rem,
       const std::vector<RCP<const Integer>> &mod)
{
    if (mod.size() > rem.size())
        throw std::runtime_error("Too few remainders");
    if (mod.size() == 0)
        throw std::runtime_error("Moduli vector cannot be empty");

    mpz_class m, r, g, s, t;
    m = mod[0]->as_mpz();
    r = rem[0]->as_mpz();

    for (unsigned i = 1; i < mod.size(); i++) {
        mpz_gcdext(g.get_mpz_t(), s.get_mpz_t(), t.get_mpz_t(), m.get_mpz_t(), mod[i]->as_mpz().get_mpz_t());
        // g = s * m + t * mod[i]
        t = rem[i]->as_mpz() - r;
        if (!mpz_divisible_p (t.get_mpz_t(), g.get_mpz_t()))
            return false;
        r += m * s * (t / g);           // r += m * (m**-1 mod[i]/g)* (rem[i] - r) / g
        m *= mod[i]->as_mpz() / g;
        mpz_fdiv_r (r.get_mpz_t(), r.get_mpz_t(), m.get_mpz_t());
    }
    *R = integer(r);
    return true;
}

// Crt over a cartesian product of vectors (Assuming that moduli are pairwise relatively prime).
void _crt_cartesian(std::vector<RCP<const Integer>> &R, const std::vector<std::vector<RCP<const Integer>>> &rem,
       const std::vector<RCP<const Integer>> &mod)
{
    if (mod.size() > rem.size())
        throw std::runtime_error("Too few remainders");
    if (mod.size() == 0)
        throw std::runtime_error("Moduli vector cannot be empty");
    mpz_class m, _m, r, s, t;
    m = mod[0]->as_mpz();
    R = rem[0];

    for (unsigned i = 1; i < mod.size(); i++) {
        std::vector<RCP<const Integer>> rem2;
        mpz_invert(s.get_mpz_t(), m.get_mpz_t(), mod[i]->as_mpz().get_mpz_t());
        _m = m;
        m *= mod[i]->as_mpz();
        for (unsigned j = 0; j < R.size(); j++) {
            for (unsigned k = 0;
            k < rem[i].size(); k++) {
                r = R[j]->as_mpz();
                r += _m * s * (rem[i][k]->as_mpz() - r);
                mpz_fdiv_r (r.get_mpz_t(), r.get_mpz_t(), m.get_mpz_t());
                rem2.push_back(integer(r));
            }
        }
        R = rem2;
    }
}

// Tests whether n is a prime power and finds a prime p and e such that n = p**e.
bool _prime_power(mpz_class &p, mpz_class &e, const mpz_class &n)
{
    if (n < 2)
        return false;
    mpz_class _n = n, temp;
    e = 1;
    unsigned i = 2;
    while (mpz_perfect_power_p(_n.get_mpz_t()) && _n >= 2) {
        if (mpz_root(temp.get_mpz_t(), _n.get_mpz_t(), i) != 0) {
            mpz_mul_ui(e.get_mpz_t(), e.get_mpz_t(), i);
            _n = temp;
        } else {
            i++;
        }
    }
    if (mpz_probab_prime_p(_n.get_mpz_t(), 25)) {
        p = _n;
        return true;
    }
    return false;
}

// Computes a primitive root modulo p**e or 2*p**e where p is an odd prime.
// References : Cohen H., A course in computational algebraic number theory (2009), pages 25-27.
void _primitive_root(mpz_class &g, const mpz_class &p, const mpz_class &e, 
        bool even = false)
{
    std::vector<RCP<const Integer>> primes;
    prime_factors(primes, *integer(p - 1));

    mpz_class t;
    g = 2;
    while (g < p) {
        bool root = true;
        for (auto &it: primes) {
            t = it->as_mpz();
            t = (p - 1)/t;
            mpz_powm(t.get_mpz_t(), g.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
            if (t == 1) {           // If g**(p-1)/q is 1 then g is not a primitive root.
               root = false;
               break;
            }
        }
        if (root)
            break;
        g++;
    }

    if (e > 1) {
        t = p * p;
        mpz_class pm1 = p - 1;
        mpz_powm(t.get_mpz_t(), g.get_mpz_t(), pm1.get_mpz_t(), t.get_mpz_t());
        if (t == 1) {               // If g**(p-1) mod (p**2) == 1 then g + p is a primitive root.
            g += p;
        }
    }
    if (even && g % 2 == 0) {
        mpz_pow_ui(t.get_mpz_t(), p.get_mpz_t(), e.get_ui());
        g += t;                     // If g is even then root of 2*p**e is g + p**e.
    }
}

bool primitive_root(const Ptr<RCP<const Integer>> &g, const Integer &n)
{
    mpz_class _n = n.as_mpz();
    if (_n < 0)
        _n = -_n;
    if (_n <= 1)
        return false;
    if (_n < 5) {
        *g = integer(_n - 1);
        return true;
    }
    bool even = false;
    if (_n % 2 == 0) {
        if (_n % 4 == 0) {
            return false;    // If n mod 4 == 0 and n > 4, then no primitive roots.
        }
        _n /= 2;
        even = true;
    }
    mpz_class p, e;
    if (!_prime_power(p, e, _n))
        return false;
    _primitive_root(_n, p, e, even);
    *g = integer(_n);
    return true;
}

// Computes primitive roots modulo p**e or 2*p**e where p is an odd prime.
// References :
// [1] Cohen H., A course in computational algebraic number theory (1996), pages 25-27.
// [2] Hackman P., Elementary number theory (2009), page 28.
void _primitive_root_list(std::vector<RCP<const Integer>> &roots, const mpz_class &p,
        const mpz_class &e, bool even = false)
{
    mpz_class g, h, d, t, pe2, n, pm1;
    _primitive_root(g, p, 1, false); // Find one primitive root for p.
    h = 1;
    pm1 = p - 1;
    // Generate other primitive roots for p. h = g**i and gcd(i,p-1) = 1. Ref[2]
    mpz_pow_ui(n.get_mpz_t(), p.get_mpz_t(), e.get_ui());
    for (unsigned long i = 1; i < p; i++) {
        h *= g;
        h %= p;
        mpz_gcd_ui(d.get_mpz_t(), pm1.get_mpz_t(), i);
        if (d == 1) {
            if (e == 1) {
                if (even && h % 2 == 0)
                    roots.push_back(integer(h + n));
                else
                    roots.push_back(integer(h));
            } else {
                mpz_class pp = p * p;
                // Find d such that (h + d*p)**(p-1) mod (p**2) == 1. Ref[1]
                // h**(p-1) - 1 = d*p*h**(p-2)
                // d = (h - h**(2-p)) / p
                t = 2 - p;
                mpz_powm(d.get_mpz_t(), h.get_mpz_t(), t.get_mpz_t(), pp.get_mpz_t());
                d = ((h - d) / p + p) % p;
                t = h;
                // t = h + i * p + j * p * p and i != d
                mpz_pow_ui(pe2.get_mpz_t(), p.get_mpz_t(), e.get_ui() - 2);
                for (unsigned long j = 0; j < pe2; j++) {
                    for (unsigned long i = 0; i < p; i++) {
                        if (i != d) {
                            if (even && t % 2 == 0)
                                roots.push_back(integer(t + n));
                            else
                                roots.push_back(integer(t));
                        }
                        t += p;
                    }
                }
            }
        }
    }
}

void primitive_root_list(std::vector<RCP<const Integer>> &roots, const Integer &n)
{
    mpz_class _n = n.as_mpz();
    if (_n < 0)
        _n = -_n;
    if (_n <= 1)
        return;
    if (_n < 5) {
        roots.push_back(integer(_n - 1));
        return;
    }
    bool even = false;
    if (_n % 2 == 0) {
        if (_n % 4 == 0) {
            return;    // If n%4 == 0 and n > 4, then no primitive roots.
        }
        _n /= 2;
        even = true;
    }
    mpz_class p, e;
    if (!_prime_power(p, e, _n))
        return;
    _primitive_root_list(roots, p, e, even);
    std::sort(roots.begin(), roots.end(), SymEngine::RCPIntegerKeyLess());
    return;
}

RCP<const Integer> totient(const RCP<const Integer> &n) {
    if (n->is_zero())
        return integer(1);

    mpz_class phi = n->as_mpz(), p;
    if (phi < 0)
        phi = -phi;
    map_integer_uint prime_mul;
    prime_factor_multiplicities(prime_mul, *n);

    for (auto &it: prime_mul) {
        p = it.first->as_mpz();
        mpz_divexact(phi.get_mpz_t(), phi.get_mpz_t(), p.get_mpz_t());
        // phi is exactly divisible by p.
        phi *= p - 1;
    }
    return integer(phi);
}

RCP<const Integer> carmichael(const RCP<const Integer> &n) {
    if (n->is_zero())
        return integer(1);

    map_integer_uint prime_mul;
    mpz_class lambda, t, p;
    unsigned multiplicity;

    prime_factor_multiplicities(prime_mul, *n);
    lambda = 1;
    for (auto it : prime_mul) {
        p = it.first->as_mpz();
        multiplicity = it.second;
        if (p == 2 && multiplicity > 2) {     // For powers of 2 greater than 4 divide by 2.
            multiplicity--;
        }
        t = p - 1;
        mpz_lcm(lambda.get_mpz_t(), lambda.get_mpz_t(), t.get_mpz_t());
        mpz_pow_ui(t.get_mpz_t(), p.get_mpz_t(), multiplicity - 1);
        // lambda and p are relatively prime.
        lambda = lambda * t;
    }
    return integer(lambda);
}

// References : Cohen H., A course in computational algebraic number theory (1996), page 25.
bool multiplicative_order(const Ptr<RCP<const Integer>> &o, const RCP<const Integer> &a,
        const RCP<const Integer> &n)
{
    mpz_class order, p, t;
    mpz_class _a = a->as_mpz(), _n = n->as_mpz();
    mpz_gcd(t.get_mpz_t(), _a.get_mpz_t(), _n.get_mpz_t());
    if (t != 1)
        return false;

    RCP<const Integer> lambda = carmichael(n);
    map_integer_uint prime_mul;
    prime_factor_multiplicities(prime_mul, *lambda);
    _a %= _n;
    order = lambda->as_mpz();

    for (auto it : prime_mul) {
        p = it.first->as_mpz();
        mpz_pow_ui(t.get_mpz_t(), p.get_mpz_t(), it.second);
        mpz_divexact(order.get_mpz_t(), order.get_mpz_t(), t.get_mpz_t());
        mpz_powm(t.get_mpz_t(), _a.get_mpz_t(), order.get_mpz_t(), _n.get_mpz_t());
        while (t != 1) {
            mpz_powm(t.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t(), _n.get_mpz_t());
            order *= p;
        }
    }
    *o = integer(order);
    return true;
}
int legendre(const Integer &a, const Integer &n)
{
    return mpz_legendre(a.as_mpz().get_mpz_t(), n.as_mpz().get_mpz_t());
}

int jacobi(const Integer &a, const Integer &n)
{
    return mpz_jacobi(a.as_mpz().get_mpz_t(), n.as_mpz().get_mpz_t());
}

int kronecker(const Integer &a, const Integer &n)
{
    return mpz_kronecker(a.as_mpz().get_mpz_t(), n.as_mpz().get_mpz_t());
}

bool _sqrt_mod_tonelli_shanks(mpz_class &rop, const mpz_class &a, const mpz_class &p)
{
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed(state, p.get_mpz_t());

    mpz_class n, y, b, q, pm1, t = 1;
    pm1 = p - 1;
    unsigned e, m;
    e = mpz_scan1 (pm1.get_mpz_t(), 0);
    mpz_tdiv_q_2exp (q.get_mpz_t(), pm1.get_mpz_t(), e);    //p - 1 = 2**e*q

    while (t != -1) {
        mpz_urandomm(n.get_mpz_t(), state, p.get_mpz_t());
        t = mpz_legendre(n.get_mpz_t(), p.get_mpz_t());
    }
    mpz_powm(y.get_mpz_t(), n.get_mpz_t(), q.get_mpz_t(), p.get_mpz_t());   //y = n**q mod p
    mpz_powm(b.get_mpz_t(), a.get_mpz_t(), q.get_mpz_t(), p.get_mpz_t());   //b = a**q mod p
    t = (q + 1) / 2;
    mpz_powm(rop.get_mpz_t(), a.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());   //rop = a**((q + 1) / 2) mod p

    while (b != 1) {
        m = 0;
        t = b;
        while (t != 1) {
            mpz_powm_ui(t.get_mpz_t(), t.get_mpz_t(), 2, p.get_mpz_t());
            m++;                                                            //t = t**2 = b**2**(m)
        }
        if(m == e)
            return false;
        q = 2;
        mpz_pow_ui(q.get_mpz_t(), q.get_mpz_t(), e - m - 1);    //q = 2**(e - m - 1)
        mpz_powm(t.get_mpz_t(), y.get_mpz_t(), q.get_mpz_t(), p.get_mpz_t());   // t = y**(2**(e - m - 1))
        mpz_powm_ui(y.get_mpz_t(), t.get_mpz_t(), 2, p.get_mpz_t());    //y = t**2
        e = m;
        rop = (rop * t) % p;
        b = (b * y) % p;
    }
    return true;
}

bool _sqrt_mod_prime(mpz_class &rop, const mpz_class &a, const mpz_class &p)
{
    if (p == 2) {
        rop = a % p;
        return true;
    }
    int l = mpz_legendre(a.get_mpz_t(), p.get_mpz_t());
    mpz_class t;
    if (l == -1) {
        return false;
    } else if (l == 0) {
        rop = 0;
    } else if (p % 4 == 3) {
        t = (p + 1) / 4;
        mpz_powm(rop.get_mpz_t(), a.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
    } else if (p % 8 == 5) {
        t = (p - 1) / 4;
        mpz_powm(t.get_mpz_t(), a.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
        if (t == 1) {
            t = (p + 3) / 8;
            mpz_powm(rop.get_mpz_t(), a.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
        } else {
            t = (p - 5) / 8;
            mpz_class t1 = 4 * a;
            mpz_powm(t.get_mpz_t(), t1.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
            rop = (2 * a * t) % p;
        }
    } else {
        if(p < 10000) { // If p < 10000, brute force is faster.
            mpz_class sq = 1, _a;
            mpz_fdiv_r(_a.get_mpz_t(), a.get_mpz_t(), p.get_mpz_t());
            for(unsigned i = 1; i < p; i++) {
                if (sq == _a) {
                    rop = i;
                    return true;
                }
                sq += 2 * i + 1;
                mpz_fdiv_r(sq.get_mpz_t(), sq.get_mpz_t(), p.get_mpz_t());
            }
            return false;
        } else {
            return _sqrt_mod_tonelli_shanks(rop, a, p);
        }
    }
    return true;
}

// References : Menezes, Alfred J., Paul C. Van Oorschot, and Scott A. Vanstone. Handbook of applied cryptography. CRC press, 2010. pages 104 - 108
// Calculates log = x mod q**k where g**x == a mod p and order(g, p) = n.
void _discrete_log(mpz_class &log, const mpz_class &a, const mpz_class &g, const mpz_class &n, 
        const mpz_class &q, const unsigned &k, const mpz_class &p)
{
    log = 0;
    mpz_class gamma = a, alpha, _n, t, beta, qj = 1, m, l;
    _n = n / q;
    mpz_powm(alpha.get_mpz_t(), g.get_mpz_t(), _n.get_mpz_t(), p.get_mpz_t());
    mpz_sqrtrem(m.get_mpz_t(), t.get_mpz_t(), q.get_mpz_t());
    if (t != 0)
        m++;    // m = ceiling(sqrt(q)).
    map_integer_uint table; // Table for lookup in baby-step giant-step algorithm
    mpz_class alpha_j = 1, d, s;
    s = -m;
    mpz_powm(s.get_mpz_t(), alpha.get_mpz_t(), s.get_mpz_t(), p.get_mpz_t());

    for (unsigned j = 0; j < m; j++) {
        insert(table, integer(alpha_j), j);
        alpha_j = (alpha_j * alpha) % p;
    }

    for (unsigned long j = 0; j < k; j++) { // Pohlig-Hellman
        mpz_powm(beta.get_mpz_t(), gamma.get_mpz_t(), _n.get_mpz_t(), p.get_mpz_t());
        // Baby-step giant-step algorithm for l = log_alpha(beta)
        d = beta;
        bool found = false;
        for (unsigned i = 0; !found &&i < m; i++) {
            if (table.find(integer(d)) != table.end()) {
                l = i * m + table[integer(d)];
                found = true;
                break;
            }
            d = (d * s) % p;
        }
        _n /= q;
        t = -l * qj;

        log -= t;
        mpz_powm(t.get_mpz_t(), g.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
        gamma *= t; //gamma *= g ** (-l * (q ** j))
        qj *= q;
    }
}

// References : Johnston A., A generalised qth root algorithm.
// Solution for x**n == a mod p**k where a != 0 mod p and p is an odd prime.
bool _nthroot_mod1(std::vector<RCP<const Integer>> &roots, const mpz_class &a, const mpz_class &n, 
        const mpz_class &p, const unsigned k, bool all_roots = false)
{
    mpz_class _n, r, root, s, t, g = 0, pk, m, phi;
    mpz_pow_ui(pk.get_mpz_t(), p.get_mpz_t(), k);
    phi = pk * (p - 1) / p;
    mpz_gcd(m.get_mpz_t(), phi.get_mpz_t(), n.get_mpz_t());
    t = phi / m;
    mpz_powm(t.get_mpz_t(), a.get_mpz_t(), t.get_mpz_t(), pk.get_mpz_t());
    // Check whether a**(phi / gcd(phi, n)) == 1 mod p**k.
    if (t != 1) {
        return false;
    }
    // Solve x**n == a mod p first.
    t = p - 1;
    mpz_gcdext(_n.get_mpz_t(), r.get_mpz_t(), s.get_mpz_t(), n.get_mpz_t(), t.get_mpz_t());
    mpz_powm(s.get_mpz_t(), a.get_mpz_t(), r.get_mpz_t(), p.get_mpz_t());

    // Solve x**(_n) == s mod p where _n | p - 1.
    if (_n == 1) {
        root = s;
    } else if (_n == 2) {
        _sqrt_mod_prime(root, s, p);
    } else { // Ref[1]
        map_integer_uint prime_mul;
        prime_factor_multiplicities(prime_mul, *integer(_n));
        mpz_class h, q, qt, z, v, x, s1 = s;
        _primitive_root(g, p, 2);
        unsigned c;
        for (auto &it: prime_mul) {
            q = it.first->as_mpz();
            mpz_pow_ui(qt.get_mpz_t(), q.get_mpz_t(), it.second);
            h = (p - 1) / q;
            c = 1;
            while (h % q == 0) {
                c++;
                h /= q;
            }
            mpz_invert(t.get_mpz_t(), h.get_mpz_t(), qt.get_mpz_t());
            z = t * -h;
            x = (1 + z) / qt;
            mpz_powm(v.get_mpz_t(), s1.get_mpz_t(), x.get_mpz_t(), p.get_mpz_t());

            if (c == it.second) {
                s1 = v;
            } else {
                mpz_powm(x.get_mpz_t(), s1.get_mpz_t(), h.get_mpz_t(), p.get_mpz_t());
                t = h * qt;
                mpz_powm(r.get_mpz_t(), g.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
                mpz_pow_ui(qt.get_mpz_t(), q.get_mpz_t(), c - it.second);
                _discrete_log(t, x, r, qt, q, c - it.second, p);
                t = -z * t;
                mpz_powm(r.get_mpz_t(), g.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
                v *= r;
                mpz_fdiv_r(v.get_mpz_t(), v.get_mpz_t(), p.get_mpz_t());
                s1 = v;
            }
        }
        root = s1;
    }
    r = n;
    unsigned c = 0;
    while(r % p == 0) {
        mpz_divexact(r.get_mpz_t(), r.get_mpz_t(), p.get_mpz_t());
        c++;
    }

    // Solve s == x**r mod p**k where (x**r)**(p**c)) == a mod p**k
    mpz_class pc = n / r, pd = pc * p;
    if (c >= 1) {
        mpz_powm(s.get_mpz_t(), root.get_mpz_t(), r.get_mpz_t(), p.get_mpz_t());
        // s == root**r mod p. Since s**(p**c) == 1 == a mod p**(c + 1), lift until p**k.
        for (unsigned d = c + 2; d <= k; d++) {
            t = 1 - pc;
            pd *= p;
            mpz_powm(t.get_mpz_t(), s.get_mpz_t(), t.get_mpz_t(), pd.get_mpz_t());
            t = (a * t - s) / pc;
            s += t;
        }
    } else {
        s = a;
    }

    // Solve x**r == s mod p**k given that root**r == s mod p and r % p != 0.
    mpz_class u;
    pd = p;
    for (unsigned d = 2; d < 2 * k; d *= 2) { // Hensel lifting
        t = r - 1;
        pd *= pd;
        if (d > k)
            pd = pk;
        mpz_powm(u.get_mpz_t(), root.get_mpz_t(), t.get_mpz_t(), pd.get_mpz_t());
        t = r * u;
        mpz_invert(t.get_mpz_t(), t.get_mpz_t(), pd.get_mpz_t());
        root += (s - u * root) * t;
        mpz_fdiv_r(root.get_mpz_t(), root.get_mpz_t(), pd.get_mpz_t());
    }
    if (m != 1 && all_roots) {
        // All roots are generated by root*(g**(phi / gcd(phi , n)))**j
        if (n == 2) {
            t = -1;
        } else {
            if (g == 0)
                _primitive_root(g, p, 2);
            t = phi / m;
            mpz_powm(t.get_mpz_t(), g.get_mpz_t(), t.get_mpz_t(), pk.get_mpz_t());
        }
        for (unsigned j = 0; j < m; j++) {
            roots.push_back(integer(root));
            root *= t;
            mpz_fdiv_r(root.get_mpz_t(), root.get_mpz_t(), pk.get_mpz_t());
        }
    } else {
        roots.push_back(integer(root));
    }
    return true;
}

// Solution for x**n == a mod p**k.
bool _nthroot_mod_prime_power(std::vector<RCP<const Integer>> &roots, const mpz_class &a, const mpz_class &n, 
        const mpz_class &p, const unsigned k, bool all_roots = false)
{
    mpz_class pk, root;
    std::vector<RCP<const Integer>> _roots;
    if (a % p != 0) {
        if (p == 2) {
            mpz_class r = n, t, s, pc, pj;
            pk = 0;
            mpz_setbit(pk.get_mpz_t(), k);
            unsigned c = mpz_scan1(n.get_mpz_t(), 0);
            mpz_tdiv_q_2exp (r.get_mpz_t(), n.get_mpz_t(), c); // n = 2**c * r where r is odd.

            // Handle special cases of k = 1 and k = 2.
            if (k == 1) {
                roots.push_back(integer(1));
                return true;
            }
            if (k == 2) {
                if (c > 0 && a % 4 == 3) {
                    return false;
                }
                roots.push_back(integer(a % 4));
                if (all_roots && c > 0)
                    roots.push_back(integer(3));
                return true;
            }
            if (c >= k - 2) {
                c = k - 2;  // Since x**(2**c) == x**(2**(k - 2)) mod 2**k, let c = k - 2.
            }
            t = 0; pc = 0;
            mpz_setbit(t.get_mpz_t(), k - 2);
            mpz_setbit(pc.get_mpz_t(), c);

            mpz_invert(s.get_mpz_t(), r.get_mpz_t(), t.get_mpz_t());
            if (c == 0) {
                // x**r == a mod 2**k and x**2**(k - 2) == 1 mod 2**k, implies x**(r * s) == x == a**s mod 2**k.
                mpz_powm(root.get_mpz_t(), a.get_mpz_t(), s.get_mpz_t(), pk.get_mpz_t());
                roots.push_back(integer(root));
                return true;
            }

            // First, solve for y**2**c == a mod 2**k where y == x**r
            t = 0;
            mpz_setbit(t.get_mpz_t(), c + 2);
            mpz_fdiv_r(t.get_mpz_t(), a.get_mpz_t(), t.get_mpz_t());
            // Check for a == y**2**c == 1 mod 2**(c + 2).
            if (t != 1)
                return false;
            root = 1;
            pj = pc * 4;
            // 1 is a root of x**2**c == 1 mod 2**(c + 2). Lift till 2**k.
            for (unsigned j = c + 2; j < k; j++) {
                pj *= 2;
                mpz_powm(t.get_mpz_t(), root.get_mpz_t(), pc.get_mpz_t(), pj.get_mpz_t());
                t -= a;
                if (t % pj != 0)
                    // Add 2**(j - c) by setting bit j - c = 1 (bit j - c is 0).
                    mpz_setbit(root.get_mpz_t(), j - c);
            }
            // Solve x**r == root mod 2**k.
            mpz_powm(root.get_mpz_t(), root.get_mpz_t(), s.get_mpz_t(), pk.get_mpz_t());

            if (all_roots) {
                // All roots are generated by, root * (j * (2**(k - c) +/- 1)).
                t = pk / pc * root;
                for (unsigned i = 0 ; i < 2; i++) {
                    for (unsigned long j = 0; j < pc; j++) {
                        roots.push_back(integer(root));
                        root += t;
                    }
                    root = t - root;
                }
            } else {
                roots.push_back(integer(root));
            }
            return true;
        } else {
            return _nthroot_mod1(roots, a, n, p, k, all_roots);
        }
    } else {
        mpz_class _a;
        mpz_pow_ui(pk.get_mpz_t(), p.get_mpz_t(), k);
        _a = a % pk;
        unsigned m;
        mpz_class pm;
        if (_a == 0) {
            if (!all_roots) {
                roots.push_back(integer(0));
                return true;
            }
            _roots.push_back(integer(0));
            if (n >= k)
                m = k - 1;
            else
                m = k - 1 - (k - 1) / n.get_ui();
            mpz_pow_ui(pm.get_mpz_t(), p.get_mpz_t(), m);
        } else {
            unsigned r = 1;
            mpz_divexact(_a.get_mpz_t(), _a.get_mpz_t(), p.get_mpz_t());
            while (_a % p == 0) {
                mpz_divexact(_a.get_mpz_t(), _a.get_mpz_t(), p.get_mpz_t());
                r++;
            }
            if (r < n || r % n != 0 || !_nthroot_mod_prime_power(_roots, _a, n, p, k - r, all_roots)) {
                return false;
            }
            m = r / n.get_ui();
            mpz_pow_ui(pm.get_mpz_t(), p.get_mpz_t(), m);
            if (!all_roots) {
                roots.push_back(integer(_roots.back()->as_mpz() * pm));
                return true;
            }
            for (auto &it : _roots) {
                it = integer (it->as_mpz() * pm);
            }
            m = r - r / n.get_ui();
            mpz_pow_ui(pm.get_mpz_t(), p.get_mpz_t(), m);
        }
        mpz_class pkm;
        mpz_pow_ui(pkm.get_mpz_t(), p.get_mpz_t(), k - m);

        for (auto &it : _roots) {
            root = it->as_mpz();
            for (unsigned long i = 0; i < pm; i++) {
                roots.push_back(integer(root));
                root += pkm;
            }
        }
    }
    return true;
}

bool nthroot_mod(const Ptr<RCP<const Integer>> &root, const RCP<const Integer> &a,
        const RCP<const Integer> &n, const RCP<const Integer> &mod)
{
    if (mod->as_mpz() <= 0) {
        return false;
    } else if (mod->as_mpz() == 1) {
        *root = integer(0);
        return true;
    }
    map_integer_uint prime_mul;
    prime_factor_multiplicities(prime_mul, *mod);
    std::vector<RCP<const Integer>> moduli;
    bool ret_val;
    mpz_class _mod;

    std::vector<RCP<const Integer>> rem;
    for (auto &it: prime_mul) {
        mpz_pow_ui(_mod.get_mpz_t(), it.first->as_mpz().get_mpz_t(), it.second);
        moduli.push_back(integer(_mod));
        ret_val = _nthroot_mod_prime_power(rem, a->as_mpz(), n->as_mpz(), it.first->as_mpz(), it.second, false);
        if(!ret_val) return false;
    }
    crt(root, rem, moduli);
    return true;
}

void nthroot_mod_list(std::vector<RCP<const Integer>> &roots, const RCP<const Integer> &a, 
        const RCP<const Integer> &n, const RCP<const Integer> &mod)
{
    if (mod->as_mpz() <= 0) {
        return;
    } else if (mod->as_mpz() == 1) {
        roots.push_back(integer(0));
        return;
    }
    map_integer_uint prime_mul;
    prime_factor_multiplicities(prime_mul, *mod);
    std::vector<RCP<const Integer>> moduli;
    bool ret_val;
    mpz_class _mod;

    std::vector<std::vector<RCP<const Integer>>> rem;
    for (auto &it: prime_mul) {
        mpz_pow_ui(_mod.get_mpz_t(), it.first->as_mpz().get_mpz_t(), it.second);
        moduli.push_back(integer(_mod));
        std::vector<RCP<const Integer>> rem1;
        ret_val = _nthroot_mod_prime_power(rem1, a->as_mpz(), n->as_mpz(), it.first->as_mpz(), it.second, true);
        if(!ret_val) return;
        rem.push_back(rem1);
    }
    _crt_cartesian(roots, rem, moduli);
    std::sort(roots.begin(), roots.end(), SymEngine::RCPIntegerKeyLess());
}

bool powermod(const Ptr<RCP<const Integer>> &powm, const RCP<const Integer> &a,
        const RCP<const Number> &b, const RCP<const Integer> &m)
{
    if(is_a<Integer>(*b)) {
        mpz_class t = rcp_static_cast<const Integer>(b)->as_mpz();
        if (b->is_negative())
            t *= -1;
        mpz_powm(t.get_mpz_t(), a->as_mpz().get_mpz_t(), t.get_mpz_t(), m->as_mpz().get_mpz_t());
        if (b->is_negative()) {
            bool ret_val = mpz_invert(t.get_mpz_t(), t.get_mpz_t(), m->as_mpz().get_mpz_t());
            if (!ret_val)
                return false;
        }
        *powm = integer(t);
        return true;
    } else if (is_a<Rational>(*b)) {
        RCP<const Integer> num, den, r;
        get_num_den(rcp_static_cast<const Rational>(b), outArg(num), outArg(den));
        if (den->is_negative()) {
            den = den->mulint(*integer(-1));
            num = num->mulint(*integer(-1));
        }
        mpz_class t = num->as_mpz();
        if (num->is_negative())
            t *= -1;
        mpz_powm(t.get_mpz_t(), a->as_mpz().get_mpz_t(), t.get_mpz_t(), m->as_mpz().get_mpz_t());
        if (num->is_negative()) {
            bool ret_val = mpz_invert(t.get_mpz_t(), t.get_mpz_t(), m->as_mpz().get_mpz_t());
            if (!ret_val)
                return false;
        }
        r = integer(t);
        return nthroot_mod(powm, r, den, m);
    }
    return false;
}

void powermod_list(std::vector<RCP<const Integer>> &pows, const RCP<const Integer> &a,
        const RCP<const Number> &b, const RCP<const Integer> &m)
{
    if(is_a<Integer>(*b)) {
        mpz_class t = rcp_static_cast<const Integer>(b)->as_mpz();
        if (b->is_negative())
            t *= -1;
        mpz_powm(t.get_mpz_t(), a->as_mpz().get_mpz_t(), t.get_mpz_t(), m->as_mpz().get_mpz_t());
        if (b->is_negative()) {
            bool ret_val = mpz_invert(t.get_mpz_t(), t.get_mpz_t(), m->as_mpz().get_mpz_t());
            if (!ret_val)
                return;
        }
        pows.push_back(integer(t));
    } else if (is_a<Rational>(*b)) {
        RCP<const Integer> num, den, r;
        get_num_den(rcp_static_cast<const Rational>(b), outArg(num), outArg(den));
        if (den->is_negative()) {
            den = den->mulint(*integer(-1));
            num = num->mulint(*integer(-1));
        }
        mpz_class t = num->as_mpz();
        if (num->is_negative())
            t *= -1;
        mpz_powm(t.get_mpz_t(), a->as_mpz().get_mpz_t(), t.get_mpz_t(), m->as_mpz().get_mpz_t());
        if (num->is_negative()) {
            bool ret_val = mpz_invert(t.get_mpz_t(), t.get_mpz_t(), m->as_mpz().get_mpz_t());
            if (!ret_val)
                return;
        }
        r = integer(t);
        nthroot_mod_list(pows, r, den, m);
    }
}

} // SymEngine

