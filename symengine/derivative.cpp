#include <symengine/visitor.h>
#include <symengine/subs.h>
#include <symengine/symengine_casts.h>

namespace SymEngine
{

extern RCP<const Basic> i2;

class DiffVisitor : public BaseVisitor<DiffVisitor>
{
protected:
    const RCP<const Symbol> x;
    RCP<const Basic> result_;

public:
    DiffVisitor(const RCP<const Symbol> &x) : x(x)
    {
    }

// Uncomment the following define in order to debug the methods:
#define debug_methods
#ifndef debug_methods

    void bvisit(const Basic &self)
    {
        result_ = Derivative::create(self.rcp_from_this(), {x});
    }

#else
// Here we do not have a 'Basic' fallback, but rather must implement all
// virtual methods explicitly (if we miss one, the code will not compile).
// This is useful to check that we have implemented all methods that we
// wanted.

#define DIFF0(CLASS)                                                           \
    void bvisit(const CLASS &self, const RCP<const Symbol> &x)                 \
    {                                                                          \
        result_ = Derivative::create(self.rcp_from_this(), {x});               \
    }

    DIFF0(UnivariateSeries)
    DIFF0(Max)
    DIFF0(Min)
#endif

    void bvisit(const Number &self)
    {
        result_ = zero;
    }

    void bvisit(const Constant &self)
    {
        result_ = zero;
    }

    void bvisit(const Symbol &self)
    {
        if (x->get_name() == self.get_name()) {
            result_ = one;
        } else {
            result_ = zero;
        }
    }

    void bvisit(const Log &self)
    {
        apply(self.get_arg());
        result_ = mul(div(one, self.get_arg()), result_);
    }

    void bvisit(const Abs &self)
    {
        apply(self.get_arg());
        if (eq(*result_, *zero)) {
            result_ = zero;
        } else {
            result_ = Derivative::create(self.rcp_from_this(), {x});
        }
    }

    // Needs create(vec_basic) method to be used.
    template <typename T>
    void fdiff(const T &self)
    {
        RCP<const Basic> diff = zero;
        RCP<const Basic> ret;
        bool know_deriv;

        vec_basic v = self.get_args();
        vec_basic vdiff(v.size());

        unsigned count = 0;
        for (unsigned i = 0; i < v.size(); i++) {
            apply(v[i]);
            vdiff[i] = result_;
            if (neq(*vdiff[i], *zero)) {
                count++;
            }
        }

        if (count == 0) {
            result_ = diff;
            return;
        }

        for (unsigned i = 0; i < v.size(); i++) {
            if (eq(*vdiff[i], *zero))
                continue;
            know_deriv = fdiff(outArg(ret), self, i);
            if (know_deriv) {
                diff = add(diff, mul(ret, vdiff[i]));
            } else {
                if (count == 1 and eq(*v[i], *x)) {
                    result_ = Derivative::create(self.rcp_from_this(), {x});
                    return;
                }
                vec_basic new_args = v;
                std::ostringstream stm;
                stm << (i + 1);
                new_args[i] = get_dummy(self, "xi_" + stm.str());
                map_basic_basic m;
                insert(m, new_args[i], v[i]);
                diff = add(
                    diff,
                    mul(vdiff[i], make_rcp<const Subs>(
                                      Derivative::create(self.create(new_args),
                                                         {new_args[i]}),
                                      m)));
            }
        }
        result_ = diff;
    }

    template <typename T>
    void bvisit(
        const T &self,
        typename std::enable_if<std::is_base_of<TwoArgFunction, T>::value>::type
            * = nullptr)
    {
        fdiff(self);
    }

    static bool fdiff(const Ptr<RCP<const Basic>> &ret, const Zeta &self,
                      unsigned index)
    {
        if (index == 1) {
            *ret = mul(mul(minus_one, self.get_arg1()),
                       zeta(add(self.get_arg1(), one), self.get_arg2()));
            return true;
        } else {
            return false;
        }
    }

    static bool fdiff(const Ptr<RCP<const Basic>> &ret, const UpperGamma &self,
                      unsigned index)
    {
        if (index == 1) {
            *ret = mul(mul(pow(self.get_arg2(), sub(self.get_arg1(), one)),
                           exp(neg(self.get_arg2()))),
                       minus_one);
            return true;
        } else {
            return false;
        }
    }

