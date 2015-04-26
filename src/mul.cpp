#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "complex.h"
#include "functions.h"
#include "constants.h"

namespace CSymPy {

Mul::Mul(const RCP<const Number> &coef, map_basic_basic&& dict)
    : coef_{coef}, dict_{std::move(dict)}
{
    CSYMPY_ASSERT(is_canonical(coef, dict_))
}

bool Mul::is_canonical(const RCP<const Number> &coef,
        const map_basic_basic& dict)
{
    if (coef == null) return false;
    // e.g. 0*x*y
    if (coef->is_zero())
        return false;
    if (dict.size() == 0) return false;
    if (dict.size() == 1) {
        // e.g. 1*x, 1*x^2
        if (coef->is_one()) return false;
    }
    // Check that each term in 'dict' is in canonical form
    for (auto &p: dict) {
        if (p.first == null) return false;
        if (p.second == null) return false;
        // e.g. 2^3, (2/3)^4
        // However for Complex no simplification is done
        if ((is_a<Integer>(*p.first) || is_a<Rational>(*p.first))
            && is_a<Integer>(*p.second))
            return false;
        // e.g. 0^x
        if (is_a<Integer>(*p.first) &&
                rcp_static_cast<const Integer>(p.first)->is_zero())
            return false;
        // e.g. 1^x
        if (is_a<Integer>(*p.first) &&
                rcp_static_cast<const Integer>(p.first)->is_one())
            return false;
        // e.g. x^0
        if (is_a<Integer>(*p.second) &&
                rcp_static_cast<const Integer>(p.second)->is_zero())
            return false;
        // e.g. (x*y)^2 (={xy:2}), which should be represented as x^2*y^2
        //     (={x:2, y:2})
        if (is_a<Mul>(*p.first))
            return false;
        // e.g. x^2^y (={x^2:y}), which should be represented as x^(2y)
        //     (={x:2y})
        if (is_a<Pow>(*p.first))
            return false;
    }
    return true;
}

std::size_t Mul::__hash__() const
{
    std::size_t seed = MUL;
    hash_combine<Basic>(seed, *coef_);
    for (auto &p: dict_) {
        hash_combine<Basic>(seed, *(p.first));
        hash_combine<Basic>(seed, *(p.second));
    }
    return seed;
}

bool Mul::__eq__(const Basic &o) const
{
    if (is_a<Mul>(o) &&
        eq(coef_, static_cast<const Mul &>(o).coef_) &&
        map_basic_basic_eq(dict_, static_cast<const Mul &>(o).dict_))
        return true;

    return false;
}

int Mul::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Mul>(o))
    const Mul &s = static_cast<const Mul &>(o);
    // # of elements
    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    // coef
    int cmp = coef_->__cmp__(*s.coef_);
    if (cmp != 0)
        return cmp;

    // Compare dictionaries:
    return map_basic_basic_compare(dict_, s.dict_);
}

RCP<const CSymPy::Basic> Mul::from_dict(const RCP<const Number> &coef, map_basic_basic &&d)
{
    if (coef->is_zero()) return zero;
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if (coef->is_one()) {
                if ((rcp_static_cast<const Integer>(p->second))->is_one()) {
                    // For x^1 we simply return "x":
                    return p->first;
                }
            } else {
                // For coef*x or coef*x^3 we simply return Mul:
                return rcp(new Mul(coef, std::move(d)));
            }
        }
        if (coef->is_one()) {
            // Create a Pow() here:
            return pow(p->first, p->second);
        } else {
            return rcp(new Mul(coef, std::move(d)));
        }
    } else {
        return rcp(new Mul(coef, std::move(d)));
    }
}

// Mul (t^exp) to the dict "d"
void Mul::dict_add_term(map_basic_basic &d, const RCP<const Basic> &exp,
        const RCP<const Basic> &t)
{
    auto it = d.find(t);
    if (it == d.end()) {
        insert(d, t, exp);
    } else {
        // Very common case, needs to be fast:
        if (is_a_Number(*it->second) && is_a_Number(*exp)) {
            RCP<const Number> tmp = rcp_static_cast<const Number>(it->second);
            iaddnum(outArg(tmp), rcp_static_cast<const Number>(exp));
            if (tmp->is_zero()) {
                d.erase(it);
            } else {
                it->second = tmp;
            }
        } else {
            // General case:
            it->second = add(it->second, exp);
            if (is_a<Integer>(*it->second) &&
                    rcp_static_cast<const Integer>(it->second)->is_zero()) {
                d.erase(it);
            }
        }
    }
}

