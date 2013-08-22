#include <stdexcept>

#include "add.h"
#include "symbol.h"
#include "mul.h"
#include "pow.h"
#include "rational.h"
#include "functions.h"


namespace CSymPy {

Add::Add(const RCP<Number> &coef, const umap_basic_int& dict)
    : coef_{coef}, dict_{dict}
{
    CSYMPY_ASSERT(is_canonical(coef, dict))
}

bool Add::is_canonical(const RCP<Number> &coef,
        const umap_basic_int& dict)
{
    if (dict.size() == 0) return false;
    if (dict.size() == 1) {
        // e.g. 0 + x, 0 + 2x
        if (coef->is_zero()) return false;
    }
    // Check that each term in 'dict' is in canonical form
    for (auto &p: dict) {
        // e.g. 2*3
        if (is_a_Number(*p.first) && is_a_Number(*p.second))
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

        // e.g. {3x: 2}, this should rather be just {x: 6}
        if (is_a<Mul>(*p.first) &&
                !(rcp_static_cast<Mul>(p.first)->coef_->is_one()))
            return false;
    }
    return true;
}

std::size_t Add::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *coef_);
    map_basic_int ordered(dict_.begin(), dict_.end());
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

int Add::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Add>(o))
    const Add &s = static_cast<const Add &>(o);
    // # of elements
    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    // coef
    int cmp = coef_->__cmp__(*s.coef_);
    if (cmp != 0)
        return cmp;

    // Compare dictionaries:
    // NOTE: This is slow. Add should cache this map_basic_int representation
    // once it is computed.
    map_basic_int adict(dict_.begin(), dict_.end());
    map_basic_int bdict(s.dict_.begin(), s.dict_.end());
    return map_basic_int_compare(adict, bdict);
}

std::string Add::__str__() const
{
    std::ostringstream o;
    if (neq(coef_, zero))
        o << *coef_ << " + ";
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
RCP<Basic> Add::from_dict(const RCP<Number> &coef, const umap_basic_int &d)
{
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1 && coef->is_zero()) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if (rcp_static_cast<Integer>(p->second)->is_zero()) {
                return zero;
            }
            if (rcp_static_cast<Integer>(p->second)->is_one()) {
                return p->first;
            }
            if (is_a<Mul>(*(p->first))) {
                return Mul::from_dict(p->second,
                        rcp_static_cast<Mul>(p->first)->dict_);
            }
            map_basic_basic m;
            if (is_a<Pow>(*(p->first))) {
                insert(m, rcp_static_cast<Pow>(p->first)->base_,
                    rcp_static_cast<Pow>(p->first)->exp_);
            } else {
                insert(m, p->first, one);
            }
            return rcp(new Mul(p->second, m));
        }
        map_basic_basic m;
        if (is_a_Number(*p->second)) {
            if (is_a<Mul>(*(p->first))) {
                return Mul::from_dict(p->second,
                        rcp_static_cast<Mul>(p->first)->dict_);
            }
            if (is_a<Pow>(*p->first)) {
                insert(m, rcp_static_cast<Pow>(p->first)->base_,
                    rcp_static_cast<Pow>(p->first)->exp_);
            } else {
                insert(m, p->first, one);
            }
            return rcp(new Mul(p->second, m));
        } else {
            insert(m, p->first, one);
            insert(m, p->second, one);
            return rcp(new Mul(one, m));
        }
    } else {
        return rcp(new Add(coef, d));
    }
}

// Adds (coef*t) to the dict "d"
// Assumption: "t" does not have any numerical coefficients, those are in "coef"
void Add::dict_add_term(umap_basic_int &d, const RCP<Number> &coef,
        const RCP<Basic> &t)
{
    auto it = d.find(t);
    if (it == d.end()) {
        // Not found, add it in if it is nonzero:
        if (!(coef->is_zero())) insert(d, t, coef);
    } else {
        iaddnum(outArg(it->second), coef);
        if (it->second->is_zero()) d.erase(it);
    }
}


