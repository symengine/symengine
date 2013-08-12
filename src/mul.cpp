#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;

namespace CSymPy {

Mul::Mul(const Teuchos::RCP<Basic> &coef, const map_basic_basic& dict)
    : coef_{coef}, dict_{dict}
{
    CSYMPY_ASSERT(is_canonical(coef, dict))
}

bool Mul::is_canonical(const Teuchos::RCP<Basic> &coef,
        const map_basic_basic& dict)
{
    if (coef == Teuchos::null) return false;
    // e.g. 0*x*y
    if (is_a<Integer>(*coef) && rcp_static_cast<Integer>(coef)->is_zero())
        return false;
    if (dict.size() == 0) return false;
    if (dict.size() == 1) {
        // e.g. 1*x, 1*x^2
        if (is_a<Integer>(*coef) && rcp_static_cast<Integer>(coef)->is_one())
            return false;
    }
    // Check that each term in 'dict' is in canonical form
    for (auto &p: dict) {
        if (p.first == Teuchos::null) return false;
        if (p.second == Teuchos::null) return false;
        // e.g. 2^3
        if (is_a<Integer>(*p.first) && is_a<Integer>(*p.second))
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

RCP<CSymPy::Basic> Mul::from_dict(const RCP<Basic> &coef, const map_basic_basic &d)
{
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second)) && is_a<Integer>(*coef)) {
            if (rcp_dynamic_cast<Integer>(coef)->is_one()) {
                if ((rcp_dynamic_cast<Integer>(p->second))->is_one()) {
                    // For x^1 we simply return "x":
                    return p->first;
                }
            } else {
                if ((rcp_dynamic_cast<Integer>(p->second))->is_one()) {
                    // For coef*x^1 we simply return "coef*x":
                    return rcp(new Mul(coef, d));
                }
                std::cout << "DEBUG: " << coef << " " << d << std::endl;
                throw std::runtime_error("Not implemented.");
            }
        }
        // Otherwise create a Pow() here:
        return pow(p->first, p->second);
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
        d[t] = exp;
    } else {
        // Very common case, needs to be fast:
        if (is_a<Integer>(*it->second) && is_a<Integer>(*exp)) {
            RCP<Integer> tmp = rcp_static_cast<Integer>(it->second);
            iaddint(outArg(tmp), rcp_static_cast<Integer>(exp));
            it->second = tmp;
        } else {
            // General case:
            it->second = add(it->second, exp);
        }
    }
}

void Mul::as_coef_term(const Teuchos::Ptr<Teuchos::RCP<Basic>> &coef,
            const Teuchos::Ptr<Teuchos::RCP<Basic>> &term)
{
    *coef = coef_;
    *term = from_dict(one, dict_);
}

void Mul::as_two_terms(const Teuchos::Ptr<RCP<Basic>> &a,
            const Teuchos::Ptr<RCP<Basic>> &b)
{
    // Example: if this=3*x^2*y^2*z^2, then a=x^2 and b=3*y^2*z^2
    auto p = dict_.begin();
    *a = pow(p->first, p->second);
    map_basic_basic d = dict_;
    d.erase(p->first);
    *b = Mul::from_dict(coef_, d);
}

void as_base_exp(const RCP<Basic> &self, const Ptr<RCP<Basic>> &exp,
        const Ptr<RCP<Basic>> &base)
{
    if (is_a<Symbol>(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a<Integer>(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a<Pow>(*self)) {
        *exp = rcp_dynamic_cast<Pow>(self)->exp_;
        *base = rcp_dynamic_cast<Pow>(self)->base_;
    } else if (is_a<Add>(*self)) {
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
    RCP<Basic> coef = one;
    if (CSymPy::is_a<Mul>(*a) && CSymPy::is_a<Mul>(*b)) {
        d = (rcp_static_cast<Mul>(a))->dict_;
        for (auto &p: (rcp_static_cast<Mul>(b))->dict_)
            Mul::dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Mul>(*a)) {
        RCP<Basic> exp;
        RCP<Basic> t;
        coef = (rcp_static_cast<Mul>(a))->coef_;
        d = (rcp_static_cast<Mul>(a))->dict_;
        as_base_exp(b, outArg(exp), outArg(t));
        Mul::dict_add_term(d, exp, t);
    } else if (CSymPy::is_a<Mul>(*b)) {
        RCP<Basic> exp;
        RCP<Basic> t;
        coef = (rcp_static_cast<Mul>(b))->coef_;
        d = (rcp_static_cast<Mul>(b))->dict_;
        as_base_exp(a, outArg(exp), outArg(t));
        Mul::dict_add_term(d, exp, t);
    } else {
        RCP<Basic> exp;
        RCP<Basic> t;
        as_base_exp(a, outArg(exp), outArg(t));
        d[t] = exp;
        as_base_exp(b, outArg(exp), outArg(t));
        Mul::dict_add_term(d, exp, t);

        CSymPy::map_basic_basic d2;
        RCP<Integer> coef2 = rcp_dynamic_cast<Integer>(coef);
        for (auto &p: d) {
            if (is_a<Integer>(*(p.first)) && is_a<Integer>(*(p.second))) {
                RCP<Integer> f = rcp_dynamic_cast<Integer>(p.first);
                RCP<Integer> s = rcp_dynamic_cast<Integer>(p.second);
                RCP<Integer> r = powint(f, s);
                imulint(outArg(coef2), r);
            } else {
                Mul::dict_add_term(d2, p.second, p.first);
            }
        }
        return Mul::from_dict(coef2, d2);
    }
    return Mul::from_dict(coef, d);
}

RCP<Basic> mul_expand_two(const RCP<Basic> &a, const RCP<Basic> &b)
{
    // Both a and b are assumed to be expanded
    if (is_a<Add>(*a) && is_a<Add>(*b)) {
        umap_basic_int d;
        for (auto &p: (rcp_dynamic_cast<Add>(a))->dict_) {
            for (auto &q: (rcp_dynamic_cast<Add>(b))->dict_) {
                // The main bottleneck here is the mul(p.first, q.first) command
                Add::dict_add_term(d, mulint(p.second, q.second),
                        mul(p.first, q.first));
            }
        }
        return Add::from_dict(zero, d);
    } else if (is_a<Add>(*a)) {
        return mul_expand_two(b, a);
    } else if (is_a<Add>(*b)) {
        umap_basic_int d;
        RCP<Basic> coef, tmp;
        for (auto &p: (rcp_dynamic_cast<Add>(b))->dict_) {
            tmp = mul(a, p.first);
            if (is_a<Mul>(*tmp)) {
                rcp_dynamic_cast<Mul>(tmp)->as_coef_term(outArg(coef),
                        outArg(tmp));
            } else {
                coef = one;
            }
            if (!is_a<Integer>(*coef))
                throw std::runtime_error("Not implemented.");
            Add::dict_add_term(d,
                    mulint(p.second, rcp_dynamic_cast<Integer>(coef)), tmp);
        }
        return Add::from_dict(zero, d);
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

} // CSymPy