// Mul (t^exp) to the dict "d"
void Mul::dict_add_term_new(const Ptr<RCP<const Number>> &coef, map_basic_basic &d,
    const RCP<const Basic> &exp, const RCP<const Basic> &t)
{
    auto it = d.find(t);
    if (it == d.end()) {
        // Don't check for `exp = 0` here
        // `pow` for Complex is not expanded by default
        if (is_a<Integer>(*t) || is_a<Rational>(*t)) {
            if (is_a<Integer>(*exp)) {
                imulnum(outArg(*coef), pownum(rcp_static_cast<const Number>(t),
                    rcp_static_cast<const Number>(exp)));
            } else if (is_a<Rational>(*exp)) {
                // Here we make the exponent postive and a fraction between
                // 0 and 1.
                mpz_class q, r, num, den;
                num = rcp_static_cast<const Rational>(exp)->i.get_num();
                den = rcp_static_cast<const Rational>(exp)->i.get_den();
                mpz_fdiv_qr(q.get_mpz_t(), r.get_mpz_t(), num.get_mpz_t(),
                    den.get_mpz_t());
                
                insert(d, t, Rational::from_mpq(mpq_class(r, den)));
                imulnum(outArg(*coef), pownum(rcp_static_cast<const Number>(t),
                    rcp_static_cast<const Number>(integer(q))));
            } else {
                insert(d, t, exp);
            }
        } else if (is_a<Integer>(*exp) && is_a<Complex>(*t)) {
            if (rcp_static_cast<const Integer>(exp)->is_one()) {
                imulnum(outArg(*coef), rcp_static_cast<const Number>(t));
            } else if (rcp_static_cast<const Integer>(exp)->is_minus_one()) {
                idivnum(outArg(*coef), rcp_static_cast<const Number>(t));
            } else {
                insert(d, t, exp);
            }
        } else {
            insert(d, t, exp);
        }
    } else {
        // Very common case, needs to be fast:
        if (is_a_Number(*exp) && is_a_Number(*it->second)) {
            RCP<const Number> tmp = rcp_static_cast<const Number>(it->second);
            iaddnum(outArg(tmp),
                rcp_static_cast<const Number>(exp));
            it->second = tmp;
        }
        else
            it->second = add(it->second, exp);

        if (is_a<Integer>(*it->second)) {
            // `pow` for Complex is not expanded by default
            if (is_a<Integer>(*t) || is_a<Rational>(*t)) {
                if (!rcp_static_cast<const Integer>(it->second)->is_zero()) {
                    imulnum(outArg(*coef), pownum(rcp_static_cast<const Number>(t),
                        rcp_static_cast<const Number>(it->second)));
                }
                d.erase(it);
            } else if (rcp_static_cast<const Integer>(it->second)->is_zero()) {
                d.erase(it);
            } else if (is_a<Complex>(*t)) {
                if (rcp_static_cast<const Integer>(it->second)->is_one()) {
                    imulnum(outArg(*coef), rcp_static_cast<const Number>(t));
                    d.erase(it);
                } else if (rcp_static_cast<const Integer>(it->second)->is_minus_one()) {
                    idivnum(outArg(*coef), rcp_static_cast<const Number>(t));
                    d.erase(it);
                }
            }
        } else if (is_a<Rational>(*it->second)) {
            if (is_a_Number(*t)) {
                mpz_class q, r, num, den;
                num = rcp_static_cast<const Rational>(it->second)->i.get_num();
                den = rcp_static_cast<const Rational>(it->second)->i.get_den();
                // Here we make the exponent postive and a fraction between
                // 0 and 1.
                if (num > den || num < 0) {
                    mpz_fdiv_qr(q.get_mpz_t(), r.get_mpz_t(), num.get_mpz_t(),
                                den.get_mpz_t());

                    it->second = Rational::from_mpq(mpq_class(r, den));
                    imulnum(outArg(*coef), pownum(rcp_static_cast<const Number>(t),
                                                  rcp_static_cast<const Number>(integer(q))));
                }
            }
        }
    }
}

