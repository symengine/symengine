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
#include <symengine/polynomial.h>
#include <symengine/complex_double.h>
#include <symengine/complex_mpc.h>

namespace SymEngine {

extern RCP<const Basic> i2;

class DiffImplementation {
public:
// Uncomment the following define in order to debug the methods:
#define debug_methods
#ifndef debug_methods

    static RCP<const Basic> diff(const Basic &self,
            const RCP<const Symbol> &x) {
        return Derivative::create(self.rcp_from_this(), {x});
    }

#else
    // Here we do not have a 'Basic' fallback, but rather must implement all
    // virtual methods explicitly (if we miss one, the code will not compile).
    // This is useful to check that we have implemented all methods that we
    // wanted.

#define DIFF0(CLASS) \
static RCP<const Basic> diff(const CLASS &self, \
        const RCP<const Symbol> &x) { \
    return Derivative::create(self.rcp_from_this(), {x}); \
}

    DIFF0(UnivariateSeries)
    DIFF0(KroneckerDelta)
    DIFF0(Dirichlet_eta)
    DIFF0(UpperGamma)
    DIFF0(LowerGamma)
    DIFF0(Beta)
    DIFF0(PolyGamma)
    DIFF0(LeviCivita)

#endif


    static RCP<const Basic> diff(const Number &self,
            const RCP<const Symbol> &x) {
        return zero;
    }

    static RCP<const Basic> diff(const Constant &self,
            const RCP<const Symbol> &x) {
        return zero;
    }

    static RCP<const Basic> diff(const Symbol &self,
            const RCP<const Symbol> &x) {
        if (x->get_name() == self.get_name())
            return one;
        else
            return zero;
    }

    static RCP<const Basic> diff(const Log &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Abs &self,
            const RCP<const Symbol> &x) {
        if (eq(*self.get_arg()->diff(x), *zero))
            return zero;
        else
            return Derivative::create(self.rcp_from_this(), {x});
    }

    static RCP<const Basic> diff(const Zeta &self,
            const RCP<const Symbol> &x) {
        // TODO: check if it is differentiated wrt s
        return mul(mul(mul(minus_one, self.get_s()),
                zeta(add(self.get_s(), one), self.get_a())),
                self.get_a()->diff(x));
    }

