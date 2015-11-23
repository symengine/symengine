#include <symengine/basic.h>
#include <symengine/visitor.h>
#include <symengine/pow.h>

namespace SymEngine {
class ExpandVisitor : public BaseVisitor<ExpandVisitor> {
public:
    umap_basic_num d_;
    RCP<const Number> coeff = zero;
    RCP<const Number> multiply = one;

    ExpandVisitor() : BaseVisitor<ExpandVisitor>(this) {

    }

    void apply(const RCP<const Basic> &b) {
        b->accept(*this);
    }

    void apply(const Basic &b) {
        b.accept(*this);
    }

    void bvisit(const Basic &x) {
        Add::dict_add_term(d_, multiply, x.rcp_from_this());
    }

    void bvisit(const Number &x) {
        iaddnum(outArg(coeff), _mulnum(multiply, x.rcp_from_this_cast<const Number>()));
    }

    void bvisit(const Add &self) {
        coeff = self.coef_;
        for (auto &p: self.dict_) {
            multiply = p.second;
            this->apply(p.first);
        }
    }

    void bvisit(const Mul &self) {
        for(auto &p: self.dict_) {
            if (!is_a<Symbol>(*p.first)) {
                RCP<const Basic> a, b;
                self.as_two_terms(outArg(a), outArg(b));
                a = expand(a);
                b = expand(b);
                mul_expand_two(a, b);
                return;
            }
        }
        this->_add_dict_term(self.rcp_from_this());
    }

    void mul_expand_two(const RCP<const Basic> &a, const RCP<const Basic> &b) {
        // Both a and b are assumed to be expanded
        if (is_a<Add>(*a) && is_a<Add>(*b)) {
            iaddnum(outArg(coeff), _mulnum(multiply, _mulnum(rcp_static_cast<const Add>(a)->coef_,
                                                           rcp_static_cast<const Add>(b)->coef_)));
            // Improves (x+1)**3*(x+2)**3*...(x+350)**3 expansion from 0.97s to 0.93s:
#if defined(HAVE_SYMENGINE_RESERVE)
            d_.reserve(d_.size() + (rcp_static_cast<const Add>(a))->dict_.size() *
                                   (rcp_static_cast<const Add>(b))->dict_.size());
#endif
            // Expand dicts first:
            for (auto &p: (rcp_static_cast<const Add>(a))->dict_) {
                RCP<const Number> temp = _mulnum(p.second, multiply);
                for (auto &q: (rcp_static_cast<const Add>(b))->dict_) {
                    // The main bottleneck here is the mul(p.first, q.first) command
                    RCP<const Basic> term = mul(p.first, q.first);
                    if (is_a_Number(*term)) {
                        iaddnum(outArg(coeff),
                                _mulnum(_mulnum(temp, q.second), rcp_static_cast<const Number>(term)));
                    } else {
                        if (is_a<Mul>(*term) &&
                            !(rcp_static_cast<const Mul>(term)->coef_->is_one())) {
                            // Tidy up things like {2x: 3} -> {x: 6}
                            RCP<const Number> coef2 =
                                    rcp_static_cast<const Mul>(term)->coef_;
                            // We make a copy of the dict_:
                            map_basic_basic d2 = rcp_static_cast<const Mul>(term)->dict_;
                            term = Mul::from_dict(one, std::move(d2));
                            Add::dict_add_term(d_, _mulnum(_mulnum(temp, q.second), coef2), term);
                        } else {
                            Add::dict_add_term(d_, _mulnum(temp, q.second), term);
                        }
                    }
                }
                Add::dict_add_term(d_, _mulnum(rcp_static_cast<const Add>(b)->coef_, temp), p.first);
            }
            // Handle the coefficient of "a":
            RCP<const Number> temp = _mulnum(rcp_static_cast<const Add>(a)->coef_, multiply);
            for (auto &q: (rcp_static_cast<const Add>(b))->dict_) {
                Add::dict_add_term(d_, _mulnum(temp, q.second), q.first);
            }
            return;
        } else if (is_a<Add>(*a)) {
            mul_expand_two(b, a);
            return;
        } else if (is_a<Add>(*b)) {
            RCP<const Number> a_coef;
            RCP<const Basic> a_term;
            Add::as_coef_term(a, outArg(a_coef), outArg(a_term));
            _imulnum(outArg(a_coef), multiply);

#if defined(HAVE_SYMENGINE_RESERVE)
            d_.reserve(d_.size() + (rcp_static_cast<const Add>(b))->dict_.size());
#endif
            for (auto &q: (rcp_static_cast<const Add>(b))->dict_) {
                RCP<const Basic> term = mul(a_term, q.first);
                if (is_a_Number(*term)) {
                    iaddnum(outArg(coeff), _mulnum(_mulnum(q.second, a_coef),
                                                  rcp_static_cast<const Number>(term)));
                } else {
                    if (is_a<Mul>(*term) &&
                        !(rcp_static_cast<const Mul>(term)->coef_->is_one())) {
                        // Tidy up things like {2x: 3} -> {x: 6}
                        RCP<const Number> coef2 =
                                rcp_static_cast<const Mul>(term)->coef_;
                        // We make a copy of the dict_:
                        map_basic_basic d2 = rcp_static_cast<const Mul>(term)->dict_;
                        term = Mul::from_dict(one, std::move(d2));
                        Add::dict_add_term(d_, _mulnum(_mulnum(q.second, a_coef), coef2), term);
                    } else {
                        // TODO: check if it's a Add
                        Add::dict_add_term(d_, _mulnum(a_coef, q.second), term);
                    }
                }
            }
            if (eq(*a_term, *one)) {
                iaddnum(outArg(coeff),
                        _mulnum(multiply, _mulnum(rcp_static_cast<const Add>(b)->coef_, a_coef)));
            } else {
                Add::dict_add_term(d_, _mulnum(multiply, _mulnum(rcp_static_cast<const Add>(b)->coef_, a_coef)),
                                   a_term);
            }
            return;
        }
        _add_dict_term(mul(a, b));
    }