    static bool fdiff(const Ptr<RCP<const Basic>> &ret, const LowerGamma &self,
                      unsigned index)
    {
        if (index == 1) {
            *ret = mul(pow(self.get_arg2(), sub(self.get_arg1(), one)),
                       exp(neg(self.get_arg2())));
            return true;
        } else {
            return false;
        }
    }

    static bool fdiff(const Ptr<RCP<const Basic>> &ret, const PolyGamma &self,
                      unsigned index)
    {
        if (index == 1) {
            *ret = polygamma(add(self.get_arg1(), one), self.get_arg2());
            return true;
        } else {
            return false;
        }
    }

    static bool fdiff(const Ptr<RCP<const Basic>> &ret, const Function &self,
                      unsigned index)
    {
        // Don't know the derivative, fallback to `Derivative` instances
        return false;
    }

    void bvisit(const ASech &self)
    {
        apply(self.get_arg());
        result_
            = mul(div(minus_one, mul(sqrt(sub(one, pow(self.get_arg(), i2))),
                                     self.get_arg())),
                  result_);
    }

    void bvisit(const ACoth &self)
    {
        apply(self.get_arg());
        result_ = mul(div(one, sub(one, pow(self.get_arg(), i2))), result_);
    }

    void bvisit(const ATanh &self)
    {
        apply(self.get_arg());
        result_ = mul(div(one, sub(one, pow(self.get_arg(), i2))), result_);
    }

    void bvisit(const ACosh &self)
    {
        apply(self.get_arg());
        result_
            = mul(div(one, sqrt(sub(pow(self.get_arg(), i2), one))), result_);
    }

    void bvisit(const ACsch &self)
    {
        apply(self.get_arg());
        result_ = mul(div(minus_one,
                          mul(sqrt(add(one, div(one, pow(self.get_arg(), i2)))),
                              pow(self.get_arg(), i2))),
                      result_);
    }

    void bvisit(const ASinh &self)
    {
        apply(self.get_arg());
        result_
            = mul(div(one, sqrt(add(pow(self.get_arg(), i2), one))), result_);
    }

    void bvisit(const Coth &self)
    {
        apply(self.get_arg());
        result_ = mul(div(minus_one, pow(sinh(self.get_arg()), i2)), result_);
    }

    void bvisit(const Tanh &self)
    {
        apply(self.get_arg());
        result_ = mul(sub(one, pow(tanh(self.get_arg()), i2)), result_);
    }

    void bvisit(const Sech &self)
    {
        apply(self.get_arg());
        result_ = mul(
            mul(mul(minus_one, sech(self.get_arg())), tanh(self.get_arg())),
            result_);
    }

    void bvisit(const Cosh &self)
    {
        apply(self.get_arg());
        result_ = mul(sinh(self.get_arg()), result_);
    }

    void bvisit(const Csch &self)
    {
        apply(self.get_arg());
        result_ = mul(
            mul(mul(minus_one, csch(self.get_arg())), coth(self.get_arg())),
            result_);
    }

    void bvisit(const Sinh &self)
    {
        apply(self.get_arg());
        result_ = mul(cosh(self.get_arg()), result_);
    }

    void bvisit(const Subs &self)
    {
        RCP<const Basic> d = zero, t;
        if (self.get_dict().count(x) == 0) {
            apply(self.get_arg());
            d = result_->subs(self.get_dict());
        }
        for (const auto &p : self.get_dict()) {
            apply(p.second);
            t = result_;
            if (neq(*t, *zero)) {
                if (is_a<Symbol>(*p.first)) {
                    d = add(d,
                            mul(t, diff(self.get_arg(),
                                        rcp_static_cast<const Symbol>(p.first))
                                       ->subs(self.get_dict())));
                } else {
                    result_ = Derivative::create(self.rcp_from_this(), {x});
                    return;
                }
            }
        }
        result_ = d;
    }

    void bvisit(const Derivative &self)
    {
        apply(self.get_arg());
        RCP<const Basic> ret = result_;
        if (eq(*ret, *zero)) {
            result_ = zero;
        }
        multiset_basic t = self.get_symbols();
        for (auto &p : t) {
            // If x is already there in symbols multi-set add x to the symbols
            // multi-set
            if (eq(*p, *x)) {
                t.insert(x);
                result_ = Derivative::create(self.get_arg(), t);
                return;
            }
        }
        // Avoid cycles
        if (is_a<Derivative>(*ret)
            && eq(*down_cast<const Derivative &>(*ret).get_arg(),
                  *self.get_arg())) {
            t.insert(x);
            result_ = Derivative::create(self.get_arg(), t);
            return;
        }
        for (auto &p : t) {
            ret = diff(ret, rcp_static_cast<const Symbol>(p));
        }
        result_ = ret;
    }

