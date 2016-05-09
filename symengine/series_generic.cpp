#include <algorithm>
#include <exception>
#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>
#include <valarray>
#include <typeinfo>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine
{

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t,
                                                     const std::string &x,
                                                     unsigned int prec)
{
    UnivariateExprPolynomial p(UnivariatePolynomial::create(symbol(x), {0, 1}));
    SeriesVisitor<UnivariateExprPolynomial, Expression, UnivariateSeries>
        visitor(std::move(p), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const
{
    return p_.get_univariate_poly()->hash()
           + std::size_t(get_degree() * 84728863L);
}

int UnivariateSeries::compare(const Basic &other) const
{
    SYMENGINE_ASSERT(is_a<UnivariateSeries>(other))
    const UnivariateSeries &o = static_cast<const UnivariateSeries &>(other);
    return p_.get_basic()->__cmp__(*o.p_.get_basic());
}

RCP<const Basic> UnivariateSeries::as_basic() const
{
    return p_.get_basic();
}

umap_int_basic UnivariateSeries::as_dict() const
{
    umap_int_basic map;
    for (const auto &it : p_.get_univariate_poly()->get_dict())
        if (it.second != 0)
            map[it.first] = it.second.get_basic();
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int deg) const
{
    if (p_.get_univariate_poly()->get_dict().count(deg) == 0)
        return zero;
    else
        return p_.get_univariate_poly()->get_dict().at(deg).get_basic();
}

UnivariateExprPolynomial UnivariateSeries::var(const std::string &s)
{
    return UnivariateExprPolynomial(
        UnivariatePolynomial::create(symbol(s), {0, 1}));
}

Expression UnivariateSeries::convert(const Basic &x)
{
    return Expression(x.rcp_from_this());
}

int UnivariateSeries::ldegree(const UnivariateExprPolynomial &s)
{
    return s.get_univariate_poly()->get_dict().begin()->first;
}

typedef std::complex<Expression> base;

base operator*(const base &a, const base &b)
{
    return base(a.real() * b.real() - a.imag() * b.imag(),
                a.imag() * b.real() + a.real() * b.imag());
}

typedef std::vector<base> CArray;

// Optimized Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
void fft(CArray &x)
{
    // DFT
    unsigned int N = x.size(), k = N, n;
    double thetaT = M_PI / N;
    base phiT(std::cos(thetaT), std::sin(thetaT)), T;
    while (k > 1) {
        n = k;
        k >>= 1;
        phiT *= phiT;
        T = 1.0L;
        for (unsigned int l = 0; l < k; l++) {
            for (unsigned int a = l; a < N; a += n) {
                unsigned int b = a + k;
                base t = x[a] - x[b];
                x[a] += x[b];
                x[b] = t * T;
            }
            T *= phiT;
        }
    }
    for (const auto& it : x)
        std::cout << it << " " << typeid(*it.real().get_basic()).name() << std::endl;
    // Decimate
    unsigned int m = (unsigned int)log2(N);
    for (unsigned int a = 0; a < N; a++) {
        unsigned int b = a;
        // Reverse bits
        b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
        b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
        b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
        b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if (b > a) {
            base t = x[a];
            x[a] = x[b];
            x[b] = t;
        }
    }
}

// Inverse fft (in-place)
void ifft(CArray &x)
{
    // Conjugate the complex numbers
    for_each(x.begin(), x.end(), [](base &c) { c = std::conj(c); });
    // forward fft
    fft(x);

    // Conjugate the complex numbers again
    for_each(x.begin(), x.end(), [](base &c) { c = std::conj(c); });

    // Scale the numbers
    for (size_t i = 0; i < x.size(); i++)
        x[i] /= x.size();
}

UnivariateExprPolynomial
UnivariateSeries::mul(const UnivariateExprPolynomial &a,
                      const UnivariateExprPolynomial &b, unsigned prec)
{
    unsigned long n = 1, t = a.get_univariate_poly()->get_degree()
                             + b.get_univariate_poly()->get_degree() + 1;
    while (n <= t)
        n <<= 1;

    CArray fa(n), fb(n);

    for (int i = 0;
         i <= std::max(a.get_univariate_poly()->get_degree(),
                       b.get_univariate_poly()->get_degree());
         i++) {
        fa[i] = base(a.find_cf(i));
        fb[i] = base(b.find_cf(i));
    }

    fft(fa), fft(fb);
    for (unsigned long i = 0; i < n; ++i)
        fa[i] *= fb[i];
    ifft(fa);

    std::vector<Expression> res(n);
    for (unsigned long i = 0; i < prec && i <= t; ++i) {
        res[i] = fa[i].real();
    }
    if (a.get_univariate_poly()->get_var()->get_name() == "")
        return UnivariateExprPolynomial(UnivariatePolynomial::create(
            b.get_univariate_poly()->get_var(), std::move(res)));
    else
        return UnivariateExprPolynomial(UnivariatePolynomial::create(
            a.get_univariate_poly()->get_var(), std::move(res)));
}

UnivariateExprPolynomial
UnivariateSeries::pow(const UnivariateExprPolynomial &base, int exp,
                      unsigned prec)
{
    if (exp < 0) {
        SYMENGINE_ASSERT(base.get_univariate_poly()->get_dict().size() == 1)
        map_int_Expr dict;
        dict[-(base.get_univariate_poly()->get_dict().begin()->first)]
            = 1 / base.get_univariate_poly()->get_dict().begin()->second;
        return pow(UnivariateExprPolynomial(univariate_polynomial(
                       base.get_univariate_poly()->get_var(), std::move(dict))),
                   -exp, prec);
    }
    if (exp == 0) {
        if (base == 0) {
            throw std::runtime_error("Error: 0**0 is undefined.");
        } else {
            return UnivariateExprPolynomial(1);
        }
    }

    UnivariateExprPolynomial x(base);
    UnivariateExprPolynomial y(1);
    while (exp > 1) {
        if (exp % 2 == 0) {
            x = mul(x, x, prec);
            exp /= 2;
        } else {
            y = mul(x, y, prec);
            x = mul(x, x, prec);
            exp = (exp - 1) / 2;
        }
    }
    return mul(x, y, prec);
}

Expression UnivariateSeries::find_cf(const UnivariateExprPolynomial &s,
                                     const UnivariateExprPolynomial &var,
                                     int deg)
{
    if (s.get_univariate_poly()->get_dict().count(deg) == 0)
        return Expression(0);
    else
        return (s.get_univariate_poly()->get_dict()).at(deg);
}

Expression UnivariateSeries::root(Expression &c, unsigned n)
{
    return pow_ex(c, 1 / Expression(n));
}

UnivariateExprPolynomial
UnivariateSeries::diff(const UnivariateExprPolynomial &s,
                       const UnivariateExprPolynomial &var)
{
    RCP<const Basic> p
        = s.get_univariate_poly()->diff(var.get_univariate_poly()->get_var());
    if (is_a<const UnivariatePolynomial>(*p))
        return UnivariateExprPolynomial(
            rcp_static_cast<const UnivariatePolynomial>(p));
    else
        throw std::runtime_error("Not a UnivariatePolynomial");
}

UnivariateExprPolynomial
UnivariateSeries::integrate(const UnivariateExprPolynomial &s,
                            const UnivariateExprPolynomial &var)
{
    map_int_Expr dict;
    for (auto &it : s.get_univariate_poly()->get_dict()) {
        if (it.first != -1) {
            dict.insert(std::pair<int, Expression>(it.first + 1,
                                                   it.second / (it.first + 1)));
        } else {
            throw std::runtime_error("Not Implemented");
        }
    }
    return UnivariateExprPolynomial(univariate_polynomial(
        var.get_univariate_poly()->get_var(), std::move(dict)));
}

UnivariateExprPolynomial
UnivariateSeries::subs(const UnivariateExprPolynomial &s,
                       const UnivariateExprPolynomial &var,
                       const UnivariateExprPolynomial &r, unsigned prec)
{
    UnivariateExprPolynomial result(
        r.get_univariate_poly()->get_var()->get_name());
    for (auto &i : s.get_univariate_poly()->get_dict())
        result += i.second * pow(r, i.first, prec);
    return result;
}

Expression UnivariateSeries::sin(const Expression &c)
{
    return SymEngine::sin(c.get_basic());
}

Expression UnivariateSeries::cos(const Expression &c)
{
    return SymEngine::cos(c.get_basic());
}

Expression UnivariateSeries::tan(const Expression &c)
{
    return SymEngine::tan(c.get_basic());
}

Expression UnivariateSeries::asin(const Expression &c)
{
    return SymEngine::asin(c.get_basic());
}

Expression UnivariateSeries::acos(const Expression &c)
{
    return SymEngine::acos(c.get_basic());
}

Expression UnivariateSeries::atan(const Expression &c)
{
    return SymEngine::atan(c.get_basic());
}

Expression UnivariateSeries::sinh(const Expression &c)
{
    return SymEngine::sinh(c.get_basic());
}

Expression UnivariateSeries::cosh(const Expression &c)
{
    return SymEngine::cosh(c.get_basic());
}

Expression UnivariateSeries::tanh(const Expression &c)
{
    return SymEngine::tanh(c.get_basic());
}

Expression UnivariateSeries::asinh(const Expression &c)
{
    return SymEngine::asinh(c.get_basic());
}

Expression UnivariateSeries::atanh(const Expression &c)
{
    return SymEngine::atanh(c.get_basic());
}

Expression UnivariateSeries::exp(const Expression &c)
{
    return SymEngine::exp(c.get_basic());
}

Expression UnivariateSeries::log(const Expression &c)
{
    return SymEngine::log(c.get_basic());
}

} // SymEngine