    void pow_expand_two(umap_basic_num &base_dict) {
        long m = base_dict.size();
#if defined(HAVE_SYMENGINE_RESERVE)
        d_.reserve(d_.size() + m * (m + 1) / 2);
#endif
        RCP<const Basic> t;
        RCP<const Number> coef, two = integer(2);
        for (auto p = base_dict.begin(); p != base_dict.end(); ++p) {
            for (auto q = p; q != base_dict.end(); ++q) {
                if (q == p) {
                    Add::as_coef_term(pow((*p).first, two), outArg(coef), outArg(t));
                    _imulnum(outArg(coef), pownum((*p).second, two));
                    Add::dict_add_term(d_, _mulnum(coef, multiply), t);
                } else {
                    Add::as_coef_term(mul((*q).first, (*p).first), outArg(coef), outArg(t));
                    _imulnum(outArg(coef), _mulnum((*p).second, _mulnum((*q).second, two)));
                    Add::dict_add_term(d_, _mulnum(coef, multiply), t);
                }
            }
        }
    }


    void pow_expand(umap_basic_num& base_dict, unsigned long n)
    {
        map_vec_mpz r;
        long m = base_dict.size();
        multinomial_coefficients_mpz(m, n, r);
        // This speeds up overall expansion. For example for the benchmark
        // (y + x + z + w)**60 it improves the timing from 135ms to 124ms.
#if defined(HAVE_SYMENGINE_RESERVE)
        d_.reserve(d_.size() + 2 * r.size());
#endif
        for (auto &p: r) {
            auto power = p.first.begin();
            auto i2 = base_dict.begin();
            map_basic_basic d;
            RCP<const Number> overall_coeff = one;
            for (; power != p.first.end(); ++power, ++i2) {
                if (*power > 0) {
                    RCP<const Integer> exp = integer(*power);
                    RCP<const Basic> base = i2->first;
                    if (is_a<Integer>(*base)) {
                        _imulnum(outArg(overall_coeff),
                                rcp_static_cast<const Number>(
                                        rcp_static_cast<const Integer>(base)->powint(*exp)));
                    } else if (is_a<Symbol>(*base)) {
                        Mul::dict_add_term(d, exp, base);
                    } else {
                        RCP<const Basic> exp2, t, tmp;
                        tmp = pow(base, exp);
                        if (is_a<Mul>(*tmp)) {
                            for (auto &p: (rcp_static_cast<const Mul>(tmp))->dict_) {
                                Mul::dict_add_term_new(outArg(overall_coeff), d,
                                                       p.second, p.first);
                            }
                            _imulnum(outArg(overall_coeff), (rcp_static_cast<const Mul>(tmp))->coef_);
                        } else if (is_a_Number(*tmp)) {
                            _imulnum(outArg(overall_coeff), rcp_static_cast<const Number>(tmp));
                        } else {
                            Mul::as_base_exp(tmp, outArg(exp2), outArg(t));
                            Mul::dict_add_term_new(outArg(overall_coeff), d, exp2, t);
                        }
                    }
                    if (!(i2->second->is_one())) {
                        if (is_a<Integer>(*(i2->second)) || is_a<Rational>(*(i2->second))) {
                            _imulnum(outArg(overall_coeff),
                                    pownum(i2->second,
                                           rcp_static_cast<const Number>(exp)));
                        } else if (is_a<Complex>(*(i2->second))) {
                            RCP<const Number> tmp = rcp_static_cast<const Complex>(i2->second)->pow(*exp);
                            _imulnum(outArg(overall_coeff), tmp);
                        }
                    }
                }
            }
            RCP<const Basic> term = Mul::from_dict(overall_coeff, std::move(d));
            RCP<const Number> coef2 = integer(p.second);
            if (is_a_Number(*term)) {
                iaddnum(outArg(coeff),
                        _mulnum(_mulnum(multiply, rcp_static_cast<const Number>(term)), coef2));
            } else {
                if (is_a<Mul>(*term) &&
                    !(rcp_static_cast<const Mul>(term)->coef_->is_one())) {
                    // Tidy up things like {2x: 3} -> {x: 6}
                    _imulnum(outArg(coef2),
                            rcp_static_cast<const Mul>(term)->coef_);
                    // We make a copy of the dict_:
                    map_basic_basic d2 = rcp_static_cast<const Mul>(term)->dict_;
                    term = Mul::from_dict(one, std::move(d2));
                }
                Add::dict_add_term(d_, _mulnum(multiply, coef2), term);
            }
        }
    }