    static inline RCP<const Symbol> get_dummy(const Basic &b, std::string name)
    {
        RCP<const Symbol> s;
        do {
            name = "_" + name;
            s = symbol(name);
        } while (has_symbol(b, *s));
        return s;
    }

    void bvisit(const OneArgFunction &self)
    {
        fdiff(self);
    }

    void bvisit(const MultiArgFunction &self)
    {
        fdiff(self);
    }

    void bvisit(const LambertW &self)
    {
        // check http://en.wikipedia.org/wiki/Lambert_W_function#Derivative
        // for the equation
        apply(self.get_arg());
        RCP<const Basic> lambertw_val = lambertw(self.get_arg());
        result_ = mul(
            div(lambertw_val, mul(self.get_arg(), add(lambertw_val, one))),
            result_);
    }

    void bvisit(const Add &self)
    {
        SymEngine::umap_basic_num d;
        RCP<const Number> coef = zero, coef2;
        RCP<const Basic> t;
        for (auto &p : self.get_dict()) {
            apply(p.first);
            RCP<const Basic> term = result_;
            if (is_a<Integer>(*term)
                && down_cast<const Integer &>(*term).is_zero()) {
                continue;
            } else if (is_a_Number(*term)) {
                iaddnum(outArg(coef),
                        mulnum(p.second, rcp_static_cast<const Number>(term)));
            } else if (is_a<Add>(*term)) {
                for (auto &q : (down_cast<const Add &>(*term)).get_dict())
                    Add::dict_add_term(d, mulnum(q.second, p.second), q.first);
                iaddnum(
                    outArg(coef),
                    mulnum(p.second, down_cast<const Add &>(*term).get_coef()));
            } else {
                Add::as_coef_term(mul(p.second, term), outArg(coef2),
                                  outArg(t));
                Add::dict_add_term(d, coef2, t);
            }
        }
        result_ = Add::from_dict(coef, std::move(d));
    }

