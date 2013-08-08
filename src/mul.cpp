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

namespace CSymPy {

Mul::Mul(const Teuchos::RCP<Basic> &coef, const Dict_int& dict)
{
    this->coef = coef;
    this->dict = dict;
}

std::size_t Mul::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *(this->coef));
    std::map<RCP<Basic>, RCP<Integer>, RCPBasicKeyLess>
        ordered(this->dict.begin(), this->dict.end());
    for (auto &p: ordered) {
        hash_combine<Basic>(seed, *(p.first));
        hash_combine<Basic>(seed, *(p.second));
    }
    return seed;
}

bool Mul::__eq__(const Basic &o) const
{
    if (is_a<Mul>(o)) {
        const Mul &s = static_cast<const Mul &>(o);
        if (eq(this->coef, (s.coef))) {
            if (dicts_equal(this->dict, s.dict)) {
                return true;
            }
        }
    }
    return false;
}

std::string Mul::__str__() const
{
    std::ostringstream o;
    if (neq(this->coef, one))
        o << *(this->coef);
    for (auto &p: this->dict) {
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

RCP<CSymPy::Basic> Mul::from_dict(const RCP<Basic> &coef, const Dict_int &d)
{
    if (d.size() == 0) {
        throw std::runtime_error("Not implemented.");
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
        CSymPy::Dict_int d2;
        // TODO: handle non-integer coefs like sqrt(2) here:
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
        return rcp(new Mul(coef2, d2));
    }
}

// Mul (t^exp) to the dict "d"
void Mul::dict_add_term(Dict_int &d, const RCP<Integer> &exp,
        const RCP<Basic> &t)
{
    if (d.find(t) == d.end()) {
        // "t" not found in "d":
        d[t] = exp;
    } else {
        iaddint(outArg(d[t]), exp);
    }
}

void Mul::as_coef_term(const Teuchos::Ptr<Teuchos::RCP<Basic>> &coef,
            const Teuchos::Ptr<Teuchos::RCP<Basic>> &term)
{
    *coef = this->coef;
    *term = this->from_dict(one, this->dict);
}

void Mul::as_two_terms(const Teuchos::Ptr<RCP<Basic>> &a,
            const Teuchos::Ptr<RCP<Basic>> &b)
{
    // Example: if this=3*x^2*y^2*z^2, then a=x^2 and b=3*y^2*z^2
    auto p = this->dict.begin();
    *a = pow(p->first, p->second);
    Dict_int d = this->dict;
    d.erase(p);
    *b = Mul::from_dict(this->coef, d);
}

void as_base_exp(const RCP<Basic> &self, const Ptr<RCP<Integer>> &exp,
        const Ptr<RCP<Basic>> &base)
{
    if (is_a<Symbol>(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a<Integer>(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a<Pow>(*self)) {
        // NOTE: this will raise an exception if `exp` is not Integer
        *exp = rcp_dynamic_cast<Integer>(rcp_dynamic_cast<Pow>(self)->exp);
        *base = rcp_dynamic_cast<Pow>(self)->base;
    } else if (is_a<Add>(*self)) {
        *exp = one;
        *base = self;
    } else {
        std::cout << self;
        throw std::runtime_error("Not implemented yet.");
    }
}

RCP<Basic> mul(const RCP<Basic> &a, const RCP<Basic> &b)
{
    CSymPy::Dict_int d;
    RCP<Integer> exp;
    RCP<Basic> t;
    RCP<Basic> coef = one;
    if (CSymPy::is_a<Mul>(*a) && CSymPy::is_a<Mul>(*b)) {
        d = (rcp_dynamic_cast<Mul>(a))->dict;
        for (auto &p: (rcp_dynamic_cast<Mul>(b))->dict)
            Mul::dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Mul>(*a)) {
        coef = (rcp_dynamic_cast<Mul>(a))->coef;
        d = (rcp_dynamic_cast<Mul>(a))->dict;
        as_base_exp(b, outArg(exp), outArg(t));
        Mul::dict_add_term(d, exp, t);
    } else if (CSymPy::is_a<Mul>(*b)) {
        d = (rcp_dynamic_cast<Mul>(b))->dict;
        as_base_exp(a, outArg(exp), outArg(t));
        Mul::dict_add_term(d, exp, t);
    } else {
        as_base_exp(a, outArg(exp), outArg(t));
        d[t] = exp;
        as_base_exp(b, outArg(exp), outArg(t));
        Mul::dict_add_term(d, exp, t);
    }
    return Mul::from_dict(coef, d);
}

RCP<Basic> mul_expand_two(const RCP<Basic> &a, const RCP<Basic> &b)
{
    // Both a and b are assumed to be expanded
    if (is_a<Add>(*a) && is_a<Add>(*b)) {
        throw std::runtime_error("Not implemented.");
    } else if (is_a<Add>(*a)) {
        throw std::runtime_error("Not implemented.");
    } else if (is_a<Add>(*b)) {
        Dict_int d;
        RCP<Basic> coef;
        RCP<Basic> tmp, tmp2, t;
        for (auto &p: (rcp_dynamic_cast<Add>(b))->dict) {
            tmp = mul(a, p.first);
            if (!is_a<Mul>(*tmp)) throw std::runtime_error("Not implemented.");
            rcp_dynamic_cast<Mul>(tmp)->as_coef_term(outArg(coef),
                    outArg(tmp2));
            if (!is_a<Integer>(*coef))
                throw std::runtime_error("Not implemented.");
            Add::dict_add_term(d,
                    mulint(p.second, rcp_dynamic_cast<Integer>(coef)), tmp2);
        }
        return Add::from_dict(d);
    }
    return mul(a, b);
}

RCP<Basic> mul_expand(const RCP<Mul> &self)
{
    RCP<Basic> a, b;
    self->as_two_terms(outArg(a), outArg(b));
    // TODO: add single dispatch of expand() to basic.h, rename expand to
    // pow_expand
    //a = expand(a);
    //b = expand(b);
    return mul_expand_two(a, b);
}

} // CSymPy
