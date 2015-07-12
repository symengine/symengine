#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <symengine/diff.h>

namespace SymEngine {

class DiffVisitor : public BaseVisitor<DiffVisitor> {
private:
    const RCP<const Symbol> x_;
    RCP<const Basic> result_;
public:
    DiffVisitor(const RCP<const Symbol> &x) : BaseVisitor(this), x_{x} { }

    RCP<const Basic> apply(const Basic &b) {
        b.accept(*this);
        return result_;
    }

    void bvisit(const Integer &x) {
        result_ = zero;
    }

    void bvisit(const Rational &x) {
        result_ = zero;
    }

    void bvisit(const RealDouble &x) {
        result_ = zero;
    }

    void bvisit(const Complex &x) {
        result_ = zero;
    }

    void bvisit(const ComplexDouble &x) {
        result_ = zero;
    }

    void bvisit(const RealMPFR &x) {
        result_ = zero;
    }

    void bvisit(const ComplexMPC &x) {
        result_ = zero;
    }

    void bvisit(const Add &x) {
        SymEngine::umap_basic_num d;
        RCP<const Number> coef=zero, coef2;
        RCP<const Basic> t;
        for (auto &p: x.dict_) {
            RCP<const Basic> term = p.first->diff(x_);
            if (is_a<Integer>(*term) && rcp_static_cast<const Integer>(term)->is_zero()) {
                continue;
            } else if (is_a_Number(*term)) {
                iaddnum(outArg(coef),
                        mulnum(p.second, rcp_static_cast<const Number>(term)));
            } else if (is_a<Add>(*term)) {
                for (auto &q: (rcp_static_cast<const Add>(term))->dict_)
                    Add::dict_add_term(d, mulnum(q.second, p.second), q.first);
                iaddnum(outArg(coef), mulnum(p.second, rcp_static_cast<const Add>(term)->coef_));
            } else {
                Add::as_coef_term(mul(p.second, term), outArg(coef2), outArg(t));
                Add::dict_add_term(d, coef2, t);
            }
        }
        result_ = Add::from_dict(coef, std::move(d));
    }

    void bvisit(const Mul &x) {
        RCP<const Number> overall_coef = zero;
        umap_basic_num add_dict;
        for (auto &p: x.dict_) {
            RCP<const Number> coef = x.coef_;
            RCP<const Basic> factor = pow(p.first, p.second)->diff(x_);
            if (is_a<Integer>(*factor) &&
                    rcp_static_cast<const Integer>(factor)->is_zero()) continue;
            map_basic_basic d = x.dict_;
            d.erase(p.first);
            if (is_a_Number(*factor)) {
                imulnum(outArg(coef), rcp_static_cast<const Number>(factor));
            } else if (is_a<Mul>(*factor)) {
                RCP<const Mul> tmp = rcp_static_cast<const Mul>(factor);
                imulnum(outArg(coef), tmp->coef_);
                for (auto &q: tmp->dict_) {
                    Mul::dict_add_term_new(outArg(coef), d, q.second, q.first);
                }
            } else {
                RCP<const Basic> exp, t;
                Mul::as_base_exp(factor, outArg(exp), outArg(t));
                Mul::dict_add_term_new(outArg(coef), d, exp, t);
            }
            if (d.size() == 0) {
                iaddnum(outArg(overall_coef), coef);
            } else {
                RCP<const Basic> mul = Mul::from_dict(one, std::move(d));
                Add::dict_add_term(add_dict, coef, mul);
            }
        }
        result_ = Add::from_dict(overall_coef, std::move(add_dict));
    }

    void bvisit(const Basic &x) {
        result_ = x.diff(x_);
    }

};

RCP<const Basic> diff(const RCP<const Basic> &self, const RCP<const Symbol> &x)
{
    DiffVisitor v(x);
    return v.apply(*self);
}

} // SymEngine
