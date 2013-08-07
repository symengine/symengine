#include <stdexcept>

#include "add.h"
#include "symbol.h"
#include "mul.h"
#include "pow.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;

namespace CSymPy {

Add::Add(const Dict_int& dict)
{
    this->coef = rcp(new Integer(1));
    this->dict = dict;
}

std::size_t Add::__hash__() const
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

bool Add::__eq__(const Basic &o) const
{
    if (is_a<Add>(o)) {
        const Add &s = static_cast<const Add &>(o);
        if (*(this->coef) == *(s.coef)) {
            if (dicts_equal(this->dict, s.dict)) {
                return true;
            }
        }
    }
    return false;
}

std::string Add::__str__() const
{
    std::ostringstream o;
    for (auto &p: this->dict) {
        if (*(p.second) == Integer(1))
            o << *(p.first);
        else
            o << *(p.second) << *(p.first);
        o << " + ";
    }
    std::string s = o.str();
    return s.substr(0, s.size()-3);
}

// Creates the appropriate instance (i.e. Add, Symbol, Integer, Mul) depending
// on how many (and which) items are in the dictionary "d":
RCP<Basic> Add::from_dict(const Dict_int &d)
{
    if (d.size() == 0) {
        throw std::runtime_error("Not implemented.");
    } else if (d.size() == 1) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if ((rcp_dynamic_cast<Integer>(p->second))->i == 1) {
                return p->first;
            }
            if (is_a<Mul>(*(p->first))) {
                return Mul::from_dict(p->second,
                        rcp_dynamic_cast<Mul>(p->first)->dict);
            }
            Dict_int m;
            m[p->first] = rcp(new Integer(1));
            return rcp(new Mul(p->second, m));
        }
        // TODO: if p->second is a numeric coefficient, it should still be
        // passed in as the first parameter of Mul()
        Dict_int m;
        m[p->first] = rcp(new Integer(1));
        m[p->second] = rcp(new Integer(1));
        return rcp(new Mul(rcp(new Integer(1)), m));
    } else {
        return rcp(new Add(d));
    }
}

// Adds (coef*t) to the dict "d"
// Assumption: "t" does not have any numerical coefficients, those are in "coef"
void Add::dict_add_term(Dict_int &d, const RCP<Integer> &coef,
        const RCP<Basic> &t)
{
    if (d.find(t) == d.end()) {
        // Not found, add it in if it is nonzero:
        if (coef->i != 0) d[t] = coef;
    } else {
        // TODO: remove the item if d[t] + coef is zero:
        iaddint(outArg(d[t]), coef);
    }
}

} // CSymPy

namespace {

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Integer;

void as_coef_term(const RCP<Basic> &self, const Ptr<RCP<Integer>> &coef,
        const Ptr<RCP<Basic>> &term)
{
    if (CSymPy::is_a<CSymPy::Symbol>(*self)) {
        *coef = rcp(new Integer(1));
        *term = self;
    } else if (CSymPy::is_a<CSymPy::Mul>(*self)) {
        RCP<Basic> tmp;
        (rcp_dynamic_cast<CSymPy::Mul>(self))->as_coef_term(outArg(tmp), term);
        *coef = rcp_dynamic_cast<Integer>(tmp);
    } else if (CSymPy::is_a<CSymPy::Integer>(*self)) {
        *coef = rcp_dynamic_cast<CSymPy::Integer>(self);
        *term = rcp(new Integer(1));
    } else if (CSymPy::is_a<CSymPy::Pow>(*self)) {
        *coef = rcp(new Integer(1));
        *term = self;
    } else {
        std::cout << *self << std::endl;
        throw std::runtime_error("Not implemented yet.");
    }
}

} // Anonymous

RCP<Basic> operator+(const RCP<Basic> &a, const RCP<Basic> &b)
{
    CSymPy::Dict_int d;
    RCP<Integer> coef;
    RCP<Basic> t;
    if (CSymPy::is_a<Add>(*a) && CSymPy::is_a<Add>(*b)) {
        d = (rcp_dynamic_cast<Add>(a))->dict;
        for (auto &p: (rcp_dynamic_cast<Add>(b))->dict)
            Add::dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Add>(*a)) {
        d = (rcp_dynamic_cast<Add>(a))->dict;
        as_coef_term(b, outArg(coef), outArg(t));
        Add::dict_add_term(d, coef, t);
    } else if (CSymPy::is_a<Add>(*b)) {
        d = (rcp_dynamic_cast<Add>(b))->dict;
        as_coef_term(a, outArg(coef), outArg(t));
        Add::dict_add_term(d, coef, t);
    } else {
        as_coef_term(a, outArg(coef), outArg(t));
        Add::dict_add_term(d, coef, t);
        as_coef_term(b, outArg(coef), outArg(t));
        Add::dict_add_term(d, coef, t);
    }
    return Add::from_dict(d);
}

RCP<Basic> operator-(const RCP<Basic> &a, const RCP<Basic> &b)
{
//    return a + (-b);
    throw std::runtime_error("Not implemented yet.");
}
