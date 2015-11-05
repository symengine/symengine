#include <stdexcept>

#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/symbol.h>
#include <symengine/pow.h>
#include <symengine/complex.h>
#include <symengine/functions.h>
#include <symengine/constants.h>

namespace SymEngine {

Mul::Mul(const RCP<const Number> &coef, map_basic_basic&& dict)
    : coef_{coef}, dict_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(coef, dict_))
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
        // e.g. 1*x, 1*x**2
        if (coef->is_one()) return false;
    }
    // Check that each term in 'dict' is in canonical form
    for (const auto &p: dict) {
        if (p.first == null) return false;
        if (p.second == null) return false;
        // e.g. 2**3, (2/3)**4
        // However for Complex no simplification is done
        if ((is_a<Integer>(*p.first) or is_a<Rational>(*p.first))
            and is_a<Integer>(*p.second))
            return false;
        // e.g. 0**x
        if (is_a<Integer>(*p.first) and
                rcp_static_cast<const Integer>(p.first)->is_zero())
            return false;
        // e.g. 1**x
        if (is_a<Integer>(*p.first) and
                rcp_static_cast<const Integer>(p.first)->is_one())
            return false;
        // e.g. x**0
        if (is_a_Number(*p.second) and
                rcp_static_cast<const Number>(p.second)->is_zero())
            return false;
        // e.g. (x*y)**2 (={xy:2}), which should be represented as x**2*y**2
        //     (={x:2, y:2})
        if (is_a<Mul>(*p.first)) {
            if (is_a<Integer>(*p.second)) return false;
            if (neq(*static_cast<const Mul &>(*p.first).coef_, *one) and
                neq(*static_cast<const Mul &>(*p.first).coef_, *minus_one))
                return false;
        }
        // e.g. x**2**y (={x**2:y}), which should be represented as x**(2y)
        //     (={x:2y})
        if (is_a<Pow>(*p.first))
            return false;
        // e.g. 0.5^2.0 should be represented as 0.25
        if(is_a_Number(*p.first) and not rcp_static_cast<const Number>(p.first)->is_exact() and
                is_a_Number(*p.second) and not rcp_static_cast<const Number>(p.second)->is_exact())
            return false;
    }
    return true;
}

std::size_t Mul::__hash__() const
{
    std::size_t seed = MUL;
    hash_combine<Basic>(seed, *coef_);
    for (const auto &p: dict_) {
        hash_combine<Basic>(seed, *(p.first));
        hash_combine<Basic>(seed, *(p.second));
    }
    return seed;
}

bool Mul::__eq__(const Basic &o) const
{
    if (is_a<Mul>(o) and
        eq(*coef_, *(static_cast<const Mul &>(o).coef_)) and
        map_eq(dict_, static_cast<const Mul &>(o).dict_))
        return true;

    return false;
}

int Mul::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Mul>(o))
    const Mul &s = static_cast<const Mul &>(o);
    // # of elements
    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    // coef
    int cmp = coef_->__cmp__(*s.coef_);
    if (cmp != 0)
        return cmp;

    // Compare dictionaries:
    return map_compare(dict_, s.dict_);
}

RCP<const SymEngine::Basic> Mul::from_dict(const RCP<const Number> &coef, map_basic_basic &&d)
{
    if (coef->is_zero()) return coef;
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if (coef->is_one()) {
                if ((rcp_static_cast<const Integer>(p->second))->is_one()) {
                    // For x**1 we simply return "x":
                    return p->first;
                }
            } else {
                // For coef*x or coef*x**3 we simply return Mul:
                return make_rcp<const Mul>(coef, std::move(d));
            }
        }
        if (coef->is_one()) {
            // Create a Pow() here:
            if (eq(*p->second, *one)) {
                return p->first;
            }
            return make_rcp<const Pow>(p->first, p->second);
        } else {
            return make_rcp<const Mul>(coef, std::move(d));
        }
    } else {
        return make_rcp<const Mul>(coef, std::move(d));
    }
}

