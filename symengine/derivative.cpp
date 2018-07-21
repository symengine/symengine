#include <symengine/visitor.h>
#include <symengine/subs.h>
#include <symengine/symengine_casts.h>

namespace SymEngine
{

extern RCP<const Basic> i2;

class DiffImplementation
{
public:
// Uncomment the following define in order to debug the methods:
#define debug_methods
#ifndef debug_methods

    static RCP<const Basic> diff(const Basic &self, const RCP<const Symbol> &x)
    {
        return Derivative::create(self.rcp_from_this(), {x});
    }

#else
// Here we do not have a 'Basic' fallback, but rather must implement all
// virtual methods explicitly (if we miss one, the code will not compile).
// This is useful to check that we have implemented all methods that we
// wanted.

#define DIFF0(CLASS)                                                           \
    static RCP<const Basic> diff(const CLASS &self,                            \
                                 const RCP<const Symbol> &x)                   \
    {                                                                          \
        return Derivative::create(self.rcp_from_this(), {x});                  \
    }

    DIFF0(UnivariateSeries)
    DIFF0(Max)
    DIFF0(Min)
#endif

    static RCP<const Basic> diff(const Number &self, const RCP<const Symbol> &x)
    {
        return zero;
    }

    static RCP<const Basic> diff(const Constant &self,
                                 const RCP<const Symbol> &x)
    {
        return zero;
    }

    static RCP<const Basic> diff(const Symbol &self, const RCP<const Symbol> &x)
    {
        if (x->get_name() == self.get_name())
            return one;
        else
            return zero;
    }