    void bvisit(const Mul &self)
    {
        RCP<const Number> overall_coef = zero;
        umap_basic_num add_dict;
        for (auto &p : self.get_dict()) {
            RCP<const Number> coef = self.get_coef();
            apply(pow(p.first, p.second));
            RCP<const Basic> factor = result_;
            if (is_a<Integer>(*factor)
                && down_cast<const Integer &>(*factor).is_zero())
                continue;
            map_basic_basic d = self.get_dict();
            d.erase(p.first);
            if (is_a_Number(*factor)) {
                imulnum(outArg(coef), rcp_static_cast<const Number>(factor));
            } else if (is_a<Mul>(*factor)) {
                RCP<const Mul> tmp = rcp_static_cast<const Mul>(factor);
                imulnum(outArg(coef), tmp->get_coef());
                for (auto &q : tmp->get_dict()) {
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
                Add::coef_dict_add_term(outArg(overall_coef), add_dict, coef,
                                        mul);
            }
        }
        result_ = Add::from_dict(overall_coef, std::move(add_dict));
    }

    void bvisit(const Pow &self)
    {
        if (is_a_Number(*(self.get_exp()))) {
            apply(self.get_base());
            result_ = mul(mul(self.get_exp(),
                              pow(self.get_base(), sub(self.get_exp(), one))),
                          result_);
        } else {
            apply(mul(self.get_exp(), log(self.get_base())));
            result_ = mul(self.rcp_from_this(), result_);
        }
    }

    void bvisit(const Sin &self)
    {
        apply(self.get_arg());
        result_ = mul(cos(self.get_arg()), result_);
    }

    void bvisit(const Cos &self)
    {
        apply(self.get_arg());
        result_ = mul(mul(minus_one, sin(self.get_arg())), result_);
    }

    void bvisit(const Tan &self)
    {
        apply(self.get_arg());
        RCP<const Integer> two = integer(2);
        result_ = mul(add(one, pow(tan(self.get_arg()), two)), result_);
    }

    void bvisit(const Cot &self)
    {
        apply(self.get_arg());
        RCP<const Integer> two = integer(2);
        result_ = mul(mul(add(one, pow(cot(self.get_arg()), two)), minus_one),
                      result_);
    }

    void bvisit(const Csc &self)
    {
        apply(self.get_arg());
        result_
            = mul(mul(mul(cot(self.get_arg()), csc(self.get_arg())), minus_one),
                  result_);
    }

    void bvisit(const Sec &self)
    {
        apply(self.get_arg());
        result_ = mul(mul(tan(self.get_arg()), sec(self.get_arg())), result_);
    }

    void bvisit(const ASin &self)
    {
        apply(self.get_arg());
        result_
            = mul(div(one, sqrt(sub(one, pow(self.get_arg(), i2)))), result_);
    }

    void bvisit(const ACos &self)
    {
        apply(self.get_arg());
        result_ = mul(div(minus_one, sqrt(sub(one, pow(self.get_arg(), i2)))),
                      result_);
    }

    void bvisit(const ASec &self)
    {
        apply(self.get_arg());
        result_ = mul(
            div(one, mul(pow(self.get_arg(), i2),
                         sqrt(sub(one, div(one, pow(self.get_arg(), i2)))))),
            result_);
    }

    void bvisit(const ACsc &self)
    {
        apply(self.get_arg());
        result_
            = mul(div(minus_one,
                      mul(pow(self.get_arg(), i2),
                          sqrt(sub(one, div(one, pow(self.get_arg(), i2)))))),
                  result_);
    }

    void bvisit(const ATan &self)
    {
        apply(self.get_arg());
        result_ = mul(div(one, add(one, pow(self.get_arg(), i2))), result_);
    }

    void bvisit(const ACot &self)
    {
        apply(self.get_arg());
        result_
            = mul(div(minus_one, add(one, pow(self.get_arg(), i2))), result_);
    }

    void bvisit(const ATan2 &self)
    {
        apply(div(self.get_num(), self.get_den()));
        result_
            = mul(div(pow(self.get_den(), i2),
                      add(pow(self.get_den(), i2), pow(self.get_num(), i2))),
                  result_);
    }

    void bvisit(const Erf &self)
    {
        apply(self.get_arg());
        result_ = mul(
            div(mul(integer(2), exp(neg(mul(self.get_arg(), self.get_arg())))),
                sqrt(pi)),
            result_);
    }

    void bvisit(const Erfc &self)
    {
        apply(self.get_arg());
        result_ = neg(mul(
            div(mul(integer(2), exp(neg(mul(self.get_arg(), self.get_arg())))),
                sqrt(pi)),
            result_));
    }

    void bvisit(const Gamma &self)
    {
        apply(self.get_arg());
        result_
            = mul(mul(self.rcp_from_this(), polygamma(zero, self.get_arg())),
                  result_);
    }

    void bvisit(const LogGamma &self)
    {
        apply(self.get_arg());
        result_ = mul(polygamma(zero, self.get_arg()), result_);
    }

    template <typename Poly, typename Dict>
    void diff_upoly(const Poly &self)
    {
        if (self.get_var()->__eq__(*x)) {
            Dict d;
            for (auto it = self.begin(); it != self.end(); ++it) {
                if (it->first != 0)
                    d[it->first - 1] = it->second * it->first;
            }
            result_ = Poly::from_dict(self.get_var(), std::move(d));
        } else {
            result_ = Poly::from_dict(self.get_var(), {{}});
        }
    }

    void bvisit(const UIntPoly &self)
    {
        diff_upoly<UIntPoly, map_uint_mpz>(self);
    }

    void bvisit(const URatPoly &self)
    {
        diff_upoly<URatPoly, map_uint_mpq>(self);
    }

#ifdef HAVE_SYMENGINE_PIRANHA
    void bvisit(const UIntPolyPiranha &self)
    {
        diff_upoly<UIntPolyPiranha, map_uint_mpz>(self);
    }
    void bvisit(const URatPolyPiranha &self)
    {
        diff_upoly<URatPolyPiranha, map_uint_mpq>(self);
    }
#endif

#ifdef HAVE_SYMENGINE_FLINT
    template <typename P>
    void diff_upolyflint(const P &self)
    {
        if (self.get_var()->__eq__(*x)) {
            result_ = P::from_container(self.get_var(),
                                        self.get_poly().derivative());
        } else {
            result_ = P::from_dict(self.get_var(), {{}});
        }
    }

    void bvisit(const UIntPolyFlint &self)
    {
        diff_upolyflint(self);
    }

    void bvisit(const URatPolyFlint &self)
    {
        diff_upolyflint(self);
    }
#endif

    void bvisit(const UExprPoly &self)
    {
        diff_upoly<UExprPoly, map_int_Expr>(self);
    }

    template <typename Container, typename Poly>
    void bvisit(const MSymEnginePoly<Container, Poly> &self)
    {
        using Dict = typename Container::dict_type;
        using Vec = typename Container::vec_type;
        Dict dict;

        if (self.get_vars().find(x) != self.get_vars().end()) {
            auto i = self.get_vars().begin();
            unsigned int index = 0;
            while (!(*i)->__eq__(*x)) {
                i++;
                index++;
            } // find the index of the variable we are differentiating WRT.
            for (auto bucket : self.get_poly().dict_) {
                if (bucket.first[index] != 0) {
                    Vec v = bucket.first;
                    v[index]--;
                    dict.insert({v, bucket.second * bucket.first[index]});
                }
            }
            vec_basic v;
            v.insert(v.begin(), self.get_vars().begin(), self.get_vars().end());
            result_ = Poly::from_dict(v, std::move(dict));
        } else {
            vec_basic vs;
            vs.insert(vs.begin(), self.get_vars().begin(),
                      self.get_vars().end());
            result_ = Poly::from_dict(vs, {{}});
        }
    }

    void bvisit(const FunctionWrapper &self)
    {
        result_ = self.diff_impl(x);
    }

    void bvisit(const Beta &self)
    {
        RCP<const Basic> beta_arg0 = self.get_args()[0];
        RCP<const Basic> beta_arg1 = self.get_args()[1];
        apply(beta_arg0);
        RCP<const Basic> diff_beta_arg0 = result_;
        apply(beta_arg1);
        RCP<const Basic> diff_beta_arg1 = result_;
        result_ = mul(self.rcp_from_this(),
                      add(mul(polygamma(zero, beta_arg0), diff_beta_arg0),
                          sub(mul(polygamma(zero, beta_arg1), diff_beta_arg1),
                              mul(polygamma(zero, add(beta_arg0, beta_arg1)),
                                  add(diff_beta_arg0, diff_beta_arg1)))));
    }

    void bvisit(const Set &self)
    {
        throw SymEngineException("Derivative doesn't exist.");
    }

    void bvisit(const Boolean &self)
    {
        throw SymEngineException("Derivative doesn't exist.");
    }

    void bvisit(const GaloisField &self)
    {
        GaloisFieldDict d;
        if (self.get_var()->__eq__(*x)) {
            d = self.get_poly().gf_diff();
            result_ = GaloisField::from_dict(self.get_var(), std::move(d));
        } else {
            result_ = GaloisField::from_dict(self.get_var(), std::move(d));
        }
    }

    void bvisit(const Piecewise &self)
    {
        PiecewiseVec v = self.get_vec();
        for (auto &p : v) {
            apply(p.first);
            p.first = result_;
        }
        result_ = piecewise(std::move(v));
    }

    void apply(const Basic &b)
    {
        apply(b.rcp_from_this());
    }

    void apply(const RCP<const Basic> &b)
    {
        b->accept(*this);
    }

    RCP<const Basic> get_result()
    {
        return result_;
    }
};

RCP<const Basic> diff(const RCP<const Basic> &arg, const RCP<const Symbol> &x)
{
    DiffVisitor v(x);
    v.apply(arg);
    return v.get_result();
}

RCP<const Basic> Basic::diff(const RCP<const Symbol> &x) const
{
    return SymEngine::diff(this->rcp_from_this(), x);
}

//! SymPy style differentiation for non-symbol variables
// Since SymPy's differentiation makes no sense mathematically, it is
// defined separately here for compatibility
RCP<const Basic> sdiff(const RCP<const Basic> &arg, const RCP<const Basic> &x)
{
    if (is_a<Symbol>(*x)) {
        return arg->diff(rcp_static_cast<const Symbol>(x));
    } else {
        RCP<const Symbol> d = DiffVisitor::get_dummy(*arg, "x");
        return ssubs(ssubs(arg, {{x, d}})->diff(d), {{d, x}});
    }
}

} // SymEngine
