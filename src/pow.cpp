#include <stdexcept>

#include "pow.h"
#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "integer.h"
#include "rational.h"

namespace CSymPy {

Pow::Pow(const RCP<const Basic> &base, const RCP<const Basic> &exp)
    : base_{base}, exp_{exp}
{
    CSYMPY_ASSERT(is_canonical(base, exp))
}

bool Pow::is_canonical(const RCP<const Basic> &base, const RCP<const Basic> &exp)
{
    if (base == null) return false;
    if (exp == null) return false;
    // e.g. 0^x
    if (is_a<Integer>(*base) && rcp_static_cast<const Integer>(base)->is_zero())
        return false;
    // e.g. 1^x
    if (is_a<Integer>(*base) && rcp_static_cast<const Integer>(base)->is_one())
        return false;
    // e.g. x^0
    if (is_a<Integer>(*exp) && rcp_static_cast<const Integer>(exp)->is_zero())
        return false;
    // e.g. x^1
    if (is_a<Integer>(*exp) && rcp_static_cast<const Integer>(exp)->is_one())
        return false;
    // e.g. 2^3, (2/3)^4
    if (is_a_Number(*base) && is_a<Integer>(*exp))
        return false;
    // e.g. (x*y)^2, should rather be x^2*y^2
    if (is_a<Mul>(*base))
        return false;
    // e.g. x^2^y, should rather be x^(2*y)
    if (is_a<Pow>(*base))
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

int Pow::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Pow>(o))
    const Pow &s = static_cast<const Pow &>(o);
    int base_cmp = base_->__cmp__(*s.base_);
    if (base_cmp == 0)
        return exp_->__cmp__(*s.exp_);
    else
        return base_cmp;
}


std::string Pow::__str__() const
{
    std::ostringstream o;
    if (is_a<Add>(*base_)) {
        o << "(" << *base_ << ")";
    } else {
        o << *base_;
    }
    o << "^";
    if (is_a<Add>(*exp_) || is_a<Pow>(*exp_)) {
        o << "(" << *exp_ << ")";
    } else {
        o << *exp_;
    }
    return o.str();
}

RCP<const Basic> pow(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    if (eq(b, zero)) return one;
    if (eq(b, one)) return a;
    if (eq(a, zero)) return zero;
    if (eq(a, one)) return one;
    if (is_a_Number(*a) && is_a<Integer>(*b))
        return pownum(rcp_static_cast<const Number>(a), rcp_static_cast<const Integer>(b));
    if (is_a<Mul>(*a))
        return rcp_static_cast<const Mul>(a)->power_all_terms(b);
    if (is_a<Pow>(*a)) {
        RCP<const Pow> A = rcp_static_cast<const Pow>(a);
        return pow(A->base_, mul(A->exp_, b));
    }
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

RCP<const Basic> pow_expand(const RCP<const Pow> &self)
{
    if (is_a<Integer>(*self->exp_)) {
        if (is_a<Add>(*self->base_)) {
            map_vec_mpz r;
            int n = rcp_static_cast<const Integer>(self->exp_)->as_int();

            RCP<const Add> base = rcp_static_cast<const Add>(self->base_);
            umap_basic_int base_dict = base->dict_;
            if (! (base->coef_->is_zero())) {
                // Add the numerical coefficient into the dictionary. This
                // allows a little bit easier treatment below.
                insert(base_dict, base->coef_, one);
            }
            int m = base_dict.size();
            multinomial_coefficients_mpz(m, n, r);
            umap_basic_int rd;
            // This speeds up overall expansion. For example for the benchmark
            // (y + x + z + w)^60 it improves the timing from 135ms to 124ms.
            rd.reserve(2*r.size());
            RCP<const Number> add_overall_coeff=zero;
            for (auto &p: r) {
                auto power = p.first.begin();
                auto i2 = base_dict.begin();
                map_basic_basic d;
                RCP<const Number> overall_coeff=one;
                for (; power != p.first.end(); ++power, ++i2) {
                    if (*power > 0) {
                        RCP<const Integer> exp = rcp(new Integer(*power));
                        RCP<const Basic> base = i2->first;
                        if (is_a<Integer>(*base)) {
                            imulnum(outArg(overall_coeff),
                                rcp_static_cast<const Number>(
                                rcp_static_cast<const Integer>(base)->powint(*exp)));
                        } else if (is_a<Symbol>(*base)) {
                            Mul::dict_add_term(d, exp, base);
                        } else {
                            RCP<const Basic> exp2, t, tmp;
                            tmp = pow(base, exp);
                            Mul::as_base_exp(tmp, outArg(exp2), outArg(t));
                            Mul::dict_add_term(d, exp2, t);
                        }
                        if (!(i2->second->is_one())) {
                            imulnum(outArg(overall_coeff),
                                pownum(i2->second,
                                    rcp_static_cast<const Number>(exp)));
                        }
                    }
                }
                RCP<const Basic> term = Mul::from_dict(overall_coeff, d);
                RCP<const Number> coef2 = rcp(new Integer(p.second));
                if (is_a_Number(*term)) {
                    iaddnum(outArg(add_overall_coeff),
                        mulnum(rcp_static_cast<const Number>(term), coef2));
                } else {
                    if (is_a<Mul>(*term) &&
                            !(rcp_static_cast<const Mul>(term)->coef_->is_one())) {
                        // Tidy up things like {2x: 3} -> {x: 6}
                        imulnum(outArg(coef2),
                                rcp_static_cast<const Mul>(term)->coef_);
                        term = Mul::from_dict(one,
                                rcp_static_cast<const Mul>(term)->dict_);
                    }
                    Add::dict_add_term(rd, coef2, term);
                }
            }
            RCP<const Basic> result = Add::from_dict(add_overall_coeff, rd);
            return result;
        }
    }
    return self;
}

RCP<const Basic> Pow::diff(const RCP<const Symbol> &x) const
{
    if (is_a_Number(*exp_))
        return mul(mul(exp_, pow(base_, sub(exp_, one))), base_->diff(x));
    throw std::runtime_error("Not implemented.");
}

RCP<const Basic> Pow::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Pow> self = rcp_const_cast<Pow>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> base_new = base_->subs(subs_dict);
    RCP<const Basic> exp_new = exp_->subs(subs_dict);
    if (base_new == base_ && exp_new == exp_)
        return self;
    else
        return pow(base_new, exp_new);
}

} // CSymPy