    void bvisit(const Pow &self) {
        RCP<const Basic> _base = expand(self.get_base());

        if (is_a<Integer>(*self.get_exp()) && is_a<UnivariatePolynomial>(*_base)) {
            int q = rcp_static_cast<const Integer>(self.get_exp())->as_int();
            RCP<const UnivariatePolynomial> p = rcp_static_cast<const UnivariatePolynomial>(_base);
            RCP<const UnivariatePolynomial> r = univariate_polynomial(p->var_, 0, {{0, 1}});
            while (q != 0) {
                if (q % 2 == 1) {
                    _base = mul_uni_poly(r, p);
                    q--;
                }
                p = mul_uni_poly(p, p);
                q /= 2;
            }
            _add_dict_term(_base);
            return;
        }

        if (!is_a<Integer>(*self.get_exp()) || !is_a<Add>(*_base)) {
            if (neq(*_base, *self.get_base())) {
                Add::dict_add_term(d_, multiply, pow(_base, self.get_exp()));
            } else {
                Add::dict_add_term(d_, multiply, self.rcp_from_this());
            }
            return;
        }

        mpz_class n = rcp_static_cast<const Integer>(self.get_exp())->as_mpz();
        if (n < 0) {
            return _add_dict_term(div(one, expand(pow(_base, integer(-n)))));
        }
        RCP<const Add> base = rcp_static_cast<const Add>(_base);
        umap_basic_num base_dict = base->dict_;
        if (!(base->coef_->is_zero())) {
            // Add the numerical coefficient into the dictionary. This
            // allows a little bit easier treatment below.
            insert(base_dict, base->coef_, one);
        } else {
            if (base_dict.size() == 1) {
                // Eg: (0.0 + x * 5) ** 2 == 0.0 + (x * 5) ** 2
                _base = add(base->coef_, expand(pow(mul(base_dict.begin()->first,
                                                base_dict.begin()->second), self.get_exp())));
                return _add_dict_term(_base);
            }
        }
        if (n == 2) {
            return pow_expand_two(base_dict);
        } else {
            return pow_expand(base_dict, n.get_ui());
        }
    }

    inline void _add_dict_term(const RCP<const Basic> a) {
        if (is_a<Add>(*a)) {
            for (auto &q: (rcp_static_cast<const Add>(a))->dict_) {
                Add::dict_add_term(d_, _mulnum(q.second, multiply), q.first);
            }
            iaddnum(outArg(coeff), _mulnum(multiply, rcp_static_cast<const Add>(a)->coef_));
        } else {
            RCP<const Basic> term;
            RCP<const Number> c;
            Add::as_coef_term(a, outArg(c), outArg(term));
            Add::dict_add_term(d_, _mulnum(multiply, c), term);
        }
    }

    inline RCP<const Number> _mulnum(const RCP<const Number> &x, const RCP<const Number> &y) {
        if (eq(*x, *one)) return y;
        if (eq(*y, *one)) return x;
        return x->mul(*y);
    }

    inline void _imulnum(const Ptr<RCP<const Number>> &self,
                         const RCP<const Number> &other)
    {
        *self = _mulnum(*self, other);
    }
};

//! Expands `self`
RCP<const Basic> expand(const RCP<const Basic> &self) {
    ExpandVisitor v;
    v.apply(self);
    return Add::from_dict(v.coeff, std::move(v.d_));
}

} // SymEngine
