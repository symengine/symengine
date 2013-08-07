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
        if (*(this->coef) == *(s.coef)) {
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
    if (*(this->coef) != Integer(1))
        o << *(this->coef);
    for (auto &p: this->dict) {
        if (*(p.second) == Integer(1))
            o << *(p.first);
        else
            o << *(p.first) << "^" << *(p.second);
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
            if (rcp_dynamic_cast<Integer>(coef)->i == 1) {
                if ((rcp_dynamic_cast<Integer>(p->second))->i == 1) {
                    // For x^1 we simply return "x":
                    return p->first;
                }
            } else {
                if ((rcp_dynamic_cast<Integer>(p->second))->i == 1) {
                    // For coef*x^1 we simply return "coef*x":
                    return rcp(new Mul(coef, d));
                }
                std::cout << "DEBUG: " << coef << " " << d << std::endl;
                throw std::runtime_error("Not implemented.");
            }
        }
        // Otherwise create a Pow() here:
        return rcp(new Pow(p->first, p->second));
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
    *term = this->from_dict(rcp(new Integer(1)), this->dict);
}

void as_base_exp(const RCP<Basic> &self, const Ptr<RCP<Integer>> &exp,
        const Ptr<RCP<Basic>> &base)
{
    if (is_a<Symbol>(*self)) {
        *exp = rcp(new Integer(1));
        *base = self;
    } else if (is_a<Integer>(*self)) {
        *exp = rcp(new Integer(1));
        *base = self;
    } else if (is_a<Pow>(*self)) {
        // NOTE: this will raise an exception if `exp` is not Integer
        *exp = rcp_dynamic_cast<Integer>(rcp_dynamic_cast<Pow>(self)->exp);
        *base = rcp_dynamic_cast<Pow>(self)->base;
    } else if (is_a<Add>(*self)) {
        *exp = rcp(new Integer(1));
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
    RCP<Basic> coef = rcp(new Integer(1));
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

} // CSymPy