void Mul::as_two_terms(const Ptr<RCP<const Basic>> &a,
            const Ptr<RCP<const Basic>> &b) const
{
    // Example: if this=3*x^2*y^2*z^2, then a=x^2 and b=3*y^2*z^2
    auto p = dict_.begin();
    *a = pow(p->first, p->second);
    map_basic_basic d = dict_;
    d.erase(p->first);
    *b = Mul::from_dict(coef_, std::move(d));
}

void Mul::as_base_exp(const RCP<const Basic> &self, const Ptr<RCP<const Basic>> &exp,
        const Ptr<RCP<const Basic>> &base)
{
    if (is_a_Number(*self)) {
        // Always ensure it is of form |num| > |den|
        // in case of Integers den = 1
        if (is_a<Rational>(*self)) {
            RCP<const Rational> self_new = rcp_static_cast<const Rational>(self);
            if (abs(self_new->i.get_num()) < abs(self_new->i.get_den())) {
                *exp = minus_one;
                *base = self_new->rdiv(*rcp_static_cast<const Number>(one));
            } else {
                *exp = one;
                *base = self;
            }
        } else {
            *exp = one;
            *base = self;
        }
    } else if (is_a<Pow>(*self)) {
        *exp = rcp_static_cast<const Pow>(self)->exp_;
        *base = rcp_static_cast<const Pow>(self)->base_;
    } else {
        CSYMPY_ASSERT(!is_a<Mul>(*self));
        *exp = one;
        *base = self;
    }
}

RCP<const Basic> mul(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    CSymPy::map_basic_basic d;
    RCP<const Number> coef = one;
    if (CSymPy::is_a<Mul>(*a) && CSymPy::is_a<Mul>(*b)) {
        RCP<const Mul> A = rcp_static_cast<const Mul>(a);
        RCP<const Mul> B = rcp_static_cast<const Mul>(b);
        // This is important optimization, as coef=1 if Mul is inside an Add.
        // To further speed this up, the upper level code could tell us that we
        // are inside an Add, then we don't even have can simply skip the
        // following two lines.
        if (!(A->coef_->is_one()) || !(B->coef_->is_one()))
            coef = mulnum(A->coef_, B->coef_);
        d = A->dict_;
        for (auto &p: B->dict_)
            Mul::dict_add_term_new(outArg(coef), d, p.second, p.first);
    } else if (CSymPy::is_a<Mul>(*a)) {
        RCP<const Basic> exp;
        RCP<const Basic> t;
        coef = (rcp_static_cast<const Mul>(a))->coef_;
        d = (rcp_static_cast<const Mul>(a))->dict_;
        if (is_a_Number(*b)) {
            imulnum(outArg(coef), rcp_static_cast<const Number>(b));
        } else {
            Mul::as_base_exp(b, outArg(exp), outArg(t));
            Mul::dict_add_term_new(outArg(coef), d, exp, t);
        }
    } else if (CSymPy::is_a<Mul>(*b)) {
        RCP<const Basic> exp;
        RCP<const Basic> t;
        coef = (rcp_static_cast<const Mul>(b))->coef_;
        d = (rcp_static_cast<const Mul>(b))->dict_;
        if (is_a_Number(*a)) {
            imulnum(outArg(coef), rcp_static_cast<const Number>(a));
        } else {
            Mul::as_base_exp(a, outArg(exp), outArg(t));
            Mul::dict_add_term_new(outArg(coef), d, exp, t);
        }
    } else {
        RCP<const Basic> exp;
        RCP<const Basic> t;
        Mul::as_base_exp(a, outArg(exp), outArg(t));
        Mul::dict_add_term_new(outArg(coef), d, exp, t);
        Mul::as_base_exp(b, outArg(exp), outArg(t));
        Mul::dict_add_term_new(outArg(coef), d, exp, t);
    }
    return Mul::from_dict(coef, std::move(d));
}

RCP<const Basic> div(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    return mul(a, pow(b, minus_one));
}

RCP<const Basic> neg(const RCP<const Basic> &a)
{
    return mul(minus_one, a);
}