    static RCP<const Basic> diff(const ASech &self,
            const RCP<const Symbol> &x) {
        return mul(div(minus_one, mul(sqrt(sub(one, pow(self.get_arg(), i2))),
                self.get_arg())), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACoth &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, sub(one, pow(self.get_arg(), i2))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ATanh &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, sub(one, pow(self.get_arg(), i2))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACosh &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, sqrt(sub(pow(self.get_arg(), i2), one))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ASinh &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, sqrt(add(pow(self.get_arg(), i2), one))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Coth &self,
            const RCP<const Symbol> &x) {
        return mul(div(minus_one, pow(sinh(self.get_arg()), i2)),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Tanh &self,
            const RCP<const Symbol> &x) {
        return mul(sub(one, pow(tanh(self.get_arg()), i2)),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cosh &self,
            const RCP<const Symbol> &x) {
        return mul(sinh(self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Sinh &self,
            const RCP<const Symbol> &x) {
        return mul(cosh(self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Subs &self,
            const RCP<const Symbol> &x) {
        RCP<const Basic> diff = zero, t;
        if (self.get_dict().count(x) == 0) {
            diff = self.get_arg()->diff(x)->subs(self.get_dict());
        }
        for (const auto &p: self.get_dict()) {
            t = p.second->diff(x);
            if (neq(*t, *zero)) {
                if (is_a<Symbol>(*p.first)) {
                    diff = add(diff, mul(t, self.get_arg()->diff(rcp_static_cast<const Symbol>(p.first))->subs(self.get_dict())));
                } else {
                    return Derivative::create(self.rcp_from_this(), {x});
                }
            }
        }
        return diff;
    }

    static RCP<const Basic> diff(const Derivative &self,
            const RCP<const Symbol> &x) {
        RCP<const Basic> ret = self.get_arg()->diff(x);
        if (eq(*ret, *zero)) return zero;
        multiset_basic t = self.get_symbols();
        for (auto &p: t) {
            // If x is already there in symbols multi-set add x to the symbols multi-set
            if (eq(*p, *x)) {
                t.insert(x);
                return Derivative::create(self.get_arg(), t);
            }
        }
        // Avoid cycles
        if (is_a<Derivative>(*ret) && eq(*static_cast<const Derivative &>(*ret).get_arg(), *self.get_arg())) {
            t.insert(x);
            return Derivative::create(self.get_arg(), t);
        }
        for (auto &p: t) {
            ret = ret->diff(rcp_static_cast<const Symbol>(p));
        }
        return ret;
    }

    static RCP<const Basic> diff(const FunctionSymbol &self,
            const RCP<const Symbol> &x) {
        RCP<const Basic> diff = zero, t;
        RCP<const Basic> self_ = self.rcp_from_this();
        RCP<const Symbol> s;
        std::string name;
        unsigned count  = 0;
        bool found_x = false;
        for (const auto &a : self.get_args()) {
            if (eq(*a, *x)) {
                found_x = true;
                count++;
            } else if (count < 2 and neq(*a->diff(x), *zero)) {
                count++;
            }
        }
        if (count == 1 and found_x) {
            return Derivative::create(self_, {x});
        }
        for (unsigned i = 0; i < self.get_args().size(); i++) {
            t = self.get_args()[i]->diff(x);
            if (neq(*t, *zero)) {
                name = "x";
                do {
                    name = "_" + name;
                    s = symbol(name);
                } while (has_symbol(*self_, s));
                vec_basic v = self.get_args();
                v[i] = s;
                map_basic_basic m;
                insert(m, v[i], self.get_args()[i]);
                diff = add(diff, mul(t,
                    make_rcp<const Subs>(Derivative::create(self.create(v),
                            {v[i]}), m)));
            }
        }
        return diff;
    }

    static RCP<const Basic> diff(const LambertW &self,
            const RCP<const Symbol> &x) {
        // check http://en.wikipedia.org/wiki/Lambert_W_function#Derivative
        // for the equation
        RCP<const Basic> lambertw_val = lambertw(self.get_arg());
        return mul(div(lambertw_val, mul(self.get_arg(),
                add(lambertw_val, one))), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Add &self,
            const RCP<const Symbol> &x) {
        SymEngine::umap_basic_num d;
        RCP<const Number> coef=zero, coef2;
        RCP<const Basic> t;
        for (auto &p: self.dict_) {
            RCP<const Basic> term = p.first->diff(x);
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
        return Add::from_dict(coef, std::move(d));
    }

    static RCP<const Basic> diff(const Mul &self,
            const RCP<const Symbol> &x) {
        RCP<const Number> overall_coef = zero;
        umap_basic_num add_dict;
        for (auto &p: self.dict_) {
            RCP<const Number> coef = self.coef_;
            RCP<const Basic> factor = pow(p.first, p.second)->diff(x);
            if (is_a<Integer>(*factor) &&
                    rcp_static_cast<const Integer>(factor)->is_zero()) continue;
            map_basic_basic d = self.dict_;
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
        return Add::from_dict(overall_coef, std::move(add_dict));
    }

    static RCP<const Basic> diff(const Pow &self,
            const RCP<const Symbol> &x) {
        if (is_a_Number(*(self.get_exp())))
            return mul(mul(self.get_exp(), pow(self.get_base(), sub(self.get_exp(), one))), self.get_base()->diff(x));
        else
            return mul(pow(self.get_base(), self.get_exp()), mul(self.get_exp(), log(self.get_base()))->diff(x));
    }

    static RCP<const Basic> diff(const Sin &self,
            const RCP<const Symbol> &x) {
        return mul(cos(self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cos &self,
            const RCP<const Symbol> &x) {
        return mul(mul(minus_one, sin(self.get_arg())),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Tan &self,
            const RCP<const Symbol> &x) {
        RCP<const Integer> two = integer(2);
        return mul(add(one, pow(tan(self.get_arg()), two)),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cot &self,
            const RCP<const Symbol> &x) {
        RCP<const Integer> two = integer(2);
        return mul(mul(add(one, pow(cot(self.get_arg()), two)), minus_one),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Csc &self,
            const RCP<const Symbol> &x) {
        return mul(mul(mul(cot(self.get_arg()), csc(self.get_arg())),
                    minus_one), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Sec &self,
            const RCP<const Symbol> &x) {
        return mul(mul(tan(self.get_arg()), sec(self.get_arg())),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ASin &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, sqrt(sub(one, pow(self.get_arg(), i2)))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACos &self,
            const RCP<const Symbol> &x) {
        return mul(div(minus_one, sqrt(sub(one, pow(self.get_arg(), i2)))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ASec &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, mul(pow(self.get_arg(), i2),
                sqrt(sub(one, div(one, pow(self.get_arg(), i2)))))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACsc &self,
            const RCP<const Symbol> &x) {
        return mul(div(minus_one, mul(pow(self.get_arg(), i2),
                sqrt(sub(one, div(one, pow(self.get_arg(), i2)))))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ATan &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, add(one, pow(self.get_arg(), i2))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACot &self,
            const RCP<const Symbol> &x) {
        return mul(div(minus_one, add(one, pow(self.get_arg(), i2))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ATan2 &self,
            const RCP<const Symbol> &x) {
        return mul(div(pow(self.get_den(), i2), add(pow(self.get_den(), i2),
            pow(self.get_num(), i2))), div(self.get_num(),
            self.get_den())->diff(x));
    }

    static RCP<const Basic> diff(const Gamma &self,
            const RCP<const Symbol> &x) {
        RCP<const Basic> gamma_arg = self.get_args()[0];
        return mul(mul(self.rcp_from_this(), polygamma(zero, gamma_arg)), gamma_arg->diff(x));
    }

    static RCP<const Basic> diff(const UnivariatePolynomial &self,
            const RCP<const Symbol> &x) {
        if (self.get_var()->__eq__(*x)) {
            map_uint_mpz d;
            for (const auto &p : self.get_dict()) {
                d[p.first - 1] = p.second * p.first;
            }
            return make_rcp<const UnivariatePolynomial>(self.get_var(),
                    (--(d.end()))->first, std::move(d));
        } else {
            return zero;
        }
    }

    static RCP<const Basic> diff(const FunctionWrapper &self,
            const RCP<const Symbol> &x) {
        return self.diff_impl(x);
    }

};

#define IMPLEMENT_DIFF(CLASS) \
RCP<const Basic> CLASS::diff(const RCP<const Symbol> &x) const { \
    return DiffImplementation::diff(*this, x); \
};


#define SYMENGINE_ENUM(TypeID, Class) IMPLEMENT_DIFF(Class)
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM


} // SymEngine
