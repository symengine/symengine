#ifndef SYMENGINE_SUBS_H
#define SYMENGINE_SUBS_H

#include <symengine/visitor.h>

namespace SymEngine
{

class SubsVisitor : public BaseVisitor<SubsVisitor>
{
protected:
    RCP<const Basic> result_;
    const map_basic_basic &subs_dict_;

public:
    SubsVisitor(const map_basic_basic &subs_dict) : subs_dict_(subs_dict)
    {
    }
    // TODO : Polynomials, Series, Sets
    void bvisit(const Basic &x)
    {
        result_ = x.rcp_from_this();
    }

    void bvisit(const Add &x)
    {
        SymEngine::umap_basic_num d;
        RCP<const Number> coef;

        auto it = subs_dict_.find(x.coef_);
        if (it != subs_dict_.end()) {
            coef = zero;
            Add::coef_dict_add_term(outArg(coef), d, one, it->second);
        } else {
            coef = x.coef_;
        }

        for (const auto &p : x.dict_) {
            auto it
                = subs_dict_.find(Add::from_dict(zero, {{p.first, p.second}}));
            if (it != subs_dict_.end()) {
                Add::coef_dict_add_term(outArg(coef), d, one, it->second);
            } else {
                it = subs_dict_.find(p.second);
                if (it != subs_dict_.end()) {
                    Add::coef_dict_add_term(outArg(coef), d, one,
                                            mul(it->second, apply(p.first)));
                } else {
                    Add::coef_dict_add_term(outArg(coef), d, p.second,
                                            apply(p.first));
                }
            }
        }
        result_ = Add::from_dict(coef, std::move(d));
    }

    void bvisit(const Mul &x)
    {
        RCP<const Number> coef = x.coef_;
        map_basic_basic d;
        for (const auto &p : x.dict_) {
            RCP<const Basic> factor_old;
            if (eq(*p.second, *one)) {
                factor_old = p.first;
            } else {
                factor_old = make_rcp<Pow>(p.first, p.second);
            }
            RCP<const Basic> factor = apply(factor_old);
            if (factor == factor_old) {
                // TODO: Check if Mul::dict_add_term is enough
                Mul::dict_add_term_new(outArg(coef), d, p.second, p.first);
            } else if (is_a_Number(*factor)) {
                if (rcp_static_cast<const Number>(factor)->is_zero()) {
                    result_ = factor;
                    return;
                }
                imulnum(outArg(coef), rcp_static_cast<const Number>(factor));
            } else if (is_a<Mul>(*factor)) {
                RCP<const Mul> tmp = rcp_static_cast<const Mul>(factor);
                imulnum(outArg(coef), tmp->coef_);
                for (const auto &q : tmp->dict_) {
                    Mul::dict_add_term_new(outArg(coef), d, q.second, q.first);
                }
            } else {
                RCP<const Basic> exp, t;
                Mul::as_base_exp(factor, outArg(exp), outArg(t));
                Mul::dict_add_term_new(outArg(coef), d, exp, t);
            }
        }
        result_ = Mul::from_dict(coef, std::move(d));
    }

    void bvisit(const Pow &x)
    {
        RCP<const Basic> base_new = apply(x.get_base());
        RCP<const Basic> exp_new = apply(x.get_exp());
        if (base_new == x.get_base() and exp_new == x.get_exp())
            result_ = x.rcp_from_this();
        else
            result_ = pow(base_new, exp_new);
    }

    void bvisit(const OneArgFunction &x)
    {
        apply(x.get_arg());
        if (result_ == x.get_arg()) {
            result_ = x.rcp_from_this();
        } else {
            result_ = x.create(result_);
        }
    }

    void bvisit(const TwoArgFunction &x)
    {
        RCP<const Basic> a = apply(x.get_arg1());
        RCP<const Basic> b = apply(x.get_arg2());
        if (a == x.get_arg1() and b == x.get_arg2())
            result_ = x.rcp_from_this();
        else
            result_ = x.create(a, b);
    }

    void bvisit(const MultiArgFunction &x)
    {
        vec_basic v = x.get_args();
        for (auto &elem : v) {
            elem = apply(elem);
        }
        result_ = x.create(v);
    }