RCP<const Basic> mul_expand_two(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    // Both a and b are assumed to be expanded
    if (is_a<Add>(*a) && is_a<Add>(*b)) {
        RCP<const Number> coef = mulnum(rcp_static_cast<const Add>(a)->coef_,
            rcp_static_cast<const Add>(b)->coef_);
        umap_basic_num d;
        // Improves (x+1)^3(x+2)^3...(x+350)^3 expansion from 0.97s to 0.93s:
        d.reserve((rcp_static_cast<const Add>(a))->dict_.size()*
            (rcp_static_cast<const Add>(b))->dict_.size());
        // Expand dicts first:
        for (auto &p: (rcp_static_cast<const Add>(a))->dict_) {
            for (auto &q: (rcp_static_cast<const Add>(b))->dict_) {
                // The main bottleneck here is the mul(p.first, q.first) command
                RCP<const Basic> term = mul(p.first, q.first);
                if (is_a_Number(*term)) {
                    iaddnum(outArg(coef),
                        mulnum(mulnum(p.second, q.second), rcp_static_cast<const Number>(term)));
                } else {
                    if (is_a<Mul>(*term) &&
                        !(rcp_static_cast<const Mul>(term)->coef_->is_one())) {
                        // Tidy up things like {2x: 3} -> {x: 6}
                        RCP<const Number> coef2 =
                                rcp_static_cast<const Mul>(term)->coef_;
                        // We make a copy of the dict_:
                        map_basic_basic d2 = rcp_static_cast<const Mul>(term)->dict_;
                        term = Mul::from_dict(one, std::move(d2));
                        Add::dict_add_term(d, mulnum(mulnum(p.second, q.second), coef2), term);
                    } else {
                        Add::dict_add_term(d, mulnum(p.second, q.second), term);
                    }
                }
            }
            Add::dict_add_term(d,
                    mulnum(rcp_static_cast<const Add>(b)->coef_, p.second),
                    p.first);
        }
        // Handle the coefficient of "a":
        for (auto &q: (rcp_static_cast<const Add>(b))->dict_) {
            Add::dict_add_term(d,
                    mulnum(rcp_static_cast<const Add>(a)->coef_, q.second),
                    q.first);
        }
        return Add::from_dict(coef, std::move(d));
    } else if (is_a<Add>(*a)) {
        return mul_expand_two(b, a);
    } else if (is_a<Add>(*b)) {
        RCP<const Number> a_coef;
        RCP<const Basic> a_term;
        Add::as_coef_term(a, outArg(a_coef), outArg(a_term));

        RCP<const Number> coef = zero;
        umap_basic_num d;
        d.reserve((rcp_static_cast<const Add>(b))->dict_.size());
        for (auto &q: (rcp_static_cast<const Add>(b))->dict_) {
            RCP<const Basic> term = mul(a_term, q.first);
            if (is_a_Number(*term)) {
                iaddnum(outArg(coef), mulnum(mulnum(q.second, a_coef),
                        rcp_static_cast<const Number>(term)));
            } else {
                if (is_a<Mul>(*term) &&
                    !(rcp_static_cast<const Mul>(term)->coef_->is_one())) {
                    // Tidy up things like {2x: 3} -> {x: 6}
                    RCP<const Number> coef2 =
                            rcp_static_cast<const Mul>(term)->coef_;
                    // We make a copy of the dict_:
                    map_basic_basic d2 = rcp_static_cast<const Mul>(term)->dict_;
                    term = Mul::from_dict(one, std::move(d2));
                    Add::dict_add_term(d, mulnum(mulnum(q.second, a_coef), coef2), term);
                } else {
                    Add::dict_add_term(d, mulnum(a_coef, q.second), term);
                }
            }
        }
        if (eq(a_term, one)) {
            iaddnum(outArg(coef),
                mulnum(rcp_static_cast<const Add>(b)->coef_, a_coef));
        } else {
            Add::dict_add_term(d,
                    mulnum(rcp_static_cast<const Add>(b)->coef_, a_coef),
                    a_term);
        }
        return Add::from_dict(coef, std::move(d));
    }
    return mul(a, b);
}

RCP<const Basic> mul_expand(const RCP<const Mul> &self)
{
    RCP<const Basic> a, b;
    self->as_two_terms(outArg(a), outArg(b));
    a = expand(a);
    b = expand(b);
    return mul_expand_two(a, b);
}