// Mul (t**exp) to the dict "d"
void Mul::dict_add_term(map_basic_basic &d, const RCP<const Basic> &exp,
        const RCP<const Basic> &t)
{
    auto it = d.find(t);
    if (it == d.end()) {
        insert(d, t, exp);
    } else {
        // Very common case, needs to be fast:
        if (is_a_Number(*it->second) and is_a_Number(*exp)) {
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
            if (is_a_Number(*it->second) and
                    rcp_static_cast<const Number>(it->second)->is_zero()) {
                d.erase(it);
            }
        }
    }
}

// Mul (t**exp) to the dict "d"
void Mul::dict_add_term_new(const Ptr<RCP<const Number>> &coef, map_basic_basic &d,
    const RCP<const Basic> &exp, const RCP<const Basic> &t)
{
    auto it = d.find(t);
    if (it == d.end()) {
        // Don't check for `exp = 0` here
        // `pow` for Complex is not expanded by default
        if (is_a<Integer>(*t) or is_a<Rational>(*t)) {
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
        } else if (is_a<Integer>(*exp) and is_a<Complex>(*t)) {
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
        if (is_a_Number(*exp) and is_a_Number(*it->second)) {
            RCP<const Number> tmp = rcp_static_cast<const Number>(it->second);
            iaddnum(outArg(tmp),
                rcp_static_cast<const Number>(exp));
            it->second = tmp;
        }
        else
            it->second = add(it->second, exp);

        if (is_a<Integer>(*it->second)) {
            // `pow` for Complex is not expanded by default
            if (is_a<Integer>(*t) or is_a<Rational>(*t)) {
                if (not rcp_static_cast<const Integer>(it->second)->is_zero()) {
                    imulnum(outArg(*coef), pownum(rcp_static_cast<const Number>(t),
                        rcp_static_cast<const Number>(it->second)));
                }
                d.erase(it);
                return;
            } else if (rcp_static_cast<const Integer>(it->second)->is_zero()) {
                d.erase(it);
                return;
            } else if (is_a<Complex>(*t)) {
                if (rcp_static_cast<const Integer>(it->second)->is_one()) {
                    imulnum(outArg(*coef), rcp_static_cast<const Number>(t));
                    d.erase(it);
                } else if (rcp_static_cast<const Integer>(it->second)->is_minus_one()) {
                    idivnum(outArg(*coef), rcp_static_cast<const Number>(t));
                    d.erase(it);
                }
                return;
            }
        } else if (is_a<Rational>(*it->second)) {
            if (is_a_Number(*t)) {
                mpz_class q, r, num, den;
                num = rcp_static_cast<const Rational>(it->second)->i.get_num();
                den = rcp_static_cast<const Rational>(it->second)->i.get_den();
                // Here we make the exponent postive and a fraction between
                // 0 and 1.
                if (num > den or num < 0) {
                    mpz_fdiv_qr(q.get_mpz_t(), r.get_mpz_t(), num.get_mpz_t(),
                                den.get_mpz_t());

                    it->second = Rational::from_mpq(mpq_class(r, den));
                    imulnum(outArg(*coef), pownum(rcp_static_cast<const Number>(t),
                                                  rcp_static_cast<const Number>(integer(q))));
                }
                return;
            }
        }
        if (is_a_Number(*it->second)) {
            if (static_cast<const Number &>(*it->second).is_zero()) {
                // In 1*x**0.0, result should be 1.0
                imulnum(outArg(*coef), pownum(rcp_static_cast<const Number>(it->second), zero));
                d.erase(it);
            } else if (is_a<Mul>(*it->first)) {
                RCP<const Mul> m = rcp_static_cast<const Mul>(it->first);
                if (is_a<Integer>(*it->second) or (neq(*m->coef_, *one) and neq(*m->coef_, *minus_one))) {
                    RCP<const Number> exp_ = rcp_static_cast<const Number>(it->second);
                    d.erase(it);
                    m->power_num(outArg(*coef), d, exp_);
                }
            }
        }
    }
}

void Mul::as_two_terms(const Ptr<RCP<const Basic>> &a,
            const Ptr<RCP<const Basic>> &b) const
{
    // Example: if this=3*x**2*y**2*z**2, then a=x**2 and b=3*y**2*z**2
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
        *exp = rcp_static_cast<const Pow>(self)->get_exp();
        *base = rcp_static_cast<const Pow>(self)->get_base();
    } else {
        SYMENGINE_ASSERT(not is_a<Mul>(*self));
        *exp = one;
        *base = self;
    }
}

