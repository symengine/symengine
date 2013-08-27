#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "rational.h"
#include "functions.h"


namespace CSymPy {

Mul::Mul(const RCP<Number> &coef, const map_basic_basic& dict)
    : coef_{coef}, dict_{dict}
{
    CSYMPY_ASSERT(is_canonical(coef, dict))
}

bool Mul::is_canonical(const RCP<Number> &coef,
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
        if (is_a_Number(*p.first) && is_a<Integer>(*p.second))
            return false;
        // e.g. 0^x
        if (is_a<Integer>(*p.first) &&
                rcp_static_cast<Integer>(p.first)->is_zero())
            return false;
        // e.g. 1^x
        if (is_a<Integer>(*p.first) &&
                rcp_static_cast<Integer>(p.first)->is_one())
            return false;
        // e.g. x^0
        if (is_a<Integer>(*p.second) &&
                rcp_static_cast<Integer>(p.second)->is_zero())
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
    std::size_t seed = 0;
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
        map_basic_basic_equal(dict_, static_cast<const Mul &>(o).dict_))
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

std::string Mul::__str__() const
{
    std::ostringstream o;
    if (neq(coef_, one))
        o << *coef_;
    for (auto &p: dict_) {
        if (is_a<Add>(*p.first)) o << "(";
        o << *(p.first);
        if (is_a<Add>(*p.first)) o << ")";
        if (neq(p.second, one))
            o << "^" << *(p.second);
        o << "*";
    }
    std::string s = o.str();
    return s.substr(0, s.size()-1);
}

RCP<CSymPy::Basic> Mul::from_dict(const RCP<Number> &coef, const map_basic_basic &d)
{
    if (coef->is_zero()) return zero;
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if (coef->is_one()) {
                if ((rcp_static_cast<Integer>(p->second))->is_one()) {
                    // For x^1 we simply return "x":
                    return p->first;
                }
            } else {
                // For coef*x or coef*x^3 we simply return Mul:
                return rcp(new Mul(coef, d));
            }
        }
        if (coef->is_one()) {
            // Create a Pow() here:
            return pow(p->first, p->second);
        } else {
            return rcp(new Mul(coef, d));
        }
    } else {
        return rcp(new Mul(coef, d));
    }
}

// Mul (t^exp) to the dict "d"
void Mul::dict_add_term(map_basic_basic &d, const RCP<Basic> &exp,
        const RCP<Basic> &t)
{
    auto it = d.find(t);
    if (it == d.end()) {
        insert(d, t, exp);
    } else {
        // Very common case, needs to be fast:
        if (is_a_Number(*it->second) && is_a_Number(*exp)) {
            RCP<Number> tmp = rcp_static_cast<Number>(it->second);
            iaddnum(outArg(tmp), rcp_static_cast<Number>(exp));
            if (tmp->is_zero()) {
                d.erase(it);
            } else {
                it->second = tmp;
            }
        } else {
            // General case:
            it->second = add(it->second, exp);
            if (is_a<Integer>(*it->second) &&
                    rcp_static_cast<Integer>(it->second)->is_zero()) {
                d.erase(it);
            }
        }
    }
}

void Mul::as_two_terms(const Ptr<RCP<Basic>> &a,
            const Ptr<RCP<Basic>> &b)
{
    // Example: if this=3*x^2*y^2*z^2, then a=x^2 and b=3*y^2*z^2
    auto p = dict_.begin();
    *a = pow(p->first, p->second);
    map_basic_basic d = dict_;
    d.erase(p->first);
    *b = Mul::from_dict(coef_, d);
}