RCP<const Basic> Mul::power_all_terms(const RCP<const Basic> &exp) const
{
    CSymPy::map_basic_basic d;
    RCP<const Basic> new_coef = pow(coef_, exp);
    RCP<const Number> coef_num = one;
    RCP<const Basic> new_exp;
    for (auto &p: dict_) {
        new_exp = mul(p.second, exp);
        if (is_a_Number(*new_exp)) {
            // No need for additional dict checks here.
            // The dict should be of standard form before this is
            // called.
            if (rcp_static_cast<const Number>(new_exp)->is_zero()) {
                continue;
            } else {
                Mul::dict_add_term_new(outArg(coef_num), d, new_exp, p.first);
            }
        } else {
            Mul::dict_add_term_new(outArg(coef_num), d, new_exp, p.first);
        }
    }
    if (is_a_Number(*new_coef)) {
        imulnum(outArg(coef_num), rcp_static_cast<const Number>(new_coef));
    }  else if (is_a<Mul>(*new_coef)) {
        RCP<const Mul> tmp = rcp_static_cast<const Mul>(new_coef);
        imulnum(outArg(coef_num), tmp->coef_);
        for (auto &q: tmp->dict_) {
            Mul::dict_add_term_new(outArg(coef_num), d, q.second, q.first);
        }
    } else {
        RCP<const Basic> _exp, t;
        Mul::as_base_exp(new_coef, outArg(_exp), outArg(t));
        Mul::dict_add_term_new(outArg(coef_num), d, _exp, t);
    }
    return Mul::from_dict(coef_num, std::move(d));
}

RCP<const Basic> Mul::diff(const RCP<const Symbol> &x) const
{
    RCP<const Number> overall_coef = zero;
    umap_basic_num add_dict;
    for (auto &p: dict_) {
        RCP<const Number> coef = coef_;
        RCP<const Basic> factor = pow(p.first, p.second)->diff(x);
        if (is_a<Integer>(*factor) &&
                rcp_static_cast<const Integer>(factor)->is_zero()) continue;
        map_basic_basic d = dict_;
        d.erase(p.first);
        if (is_a_Number(*factor)) {
            imulnum(outArg(coef), rcp_static_cast<const Number>(factor));
        } else if (is_a<Mul>(*factor)) {
            RCP<const Mul> tmp = rcp_static_cast<const Mul>(factor);
            imulnum(outArg(coef), tmp->coef_);
            for (auto &q: tmp->dict_) {
                Mul::dict_add_term_new(outArg(coef), d, q.second, q.first);
            }
        } else {
            RCP<const Basic> exp, t;
            Mul::as_base_exp(factor, outArg(exp), outArg(t));
            Mul::dict_add_term_new(outArg(coef), d, exp, t);
        }
        if (d.size() == 0) {
            iaddnum(outArg(overall_coef), coef);
        } else {
            RCP<const Basic> mul = Mul::from_dict(one, std::move(d));
            Add::dict_add_term(add_dict, coef, mul);
        }
    }
    return Add::from_dict(overall_coef, std::move(add_dict));
}

RCP<const Basic> Mul::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Mul> self = rcp_const_cast<Mul>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;

    RCP<const Number> coef = coef_;
    map_basic_basic d;
    for (auto &p: dict_) {
        RCP<const Basic> factor_old = pow(p.first, p.second);
        RCP<const Basic> factor = factor_old->subs(subs_dict);
        if (factor == factor_old) {
            Mul::dict_add_term_new(outArg(coef), d, p.second, p.first);
        } else if (is_a<Integer>(*factor) &&
                rcp_static_cast<const Integer>(factor)->is_zero()) {
            return zero;
        } else if (is_a_Number(*factor)) {
            imulnum(outArg(coef), rcp_static_cast<const Number>(factor));
        } else if (is_a<Mul>(*factor)) {
            RCP<const Mul> tmp = rcp_static_cast<const Mul>(factor);
            imulnum(outArg(coef), tmp->coef_);
            for (auto &q: tmp->dict_) {
                Mul::dict_add_term_new(outArg(coef), d, q.second, q.first);
            }
        } else {
            RCP<const Basic> exp, t;
            Mul::as_base_exp(factor, outArg(exp), outArg(t));
            Mul::dict_add_term_new(outArg(coef), d, exp, t);
        }
    }
    return Mul::from_dict(coef, std::move(d));
}

vec_basic Mul::get_args() const {
    vec_basic args;
    if (!coef_->is_one()) args.push_back(coef_);
    for (auto &p: dict_) {
        args.push_back(Mul::from_dict(one, {{p.first, p.second}}));
    }
    return args;
}

} // CSymPy