    static RCP<const Basic> diff(const Log &self, const RCP<const Symbol> &x)
    {
        return mul(div(one, self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Abs &self, const RCP<const Symbol> &x)
    {
        if (eq(*self.get_arg()->diff(x), *zero))
            return zero;
        else
            return Derivative::create(self.rcp_from_this(), {x});
    }

    // Needs create(vec_basic) method to be used.
    template <typename T>
    static RCP<const Basic> fdiff(const T &self, const RCP<const Symbol> &x)
    {
        RCP<const Basic> diff = zero;
        RCP<const Basic> ret;
        bool know_deriv;

        vec_basic v = self.get_args();
        vec_basic vdiff(v.size());

        unsigned count = 0;
        for (unsigned i = 0; i < v.size(); i++) {
            vdiff[i] = v[i]->diff(x);
            if (neq(*vdiff[i], *zero)) {
                count++;
            }
        }

        if (count == 0) {
            return diff;
        }

        for (unsigned i = 0; i < v.size(); i++) {
            if (eq(*vdiff[i], *zero))
                continue;
            know_deriv = fdiff(outArg(ret), self, i);
            if (know_deriv) {
                diff = add(diff, mul(ret, vdiff[i]));
            } else {
                if (count == 1 and eq(*v[i], *x)) {
                    return Derivative::create(self.rcp_from_this(), {x});
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
        return diff;
    }

    template <typename T>
    static RCP<const Basic> diff(
        const T &self, const RCP<const Symbol> &x,
        typename std::enable_if<std::is_base_of<TwoArgFunction, T>::value>::type
            * = nullptr)
    {
        return fdiff(self, x);
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

    static RCP<const Basic> diff(const ASech &self, const RCP<const Symbol> &x)
    {
        return mul(div(minus_one, mul(sqrt(sub(one, pow(self.get_arg(), i2))),
                                      self.get_arg())),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACoth &self, const RCP<const Symbol> &x)
    {
        return mul(div(one, sub(one, pow(self.get_arg(), i2))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ATanh &self, const RCP<const Symbol> &x)
    {
        return mul(div(one, sub(one, pow(self.get_arg(), i2))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACosh &self, const RCP<const Symbol> &x)
    {
        return mul(div(one, sqrt(sub(pow(self.get_arg(), i2), one))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACsch &self, const RCP<const Symbol> &x)
    {
        return mul(div(minus_one,
                       mul(sqrt(add(one, div(one, pow(self.get_arg(), i2)))),
                           pow(self.get_arg(), i2))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ASinh &self, const RCP<const Symbol> &x)
    {
        return mul(div(one, sqrt(add(pow(self.get_arg(), i2), one))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Coth &self, const RCP<const Symbol> &x)
    {
        return mul(div(minus_one, pow(sinh(self.get_arg()), i2)),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Tanh &self, const RCP<const Symbol> &x)
    {
        return mul(sub(one, pow(tanh(self.get_arg()), i2)),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Sech &self, const RCP<const Symbol> &x)
    {
        return mul(
            mul(mul(minus_one, sech(self.get_arg())), tanh(self.get_arg())),
            self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cosh &self, const RCP<const Symbol> &x)
    {
        return mul(sinh(self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Csch &self, const RCP<const Symbol> &x)
    {
        return mul(
            mul(mul(minus_one, csch(self.get_arg())), coth(self.get_arg())),
            self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Sinh &self, const RCP<const Symbol> &x)
    {
        return mul(cosh(self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Subs &self, const RCP<const Symbol> &x)
    {
        RCP<const Basic> diff = zero, t;
        if (self.get_dict().count(x) == 0) {
            diff = self.get_arg()->diff(x)->subs(self.get_dict());
        }
        for (const auto &p : self.get_dict()) {
            t = p.second->diff(x);
            if (neq(*t, *zero)) {
                if (is_a<Symbol>(*p.first)) {
                    diff = add(diff,
                               mul(t, self.get_arg()
                                          ->diff(rcp_static_cast<const Symbol>(
                                              p.first))
                                          ->subs(self.get_dict())));
                } else {
                    return Derivative::create(self.rcp_from_this(), {x});
                }
            }
        }
        return diff;
    }

    static RCP<const Basic> diff(const Derivative &self,
                                 const RCP<const Symbol> &x)
    {
        RCP<const Basic> ret = self.get_arg()->diff(x);
        if (eq(*ret, *zero))
            return zero;
        multiset_basic t = self.get_symbols();
        for (auto &p : t) {
            // If x is already there in symbols multi-set add x to the symbols
            // multi-set
            if (eq(*p, *x)) {
                t.insert(x);
                return Derivative::create(self.get_arg(), t);
            }
        }
        // Avoid cycles
        if (is_a<Derivative>(*ret)
            && eq(*down_cast<const Derivative &>(*ret).get_arg(),
                  *self.get_arg())) {
            t.insert(x);
            return Derivative::create(self.get_arg(), t);
        }
        for (auto &p : t) {
            ret = ret->diff(rcp_static_cast<const Symbol>(p));
        }
        return ret;
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

    static RCP<const Basic> diff(const OneArgFunction &self,
                                 const RCP<const Symbol> &x)
    {
        return fdiff(self, x);
    }

    static RCP<const Basic> diff(const MultiArgFunction &self,
                                 const RCP<const Symbol> &x)
    {
        return fdiff(self, x);
    }

    static RCP<const Basic> diff(const LambertW &self,
                                 const RCP<const Symbol> &x)
    {
        // check http://en.wikipedia.org/wiki/Lambert_W_function#Derivative
        // for the equation
        RCP<const Basic> lambertw_val = lambertw(self.get_arg());
        return mul(
            div(lambertw_val, mul(self.get_arg(), add(lambertw_val, one))),
            self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Add &self, const RCP<const Symbol> &x)
    {
        SymEngine::umap_basic_num d;
        RCP<const Number> coef = zero, coef2;
        RCP<const Basic> t;
        for (auto &p : self.get_dict()) {
            RCP<const Basic> term = p.first->diff(x);
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
        return Add::from_dict(coef, std::move(d));
    }

    static RCP<const Basic> diff(const Mul &self, const RCP<const Symbol> &x)
    {
        RCP<const Number> overall_coef = zero;
        umap_basic_num add_dict;
        for (auto &p : self.get_dict()) {
            RCP<const Number> coef = self.get_coef();
            RCP<const Basic> factor = pow(p.first, p.second)->diff(x);
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
                Add::dict_add_term(add_dict, coef, mul);
            }
        }
        return Add::from_dict(overall_coef, std::move(add_dict));
    }

    static RCP<const Basic> diff(const Pow &self, const RCP<const Symbol> &x)
    {
        if (is_a_Number(*(self.get_exp())))
            return mul(mul(self.get_exp(),
                           pow(self.get_base(), sub(self.get_exp(), one))),
                       self.get_base()->diff(x));
        else
            return mul(pow(self.get_base(), self.get_exp()),
                       mul(self.get_exp(), log(self.get_base()))->diff(x));
    }

    static RCP<const Basic> diff(const Sin &self, const RCP<const Symbol> &x)
    {
        return mul(cos(self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cos &self, const RCP<const Symbol> &x)
    {
        return mul(mul(minus_one, sin(self.get_arg())),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Tan &self, const RCP<const Symbol> &x)
    {
        RCP<const Integer> two = integer(2);
        return mul(add(one, pow(tan(self.get_arg()), two)),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cot &self, const RCP<const Symbol> &x)
    {
        RCP<const Integer> two = integer(2);
        return mul(mul(add(one, pow(cot(self.get_arg()), two)), minus_one),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Csc &self, const RCP<const Symbol> &x)
    {
        return mul(
            mul(mul(cot(self.get_arg()), csc(self.get_arg())), minus_one),
            self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Sec &self, const RCP<const Symbol> &x)
    {
        return mul(mul(tan(self.get_arg()), sec(self.get_arg())),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ASin &self, const RCP<const Symbol> &x)
    {
        return mul(div(one, sqrt(sub(one, pow(self.get_arg(), i2)))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACos &self, const RCP<const Symbol> &x)
    {
        return mul(div(minus_one, sqrt(sub(one, pow(self.get_arg(), i2)))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ASec &self, const RCP<const Symbol> &x)
    {
        return mul(
            div(one, mul(pow(self.get_arg(), i2),
                         sqrt(sub(one, div(one, pow(self.get_arg(), i2)))))),
            self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACsc &self, const RCP<const Symbol> &x)
    {
        return mul(div(minus_one,
                       mul(pow(self.get_arg(), i2),
                           sqrt(sub(one, div(one, pow(self.get_arg(), i2)))))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ATan &self, const RCP<const Symbol> &x)
    {
        return mul(div(one, add(one, pow(self.get_arg(), i2))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACot &self, const RCP<const Symbol> &x)
    {
        return mul(div(minus_one, add(one, pow(self.get_arg(), i2))),
                   self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ATan2 &self, const RCP<const Symbol> &x)
    {
        return mul(div(pow(self.get_den(), i2),
                       add(pow(self.get_den(), i2), pow(self.get_num(), i2))),
                   div(self.get_num(), self.get_den())->diff(x));
    }

    static RCP<const Basic> diff(const Erf &self, const RCP<const Symbol> &x)
    {
        RCP<const Basic> arg = self.get_args()[0];
        return mul(div(mul(integer(2), exp(neg(mul(arg, arg)))), sqrt(pi)),
                   arg->diff(x));
    }

    static RCP<const Basic> diff(const Erfc &self, const RCP<const Symbol> &x)
    {
        RCP<const Basic> arg = self.get_args()[0];
        return neg(mul(div(mul(integer(2), exp(neg(mul(arg, arg)))), sqrt(pi)),
                       arg->diff(x)));
    }

    static RCP<const Basic> diff(const Gamma &self, const RCP<const Symbol> &x)
    {
        RCP<const Basic> gamma_arg = self.get_args()[0];
        return mul(mul(self.rcp_from_this(), polygamma(zero, gamma_arg)),
                   gamma_arg->diff(x));
    }

    static RCP<const Basic> diff(const LogGamma &self,
                                 const RCP<const Symbol> &x)
    {
        RCP<const Basic> arg = self.get_args()[0];
        return mul(polygamma(zero, arg), arg->diff(x));
    }

    template <typename Poly, typename Dict>
    static RCP<const Basic> diff_upoly(const Poly &self,
                                       const RCP<const Symbol> &x)
    {
        if (self.get_var()->__eq__(*x)) {
            Dict d;
            for (auto it = self.begin(); it != self.end(); ++it) {
                if (it->first != 0)
                    d[it->first - 1] = it->second * it->first;
            }
            return Poly::from_dict(self.get_var(), std::move(d));
        } else {
            return Poly::from_dict(self.get_var(), {{}});
        }
    }

    static RCP<const Basic> diff(const UIntPoly &self,
                                 const RCP<const Symbol> &x)
    {
        return diff_upoly<UIntPoly, map_uint_mpz>(self, x);
    }

    static RCP<const Basic> diff(const URatPoly &self,
                                 const RCP<const Symbol> &x)
    {
        return diff_upoly<URatPoly, map_uint_mpq>(self, x);
    }

#ifdef HAVE_SYMENGINE_PIRANHA
    static RCP<const Basic> diff(const UIntPolyPiranha &self,
                                 const RCP<const Symbol> &x)
    {
        return diff_upoly<UIntPolyPiranha, map_uint_mpz>(self, x);
    }
    static RCP<const Basic> diff(const URatPolyPiranha &self,
                                 const RCP<const Symbol> &x)
    {
        return diff_upoly<URatPolyPiranha, map_uint_mpq>(self, x);
    }
#endif

#ifdef HAVE_SYMENGINE_FLINT
    template <typename P>
    static RCP<const Basic> diff_upolyflint(const P &self,
                                            const RCP<const Symbol> &x)
    {
        if (self.get_var()->__eq__(*x)) {
            return P::from_container(self.get_var(),
                                     self.get_poly().derivative());
        } else {
            return P::from_dict(self.get_var(), {{}});
        }
    }

    static RCP<const Basic> diff(const UIntPolyFlint &self,
                                 const RCP<const Symbol> &x)
    {
        return diff_upolyflint(self, x);
    }

    static RCP<const Basic> diff(const URatPolyFlint &self,
                                 const RCP<const Symbol> &x)
    {
        return diff_upolyflint(self, x);
    }
#endif

    static RCP<const Basic> diff(const UExprPoly &self,
                                 const RCP<const Symbol> &x)
    {
        return diff_upoly<UExprPoly, map_int_Expr>(self, x);
    }

    template <typename Container, typename Poly>
    static RCP<const Basic> diff(const MSymEnginePoly<Container, Poly> &self,
                                 const RCP<const Symbol> &x)
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
            return Poly::from_dict(v, std::move(dict));
        } else {
            vec_basic vs;
            vs.insert(vs.begin(), self.get_vars().begin(),
                      self.get_vars().end());
            return Poly::from_dict(vs, {{}});
        }
    }

    static RCP<const Basic> diff(const FunctionWrapper &self,
                                 const RCP<const Symbol> &x)
    {
        return self.diff_impl(x);
    }

    static RCP<const Basic> diff(const Beta &self, const RCP<const Symbol> &x)
    {
        RCP<const Basic> beta_arg0 = self.get_args()[0];
        RCP<const Basic> beta_arg1 = self.get_args()[1];
        RCP<const Basic> diff_beta_arg0 = beta_arg0->diff(x);
        RCP<const Basic> diff_beta_arg1 = beta_arg1->diff(x);
        return mul(self.rcp_from_this(),
                   add(mul(polygamma(zero, beta_arg0), diff_beta_arg0),
                       sub(mul(polygamma(zero, beta_arg1), diff_beta_arg1),
                           mul(polygamma(zero, add(beta_arg0, beta_arg1)),
                               add(diff_beta_arg0, diff_beta_arg1)))));
    }

    static RCP<const Basic> diff(const Set &self, const RCP<const Symbol> &x)
    {
        throw SymEngineException("Derivative doesn't exist.");
    }

    static RCP<const Basic> diff(const Boolean &self,
                                 const RCP<const Symbol> &x)
    {
        throw SymEngineException("Derivative doesn't exist.");
    }

    static RCP<const Basic> diff(const GaloisField &self,
                                 const RCP<const Symbol> &x)
    {
        GaloisFieldDict d;
        if (self.get_var()->__eq__(*x)) {
            d = self.get_poly().gf_diff();
            return GaloisField::from_dict(self.get_var(), std::move(d));
        } else {
            return GaloisField::from_dict(self.get_var(), std::move(d));
        }
    }

    static RCP<const Basic> diff(const Piecewise &self,
                                 const RCP<const Symbol> &x)
    {
        PiecewiseVec v = self.get_vec();
        for (auto &p : v) {
            p.first = p.first->diff(x);
        }
        return piecewise(std::move(v));
    }
};

#define IMPLEMENT_DIFF(CLASS)                                                  \
    RCP<const Basic> CLASS::diff(const RCP<const Symbol> &x) const             \
    {                                                                          \
        return DiffImplementation::diff(*this, x);                             \
    }

#define SYMENGINE_ENUM(TypeID, Class) IMPLEMENT_DIFF(Class)
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM

RCP<const Basic> diff(const RCP<const Basic> &arg, const RCP<const Symbol> &x)
{
    return arg->diff(x);
}

//! SymPy style differentiation for non-symbol variables
// Since SymPy's differentiation makes no sense mathematically, it is
// defined separately here for compatibility
RCP<const Basic> sdiff(const RCP<const Basic> &arg, const RCP<const Basic> &x)
{
    if (is_a<Symbol>(*x)) {
        return arg->diff(rcp_static_cast<const Symbol>(x));
    } else {
        RCP<const Symbol> d = DiffImplementation::get_dummy(*arg, "x");
        return ssubs(ssubs(arg, {{x, d}})->diff(d), {{d, x}});
    }
}

} // SymEngine
