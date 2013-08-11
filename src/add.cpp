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

Add::Add(const RCP<Basic> &coef, const Dict_int& dict)
    : coef_{coef}, dict_{dict}
{
}

std::size_t Add::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *coef_);
    std::map<RCP<Basic>, RCP<Integer>, RCPBasicKeyLess>
        ordered(dict_.begin(), dict_.end());
    for (auto &p: ordered) {
        hash_combine<Basic>(seed, *(p.first));
        hash_combine<Basic>(seed, *(p.second));
    }
    return seed;
}

bool Add::__eq__(const Basic &o) const
{
    if (is_a<Add>(o) &&
        eq(coef_, static_cast<const Add &>(o).coef_) &&
        dicts_equal(dict_, static_cast<const Add &>(o).dict_))
        return true;

    return false;
}

std::string Add::__str__() const
{
    std::ostringstream o;
    for (auto &p: dict_) {
        if (eq(p.second, one))
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
            if ((rcp_dynamic_cast<Integer>(p->second))->is_one()) {
                return p->first;
            }
            if (is_a<Mul>(*(p->first))) {
                return Mul::from_dict(p->second,
                        rcp_dynamic_cast<Mul>(p->first)->dict_);
            }
            map_basic_int m;
            m[p->first] = one;
            return rcp(new Mul(p->second, m));
        }
        // TODO: if p->second is a numeric coefficient, it should still be
        // passed in as the first parameter of Mul()
        map_basic_int m;
        m[p->first] = one;
        m[p->second] = one;
        return rcp(new Mul(one, m));
    } else {
        return rcp(new Add(zero, d));
    }
}

// Adds (coef*t) to the dict "d"
// Assumption: "t" does not have any numerical coefficients, those are in "coef"
void Add::dict_add_term(Dict_int &d, const RCP<Integer> &coef,
        const RCP<Basic> &t)
{
    if (d.find(t) == d.end()) {
        // Not found, add it in if it is nonzero:
        if (!(coef->is_zero())) d[t] = coef;
    } else {
        // TODO: remove the item if d[t] + coef is zero:
        iaddint(outArg(d[t]), coef);
    }
}


void as_coef_term(const RCP<Basic> &self, const Ptr<RCP<Integer>> &coef,
        const Ptr<RCP<Basic>> &term)
{
    if (CSymPy::is_a<CSymPy::Symbol>(*self)) {
        *coef = one;
        *term = self;
    } else if (CSymPy::is_a<CSymPy::Mul>(*self)) {
        RCP<Basic> tmp;
        (rcp_dynamic_cast<CSymPy::Mul>(self))->as_coef_term(outArg(tmp), term);
        *coef = rcp_dynamic_cast<Integer>(tmp);
    } else if (CSymPy::is_a<CSymPy::Integer>(*self)) {
        *coef = rcp_dynamic_cast<CSymPy::Integer>(self);
        *term = one;
    } else if (CSymPy::is_a<CSymPy::Pow>(*self)) {
        *coef = one;
        *term = self;
    } else {
        std::cout << *self << std::endl;
        throw std::runtime_error("Not implemented yet.");
    }
}

RCP<Basic> add(const RCP<Basic> &a, const RCP<Basic> &b)
{
    CSymPy::Dict_int d;
    RCP<Integer> coef;
    RCP<Basic> t;
    if (CSymPy::is_a<Add>(*a) && CSymPy::is_a<Add>(*b)) {
        d = (rcp_dynamic_cast<Add>(a))->dict_;
        for (auto &p: (rcp_dynamic_cast<Add>(b))->dict_)
            Add::dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Add>(*a)) {
        d = (rcp_dynamic_cast<Add>(a))->dict_;
        as_coef_term(b, outArg(coef), outArg(t));
        Add::dict_add_term(d, coef, t);
    } else if (CSymPy::is_a<Add>(*b)) {
        d = (rcp_dynamic_cast<Add>(b))->dict_;
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

RCP<Basic> add_expand(const RCP<Add> &self)
{
    Dict_int d;
    RCP<Basic> coef, tmp, tmp2;
    for (auto &p: self->dict_) {
        tmp = expand(p.first);
        if (is_a<Add>(*tmp)) {
            for (auto &q: (rcp_dynamic_cast<Add>(tmp))->dict_) {
                tmp2 = q.first;
                if (is_a<Mul>(*tmp2)) {
                    rcp_dynamic_cast<Mul>(tmp2)->as_coef_term(outArg(coef),
                            outArg(tmp2));
                } else {
                    coef = one;
                }
                if (!is_a<Integer>(*coef))
                    throw std::runtime_error("Not implemented.");
                Add::dict_add_term(d, mulint(mulint(p.second, q.second),
                            rcp_dynamic_cast<Integer>(coef)), tmp2);
            }
        } else {
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
    }
    return Add::from_dict(d);
}

} // CSymPy
