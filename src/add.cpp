#include <stdexcept>

#include "add.h"
#include "symbol.h"
#include "mul.h"
#include "pow.h"
#include "rational.h"
#include "functions.h"


namespace CSymPy {

Add::Add(const RCP<const Number> &coef, const umap_basic_int& dict)
    : coef_{coef}, dict_{dict}
{
    CSYMPY_ASSERT(is_canonical(coef, dict))
}

bool Add::is_canonical(const RCP<const Number> &coef,
        const umap_basic_int& dict)
{
    if (coef == null) return false;
    if (dict.size() == 0) return false;
    if (dict.size() == 1) {
        // e.g. 0 + x, 0 + 2x
        if (coef->is_zero()) return false;
    }
    // Check that each term in 'dict' is in canonical form
    for (auto &p: dict) {
        if (p.first == null) return false;
        if (p.second == null) return false;
        // e.g. 2*3
        if (is_a_Number(*p.first) && is_a_Number(*p.second))
            return false;
        // e.g. 0*x
        if (is_a<Integer>(*p.first) &&
                rcp_static_cast<const Integer>(p.first)->is_zero())
            return false;
        // e.g. 1*x (={1:x}), this should rather be just x (={x:1})
        if (is_a<Integer>(*p.first) &&
                rcp_static_cast<const Integer>(p.first)->is_one())
            return false;
        // e.g. x*0
        if (is_a<Integer>(*p.second) &&
                rcp_static_cast<const Integer>(p.second)->is_zero())
            return false;

        // e.g. {3x: 2}, this should rather be just {x: 6}
        if (is_a<Mul>(*p.first) &&
                !(rcp_static_cast<const Mul>(p.first)->coef_->is_one()))
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
    int counter = 0;
    for (auto &p: dict_) {
        if (eq(p.second, one))
            o << *(p.first);
        else {
            // TODO: extend this for Rationals as well:
            if (is_a<Integer>(*p.second) &&
                    rcp_static_cast<const Integer>(p.second)->is_negative()
                    && o.tellp() >= 3)
                o.seekp(-3, std::ios_base::cur);
            if (eq(p.second, minus_one)) {
                if (counter >= 1)
                    o << " - ";
                else
                    o << "-";
            } else {
                // TODO: extend this for Rationals as well:
                if (is_a<Integer>(*p.second) &&
                        rcp_static_cast<const Integer>(p.second)->is_negative()) {
                    if (counter >= 1)
                        o << " - ";
                    else
                        o << "-";
                    o << -(rcp_static_cast<const Integer>(p.second))->i;
                } else {
                    o << *(p.second);
                }
            }
            if (is_a<Add>(*p.first)) {
                if (!eq(p.second, minus_one)) o << "*";
                o << "(";
            }
            o << *(p.first);
            if (is_a<Add>(*p.first)) o << ")";
        }
        o << " + ";
        counter++;
    }
    o.seekp(-3, std::ios_base::cur);
    std::string s = o.str();
    return s.substr(0, o.tellp());
}

// Very quickly (!) creates the appropriate instance (i.e. Add, Symbol,
// Integer, Mul) depending on the size of the dictionary 'd'.
// If d.size() > 1 then it just returns Add. This means that the dictionary
// must be in canonical form already. For d.size == 1, it returns Mul, Pow,
// Symbol or Integer, depending on the expression.
RCP<const Basic> Add::from_dict(const RCP<const Number> &coef, const umap_basic_int &d)
{
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1 && coef->is_zero()) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if (rcp_static_cast<const Integer>(p->second)->is_zero()) {
                return zero;
            }
            if (rcp_static_cast<const Integer>(p->second)->is_one()) {
                return p->first;
            }
            if (is_a<Mul>(*(p->first))) {
                return Mul::from_dict(p->second,
                        rcp_static_cast<const Mul>(p->first)->dict_);
            }
            map_basic_basic m;
            if (is_a<Pow>(*(p->first))) {
                insert(m, rcp_static_cast<const Pow>(p->first)->base_,
                    rcp_static_cast<const Pow>(p->first)->exp_);
            } else {
                insert(m, p->first, one);
            }
            return rcp(new Mul(p->second, m));
        }
        map_basic_basic m;
        if (is_a_Number(*p->second)) {
            if (is_a<Mul>(*(p->first))) {
                return Mul::from_dict(p->second,
                        rcp_static_cast<const Mul>(p->first)->dict_);
            }
            if (is_a<Pow>(*p->first)) {
                insert(m, rcp_static_cast<const Pow>(p->first)->base_,
                    rcp_static_cast<const Pow>(p->first)->exp_);
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
void Add::dict_add_term(umap_basic_int &d, const RCP<const Number> &coef,
        const RCP<const Basic> &t)
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


void Add::as_coef_term(const RCP<const Basic> &self,
        const Ptr<RCP<const Number>> &coef, const Ptr<RCP<const Basic>> &term)
{
    if (is_a<Mul>(*self)) {
        *coef = (rcp_static_cast<const Mul>(self))->coef_;
        *term = Mul::from_dict(one, (rcp_static_cast<const Mul>(self))->dict_);
    } else if (is_a_Number(*self)) {
        *coef = rcp_static_cast<const Number>(self);
        *term = one;
    } else {
        *coef = one;
        *term = self;
    }
}

RCP<const Basic> add(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    CSymPy::umap_basic_int d;
    RCP<const Number> coef;
    RCP<const Basic> t;
    if (CSymPy::is_a<Add>(*a) && CSymPy::is_a<Add>(*b)) {
        coef = (rcp_static_cast<const Add>(a))->coef_;
        d = (rcp_static_cast<const Add>(a))->dict_;
        for (auto &p: (rcp_static_cast<const Add>(b))->dict_)
            Add::dict_add_term(d, p.second, p.first);
        iaddnum(outArg(coef), rcp_static_cast<const Add>(b)->coef_);
    } else if (CSymPy::is_a<Add>(*a)) {
        coef = (rcp_static_cast<const Add>(a))->coef_;
        d = (rcp_static_cast<const Add>(a))->dict_;
        if (is_a_Number(*b)) {
            iaddnum(outArg(coef), rcp_static_cast<const Number>(b));
        } else {
            RCP<const Number> coef2;
            Add::as_coef_term(b, outArg(coef2), outArg(t));
            Add::dict_add_term(d, coef2, t);
        }
    } else if (CSymPy::is_a<Add>(*b)) {
        coef = (rcp_static_cast<const Add>(b))->coef_;
        d = (rcp_static_cast<const Add>(b))->dict_;
        if (is_a_Number(*a)) {
            iaddnum(outArg(coef), rcp_static_cast<const Number>(a));
        } else {
            RCP<const Number> coef2;
            Add::as_coef_term(a, outArg(coef2), outArg(t));
            Add::dict_add_term(d, coef2, t);
        }
    } else {
        Add::as_coef_term(a, outArg(coef), outArg(t));
        Add::dict_add_term(d, coef, t);
        Add::as_coef_term(b, outArg(coef), outArg(t));
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

RCP<const Basic> sub(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    return add(a, mul(minus_one, b));
}

RCP<const Basic> add_expand(const RCP<const Add> &self)
{
    umap_basic_int d;
    RCP<const Number> coef_overall = self->coef_;
    RCP<const Number> coef;
    RCP<const Basic> tmp, tmp2;
    for (auto &p: self->dict_) {
        tmp = expand(p.first);
        if (is_a<Add>(*tmp)) {
            for (auto &q: (rcp_static_cast<const Add>(tmp))->dict_) {
                Add::as_coef_term(q.first, outArg(coef), outArg(tmp2));
                Add::dict_add_term(d,
                        mulnum(mulnum(p.second, q.second), coef), tmp2);
            }
            iaddnum(outArg(coef_overall), mulnum(p.second,
                        rcp_static_cast<const Add>(tmp)->coef_));
        } else {
            Add::as_coef_term(tmp, outArg(coef), outArg(tmp));
            Add::dict_add_term(d, mulnum(p.second, coef), tmp);
        }
    }
    return Add::from_dict(coef_overall, d);
}

RCP<const Basic> Add::diff(const RCP<const Symbol> &x) const
{
    CSymPy::umap_basic_int d;
    RCP<const Number> coef=zero, coef2;
    RCP<const Basic> t;
    for (auto &p: dict_) {
        RCP<const Basic> term = p.first->diff(x);
        if (is_a<Integer>(*term) && rcp_static_cast<const Integer>(term)->is_zero()) {
            continue;
        } else if (is_a_Number(*term)) {
            iaddnum(outArg(coef),
                    mulnum(p.second, rcp_static_cast<const Number>(term)));
        } else if (is_a<Add>(*term)) {
            for (auto &q: (rcp_static_cast<const Add>(term))->dict_)
                Add::dict_add_term(d, q.second, q.first);
            iaddnum(outArg(coef), rcp_static_cast<const Add>(term)->coef_);
        } else {
            Add::as_coef_term(mul(p.second, term), outArg(coef2), outArg(t));
            Add::dict_add_term(d, coef2, t);
        }
    }
    return Add::from_dict(coef, d);
}

void Add::as_two_terms(const Ptr<RCP<const Basic>> &a,
            const Ptr<RCP<const Basic>> &b) const
{
    auto p = dict_.begin();
    *a = mul(p->first, p->second);
    umap_basic_int d = dict_;
    d.erase(p->first);
    *b = Add::from_dict(coef_, d);
}

RCP<const Basic> Add::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Add> self = rcp_const_cast<Add>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;

    CSymPy::umap_basic_int d;
    RCP<const Number> coef=coef_, coef2;
    RCP<const Basic> t;
    for (auto &p: dict_) {
        RCP<const Basic> term = p.first->subs(subs_dict);
        if (term == p.first) {
            Add::dict_add_term(d, p.second, p.first);
        } else if (is_a<Integer>(*term) &&
                rcp_static_cast<const Integer>(term)->is_zero()) {
            continue;
        } else if (is_a_Number(*term)) {
            iaddnum(outArg(coef),
                    mulnum(p.second, rcp_static_cast<const Number>(term)));
        } else if (is_a<Add>(*term)) {
            for (auto &q: (rcp_static_cast<const Add>(term))->dict_)
                Add::dict_add_term(d, q.second, q.first);
            iaddnum(outArg(coef), rcp_static_cast<const Add>(term)->coef_);
        } else {
            Add::as_coef_term(mul(p.second, term), outArg(coef2), outArg(t));
            Add::dict_add_term(d, coef2, t);
        }
    }
    return Add::from_dict(coef, d);
}

} // CSymPy