void Mul::as_base_exp(const RCP<Basic> &self, const Ptr<RCP<Basic>> &exp,
        const Ptr<RCP<Basic>> &base)
{
    if (is_a<Symbol>(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a_Number(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a<Pow>(*self)) {
        *exp = rcp_static_cast<Pow>(self)->exp_;
        *base = rcp_static_cast<Pow>(self)->base_;
    } else if (is_a<Add>(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a_sub<Function>(*self)) {
        *exp = one;
        *base = self;
    } else {
        std::cout << "as_base_exp: " << *self << std::endl;
        throw std::runtime_error("Not implemented yet.");
    }
}

RCP<Basic> mul(const RCP<Basic> &a, const RCP<Basic> &b)
{
    CSymPy::map_basic_basic d;
    RCP<Number> coef = one;
    if (CSymPy::is_a<Mul>(*a) && CSymPy::is_a<Mul>(*b)) {
        RCP<Mul> A = rcp_static_cast<Mul>(a);
        RCP<Mul> B = rcp_static_cast<Mul>(b);
        // This is important optimization, as coef=1 if Mul is inside an Add.
        // To further speed this up, the upper level code could tell us that we
        // are inside an Add, then we don't even have can simply skip the
        // following two lines.
        if (!(A->coef_->is_one()) || !(B->coef_->is_one()))
            coef = mulnum(A->coef_, B->coef_);
        d = A->dict_;
        for (auto &p: B->dict_)
            Mul::dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Mul>(*a)) {
        RCP<Basic> exp;
        RCP<Basic> t;
        coef = (rcp_static_cast<Mul>(a))->coef_;
        d = (rcp_static_cast<Mul>(a))->dict_;
        if (is_a_Number(*b)) {
            imulnum(outArg(coef), rcp_static_cast<Number>(b));
        } else {
            Mul::as_base_exp(b, outArg(exp), outArg(t));
            Mul::dict_add_term(d, exp, t);
        }
    } else if (CSymPy::is_a<Mul>(*b)) {
        RCP<Basic> exp;
        RCP<Basic> t;
        coef = (rcp_static_cast<Mul>(b))->coef_;
        d = (rcp_static_cast<Mul>(b))->dict_;
        if (is_a_Number(*a)) {
            imulnum(outArg(coef), rcp_static_cast<Number>(a));
        } else {
            Mul::as_base_exp(a, outArg(exp), outArg(t));
            Mul::dict_add_term(d, exp, t);
        }
    } else {
        RCP<Basic> exp;
        RCP<Basic> t;
        Mul::as_base_exp(a, outArg(exp), outArg(t));
        insert(d, t, exp);
        Mul::as_base_exp(b, outArg(exp), outArg(t));
        Mul::dict_add_term(d, exp, t);

        CSymPy::map_basic_basic d2;
        for (auto &p: d) {
            if (is_a_Number(*(p.first)) && is_a<Integer>(*(p.second))) {
                RCP<Number> f = rcp_static_cast<Number>(p.first);
                RCP<Integer> s = rcp_static_cast<Integer>(p.second);
                RCP<Number> r = pownum(f, s);
                imulnum(outArg(coef), r);
            } else {
                // TODO: this can be spedup
                Mul::dict_add_term(d2, p.second, p.first);
            }
        }
        return Mul::from_dict(coef, d2);
    }
    return Mul::from_dict(coef, d);
}

RCP<Basic> div(const RCP<Basic> &a, const RCP<Basic> &b)
{
    return mul(a, pow(b, minus_one));
}

RCP<Basic> neg(const RCP<Basic> &a)
{
    return mul(minus_one, a);
}

RCP<Basic> mul_expand_two(const RCP<Basic> &a, const RCP<Basic> &b)
{
    // Both a and b are assumed to be expanded
    if (is_a<Add>(*a) && is_a<Add>(*b)) {
        RCP<Number> coef = mulnum(rcp_static_cast<Add>(a)->coef_,
            rcp_static_cast<Add>(b)->coef_);
        umap_basic_int d;
        // Improves (x+1)^3(x+2)^3...(x+350)^3 expansion from 0.97s to 0.93s:
        d.reserve((rcp_static_cast<Add>(a))->dict_.size()*
            (rcp_static_cast<Add>(b))->dict_.size());
        // Expand dicts first:
        for (auto &p: (rcp_static_cast<Add>(a))->dict_) {
            for (auto &q: (rcp_static_cast<Add>(b))->dict_) {
                // The main bottleneck here is the mul(p.first, q.first) command
                RCP<Basic> term = mul(p.first, q.first);
                if (is_a_Number(*term)) {
                    iaddnum(outArg(coef), rcp_static_cast<Number>(term));
                } else {
                    Add::dict_add_term(d, mulnum(p.second, q.second), term);
                }
            }
            Add::dict_add_term(d,
                    mulnum(rcp_static_cast<Add>(b)->coef_, p.second),
                    p.first);
        }
        // Handle the coefficient of "a":
        for (auto &q: (rcp_static_cast<Add>(b))->dict_) {
            Add::dict_add_term(d,
                    mulnum(rcp_static_cast<Add>(a)->coef_, q.second),
                    q.first);
        }
        return Add::from_dict(coef, d);
    } else if (is_a<Add>(*a)) {
        return mul_expand_two(b, a);
    } else if (is_a<Add>(*b)) {
        RCP<Number> a_coef;
        RCP<Basic> a_term;
        Add::as_coef_term(a, outArg(a_coef), outArg(a_term));

        RCP<Number> coef = zero;
        umap_basic_int d;
        d.reserve((rcp_static_cast<Add>(b))->dict_.size());
        for (auto &q: (rcp_static_cast<Add>(b))->dict_) {
            RCP<Basic> term = mul(a_term, q.first);
            if (is_a_Number(*term)) {
                iaddnum(outArg(coef), rcp_static_cast<Number>(term));
            } else {
                Add::dict_add_term(d, mulnum(a_coef, q.second), term);
            }
        }
        if (eq(a_term, one)) {
            iaddnum(outArg(coef),
                mulnum(rcp_static_cast<Add>(b)->coef_, a_coef));
        } else {
            Add::dict_add_term(d,
                    mulnum(rcp_static_cast<Add>(b)->coef_, a_coef),
                    a_term);
        }
        return Add::from_dict(coef, d);
    }
    return mul(a, b);
}

RCP<Basic> mul_expand(const RCP<Mul> &self)
{
    RCP<Basic> a, b;
    self->as_two_terms(outArg(a), outArg(b));
    a = expand(a);
    b = expand(b);
    return mul_expand_two(a, b);
}

RCP<Basic> Mul::power_all_terms(const RCP<Basic> &exp)
{
    CSymPy::map_basic_basic d;
    RCP<Basic> new_coef = pow(coef_, exp);
    RCP<Basic> new_exp;
    for (auto &p: dict_) {
        new_exp = mul(p.second, exp);
        if (is_a<Integer>(*new_exp) &&
                rcp_static_cast<Integer>(new_exp)->is_zero()) continue;
        Mul::dict_add_term(d, new_exp, p.first);
    }
    if (is_a_Number(*new_coef)) {
        return Mul::from_dict(rcp_static_cast<Number>(new_coef), d);
    } else {
        // TODO: this can be made faster probably:
        return mul(new_coef, Mul::from_dict(one, d));
    }
}

RCP<Basic> Mul::diff(const RCP<Symbol> &x) const
{
    RCP<Basic> r=zero;
    for (auto &p: dict_) {
        RCP<Number> coef = coef_;
        RCP<Basic> factor = pow(p.first, p.second)->diff(x);
        if (is_a<Integer>(*factor) &&
                rcp_static_cast<Integer>(factor)->is_zero()) continue;
        map_basic_basic d = dict_;
        d.erase(p.first);
        if (is_a_Number(*factor)) {
            imulnum(outArg(coef), rcp_static_cast<Number>(factor));
        } else if (is_a<Mul>(*factor)) {
            RCP<Mul> tmp = rcp_static_cast<Mul>(factor);
            imulnum(outArg(coef), tmp->coef_);
            for (auto &q: tmp->dict_) {
                Mul::dict_add_term(d, q.second, q.first);
            }
        } else {
            RCP<Basic> exp, t;
            Mul::as_base_exp(factor, outArg(exp), outArg(t));
            Mul::dict_add_term(d, exp, t);
        }
        // TODO: speed this up:
        r = add(r, Mul::from_dict(coef, d));
    }
    return r;
}

RCP<Basic> Mul::subs(const map_basic_basic &subs_dict) const
{
    RCP<Mul> self = rcp_const_cast<Mul>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;

    RCP<Number> coef = coef_;
    map_basic_basic d;
    for (auto &p: dict_) {
        RCP<Basic> factor_old = pow(p.first, p.second);
        RCP<Basic> factor = factor_old->subs(subs_dict);
        if (factor == factor_old) {
            Mul::dict_add_term(d, p.second, p.first);
        } else if (is_a<Integer>(*factor) &&
                rcp_static_cast<Integer>(factor)->is_zero()) {
            return zero;
        } else if (is_a_Number(*factor)) {
            imulnum(outArg(coef), rcp_static_cast<Number>(factor));
        } else if (is_a<Mul>(*factor)) {
            RCP<Mul> tmp = rcp_static_cast<Mul>(factor);
            imulnum(outArg(coef), tmp->coef_);
            for (auto &q: tmp->dict_) {
                Mul::dict_add_term(d, q.second, q.first);
            }
        } else {
            RCP<Basic> exp, t;
            Mul::as_base_exp(factor, outArg(exp), outArg(t));
            Mul::dict_add_term(d, exp, t);
        }
    }
    return Mul::from_dict(coef, d);
}

} // CSymPy
