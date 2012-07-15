#include <stdexcept>

#include "mul.h"
#include "symbol.h"

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
    throw std::runtime_error("Not implemented yet.");
}

bool Mul::__eq__(const Basic &o) const
{
    throw std::runtime_error("Not implemented yet.");
}

std::string Mul::__str__() const
{
    std::ostringstream s;
    s << *(this->coef);
    for (auto &p: this->dict)
        s << *(p.first) << "^" << *(p.second) << " * ";
    return s.str();
}

RCP<CSymPy::Basic> Mul::from_dict(const Dict_int &d)
{
    if (d.size() == 0) {
        throw std::runtime_error("Not implemented.");
    } else if (d.size() == 1) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if ((rcp_dynamic_cast<Integer>(p->second))->i == 1) {
                // For x^1 we simply return "x":
                return p->first;
            }
        }
        // Otherwise create a Pow() here:
        throw std::runtime_error("Not implemented.");
    } else {
        // TODO: Make sure there are no numeric "keys" in "d", as those should
        // go into "coef":
        RCP<Basic> coef = rcp(new Integer(1));
        return rcp(new Mul(coef, d));
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
        d[t] = d[t] + exp;
    }
}

void Mul::as_coef_term(const Teuchos::Ptr<Teuchos::RCP<Basic>> &coef,
            const Teuchos::Ptr<Teuchos::RCP<Basic>> &term)
{
    *coef = this->coef;
    *term = this->from_dict(this->dict);
}

} // CSymPy

namespace {

using CSymPy::Basic;
using CSymPy::Mul;
using CSymPy::Integer;

void as_base_exp(const RCP<Basic> &self, const Ptr<RCP<Integer>> &exp,
        const Ptr<RCP<Basic>> &base)
{
    if (CSymPy::is_a<CSymPy::Symbol>(*self)) {
        *exp = rcp(new Integer(1));
        *base = self;
    } else {
        throw std::runtime_error("Not implemented yet.");
    }
}

} // Anonymous

RCP<Basic> operator*(const RCP<Basic> &a, const RCP<Basic> &b)
{
    CSymPy::Dict_int d;
    RCP<Integer> exp;
    RCP<Basic> t;
    if (CSymPy::is_a<Mul>(*a) && CSymPy::is_a<Mul>(*b)) {
        d = (rcp_dynamic_cast<Mul>(a))->dict;
        for (auto &p: (rcp_dynamic_cast<Mul>(b))->dict)
            Mul::dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Mul>(*a)) {
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
    return Mul::from_dict(d);
}

RCP<Basic> operator/(const RCP<Basic> &a, const RCP<Basic> &b)
{
//    return a * b**(-1);
    throw std::runtime_error("Not implemented yet.");
}
