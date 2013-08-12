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
using Teuchos::rcp_static_cast;

namespace CSymPy {

Add::Add(const RCP<Basic> &coef, const umap_basic_int& dict)
    : coef_{coef}, dict_{dict}
{
    CSYMPY_ASSERT(is_canonical(coef, dict))
}

bool Add::is_canonical(const Teuchos::RCP<Basic> &coef,
        const umap_basic_int& dict)
{
    if (coef == Teuchos::null) return false;
    if (dict.size() == 0) return false;
    if (dict.size() == 1) {
        // e.g. 0 + x, 0 + 2x
        if (is_a<Integer>(*coef) && rcp_static_cast<Integer>(coef)->is_zero())
            return false;
    }
    // Check that each term in 'dict' is in canonical form
    for (auto &p: dict) {
        if (p.first == Teuchos::null) return false;
        if (p.second == Teuchos::null) return false;
        // e.g. 2*3
        if (is_a<Integer>(*p.first) && is_a<Integer>(*p.second))
            return false;
        // e.g. 0*x
        if (is_a<Integer>(*p.first) &&
                rcp_static_cast<Integer>(p.first)->is_zero())
            return false;
        // e.g. 1*x (={1:x}), this should rather be just x (={x:1})
        if (is_a<Integer>(*p.first) &&
                rcp_static_cast<Integer>(p.first)->is_one())
            return false;
        // e.g. x*0
        if (is_a<Integer>(*p.second) &&
                rcp_static_cast<Integer>(p.second)->is_zero())
            return false;
    }
    return true;
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

// Very quickly (!) creates the appropriate instance (i.e. Add, Symbol,
// Integer, Mul) depending on the size of the dictionary 'd'.
// If d.size() > 1 then it just returns Add. This means that the dictionary
// must be in canonical form already. For d.size == 1, it returns Mul, Pow,
// Symbol or Integer, depending on the expression.
RCP<Basic> Add::from_dict(const RCP<Basic> &coef, const umap_basic_int &d)
{
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1 && is_a<Integer>(*coef) &&
            rcp_static_cast<Integer>(coef)->is_zero()) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if (rcp_static_cast<Integer>(p->second)->is_one()) {
                return p->first;
            }
            if (is_a<Mul>(*(p->first))) {
                return Mul::from_dict(p->second,
                        rcp_static_cast<Mul>(p->first)->dict_);
            }
            map_basic_basic m;
            m[p->first] = one;
            return rcp(new Mul(p->second, m));
        }
        // TODO: if p->second is a numeric coefficient, it should still be
        // passed in as the first parameter of Mul()
        map_basic_basic m;
        m[p->first] = one;
        m[p->second] = one;
        return rcp(new Mul(one, m));
    } else {
        return rcp(new Add(coef, d));
    }
}

// Adds (coef*t) to the dict "d"
// Assumption: "t" does not have any numerical coefficients, those are in "coef"
void Add::dict_add_term(umap_basic_int &d, const RCP<Integer> &coef,
        const RCP<Basic> &t)
{
    auto it = d.find(t);
    if (it == d.end()) {
        // Not found, add it in if it is nonzero:
        if (!(coef->is_zero())) d[t] = coef;
    } else {
        // TODO: remove the item if d[t] + coef is zero:
        iaddint(outArg(it->second), coef);
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
    CSymPy::umap_basic_int d;
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
        auto it = d.find(one);
        if (it == d.end()) {
            coef = zero;
        } else {
            coef = it->second;
            d.erase(it);
        }
        return Add::from_dict(coef, d);
    }
    return Add::from_dict(zero, d);
}

RCP<Basic> add_expand(const RCP<Add> &self)
{
    umap_basic_int d;
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
    return Add::from_dict(zero, d);
}

} // CSymPy