RCP<const Basic> mul(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    SymEngine::map_basic_basic d;
    RCP<const Number> coef = one;
    if (SymEngine::is_a<Mul>(*a) and SymEngine::is_a<Mul>(*b)) {
        RCP<const Mul> A = rcp_static_cast<const Mul>(a);
        RCP<const Mul> B = rcp_static_cast<const Mul>(b);
        // This is important optimization, as coef=1 if Mul is inside an Add.
        // To further speed this up, the upper level code could tell us that we
        // are inside an Add, then we don't even have can simply skip the
        // following two lines.
        if (not (A->coef_->is_one()) or not (B->coef_->is_one()))
            coef = mulnum(A->coef_, B->coef_);
        d = A->dict_;
        for (const auto &p: B->dict_)
            Mul::dict_add_term_new(outArg(coef), d, p.second, p.first);
    } else if (SymEngine::is_a<Mul>(*a)) {
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
    } else if (SymEngine::is_a<Mul>(*b)) {
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
        if (is_a_Number(*a)) {
            imulnum(outArg(coef), rcp_static_cast<const Number>(a));
        } else {
            Mul::as_base_exp(a, outArg(exp), outArg(t));
            Mul::dict_add_term_new(outArg(coef), d, exp, t);
        }
        if (is_a_Number(*b)) {
            imulnum(outArg(coef), rcp_static_cast<const Number>(b));
        } else {
            Mul::as_base_exp(b, outArg(exp), outArg(t));
            Mul::dict_add_term_new(outArg(coef), d, exp, t);
        }
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
    if (is_a<Add>(*a) and is_a<Add>(*b)) {
        RCP<const Number> coef = mulnum(rcp_static_cast<const Add>(a)->coef_,
            rcp_static_cast<const Add>(b)->coef_);
        umap_basic_num d;
        // Improves (x+1)**3*(x+2)**3*...(x+350)**3 expansion from 0.97s to 0.93s:
#if defined(HAVE_SYMENGINE_RESERVE)
        d.reserve((rcp_static_cast<const Add>(a))->dict_.size()*
            (rcp_static_cast<const Add>(b))->dict_.size());
#endif
        // Expand dicts first:
        for (const auto &p: (rcp_static_cast<const Add>(a))->dict_) {
            for (const auto &q: (rcp_static_cast<const Add>(b))->dict_) {
                // The main bottleneck here is the mul(p.first, q.first) command
                RCP<const Basic> term = mul(p.first, q.first);
                if (is_a_Number(*term)) {
                    iaddnum(outArg(coef),
                        mulnum(mulnum(p.second, q.second), rcp_static_cast<const Number>(term)));
                } else {
                    if (is_a<Mul>(*term) and
                        not (rcp_static_cast<const Mul>(term)->coef_->is_one())) {
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
        for (const auto &q: (rcp_static_cast<const Add>(b))->dict_) {
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
#if defined(HAVE_SYMENGINE_RESERVE)
        d.reserve((rcp_static_cast<const Add>(b))->dict_.size());
#endif
        for (const auto &q: (rcp_static_cast<const Add>(b))->dict_) {
            RCP<const Basic> term = mul(a_term, q.first);
            if (is_a_Number(*term)) {
                iaddnum(outArg(coef), mulnum(mulnum(q.second, a_coef),
                        rcp_static_cast<const Number>(term)));
            } else {
                if (is_a<Mul>(*term) and
                    not (rcp_static_cast<const Mul>(term)->coef_->is_one())) {
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
        if (eq(*a_term, *one)) {
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

void Mul::power_num(const Ptr<RCP<const Number>> &coef, map_basic_basic &d,
                    const RCP<const Number> &exp) const
{
    if (is_a_Number(*exp) and rcp_static_cast<const Number>(exp)->is_zero()) {
        // (x*y)**(0.0) should return 1.0
        imulnum(coef, pownum(rcp_static_cast<const Number>(exp), zero));
        return;
    }
    RCP<const Basic> new_coef;
    RCP<const Basic> new_exp;
    if (is_a<Integer>(*exp)) {
        // For eg. (3*y*(x**(1/2))**2 should be expanded to 9*x*y**2
        new_coef = pow(coef_, exp);
        for (const auto &p: dict_) {
            new_exp = mul(p.second, exp);
            if (is_a<Integer>(*new_exp) and is_a<Mul>(*p.first)) {
                static_cast<const Mul &>(*p.first).power_num(coef, d, rcp_static_cast<const Number>(new_exp));
            } else {
                // No need for additional dict checks here.
                // The dict should be of standard form before this is
                // called.
                Mul::dict_add_term_new(coef, d, new_exp, p.first);
            }
        }
    } else {
        if (coef_->is_negative()) {
            // (3*x*y)**(1/2) -> 3**(1/2)*(x*y)**(1/2)
            new_coef = pow(coef_->mul(*minus_one), exp);
            map_basic_basic d1 = dict_;
            Mul::dict_add_term_new(coef, d, exp, Mul::from_dict(minus_one, std::move(d1)));
        } else if (coef_->is_positive()) {
            // (-3*x*y)**(1/2) -> 3**(1/2)*(-x*y)**(1/2)
            new_coef = pow(coef_, exp);
            map_basic_basic d1 = dict_;
            Mul::dict_add_term_new(coef, d, exp, Mul::from_dict(one, std::move(d1)));
        } else {
            // ((1+2*I)*x*y)**(1/2) is kept as it is
            new_coef = one;
            Mul::dict_add_term_new(coef, d, exp, this->rcp_from_this());
        }
    }
    if (is_a_Number(*new_coef)) {
        imulnum(coef, rcp_static_cast<const Number>(new_coef));
    }  else if (is_a<Mul>(*new_coef)) {
        RCP<const Mul> tmp = rcp_static_cast<const Mul>(new_coef);
        imulnum(coef, tmp->coef_);
        for (const auto &q: tmp->dict_) {
            Mul::dict_add_term_new(coef, d, q.second, q.first);
        }
    } else {
        RCP<const Basic> _exp, t;
        Mul::as_base_exp(new_coef, outArg(_exp), outArg(t));
        Mul::dict_add_term_new(coef, d, _exp, t);
    }
}

RCP<const Basic> Mul::diff(const RCP<const Symbol> &x) const
{
    RCP<const Number> overall_coef = zero;
    umap_basic_num add_dict;
    for (const auto &p: dict_) {
        RCP<const Number> coef = coef_;
        RCP<const Basic> factor = pow(p.first, p.second)->diff(x);
        if (is_a_Number(*factor) and
                rcp_static_cast<const Number>(factor)->is_zero()) {
            // overall_coef += factor is done to coerce overall_coef to the type of factor
            iaddnum(outArg(overall_coef), rcp_static_cast<const Number>(factor));
            continue;
        }
        map_basic_basic d = dict_;
        d.erase(p.first);
        if (is_a_Number(*factor)) {
            imulnum(outArg(coef), rcp_static_cast<const Number>(factor));
        } else if (is_a<Mul>(*factor)) {
            RCP<const Mul> tmp = rcp_static_cast<const Mul>(factor);
            imulnum(outArg(coef), tmp->coef_);
            for (const auto &q: tmp->dict_) {
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
    RCP<const Mul> self = rcp_from_this_cast<const Mul>();
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;

    RCP<const Number> coef = coef_;
    map_basic_basic d;
    for (const auto &p: dict_) {
        RCP<const Basic> factor_old = pow(p.first, p.second);
        RCP<const Basic> factor = factor_old->subs(subs_dict);
        if (factor == factor_old) {
            Mul::dict_add_term_new(outArg(coef), d, p.second, p.first);
        } else if (is_a_Number(*factor)) {
            if (rcp_static_cast<const Number>(factor)->is_zero()) {
                return factor;
            }
            imulnum(outArg(coef), rcp_static_cast<const Number>(factor));
        } else if (is_a<Mul>(*factor)) {
            RCP<const Mul> tmp = rcp_static_cast<const Mul>(factor);
            imulnum(outArg(coef), tmp->coef_);
            for (const auto &q: tmp->dict_) {
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
    if (not coef_->is_one()) args.push_back(coef_);
    for (const auto &p: dict_) {
        args.push_back(Mul::from_dict(one, {{p.first, p.second}}));
    }
    return args;
}

} // SymEngine
