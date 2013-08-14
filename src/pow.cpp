#include <stdexcept>

#include "pow.h"
#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "integer.h"
#include "rational.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;

namespace CSymPy {

Pow::Pow(const Teuchos::RCP<Basic> &base, const Teuchos::RCP<Basic> &exp)
    : base_{base}, exp_{exp}
{
    CSYMPY_ASSERT(is_canonical(base, exp))
}

bool Pow::is_canonical(const RCP<Basic> &base, const RCP<Basic> &exp)
{
    if (base == Teuchos::null) return false;
    if (exp == Teuchos::null) return false;
    // e.g. 0^x
    if (is_a<Integer>(*base) && rcp_static_cast<Integer>(base)->is_zero())
        return false;
    // e.g. 1^x
    if (is_a<Integer>(*base) && rcp_static_cast<Integer>(base)->is_one())
        return false;
    // e.g. x^0
    if (is_a<Integer>(*exp) && rcp_static_cast<Integer>(exp)->is_zero())
        return false;
    // e.g. x^1
    if (is_a<Integer>(*exp) && rcp_static_cast<Integer>(exp)->is_one())
        return false;
    // e.g. 2^3, (2/3)^4
    if (is_a_Number(*base) && is_a<Integer>(*exp))
        return false;
    // e.g. (x*y)^2, should rather by x^2*y^2
    if (is_a<Mul>(*base))
        return false;
    return true;
}

std::size_t Pow::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *base_);
    hash_combine<Basic>(seed, *exp_);
    return seed;
}

bool Pow::__eq__(const Basic &o) const
{
    if (is_a<Pow>(o) &&
        eq(base_, static_cast<const Pow &>(o).base_) &&
        eq(exp_, static_cast<const Pow &>(o).exp_))
            return true;

    return false;
}

std::string Pow::__str__() const
{
    std::ostringstream o;
    if (is_a<Add>(*base_)) {
        o << "(" << *base_ << ")";
    } else {
        o << *base_;
    }
    o << "^" << *exp_;
    return o.str();
}

RCP<Basic> pow(const RCP<Basic> &a, const RCP<Basic> &b)
{
    if (eq(b, zero)) return one;
    if (eq(b, one)) return a;
    if (eq(a, zero)) return zero;
    if (eq(a, one)) return one;
    if (is_a_Number(*a) && is_a<Integer>(*b))
        return pownum(rcp_static_cast<Number>(a), rcp_static_cast<Integer>(b));
    if (is_a<Mul>(*a))
        return rcp_static_cast<Mul>(a)->power_all_terms(b);
    return rcp(new Pow(a, b));
}

// This function can overflow, but it is fast.
// TODO: figure out condition for (m, n) when it overflows and raise an
// exception.
void multinomial_coefficients(int m, int n, map_vec_int &r)
{
    vec_int t;
    int j, tj, start, k;
    long long int v;
    if (m < 2)
        throw std::runtime_error("multinomial_coefficients: m >= 2 must hold.");
    if (n < 0)
        throw std::runtime_error("multinomial_coefficients: n >= 0 must hold.");
    t.assign(m, 0);
    t[0] = n;
    r[t] = 1;
    if (n == 0) return;
    j = 0;
    while (j < m - 1) {
        tj = t[j];
        if (j) {
            t[j] = 0;
            t[0] = tj;
        }
        if (tj > 1) {
            t[j+1] += 1;
            j = 0;
            start = 1;
            v = 0;
        } else {
            j += 1;
            start = j + 1;
            v = r[t];
            t[j] += 1;
        }
        for (k=start; k<m; k++) {
            if (t[k]) {
                t[k] -= 1;
                v += r[t];
                t[k] += 1;
            }
        }
        t[0] -= 1;
        r[t] = (v*tj) / (n-t[0]);
    }
}

// Slower, but returns exact (possibly large) integers (as mpz)
void multinomial_coefficients_mpz(int m, int n, map_vec_mpz &r)
{
    vec_int t;
    int j, tj, start, k;
    mpz_class v;
    if (m < 2)
        throw std::runtime_error("multinomial_coefficients: m >= 2 must hold.");
    if (n < 0)
        throw std::runtime_error("multinomial_coefficients: n >= 0 must hold.");
    t.assign(m, 0);
    t[0] = n;
    r[t] = 1;
    if (n == 0) return;
    j = 0;
    while (j < m - 1) {
        tj = t[j];
        if (j) {
            t[j] = 0;
            t[0] = tj;
        }
        if (tj > 1) {
            t[j+1] += 1;
            j = 0;
            start = 1;
            v = 0;
        } else {
            j += 1;
            start = j + 1;
            v = r[t];
            t[j] += 1;
        }
        for (k=start; k<m; k++) {
            if (t[k]) {
                t[k] -= 1;
                v += r[t];
                t[k] += 1;
            }
        }
        t[0] -= 1;
        r[t] = (v*tj) / (n-t[0]);
    }
}

RCP<Basic> pow_expand(const RCP<Pow> &self)
{
    if (is_a<Integer>(*self->exp_)) {
        if (is_a<Add>(*self->base_)) {
            map_vec_mpz r;
            int n = rcp_static_cast<Integer>(self->exp_)->as_int();

            RCP<Add> base = rcp_static_cast<Add>(self->base_);
            umap_basic_int base_dict = base->dict_;
            if (! (base->coef_->is_zero())) {
                // Add the numerical coefficient into the dictionary. This
                // allows a little bit easier treatment below.
                base_dict[base->coef_] = one;
            }
            int m = base_dict.size();
            multinomial_coefficients_mpz(m, n, r);
            umap_basic_int rd;
            RCP<Number> add_overall_coeff=zero;
            for (auto &p: r) {
                auto power = p.first.begin();
                auto i2 = base_dict.begin();
                map_basic_basic d;
                RCP<Number> overall_coeff=one;
                for (; power != p.first.end(); ++power, ++i2) {
                    if (*power > 0) {
                        RCP<Integer> exp = rcp(new Integer(*power));
                        RCP<Basic> base = i2->first;
                        if (is_a<Integer>(*base)) {
                            imulnum(outArg(overall_coeff),
                                rcp_static_cast<Number>(
                                rcp_static_cast<Integer>(base)->powint(*exp)));
                        } else {
                            d[base] = exp;
                        }
                        if (!(i2->second->is_one())) {
                            imulnum(outArg(overall_coeff),
                                pownum(i2->second,
                                    rcp_static_cast<Number>(exp)));
                        }
                    }
                }
                RCP<Basic> term = Mul::from_dict(overall_coeff, d);
                RCP<Number> coef2 = rcp(new Integer(p.second));
                if (is_a_Number(*term)) {
                    iaddnum(outArg(add_overall_coeff),
                        mulnum(rcp_static_cast<Number>(term), coef2));
                } else {
                    if (is_a<Mul>(*term) &&
                            !(rcp_static_cast<Mul>(term)->coef_->is_one())) {
                        // Tidy up things like {2x: 3} -> {x: 6}
                        imulnum(outArg(coef2),
                                rcp_static_cast<Mul>(term)->coef_);
                        term = Mul::from_dict(one,
                                rcp_static_cast<Mul>(term)->dict_);
                    }
                    rd[term] = coef2;
                }
            }
            RCP<Basic> result = Add::from_dict(add_overall_coeff, rd);
            return result;
        }
    }
    return self;
}

} // CSymPy
