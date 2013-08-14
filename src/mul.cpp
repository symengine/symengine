#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "rational.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;

namespace CSymPy {

Mul::Mul(const Teuchos::RCP<Number> &coef, const map_basic_basic& dict)
    : coef_{coef}, dict_{dict}
{
    CSYMPY_ASSERT(is_canonical(coef, dict))
}

bool Mul::is_canonical(const Teuchos::RCP<Number> &coef,
        const map_basic_basic& dict)
{
    if (coef == Teuchos::null) return false;
    // e.g. 0*x*y
    if (coef->is_zero())
        return false;
    if (dict.size() == 0) return false;
    if (dict.size() == 1) {
        // e.g. 1*x, 1*x^2
        if (coef->is_one()) return false;
    }
    // Check that each term in 'dict' is in canonical form
    for (auto &p: dict) {
        if (p.first == Teuchos::null) return false;
        if (p.second == Teuchos::null) return false;
        // e.g. 2^3, (2/3)^4
        if (is_a_Number(*p.first) && is_a<Integer>(*p.second))
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
        // e.g. (x*y)^2 (={xy:2}), which should be represented as x^2*y^2
        //     (={x:2, y:2})
        if (is_a<Mul>(*p.first))
            return false;
        // e.g. x^2^y (={x^2:y}), which should be represented as x^(2y)
        //     (={x:2y})
        if (is_a<Pow>(*p.first))
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

RCP<CSymPy::Basic> Mul::from_dict(const RCP<Number> &coef, const map_basic_basic &d)
{
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if (coef->is_one()) {
                if ((rcp_static_cast<Integer>(p->second))->is_one()) {
                    // For x^1 we simply return "x":
                    return p->first;
                }
            } else {
                // For coef*x or coef*x^3 we simply return Mul:
                return rcp(new Mul(coef, d));
            }
        }
        if (coef->is_one()) {
            // Create a Pow() here:
            return pow(p->first, p->second);
        } else {
            return rcp(new Mul(coef, d));
        }
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
        if (is_a_Number(*it->second) && is_a_Number(*exp)) {
            RCP<Number> tmp = rcp_static_cast<Number>(it->second);
            iaddnum(outArg(tmp), rcp_static_cast<Number>(exp));
            if (tmp->is_zero()) {
                d.erase(it);
            } else {
                it->second = tmp;
            }
        } else {
            // General case:
            it->second = add(it->second, exp);
            if (is_a_Number(*it->second) &&
                    rcp_static_cast<Number>(it->second)->is_zero()) {
                d.erase(it);
            }
        }
    }
}

void Mul::as_coef_term(const Teuchos::Ptr<Teuchos::RCP<Number>> &coef,
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

void Mul::as_base_exp(const RCP<Basic> &self, const Ptr<RCP<Basic>> &exp,
        const Ptr<RCP<Basic>> &base)
{
    if (is_a<Symbol>(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a_Number(*self)) {
        *exp = one;
        *base = self;
    } else if (is_a<Pow>(*self)) {
        *exp = rcp_static_cast<Pow>(self)->exp_;
        *base = rcp_static_cast<Pow>(self)->base_;
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
    RCP<Number> coef = one;
    if (CSymPy::is_a<Mul>(*a) && CSymPy::is_a<Mul>(*b)) {
        RCP<Mul> A = rcp_static_cast<Mul>(a);
        RCP<Mul> B = rcp_static_cast<Mul>(b);
        // This is important optimization, as coef=1 if Mul is inside an Add.
        // To further speed this up, the upper level code could tell us that we
        // are inside an Add, then we don't even have can simply skip the
        // following two lines.
        if (!(A->coef_->is_one()) || !(B->coef_->is_one()))
            coef = mulnum(A->coef_, B->coef_);
        d = A->dict_;
        for (auto &p: B->dict_)
            Mul::dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Mul>(*a)) {
        RCP<Basic> exp;
        RCP<Basic> t;
        coef = (rcp_static_cast<Mul>(a))->coef_;
        d = (rcp_static_cast<Mul>(a))->dict_;
        if (is_a_Number(*b)) {
            imulnum(outArg(coef), rcp_static_cast<Number>(b));
        } else {
            Mul::as_base_exp(b, outArg(exp), outArg(t));
            Mul::dict_add_term(d, exp, t);
        }
    } else if (CSymPy::is_a<Mul>(*b)) {
        RCP<Basic> exp;
        RCP<Basic> t;
        coef = (rcp_static_cast<Mul>(b))->coef_;
        d = (rcp_static_cast<Mul>(b))->dict_;
        if (is_a_Number(*a)) {
            imulnum(outArg(coef), rcp_static_cast<Number>(a));
        } else {
            Mul::as_base_exp(a, outArg(exp), outArg(t));
            Mul::dict_add_term(d, exp, t);
        }
    } else {
        RCP<Basic> exp;
        RCP<Basic> t;
        Mul::as_base_exp(a, outArg(exp), outArg(t));
        d[t] = exp;
        Mul::as_base_exp(b, outArg(exp), outArg(t));
        Mul::dict_add_term(d, exp, t);

        CSymPy::map_basic_basic d2;
        for (auto &p: d) {
            if (is_a_Number(*(p.first)) && is_a_Number(*(p.second))) {
                RCP<Number> f = rcp_static_cast<Number>(p.first);
                RCP<Number> s = rcp_static_cast<Number>(p.second);
                RCP<Number> r = pownum(f, s);
                imulnum(outArg(coef), r);
            } else {
                // TODO: this can be spedup
                Mul::dict_add_term(d2, p.second, p.first);
            }
        }
        return Mul::from_dict(coef, d2);
    }
    return Mul::from_dict(coef, d);
}

RCP<Basic> div(const RCP<Basic> &a, const RCP<Basic> &b)
{
    return mul(a, pow(b, minus_one));
}

RCP<Basic> mul_expand_two(const RCP<Basic> &a, const RCP<Basic> &b)
{
    // Both a and b are assumed to be expanded
    if (is_a<Add>(*a) && is_a<Add>(*b)) {
        umap_basic_int d;
        for (auto &p: (rcp_static_cast<Add>(a))->dict_) {
            for (auto &q: (rcp_static_cast<Add>(b))->dict_) {
                // The main bottleneck here is the mul(p.first, q.first) command
                Add::dict_add_term(d, mulnum(p.second, q.second),
                        mul(p.first, q.first));
            }
        }
        return Add::from_dict(zero, d);
    } else if (is_a<Add>(*a)) {
        return mul_expand_two(b, a);
    } else if (is_a<Add>(*b)) {
        umap_basic_int d;
        RCP<Number> coef_overall=rcp_static_cast<Add>(b)->coef_;
        RCP<Number> coef;
        RCP<Basic> tmp;

        if (!coef_overall->is_zero()) {
            tmp = mul(a, coef_overall);
            if (is_a<Mul>(*tmp)) {
                rcp_static_cast<Mul>(tmp)->as_coef_term(outArg(coef),
                        outArg(tmp));
            } else {
                coef = one;
            }
            Add::dict_add_term(d, coef, tmp);
        }

        for (auto &p: (rcp_static_cast<Add>(b))->dict_) {
            tmp = mul(a, p.first);
            if (is_a<Mul>(*tmp)) {
                rcp_static_cast<Mul>(tmp)->as_coef_term(outArg(coef),
                        outArg(tmp));
            } else {
                coef = one;
            }
            Add::dict_add_term(d,
                    mulnum(p.second, coef), tmp);
        }

        auto it = d.find(one);
        if (it == d.end()) {
            coef_overall = zero;
        } else {
            coef_overall = it->second;
            d.erase(it);
        }

        coef_overall = zero;
        CSymPy::umap_basic_int d2;
        // TODO: think about speeding this kind of loop up:
        for (auto &p: d) {
            if (is_a_Number(*(p.first)) && is_a_Number(*(p.second))) {
                RCP<Number> f = rcp_static_cast<Number>(p.first);
                RCP<Number> s = rcp_static_cast<Number>(p.second);
                RCP<Number> r = mulnum(f, s);
                iaddnum(outArg(coef_overall), r);
            } else {
                d2[p.first] = p.second;
            }
        }

        return Add::from_dict(coef_overall, d2);
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

Teuchos::RCP<Basic> Mul::power_all_terms(const Teuchos::RCP<Basic> &exp)
{
    CSymPy::map_basic_basic d;
    RCP<Basic> new_coef = pow(coef_, exp);
    RCP<Basic> new_exp;
    for (auto &p: dict_) {
        new_exp = mul(p.second, exp);
        if (is_a<Integer>(*new_exp) &&
                rcp_static_cast<Integer>(new_exp)->is_zero()) continue;
        Mul::dict_add_term(d, new_exp, p.first);
    }
    if (is_a<Number>(*new_coef)) {
        return Mul::from_dict(rcp_static_cast<Number>(new_coef), d);
    } else {
        // TODO: this can be made faster probably:
        return mul(new_coef, Mul::from_dict(one, d));
    }
}

} // CSymPy
