#include <stdexcept>

#include "mul.h"
#include "symbol.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;

namespace CSymPy {

Mul::Mul(const Dict_int& dict)
{
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
    for (auto &p: this->dict)
        s << *(p.first) << "^" << *(p.second) << " * ";
    return s.str();
}

RCP<CSymPy::Basic> Mul::add_from_dict(const Dict_int &d)
{
    return rcp(new Mul(d));
}

// Mul (coef*t) to the dict "d"
// Assumption: "t" does not have any numerical coefficients, those are in "coef"
void Mul::dict_add_term(Dict_int &d, const RCP<Integer> &coef,
        const RCP<Basic> &t)
{
    if (d.find(t) == d.end()) {
        // "t" not found in "d":
        d[t] = coef;
    } else {
        d[t] = d[t] + coef;
    }
}

} // CSymPy

namespace {

using CSymPy::Basic;
using CSymPy::Mul;
using CSymPy::Integer;

void as_coef_term(const RCP<Basic> &self, const Ptr<RCP<Integer>> &coef,
        const Ptr<RCP<Basic>> &term)
{
    if (CSymPy::is_a<CSymPy::Symbol>(*self)) {
        *coef = rcp(new Integer(1));
        *term = self;
    } else {
        throw std::runtime_error("Not implemented yet.");
    }
}

} // Anonymous

RCP<Basic> operator*(const RCP<Basic> &a, const RCP<Basic> &b)
{
    CSymPy::Dict_int d;
    RCP<Integer> coef;
    RCP<Basic> t;
    if (CSymPy::is_a<Mul>(*a) && CSymPy::is_a<Mul>(*b)) {
        d = (rcp_dynamic_cast<Mul>(a))->dict;
        for (auto &p: (rcp_dynamic_cast<Mul>(b))->dict)
            Mul::dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Mul>(*a)) {
        d = (rcp_dynamic_cast<Mul>(a))->dict;
        as_coef_term(b, outArg(coef), outArg(t));
        Mul::dict_add_term(d, coef, t);
    } else if (CSymPy::is_a<Mul>(*b)) {
        d = (rcp_dynamic_cast<Mul>(b))->dict;
        as_coef_term(a, outArg(coef), outArg(t));
        Mul::dict_add_term(d, coef, t);
    } else {
        as_coef_term(a, outArg(coef), outArg(t));
        d[t] = coef;
        as_coef_term(b, outArg(coef), outArg(t));
        Mul::dict_add_term(d, coef, t);
    }
    return Mul::add_from_dict(d);
}

RCP<Basic> operator/(const RCP<Basic> &a, const RCP<Basic> &b)
{
//    return a * b**(-1);
    throw std::runtime_error("Not implemented yet.");
}