void as_coef_term(const RCP<Basic> &self, const Ptr<RCP<Number>> &coef,
        const Ptr<RCP<Basic>> &term)
{
    if (is_a<Symbol>(*self)) {
        *coef = one;
        *term = self;
    } else if (is_a<Mul>(*self)) {
        (rcp_static_cast<Mul>(self))->
            as_coef_term(outArg(*coef), term);
    } else if (is_a_Number(*self)) {
        *coef = rcp_static_cast<Number>(self);
        *term = one;
    } else if (is_a<Pow>(*self)) {
        *coef = one;
        *term = self;
    } else if (is_a_sub<Function>(*self)) {
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
    RCP<Number> coef;
    RCP<Basic> t;
    if (CSymPy::is_a<Add>(*a) && CSymPy::is_a<Add>(*b)) {
        coef = (rcp_static_cast<Add>(a))->coef_;
        d = (rcp_static_cast<Add>(a))->dict_;
        for (auto &p: (rcp_static_cast<Add>(b))->dict_)
            Add::dict_add_term(d, p.second, p.first);
        iaddnum(outArg(coef), rcp_static_cast<Add>(b)->coef_);
    } else if (CSymPy::is_a<Add>(*a)) {
        coef = (rcp_static_cast<Add>(a))->coef_;
        d = (rcp_static_cast<Add>(a))->dict_;
        if (is_a_Number(*b)) {
            iaddnum(outArg(coef), rcp_static_cast<Number>(b));
        } else {
            RCP<Number> coef2;
            as_coef_term(b, outArg(coef2), outArg(t));
            Add::dict_add_term(d, coef2, t);
        }
    } else if (CSymPy::is_a<Add>(*b)) {
        coef = (rcp_static_cast<Add>(b))->coef_;
        d = (rcp_static_cast<Add>(b))->dict_;
        if (is_a_Number(*a)) {
            iaddnum(outArg(coef), rcp_static_cast<Number>(a));
        } else {
            RCP<Number> coef2;
            as_coef_term(a, outArg(coef2), outArg(t));
            Add::dict_add_term(d, coef2, t);
        }
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
    return Add::from_dict(coef, d);
}

RCP<Basic> sub(const RCP<Basic> &a, const RCP<Basic> &b)
{
    return add(a, mul(minus_one, b));
}

RCP<Basic> add_expand(const RCP<Add> &self)
{
    umap_basic_int d;
    RCP<Number> coef_overall = self->coef_;
    RCP<Number> coef;
    RCP<Basic> tmp, tmp2;
    for (auto &p: self->dict_) {
        tmp = expand(p.first);
        if (is_a<Add>(*tmp)) {
            for (auto &q: (rcp_static_cast<Add>(tmp))->dict_) {
                tmp2 = q.first;
                if (is_a<Mul>(*tmp2)) {
                    rcp_static_cast<Mul>(tmp2)->as_coef_term(outArg(coef),
                            outArg(tmp2));
                } else {
                    coef = one;
                }
                Add::dict_add_term(d,
                        mulnum(mulnum(p.second, q.second), coef), tmp2);
            }
            iaddnum(outArg(coef_overall), mulnum(p.second,
                        rcp_static_cast<Add>(tmp)->coef_));
        } else {
            if (is_a<Mul>(*tmp)) {
                rcp_static_cast<Mul>(tmp)->as_coef_term(outArg(coef),
                        outArg(tmp));
            } else {
                coef = one;
            }
            Add::dict_add_term(d, mulnum(p.second, coef), tmp);
        }
    }
    return Add::from_dict(coef_overall, d);
}

RCP<Basic> Add::diff(const RCP<Symbol> &x) const
{
    RCP<Basic> r=zero;
    for (auto &p: dict_) {
        RCP<Basic> term = mul(p.first, p.second)->diff(x);
        // TODO: speed this up:
        r = add(r, term);
    }
    return r;
}

void Add::as_two_terms(const Ptr<RCP<Basic>> &a,
            const Ptr<RCP<Basic>> &b)
{
    auto p = dict_.begin();
    *a = mul(p->first, p->second);
    umap_basic_int d = dict_;
    d.erase(p->first);
    *b = Add::from_dict(coef_, d);
}

} // CSymPy