    void bvisit(const FunctionSymbol &x)
    {
        vec_basic v = x.get_args();
        for (auto &elem : v) {
            elem = apply(elem);
        }
        result_ = x.create(v);
    }

    void bvisit(const Derivative &x)
    {
        RCP<const Symbol> s;
        map_basic_basic m, n;
        bool subs;
        for (const auto &p : subs_dict_) {
            subs = true;
            if (eq(*x.get_arg()->subs({{p.first, p.second}}), *x.get_arg()))
                continue;
            // If p.first and p.second are symbols and arg_ is
            // independent of p.second, p.first can be replaced
            if (is_a<Symbol>(*p.first) and is_a<Symbol>(*p.second)
                and eq(*x.get_arg()->diff(
                           rcp_static_cast<const Symbol>(p.second)),
                       *zero)) {
                insert(n, p.first, p.second);
                continue;
            }
            for (const auto &d : x.get_symbols()) {
                if (is_a<Symbol>(*d)) {
                    s = rcp_static_cast<const Symbol>(d);
                    // If p.first or p.second has non zero derivates wrt to s
                    // p.first cannot be replaced
                    if (neq(*zero, *(p.first->diff(s)))
                        || neq(*zero, *(p.second->diff(s)))) {
                        subs = false;
                        break;
                    }
                } else {
                    result_
                        = make_rcp<const Subs>(x.rcp_from_this(), subs_dict_);
                    return;
                }
            }
            if (subs) {
                insert(n, p.first, p.second);
            } else {
                insert(m, p.first, p.second);
            }
        }
        multiset_basic sym;
        for (auto &p : x.get_symbols()) {
            sym.insert(p->subs(n));
        }
        if (m.empty()) {
            result_ = Derivative::create(x.get_arg()->subs(n), sym);
        } else {
            result_ = make_rcp<const Subs>(
                Derivative::create(x.get_arg()->subs(n), sym), m);
        }
    }

    void bvisit(const Subs &x)
    {
        map_basic_basic m, n;
        for (const auto &p : subs_dict_) {
            bool found = false;
            for (const auto &s : x.get_dict()) {
                if (neq(*(s.first->subs({{p.first, p.second}})), *(s.first))) {
                    found = true;
                    break;
                }
            }
            // If p.first is not replaced in arg_ by dict_,
            // store p.first in n to replace in arg_
            if (not found) {
                insert(n, p.first, p.second);
            }
        }
        for (const auto &s : x.get_dict()) {
            insert(m, s.first, apply(s.second));
        }
        RCP<const Basic> presub = x.get_arg()->subs(n);
        if (is_a<Subs>(*presub)) {
            for (auto &q : static_cast<const Subs &>(*presub).get_dict()) {
                insert(m, q.first, q.second);
            }
            result_ = make_rcp<const Subs>(
                static_cast<const Subs &>(*presub).get_arg(), m);
        } else {
            result_ = make_rcp<const Subs>(presub, m);
        }
    }

    RCP<const Basic> apply(const Basic &x)
    {
        return apply(x.rcp_from_this());
    }

    RCP<const Basic> apply(const RCP<const Basic> &x)
    {
        auto it = subs_dict_.find(x);
        if (it != subs_dict_.end()) {
            result_ = it->second;
        } else {
            x->accept(*this);
        }
        return result_;
    }
};

class MSubsVisitor : public BaseVisitor<MSubsVisitor, SubsVisitor>
{
public:
    using SubsVisitor::bvisit;

    MSubsVisitor(const map_basic_basic &d)
        : BaseVisitor<MSubsVisitor, SubsVisitor>(d)
    {
    }

    void bvisit(const Derivative &x)
    {
        result_ = x.rcp_from_this();
    }
};

//! Mappings in the `subs_dict` are applied to the expression tree of `x`
inline RCP<const Basic> subs(const RCP<const Basic> &x,
                             const map_basic_basic &subs_dict)
{
    SubsVisitor s(subs_dict);
    return s.apply(x);
}

//! Subs which treat f(t) and Derivative(f(t), t) as separate variables
inline RCP<const Basic> msubs(const RCP<const Basic> &x,
                              const map_basic_basic &subs_dict)
{
    MSubsVisitor s(subs_dict);
    return s.apply(x);
}

} // namespace SymEngine

#endif // SYMENGINE_SUBS_H
