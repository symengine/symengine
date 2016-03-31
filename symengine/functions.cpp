#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/symbol.h>
#include <symengine/pow.h>
#include <symengine/complex.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <algorithm>

namespace SymEngine
{

extern RCP<const Basic> i2;
extern RCP<const Basic> i3;
extern RCP<const Basic> i5;
extern RCP<const Basic> im2;
extern RCP<const Basic> im3;
extern RCP<const Basic> im5;

RCP<const Basic> sqrt(RCP<const Basic> &arg)
{
    return pow(arg, div(one, i2));
}

extern RCP<const Basic> sq3;
extern RCP<const Basic> sq2;
extern RCP<const Basic> sq5;

extern RCP<const Basic> C0;
extern RCP<const Basic> C1;
extern RCP<const Basic> C2;
extern RCP<const Basic> C3;
extern RCP<const Basic> C4;
extern RCP<const Basic> C5;
extern RCP<const Basic> C6;

extern RCP<const Basic> mC0;
extern RCP<const Basic> mC1;
extern RCP<const Basic> mC2;
extern RCP<const Basic> mC3;
extern RCP<const Basic> mC4;
extern RCP<const Basic> mC5;
extern RCP<const Basic> mC6;

extern RCP<const Basic> sin_table[];

extern umap_basic_basic inverse_cst;

extern umap_basic_basic inverse_tct;

bool get_pi_shift(const RCP<const Basic> &arg, const Ptr<RCP<const Integer>> &n,
                  const Ptr<RCP<const Basic>> &x)
{
    if (is_a<Add>(*arg)) {
        const Add &s = static_cast<const Add &>(*arg);
        RCP<const Basic> coef = s.coef_;
        int size = s.dict_.size();
        if (size > 1) {
            // arg should be of form `theta + n*pi/12`
            // `n` is an integer
            // `theta` is an `Expression`
            bool check_pi = false;
            RCP<const Basic> temp;
            *x = coef;
            for (const auto &p : s.dict_) {
                temp = mul(p.second, integer(12));
                if (is_a<Constant>(*p.first)
                    and eq(*(rcp_static_cast<const Constant>(p.first)), *pi)
                    and is_a<Integer>(*temp)) {
                    check_pi = true;
                    *n = rcp_dynamic_cast<const Integer>(temp);
                } else {
                    *x = add(mul(p.first, p.second), *x);
                }
            }
            if (check_pi)
                return true;
            else // No term with `pi` found
                return false;
        } else if (size == 1) {
            // arg should be of form `a + n*pi/12`
            // where `a` is a `Number`.
            auto p = s.dict_.begin();
            RCP<const Basic> temp = mul(p->second, integer(12));
            if (is_a<Constant>(*p->first)
                and eq(*(rcp_static_cast<const Constant>(p->first)), *pi)
                and is_a<Integer>(*temp)) {

                *n = rcp_dynamic_cast<const Integer>(temp);
                *x = coef;
                return true;
            } else {
                return false;
            }
        } else { // Should never reach here though!
            // Dict of size < 1
            return false;
        }
    } else if (is_a<Mul>(*arg)) {
        // `arg` is of the form `k*pi/12`
        const Mul &s = static_cast<const Mul &>(*arg);
        RCP<const Basic> coef = s.coef_;
        coef = mul(coef, integer(12));
        auto p = s.dict_.begin();
        // dict should contain symbol `pi` only
        // and coeff should be a multiple of 12
        if (s.dict_.size() == 1 and is_a<Constant>(*p->first)
            and eq(*(rcp_static_cast<const Constant>(p->first)), *pi)
            and eq(*(rcp_static_cast<const Number>(p->second)), *one)
            and is_a<Integer>(*coef)) {

            *n = rcp_dynamic_cast<const Integer>(coef);
            *x = zero;
            return true;
        } else {
            return false;
        }
    } else if (is_a<Constant>(*arg)
               and eq(*(rcp_static_cast<const Constant>(arg)), *pi)) {
        *n = integer(12);
        *x = zero;
        return true;
    } else {
        return false;
    }
}

bool could_extract_minus(const RCP<const Basic> &arg)
{
    if (is_a<Mul>(*arg)) {
        const Mul &s = static_cast<const Mul &>(*arg);
        RCP<const Basic> coef = s.coef_;
        if (is_a<Integer>(*coef)
            and rcp_static_cast<const Integer>(coef)->is_negative())
            return true;

        else if (is_a<Rational>(*coef)
                 and rcp_static_cast<const Rational>(coef)->is_negative())
            return true;
        else
            return false;
    } else if (is_a<Add>(*arg)) {
        const Add &s = static_cast<const Add &>(*arg);
        for (const auto &p : s.dict_) {
            if (is_a<Integer>(*p.second)) {
                if (not(rcp_static_cast<const Integer>(p.second)
                            ->is_negative()))
                    return false;
            } else if (is_a<Rational>(*p.second)) {
                if (not(rcp_static_cast<const Rational>(p.second)
                            ->is_negative()))
                    return false;
            } else
                return false;
        }
        return true;
    } else
        return false;
}

bool handle_minus(const RCP<const Basic> &arg,
                  const Ptr<RCP<const Basic>> &rarg)
{
    if (could_extract_minus(arg)) {
        *rarg = mul(minus_one, arg);
        return true;
    } else {
        *rarg = arg;
        return false;
    }
}

// \return true of conjugate has to be returned finally else false
bool eval(const RCP<const Basic> &arg, unsigned period, bool odd,
          bool conj_odd,                                            // input
          const Ptr<RCP<const Basic>> &rarg, int &index, int &sign) // output
{
    bool check;
    RCP<const Integer> n;
    RCP<const Basic> r;
    RCP<const Basic> ret_arg;
    check = get_pi_shift(arg, outArg(n), outArg(r));
    if (check) {
        int m = mod_f(*n, *integer(12 * period))->as_int();
        sign = 1;
        if (eq(*r, *zero)) {
            index = m;
            *rarg = zero;
            return false;
        } else if ((m % (12 * period)) == 0) {
            index = 0;
            bool b = handle_minus(r, outArg(ret_arg));
            *rarg = ret_arg;
            if (odd and b)
                sign = -1;
            return false;
        } else if ((m % 12) == 0) {
            sign = -1;
            bool b = handle_minus(r, outArg(ret_arg));
            *rarg = ret_arg;
            if (odd and b)
                sign = -1 * sign;
            return false;
        } else if ((m % 6) == 0) {
            if (m == 6)
                sign = 1;
            else
                sign = -1;
            bool b = handle_minus(r, outArg(ret_arg));
            *rarg = ret_arg;
            if (not b and conj_odd)
                sign = -sign;
            return true;
        } else {
            *rarg = r;
            index = -1;
            return false;
        }
    } else {
        bool b = handle_minus(arg, outArg(ret_arg));
        *rarg = ret_arg;
        index = -1;
        if (odd and b)
            sign = -1;
        else
            sign = 1;
        return false;
    }
}

bool inverse_lookup(umap_basic_basic &d, const RCP<const Basic> &t,
                    const Ptr<RCP<const Basic>> &index)
{
    auto it = d.find(t);
    if (it == d.end()) {
        // Not found in lookup
        return false;
    } else {
        *index = (it->second);
        return true;
    }
}

std::size_t TrigFunction::__hash__() const
{
    std::size_t seed = this->get_type_code();
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

RCP<const Basic> TrigFunction::create(const RCP<const Basic> &arg) const
{
    throw std::runtime_error("Should be implemented by the inherited class");
}

RCP<const Basic> TrigFunction::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
        return rcp_from_this();
    else
        return this->create(arg);
}

Sin::Sin(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Sin::is_canonical(const RCP<const Basic> &arg) const
{
    // e.g. sin(0)
    if (is_a<Integer>(*arg) and rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // e.g sin(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    if (get_pi_shift(arg, outArg(n), outArg(r))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool Sin::__eq__(const Basic &o) const
{
    if (is_a<Sin>(o)
        and eq(*get_arg(), *(static_cast<const Sin &>(o).get_arg())))
        return true;
    return false;
}

int Sin::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Sin>(o))
    const Sin &s = static_cast<const Sin &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> sin(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().sin(*arg);
    }

    if (is_a<ASin>(*arg)) {
        return rcp_static_cast<const ASin>(arg)->get_arg();
    } else if (is_a<ACsc>(*arg)) {
        return div(one, rcp_static_cast<const ACsc>(arg)->get_arg());
    }

    RCP<const Basic> ret_arg;
    int index, sign;
    bool conjugate = eval(arg, 2, 1, 0,                  // input
                          outArg(ret_arg), index, sign); // output

    if (conjugate) {
        // cos has to be returned
        if (sign == 1) {
            return cos(ret_arg);
        } else {
            return mul(minus_one, cos(ret_arg));
        }
    } else {
        if (eq(*ret_arg, *zero)) {
            return mul(integer(sign), sin_table[index]);
        } else {
            // If ret_arg is the same as arg, a `Sin` instance is
            // returned
            // Or else `sin` is called again.
            if (sign == 1) {
                if (neq(*ret_arg, *arg)) {
                    return sin(ret_arg);
                } else {
                    return make_rcp<const Sin>(arg);
                }
            } else {
                return mul(minus_one, sin(ret_arg));
            }
        }
    }
}

/* ---------------------------- */

Cos::Cos(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Cos::is_canonical(const RCP<const Basic> &arg) const
{
    // e.g. cos(0)
    if (is_a<Integer>(*arg) and rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // e.g cos(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    if (get_pi_shift(arg, outArg(n), outArg(r))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool Cos::__eq__(const Basic &o) const
{
    if (is_a<Cos>(o)
        and eq(*get_arg(), *(static_cast<const Cos &>(o).get_arg())))
        return true;
    return false;
}

int Cos::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Cos>(o))
    const Cos &s = static_cast<const Cos &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> cos(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return one;
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().cos(*arg);
    }

    if (is_a<ACos>(*arg)) {
        return rcp_static_cast<const ACos>(arg)->get_arg();
    } else if (is_a<ASec>(*arg)) {
        return div(one, rcp_static_cast<const ASec>(arg)->get_arg());
    }

    RCP<const Basic> ret_arg;
    int index, sign;
    bool conjugate = eval(arg, 2, 0, 1,                  // input
                          outArg(ret_arg), index, sign); // output

    if (conjugate) {
        // sin has to be returned
        if (sign == 1) {
            return sin(ret_arg);
        } else {
            return mul(minus_one, sin(ret_arg));
        }
    } else {
        if (eq(*ret_arg, *zero)) {
            return mul(integer(sign), sin_table[(index + 6) % 24]);
        } else {
            if (sign == 1) {
                if (neq(*ret_arg, *arg)) {
                    return cos(ret_arg);
                } else {
                    return make_rcp<const Cos>(ret_arg);
                }
            } else {
                return mul(minus_one, cos(ret_arg));
            }
        }
    }
}

/* ---------------------------- */

Tan::Tan(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Tan::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (is_a<Integer>(*arg) and rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // e.g tan(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    if (get_pi_shift(arg, outArg(n), outArg(r))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool Tan::__eq__(const Basic &o) const
{
    if (is_a<Tan>(o)
        and eq(*get_arg(), *(static_cast<const Tan &>(o).get_arg())))
        return true;
    else
        return false;
}

int Tan::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Tan>(o))
    const Tan &s = static_cast<const Tan &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> tan(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().tan(*arg);
    }

    if (is_a<ATan>(*arg)) {
        return rcp_static_cast<const ATan>(arg)->get_arg();
    } else if (is_a<ACot>(*arg)) {
        return div(one, rcp_static_cast<const ACot>(arg)->get_arg());
    }

    RCP<const Basic> ret_arg;
    int index, sign;
    bool conjugate = eval(arg, 1, 1, 1,                  // input
                          outArg(ret_arg), index, sign); // output

    if (conjugate) {
        // cot has to be returned
        if (sign == 1) {
            return cot(ret_arg);
        } else {
            return mul(minus_one, cot(ret_arg));
        }
    } else {
        if (eq(*ret_arg, *zero)) {
            return mul(integer(sign),
                       div(sin_table[index], sin_table[(index + 6) % 24]));
        } else {
            if (sign == 1) {
                if (neq(*ret_arg, *arg)) {
                    return tan(ret_arg);
                } else {
                    return make_rcp<const Tan>(ret_arg);
                }
            } else {
                return mul(minus_one, tan(ret_arg));
            }
        }
    }
}

/* ---------------------------- */

Cot::Cot(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Cot::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (is_a<Integer>(*arg) and rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // e.g cot(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    if (get_pi_shift(arg, outArg(n), outArg(r))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool Cot::__eq__(const Basic &o) const
{
    if (is_a<Cot>(o)
        and eq(*get_arg(), *(static_cast<const Cot &>(o).get_arg())))
        return true;
    else
        return false;
}

int Cot::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Cot>(o))
    const Cot &s = static_cast<const Cot &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> cot(const RCP<const Basic> &arg)
{
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().cot(*arg);
    }

    if (is_a<ACot>(*arg)) {
        return rcp_static_cast<const ACot>(arg)->get_arg();
    } else if (is_a<ATan>(*arg)) {
        return div(one, rcp_static_cast<const ATan>(arg)->get_arg());
    }

    RCP<const Basic> ret_arg;
    int index, sign;
    bool conjugate = eval(arg, 1, 1, 1,                  // input
                          outArg(ret_arg), index, sign); // output

    if (conjugate) {
        // tan has to be returned
        if (sign == 1) {
            return tan(ret_arg);
        } else {
            return mul(minus_one, tan(ret_arg));
        }
    } else {
        if (eq(*ret_arg, *zero)) {
            return mul(integer(sign),
                       div(sin_table[(index + 6) % 24], sin_table[index]));
        } else {
            if (sign == 1) {
                if (neq(*ret_arg, *arg)) {
                    return cot(ret_arg);
                } else {
                    return make_rcp<const Cot>(ret_arg);
                }
            } else {
                return mul(minus_one, cot(ret_arg));
            }
        }
    }
}

/* ---------------------------- */

Csc::Csc(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Csc::is_canonical(const RCP<const Basic> &arg) const
{
    // e.g. Csc(0)
    if (is_a<Integer>(*arg) and rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // Update for +inf/-inf constraints
    // e.g csc(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    if (get_pi_shift(arg, outArg(n), outArg(r))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool Csc::__eq__(const Basic &o) const
{
    if (is_a<Csc>(o)
        and eq(*get_arg(), *(static_cast<const Csc &>(o).get_arg())))
        return true;
    return false;
}

int Csc::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Csc>(o))
    const Csc &s = static_cast<const Csc &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> csc(const RCP<const Basic> &arg)
{
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().csc(*arg);
    }

    if (is_a<ACsc>(*arg)) {
        return rcp_static_cast<const ACsc>(arg)->get_arg();
    } else if (is_a<ASin>(*arg)) {
        return div(one, rcp_static_cast<const ASin>(arg)->get_arg());
    }

    RCP<const Basic> ret_arg;
    int index, sign;
    bool conjugate = eval(arg, 2, 1, 0,                  // input
                          outArg(ret_arg), index, sign); // output

    if (conjugate) {
        // cos has to be returned
        if (sign == 1) {
            return sec(ret_arg);
        } else {
            return mul(minus_one, sec(ret_arg));
        }
    } else {
        if (eq(*ret_arg, *zero)) {
            return mul(integer(sign), div(one, sin_table[index]));
        } else {
            if (sign == 1) {
                if (neq(*ret_arg, *arg)) {
                    return csc(ret_arg);
                } else {
                    return make_rcp<const Csc>(ret_arg);
                }
            } else {
                return mul(minus_one, csc(ret_arg));
            }
        }
    }
}

/* ---------------------------- */

Sec::Sec(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Sec::is_canonical(const RCP<const Basic> &arg) const
{
    // e.g. Sec(0)
    if (is_a<Integer>(*arg) and rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // TODO: Update for +inf/-inf constraints
    // e.g sec(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    if (get_pi_shift(arg, outArg(n), outArg(r))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool Sec::__eq__(const Basic &o) const
{
    if (is_a<Sec>(o)
        and eq(*get_arg(), *(static_cast<const Sec &>(o).get_arg())))
        return true;
    return false;
}

int Sec::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Sec>(o))
    const Sec &s = static_cast<const Sec &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> sec(const RCP<const Basic> &arg)
{
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().sec(*arg);
    }

    if (is_a<ASec>(*arg)) {
        return rcp_static_cast<const ASec>(arg)->get_arg();
    } else if (is_a<ACos>(*arg)) {
        return div(one, rcp_static_cast<const ACos>(arg)->get_arg());
    }

    RCP<const Basic> ret_arg;
    int index, sign;
    bool conjugate = eval(arg, 2, 0, 1,                  // input
                          outArg(ret_arg), index, sign); // output

    if (conjugate) {
        // csc has to be returned
        if (sign == 1) {
            return csc(ret_arg);
        } else {
            return mul(minus_one, csc(ret_arg));
        }
    } else {
        if (eq(*ret_arg, *zero)) {
            return mul(integer(sign), div(one, sin_table[(index + 6) % 24]));
        } else {
            if (sign == 1) {
                if (neq(*ret_arg, *arg)) {
                    return sec(ret_arg);
                } else {
                    return make_rcp<const Sec>(ret_arg);
                }
            } else {
                return mul(minus_one, sec(ret_arg));
            }
        }
    }
}
/* ---------------------------- */

// simplifies trigonometric functions wherever possible
// currently deals with simplifications of type sin(acos())
RCP<const Basic> trig_to_sqrt(const RCP<const Basic> &arg)
{
    RCP<const Basic> i_arg;

    if (is_a<Sin>(*arg)) {
        if (is_a<ACos>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACos>(arg->get_args()[0])->get_arg();
            return sqrt(sub(one, pow(i_arg, i2)));
        } else if (is_a<ATan>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ATan>(arg->get_args()[0])->get_arg();
            return div(i_arg, sqrt(add(one, pow(i_arg, i2))));
        } else if (is_a<ASec>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ASec>(arg->get_args()[0])->get_arg();
            return sqrt(sub(one, pow(i_arg, im2)));
        } else if (is_a<ACot>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACot>(arg->get_args()[0])->get_arg();
            return div(one, mul(i_arg, sqrt(add(one, pow(i_arg, im2)))));
        }
    } else if (is_a<Cos>(*arg)) {
        if (is_a<ASin>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ASin>(arg->get_args()[0])->get_arg();
            return sqrt(sub(one, pow(i_arg, i2)));
        } else if (is_a<ATan>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ATan>(arg->get_args()[0])->get_arg();
            return div(one, sqrt(add(one, pow(i_arg, i2))));
        } else if (is_a<ACsc>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACsc>(arg->get_args()[0])->get_arg();
            return sqrt(sub(one, pow(i_arg, im2)));
        } else if (is_a<ACot>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACot>(arg->get_args()[0])->get_arg();
            return div(one, sqrt(add(one, pow(i_arg, im2))));
        }
    } else if (is_a<Tan>(*arg)) {
        if (is_a<ASin>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ASin>(arg->get_args()[0])->get_arg();
            return div(i_arg, sqrt(sub(one, pow(i_arg, i2))));
        } else if (is_a<ACos>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACos>(arg->get_args()[0])->get_arg();
            return div(sqrt(sub(one, pow(i_arg, i2))), i_arg);
        } else if (is_a<ACsc>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACsc>(arg->get_args()[0])->get_arg();
            return div(one, mul(i_arg, sqrt(sub(one, pow(i_arg, im2)))));
        } else if (is_a<ASec>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ASec>(arg->get_args()[0])->get_arg();
            return mul(i_arg, sqrt(sub(one, pow(i_arg, im2))));
        }
    } else if (is_a<Csc>(*arg)) {
        if (is_a<ACos>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACos>(arg->get_args()[0])->get_arg();
            return div(one, sqrt(sub(one, pow(i_arg, i2))));
        } else if (is_a<ATan>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ATan>(arg->get_args()[0])->get_arg();
            return div(sqrt(add(one, pow(i_arg, i2))), i_arg);
        } else if (is_a<ASec>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ASec>(arg->get_args()[0])->get_arg();
            return div(one, sqrt(sub(one, pow(i_arg, im2))));
        } else if (is_a<ACot>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACot>(arg->get_args()[0])->get_arg();
            return mul(i_arg, sqrt(add(one, pow(i_arg, im2))));
        }
    } else if (is_a<Sec>(*arg)) {
        if (is_a<ASin>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ASin>(arg->get_args()[0])->get_arg();
            return div(one, sqrt(sub(one, pow(i_arg, i2))));
        } else if (is_a<ATan>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ATan>(arg->get_args()[0])->get_arg();
            return sqrt(add(one, pow(i_arg, i2)));
        } else if (is_a<ACsc>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACsc>(arg->get_args()[0])->get_arg();
            return div(one, sqrt(sub(one, pow(i_arg, im2))));
        } else if (is_a<ACot>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACot>(arg->get_args()[0])->get_arg();
            return sqrt(add(one, pow(i_arg, im2)));
        }
    } else if (is_a<Cot>(*arg)) {
        if (is_a<ASin>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ASin>(arg->get_args()[0])->get_arg();
            return div(sqrt(sub(one, pow(i_arg, i2))), i_arg);
        } else if (is_a<ACos>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACos>(arg->get_args()[0])->get_arg();
            return div(i_arg, sqrt(sub(one, pow(i_arg, i2))));
        } else if (is_a<ACsc>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ACsc>(arg->get_args()[0])->get_arg();
            return mul(i_arg, sqrt(sub(one, pow(i_arg, im2))));
        } else if (is_a<ASec>(*arg->get_args()[0])) {
            i_arg = rcp_static_cast<const ASec>(arg->get_args()[0])->get_arg();
            return div(one, mul(i_arg, sqrt(sub(one, pow(i_arg, im2)))));
        }
    }

    return arg;
}

/* ---------------------------- */
ASin::ASin(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ASin::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero) or eq(*arg, *one) or eq(*arg, *minus_one))
        return false;
    RCP<const Basic> index;
    if (inverse_lookup(inverse_cst, get_arg(), outArg(index))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool ASin::__eq__(const Basic &o) const
{
    if (is_a<ASin>(o)
        and eq(*get_arg(), *(static_cast<const ASin &>(o).get_arg())))
        return true;
    else
        return false;
}

int ASin::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ASin>(o))
    const ASin &s = static_cast<const ASin &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> asin(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    else if (eq(*arg, *one))
        return div(pi, i2);
    else if (eq(*arg, *minus_one))
        return mul(minus_one, div(pi, i2));
    else if (is_a_Number(*arg)
             and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().asin(*arg);
    }

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, arg, outArg(index));
    if (b) {
        return div(pi, index);
    } else {
        return make_rcp<const ASin>(arg);
    }
}

ACos::ACos(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACos::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero) or eq(*arg, *one) or eq(*arg, *minus_one))
        return false;
    RCP<const Basic> index;
    if (inverse_lookup(inverse_cst, get_arg(), outArg(index))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool ACos::__eq__(const Basic &o) const
{
    if (is_a<ACos>(o)
        and eq(*get_arg(), *(static_cast<const ACos &>(o).get_arg())))
        return true;
    else
        return false;
}

int ACos::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ACos>(o))
    const ACos &s = static_cast<const ACos &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> acos(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return div(pi, i2);
    else if (eq(*arg, *one))
        return zero;
    else if (eq(*arg, *minus_one))
        return pi;
    else if (is_a_Number(*arg)
             and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().acos(*arg);
    }

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, arg, outArg(index));
    if (b) {
        if (could_extract_minus(index)) {
            return add(pi, div(pi, index));
        } else {
            return sub(div(pi, i2), div(pi, index));
        }
    } else {
        return make_rcp<const ACos>(arg);
    }
}

ASec::ASec(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ASec::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *one) or eq(*arg, *minus_one))
        return false;
    RCP<const Basic> index;
    if (inverse_lookup(inverse_cst, div(one, get_arg()), outArg(index))) {
        return false;
    } else if (is_a_Number(*arg)
               and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool ASec::__eq__(const Basic &o) const
{
    if (is_a<ASec>(o)
        and eq(*get_arg(), *(static_cast<const ASec &>(o).get_arg())))
        return true;
    else
        return false;
}

int ASec::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ASec>(o))
    const ASec &s = static_cast<const ASec &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> asec(const RCP<const Basic> &arg)
{
    if (eq(*arg, *one))
        return zero;
    else if (eq(*arg, *minus_one))
        return pi;
    else if (is_a_Number(*arg)
             and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().asec(*arg);
    }

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, div(one, arg), outArg(index));
    if (b) {
        if (could_extract_minus(index)) {
            return add(pi, div(pi, index));
        } else {
            return sub(div(pi, i2), div(pi, index));
        }
    } else {
        return make_rcp<const ASec>(arg);
    }
}

ACsc::ACsc(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACsc::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *one) or eq(*arg, *minus_one))
        return false;
    RCP<const Basic> index;
    if (inverse_lookup(inverse_cst, div(one, arg), outArg(index))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool ACsc::__eq__(const Basic &o) const
{
    if (is_a<ACsc>(o)
        and eq(*get_arg(), *(static_cast<const ASec &>(o).get_arg())))
        return true;
    else
        return false;
}

int ACsc::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ACsc>(o))
    const ACsc &s = static_cast<const ACsc &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> acsc(const RCP<const Basic> &arg)
{
    if (eq(*arg, *one))
        return div(pi, i2);
    else if (eq(*arg, *minus_one))
        return div(pi, im2);
    else if (is_a_Number(*arg)
             and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().acsc(*arg);
    }

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, div(one, arg), outArg(index));
    if (b) {
        return div(pi, index);
    } else {
        return make_rcp<const ACsc>(arg);
    }
}

ATan::ATan(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ATan::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero) or eq(*arg, *one) or eq(*arg, *minus_one))
        return false;
    RCP<const Basic> index;
    if (inverse_lookup(inverse_tct, get_arg(), outArg(index))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool ATan::__eq__(const Basic &o) const
{
    if (is_a<ATan>(o)
        and eq(*get_arg(), *(static_cast<const ATan &>(o).get_arg())))
        return true;
    else
        return false;
}

int ATan::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ATan>(o))
    const ATan &s = static_cast<const ATan &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> atan(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    else if (eq(*arg, *one))
        return div(pi, mul(i2, i2));
    else if (eq(*arg, *minus_one))
        return mul(minus_one, div(pi, mul(i2, i2)));
    else if (is_a_Number(*arg)
             and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().atan(*arg);
    }

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_tct, arg, outArg(index));
    if (b) {
        return div(pi, index);
    } else {
        return make_rcp<const ATan>(arg);
    }
}

ACot::ACot(const RCP<const Basic> &arg) : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACot::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero) or eq(*arg, *one) or eq(*arg, *minus_one))
        return false;
    RCP<const Basic> index;
    if (inverse_lookup(inverse_tct, arg, outArg(index))) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool ACot::__eq__(const Basic &o) const
{
    if (is_a<ACot>(o)
        and eq(*get_arg(), *(static_cast<const ACot &>(o).get_arg())))
        return true;
    else
        return false;
}

int ACot::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ACot>(o))
    const ACot &s = static_cast<const ACot &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> acot(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return div(pi, i2);
    else if (eq(*arg, *one))
        return div(pi, mul(i2, i2));
    else if (eq(*arg, *minus_one))
        return mul(i3, div(pi, mul(i2, i2)));
    else if (is_a_Number(*arg)
             and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().acot(*arg);
    }

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_tct, arg, outArg(index));
    if (b) {
        if (could_extract_minus(index)) {
            return add(pi, div(pi, index));
        } else {
            return sub(div(pi, i2), div(pi, index));
        }
    } else {
        return make_rcp<const ACot>(arg);
    }
}

ATan2::ATan2(const RCP<const Basic> &num, const RCP<const Basic> &den)
    : num_{num}, den_{den}
{
    SYMENGINE_ASSERT(is_canonical(num, den))
}

bool ATan2::is_canonical(const RCP<const Basic> &num,
                         const RCP<const Basic> &den) const
{
    if (eq(*num, *zero) or eq(*num, *den) or eq(*num, *mul(minus_one, den)))
        return false;
    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_tct, div(num, den), outArg(index));
    if (b)
        return false;
    else
        return true;
}

bool ATan2::__eq__(const Basic &o) const
{
    if (is_a<ATan2>(o)) {
        const ATan2 &s = static_cast<const ATan2 &>(o);
        if (eq(*num_, *(s.get_num())) and eq(*den_, *(s.get_den())))
            return true;
        else
            return false;
    } else
        return false;
}

int ATan2::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ATan2>(o))
    const ATan2 &s = static_cast<const ATan2 &>(o);
    return div(num_, den_)->__cmp__(*div(s.num_, s.den_));
}

std::size_t ATan2::__hash__() const
{
    std::size_t seed = ATAN2;
    hash_combine<Basic>(seed, *num_);
    hash_combine<Basic>(seed, *den_);
    return seed;
}

RCP<const Basic> atan2(const RCP<const Basic> &num, const RCP<const Basic> &den)
{
    if (eq(*num, *zero)) {
        if (is_a_Number(*den)) {
            RCP<const Number> den_new = rcp_static_cast<const Number>(den);
            if (den_new->is_negative())
                return pi;
            else if (den_new->is_positive())
                return zero;
            // else it is NAN, yet to be implemented
            else {
                throw std::runtime_error("Not implemented.");
            }
        }
    } else if (eq(*den, *zero)) {
        if (is_a_Number(*num)) {
            RCP<const Number> num_new = rcp_static_cast<const Number>(num);
            if (num_new->is_negative())
                return div(pi, i2);
            else if (num_new->is_positive())
                return div(pi, im2);
            // else it is NAN, yet to be implemented
            else {
                throw std::runtime_error("Not implemented.");
            }
        }
    }
    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_tct, div(num, den), outArg(index));
    if (b) {
        // Ideally the answer should depend on the signs of `num` and
        // `den`
        // Currently is_positive() and is_negative() is not implemented
        // for
        // types other than `Number`
        // Hence this will give exact answers in case when num and den
        // are
        // numbers in SymEngine sense and when num and den are positive.
        // for the remaining cases in which we just return the value
        // from
        // the lookup table.
        // TODO: update once is_positive() and is_negative() is
        // implemented
        // in `Basic`
        if (is_a_Number(*den) and is_a_Number(*num)) {
            RCP<const Number> den_new = rcp_static_cast<const Number>(den);
            RCP<const Number> num_new = rcp_static_cast<const Number>(num);

            if (den_new->is_positive()) {
                return div(pi, index);
            } else if (den_new->is_negative()) {
                if (num_new->is_negative()) {
                    return sub(div(pi, index), pi);
                } else {
                    return add(div(pi, index), pi);
                }
            } else {
                return div(pi, index);
            }
        } else {
            return div(pi, index);
        }
    } else {
        return make_rcp<const ATan2>(num, den);
    }
}

/* ---------------------------- */

RCP<const Basic> Sin::create(const RCP<const Basic> &arg) const
{
    return sin(arg);
}

RCP<const Basic> Cos::create(const RCP<const Basic> &arg) const
{
    return cos(arg);
}

RCP<const Basic> Tan::create(const RCP<const Basic> &arg) const
{
    return tan(arg);
}

RCP<const Basic> Cot::create(const RCP<const Basic> &arg) const
{
    return cot(arg);
}

RCP<const Basic> Sec::create(const RCP<const Basic> &arg) const
{
    return sec(arg);
}

RCP<const Basic> Csc::create(const RCP<const Basic> &arg) const
{
    return csc(arg);
}

RCP<const Basic> ASin::create(const RCP<const Basic> &arg) const
{
    return asin(arg);
}

RCP<const Basic> ACos::create(const RCP<const Basic> &arg) const
{
    return acos(arg);
}

RCP<const Basic> ATan::create(const RCP<const Basic> &arg) const
{
    return atan(arg);
}

RCP<const Basic> ACot::create(const RCP<const Basic> &arg) const
{
    return acot(arg);
}

RCP<const Basic> ASec::create(const RCP<const Basic> &arg) const
{
    return asec(arg);
}

RCP<const Basic> ACsc::create(const RCP<const Basic> &arg) const
{
    return acsc(arg);
}

/* ---------------------------- */
LambertW::LambertW(const RCP<const Basic> &arg) : arg_{arg}
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool LambertW::is_canonical(const RCP<const Basic> &arg) const
{
    if (eq(*arg, *zero))
        return false;
    if (eq(*arg, *E))
        return false;
    if (eq(*arg, *div(one, E)))
        return false;
    if (eq(*arg, *div(log(i2), im2)))
        return false;
    return true;
}

std::size_t LambertW::__hash__() const
{
    std::size_t seed = LAMBERTW;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool LambertW::__eq__(const Basic &o) const
{
    if (is_a<LambertW>(o)
        and eq(*arg_, *(static_cast<const LambertW &>(o).arg_)))
        return true;
    return false;
}

int LambertW::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<LambertW>(o))
    return arg_->__cmp__(*(static_cast<const LambertW &>(o).arg_));
}

RCP<const Basic> lambertw(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    if (eq(*arg, *E))
        return one;
    if (eq(*arg, *div(neg(one), E)))
        return minus_one;
    if (eq(*arg, *div(log(i2), im2)))
        return mul(minus_one, log(i2));
    return make_rcp<const LambertW>(arg);
}

FunctionSymbol::FunctionSymbol(std::string name, const RCP<const Basic> &arg)
    : name_{name}, arg_{{arg}}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

FunctionSymbol::FunctionSymbol(std::string name, const vec_basic &arg)
    : name_{name}, arg_{arg}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool FunctionSymbol::is_canonical(const vec_basic &arg) const
{
    return true;
}

std::size_t FunctionSymbol::__hash__() const
{
    std::size_t seed = FUNCTIONSYMBOL;
    for (const auto &a : arg_)
        hash_combine<Basic>(seed, *a);
    hash_combine<std::string>(seed, name_);
    return seed;
}

bool FunctionSymbol::__eq__(const Basic &o) const
{
    if (is_a<FunctionSymbol>(o)
        and name_ == static_cast<const FunctionSymbol &>(o).name_
        and vec_basic_eq(arg_, static_cast<const FunctionSymbol &>(o).arg_))
        return true;
    return false;
}

int FunctionSymbol::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<FunctionSymbol>(o))
    const FunctionSymbol &s = static_cast<const FunctionSymbol &>(o);
    if (name_ == s.name_)
        return vec_basic_compare(arg_, s.arg_);
    else
        return name_ < s.name_ ? -1 : 1;
}

RCP<const Basic> FunctionSymbol::create(const vec_basic &x) const
{
    return make_rcp<const FunctionSymbol>(name_, x);
}

RCP<const Basic> FunctionSymbol::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    vec_basic v = arg_;
    for (auto &elem : v) {
        elem = elem->subs(subs_dict);
    }
    return create(v);
}

RCP<const Basic> function_symbol(std::string name, const vec_basic &arg)
{
    return make_rcp<const FunctionSymbol>(name, arg);
}

RCP<const Basic> function_symbol(std::string name, const RCP<const Basic> &arg)
{
    return make_rcp<const FunctionSymbol>(name, arg);
}

FunctionWrapper::FunctionWrapper(std::string name, const RCP<const Basic> &arg)
    : FunctionSymbol(name, arg)
{
}

FunctionWrapper::FunctionWrapper(std::string name, const vec_basic &vec)
    : FunctionSymbol(name, vec)
{
}

/* ---------------------------- */

Derivative::Derivative(const RCP<const Basic> &arg, const multiset_basic &x)
    : arg_{arg}, x_{x}
{
    SYMENGINE_ASSERT(is_canonical(arg, x))
}

bool Derivative::is_canonical(const RCP<const Basic> &arg,
                              const multiset_basic &x) const
{
    // Check that 'x' are Symbols:
    for (const auto &a : x)
        if (not is_a<Symbol>(*a))
            return false;
    if (is_a<FunctionSymbol>(*arg)) {
        for (auto &p : x) {
            RCP<const Symbol> s = rcp_static_cast<const Symbol>(p);
            RCP<const FunctionSymbol> f
                = rcp_static_cast<const FunctionSymbol>(arg);
            bool found_s = false;
            // 's' should be one of the args of the function
            // and should not appear anywhere else.
            for (const auto &a : f->get_args()) {
                if (eq(*a, *s)) {
                    if (found_s) {
                        return false;
                    } else {
                        found_s = true;
                    }
                } else if (neq(*a->diff(s), *zero)) {
                    return false;
                }
            }
            if (!found_s) {
                return false;
            }
        }
        return true;
    } else if (is_a<Abs>(*arg)) {
        return true;
    } else if (is_a<FunctionWrapper>(*arg)) {
        return true;
    } else if (is_a<PolyGamma>(*arg)) {
        bool found = false;
        auto v = arg->get_args();
        for (auto &p : x) {
            if (has_symbol(*v[0], rcp_static_cast<const Symbol>(p))) {
                found = true;
                break;
            }
        }
        return found;
    }
    return false;
}

std::size_t Derivative::__hash__() const
{
    std::size_t seed = DERIVATIVE;
    hash_combine<Basic>(seed, *arg_);
    for (auto &p : x_) {
        hash_combine<Basic>(seed, *p);
    }
    return seed;
}

bool Derivative::__eq__(const Basic &o) const
{
    if (is_a<Derivative>(o)
        and eq(*arg_, *(static_cast<const Derivative &>(o).arg_))
        and multiset_basic_eq(x_, static_cast<const Derivative &>(o).x_))
        return true;
    return false;
}

int Derivative::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Derivative>(o))
    const Derivative &s = static_cast<const Derivative &>(o);
    int cmp = arg_->__cmp__(*(s.arg_));
    if (cmp != 0)
        return cmp;
    cmp = multiset_basic_compare(x_, s.x_);
    return cmp;
}

RCP<const Basic> Derivative::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Symbol> s;
    map_basic_basic m, n;
    bool subs;
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    for (const auto &p : subs_dict) {
        subs = true;
        if (eq(*arg_->subs({{p.first, p.second}}), *arg_))
            continue;
        // If p.first and p.second are symbols and arg_ is
        // independent of p.second, p.first can be replaced
        if (is_a<Symbol>(*p.first) and is_a<Symbol>(*p.second)
            and eq(*arg_->diff(rcp_static_cast<const Symbol>(p.second)),
                   *zero)) {
            insert(n, p.first, p.second);
            continue;
        }
        for (const auto &d : x_) {
            if (is_a<Symbol>(*d)) {
                s = rcp_static_cast<const Symbol>(d);
                // If p.first or p.second has non zero derivates wrt to
                // s
                // p.first cannot be replaced
                if (neq(*zero, *(p.first->diff(s)))
                    || neq(*zero, *(p.second->diff(s)))) {
                    subs = false;
                    break;
                }
            } else {
                return make_rcp<const Subs>(rcp_from_this(), subs_dict);
            }
        }
        if (subs) {
            insert(n, p.first, p.second);
        } else {
            insert(m, p.first, p.second);
        }
    }
    multiset_basic sym;
    for (auto &p : x_) {
        sym.insert(p->subs(n));
    }
    if (m.empty()) {
        return Derivative::create(arg_->subs(n), sym);
    } else {
        return make_rcp<const Subs>(Derivative::create(arg_->subs(n), sym), m);
    }
}

// Subs class
Subs::Subs(const RCP<const Basic> &arg, const map_basic_basic &dict)
    : arg_{arg}, dict_{dict}
{
    SYMENGINE_ASSERT(is_canonical(arg, dict))
}

bool Subs::is_canonical(const RCP<const Basic> &arg,
                        const map_basic_basic &dict) const
{
    if (is_a<Derivative>(*arg)) {
        return true;
    }
    return false;
}

std::size_t Subs::__hash__() const
{
    std::size_t seed = SUBS;
    hash_combine<Basic>(seed, *arg_);
    for (const auto &p : dict_) {
        hash_combine<Basic>(seed, *p.first);
        hash_combine<Basic>(seed, *p.second);
    }
    return seed;
}

bool Subs::__eq__(const Basic &o) const
{
    if (is_a<Subs>(o) and eq(*arg_, *(static_cast<const Subs &>(o).arg_))
        and map_eq(dict_, static_cast<const Subs &>(o).dict_))
        return true;
    return false;
}

int Subs::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Subs>(o))
    const Subs &s = static_cast<const Subs &>(o);
    int cmp = arg_->__cmp__(*(s.arg_));
    if (cmp != 0)
        return cmp;
    cmp = map_compare(dict_, s.dict_);
    return cmp;
}

vec_basic Subs::get_variables() const
{
    vec_basic v;
    for (const auto &p : dict_) {
        v.push_back(p.first);
    }
    return v;
}

vec_basic Subs::get_point() const
{
    vec_basic v;
    for (const auto &p : dict_) {
        v.push_back(p.second);
    }
    return v;
}

vec_basic Subs::get_args() const
{
    vec_basic v = {arg_};
    for (const auto &p : dict_) {
        v.push_back(p.first);
    }
    for (const auto &p : dict_) {
        v.push_back(p.second);
    }
    return v;
}

RCP<const Basic> Subs::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    map_basic_basic m, n;
    for (const auto &p : subs_dict) {
        bool found = false;
        for (const auto &s : dict_) {
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
    for (const auto &s : dict_) {
        insert(m, s.first, s.second->subs(subs_dict));
    }
    RCP<const Basic> presub = arg_->subs(n);
    if (is_a<Subs>(*presub)) {
        for (auto &q : static_cast<const Subs &>(*presub).dict_) {
            insert(m, q.first, q.second);
        }
        return make_rcp<const Subs>(static_cast<const Subs &>(*presub).arg_, m);
    } else {
        return make_rcp<const Subs>(presub, m);
    }
}

std::size_t HyperbolicFunction::__hash__() const
{
    std::size_t seed = this->get_type_code();
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

RCP<const Basic> HyperbolicFunction::create(const RCP<const Basic> &arg) const
{
    throw std::runtime_error("Should be implemented by the inherited class");
}

RCP<const Basic>
HyperbolicFunction::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
        return rcp_from_this();
    else
        return this->create(arg);
}

Sinh::Sinh(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Sinh::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Sinh::__eq__(const Basic &o) const
{
    if (is_a<Sinh>(o)
        and eq(*get_arg(), *(static_cast<const Sinh &>(o).get_arg())))
        return true;
    else
        return false;
}

int Sinh::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Sinh>(o))
    const Sinh &s = static_cast<const Sinh &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> sinh(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().sinh(*_arg);
        } else if (_arg->is_negative()) {
            return neg(sinh(zero->sub(*_arg)));
        }
    }
    if (could_extract_minus(arg)) {
        return neg(sinh(neg(arg)));
    }
    return make_rcp<const Sinh>(arg);
}

RCP<const Basic> Sinh::expand_as_exp() const
{
    return div(sub(exp(get_arg()), exp(mul(get_arg(), minus_one))), i2);
}

Csch::Csch(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Csch::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Csch::__eq__(const Basic &o) const
{
    if (is_a<Csch>(o)
        and eq(*get_arg(), *(static_cast<const Csch &>(o).get_arg())))
        return true;
    else
        return false;
}

int Csch::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Csch>(o))
    const Csch &s = static_cast<const Csch &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> csch(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero)) {
        // Answer is infinity. Yet to be implemented in SymEngine
        throw std::runtime_error("Not implemented.");
    }
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().csch(*_arg);
        } else if (_arg->is_negative()) {
            return neg(csch(zero->sub(*_arg)));
        }
    }
    if (could_extract_minus(arg)) {
        return neg(csch(neg(arg)));
    }
    return make_rcp<const Csch>(arg);
}

RCP<const Basic> Csch::expand_as_exp() const
{
    RCP<const Basic> pos_exp = exp(get_arg());
    RCP<const Basic> neg_exp = exp(mul(minus_one, get_arg()));
    return div(i2, sub(pos_exp, neg_exp));
}

Cosh::Cosh(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Cosh::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Cosh::__eq__(const Basic &o) const
{
    if (is_a<Cosh>(o)
        and eq(*get_arg(), *(static_cast<const Cosh &>(o).get_arg())))
        return true;
    else
        return false;
}

int Cosh::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Cosh>(o))
    const Cosh &s = static_cast<const Cosh &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> cosh(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return one;
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().cosh(*_arg);
        } else if (_arg->is_negative()) {
            return cosh(zero->sub(*_arg));
        }
    }
    if (could_extract_minus(arg)) {
        return cosh(neg(arg));
    }
    return make_rcp<const Cosh>(arg);
}

RCP<const Basic> Cosh::expand_as_exp() const
{
    return div(add(exp(get_arg()), exp(mul(get_arg(), minus_one))), i2);
}

Sech::Sech(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Sech::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Sech::__eq__(const Basic &o) const
{
    if (is_a<Sech>(o)
        and eq(*get_arg(), *(static_cast<const Sech &>(o).get_arg())))
        return true;
    else
        return false;
}

int Sech::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Sech>(o))
    const Sech &s = static_cast<const Sech &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> sech(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return one;
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().sech(*_arg);
        } else if (_arg->is_negative()) {
            return sech(zero->sub(*_arg));
        }
    }
    if (could_extract_minus(arg)) {
        return sech(neg(arg));
    }
    return make_rcp<const Sech>(arg);
}

RCP<const Basic> Sech::expand_as_exp() const
{
    RCP<const Basic> pos_exp = exp(get_arg());
    RCP<const Basic> neg_exp = exp(mul(minus_one, get_arg()));
    return div(i2, add(pos_exp, neg_exp));
}

Tanh::Tanh(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Tanh::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Tanh::__eq__(const Basic &o) const
{
    if (is_a<Tanh>(o)
        and eq(*get_arg(), *(static_cast<const Tanh &>(o).get_arg())))
        return true;
    else
        return false;
}

int Tanh::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Tanh>(o))
    const Tanh &s = static_cast<const Tanh &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> tanh(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().tanh(*_arg);
        } else if (_arg->is_negative()) {
            return neg(tanh(zero->sub(*_arg)));
        }
    }
    if (could_extract_minus(arg)) {
        return neg(tanh(neg(arg)));
    }
    return make_rcp<const Tanh>(arg);
}

RCP<const Basic> Tanh::expand_as_exp() const
{
    RCP<const Basic> pos_exp = exp(get_arg());
    RCP<const Basic> neg_exp = exp(mul(minus_one, get_arg()));
    return div(sub(pos_exp, neg_exp), add(pos_exp, neg_exp));
}

Coth::Coth(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Coth::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Coth::__eq__(const Basic &o) const
{
    if (is_a<Coth>(o)
        and eq(*get_arg(), *(static_cast<const Coth &>(o).get_arg())))
        return true;
    else
        return false;
}

int Coth::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Coth>(o))
    const Coth &s = static_cast<const Coth &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> coth(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero)) {
        // Answer is infinity. Yet to be implemented in SymEngine
        throw std::runtime_error("Not implemented.");
    }
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().coth(*_arg);
        } else if (_arg->is_negative()) {
            return neg(coth(zero->sub(*_arg)));
        }
    }
    if (could_extract_minus(arg)) {
        return neg(coth(neg(arg)));
    }
    return make_rcp<const Coth>(arg);
}

RCP<const Basic> Coth::expand_as_exp() const
{
    RCP<const Basic> pos_exp = exp(get_arg());
    RCP<const Basic> neg_exp = exp(mul(minus_one, get_arg()));
    return div(add(pos_exp, neg_exp), sub(pos_exp, neg_exp));
}

ASinh::ASinh(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ASinh::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero) or eq(*arg, *one) or eq(*arg, *minus_one))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool ASinh::__eq__(const Basic &o) const
{
    if (is_a<ASinh>(o)
        and eq(*get_arg(), *(static_cast<const ASinh &>(o).get_arg())))
        return true;
    else
        return false;
}

int ASinh::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ASinh>(o))
    const ASinh &s = static_cast<const ASinh &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> asinh(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    if (eq(*arg, *one))
        return log(add(one, sq2));
    if (eq(*arg, *minus_one))
        return log(sub(sq2, one));
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().asinh(*_arg);
        } else if (_arg->is_negative()) {
            return neg(asinh(zero->sub(*_arg)));
        }
    }
    if (could_extract_minus(arg)) {
        return neg(asinh(neg(arg)));
    }
    return make_rcp<const ASinh>(arg);
}

ACsch::ACsch(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACsch::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *one) or eq(*arg, *minus_one))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool ACsch::__eq__(const Basic &o) const
{
    if (is_a<ACsch>(o)
        and eq(*get_arg(), *(static_cast<const ACsch &>(o).get_arg())))
        return true;
    else
        return false;
}

int ACsch::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ACsch>(o))
    const ACsch &s = static_cast<const ACsch &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> acsch(const RCP<const Basic> &arg)
{
    if (eq(*arg, *one))
        return log(add(one, sq2));
    if (eq(*arg, *minus_one))
        return log(sub(sq2, one));
    if (could_extract_minus(arg)) {
        return neg(acsch(neg(arg)));
    }
    return make_rcp<const ACsch>(arg);
}

ACosh::ACosh(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACosh::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    // TODO: Lookup into a cst table once complex is implemented
    if (eq(*arg, *one))
        return false;
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

bool ACosh::__eq__(const Basic &o) const
{
    if (is_a<ACosh>(o)
        and eq(*get_arg(), *(static_cast<const ACosh &>(o).get_arg())))
        return true;
    else
        return false;
}

int ACosh::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ACosh>(o))
    const ACosh &s = static_cast<const ACosh &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> acosh(const RCP<const Basic> &arg)
{
    // TODO: Lookup into a cst table once complex is implemented
    if (eq(*arg, *one))
        return zero;
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().acosh(*arg);
    }
    return make_rcp<const ACosh>(arg);
}

ATanh::ATanh(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ATanh::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(*arg, *zero))
        return false;
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool ATanh::__eq__(const Basic &o) const
{
    if (is_a<ATanh>(o)
        and eq(*get_arg(), *(static_cast<const ATanh &>(o).get_arg())))
        return true;
    else
        return false;
}

int ATanh::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ATanh>(o))
    const ATanh &s = static_cast<const ATanh &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> atanh(const RCP<const Basic> &arg)
{
    if (eq(*arg, *zero))
        return zero;
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().atanh(*_arg);
        } else if (_arg->is_negative()) {
            return neg(atanh(zero->sub(*_arg)));
        }
    }
    if (could_extract_minus(arg)) {
        return neg(atanh(neg(arg)));
    }
    return make_rcp<const ATanh>(arg);
}

ACoth::ACoth(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACoth::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    if (is_a_Number(*arg)) {
        if (static_cast<const Number &>(*arg).is_negative()) {
            return false;
        } else if (not static_cast<const Number &>(*arg).is_exact()) {
            return false;
        }
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool ACoth::__eq__(const Basic &o) const
{
    if (is_a<ACoth>(o)
        and eq(*get_arg(), *(static_cast<const ACoth &>(o).get_arg())))
        return true;
    else
        return false;
}

int ACoth::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ACoth>(o))
    const ACoth &s = static_cast<const ACoth &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> acoth(const RCP<const Basic> &arg)
{
    if (is_a_Number(*arg)) {
        RCP<const Number> _arg = rcp_static_cast<const Number>(arg);
        if (not _arg->is_exact()) {
            return _arg->get_eval().acoth(*_arg);
        } else if (_arg->is_negative()) {
            return neg(acoth(zero->sub(*_arg)));
        }
    }
    if (could_extract_minus(arg)) {
        return neg(acoth(neg(arg)));
    }
    return make_rcp<const ACoth>(arg);
}

ASech::ASech(const RCP<const Basic> &arg) : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ASech::is_canonical(const RCP<const Basic> &arg) const
{
    // TODO: Add further checks for +inf -inf cases
    // TODO: Lookup into a cst table once complex is implemented
    if (eq(*arg, *one))
        return false;
    return true;
}

bool ASech::__eq__(const Basic &o) const
{
    if (is_a<ASech>(o)
        and eq(*get_arg(), *(static_cast<const ASech &>(o).get_arg())))
        return true;
    else
        return false;
}

int ASech::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ASech>(o))
    const ASech &s = static_cast<const ASech &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> asech(const RCP<const Basic> &arg)
{
    // TODO: Lookup into a cst table once complex is implemented
    if (eq(*arg, *one))
        return zero;
    return make_rcp<const ASech>(arg);
}

RCP<const Basic> Sinh::create(const RCP<const Basic> &arg) const
{
    return sinh(arg);
}

RCP<const Basic> Csch::create(const RCP<const Basic> &arg) const
{
    return csch(arg);
}

RCP<const Basic> Cosh::create(const RCP<const Basic> &arg) const
{
    return cosh(arg);
}

RCP<const Basic> Sech::create(const RCP<const Basic> &arg) const
{
    return sech(arg);
}

RCP<const Basic> Tanh::create(const RCP<const Basic> &arg) const
{
    return tanh(arg);
}

RCP<const Basic> Coth::create(const RCP<const Basic> &arg) const
{
    return coth(arg);
}

RCP<const Basic> ASinh::create(const RCP<const Basic> &arg) const
{
    return asinh(arg);
}

RCP<const Basic> ACsch::create(const RCP<const Basic> &arg) const
{
    return acsch(arg);
}

RCP<const Basic> ACosh::create(const RCP<const Basic> &arg) const
{
    return acosh(arg);
}

RCP<const Basic> ATanh::create(const RCP<const Basic> &arg) const
{
    return atanh(arg);
}

RCP<const Basic> ACoth::create(const RCP<const Basic> &arg) const
{
    return acoth(arg);
}

RCP<const Basic> ASech::create(const RCP<const Basic> &arg) const
{
    return asech(arg);
}

KroneckerDelta::KroneckerDelta(const RCP<const Basic> &i,
                               const RCP<const Basic> &j)
    : i_{i}, j_{j}
{
    SYMENGINE_ASSERT(is_canonical(i_, j_))
}

bool KroneckerDelta::is_canonical(const RCP<const Basic> &i,
                                  const RCP<const Basic> &j) const
{
    RCP<const Basic> diff = expand(sub(i, j));
    if (eq(*diff, *zero)) {
        return false;
    } else if (is_a_Number(*diff)) {
        return false;
    } else {
        // TODO: SymPy uses default key sorting to return in order
        return true;
    }
}

bool KroneckerDelta::__eq__(const Basic &o) const
{
    if (is_a<KroneckerDelta>(o)
        and eq(*i_, *(static_cast<const KroneckerDelta &>(o).i_))
        and eq(*j_, *(static_cast<const KroneckerDelta &>(o).j_)))
        return true;
    else
        return false;
}

int KroneckerDelta::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<KroneckerDelta>(o))
    const KroneckerDelta &s = static_cast<const KroneckerDelta &>(o);
    return i_->__cmp__(*(s.i_));
}

std::size_t KroneckerDelta::__hash__() const
{
    std::size_t seed = KRONECKERDELTA;
    hash_combine<Basic>(seed, *i_);
    hash_combine<Basic>(seed, *j_);
    return seed;
}

RCP<const Basic> KroneckerDelta::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> i = i_->subs(subs_dict);
    RCP<const Basic> j = j_->subs(subs_dict);
    if (i == i_ and j == j_) {
        return rcp_from_this();
    } else {
        return kronecker_delta(i, j);
    }
}

RCP<const Basic> kronecker_delta(const RCP<const Basic> &i,
                                 const RCP<const Basic> &j)
{
    // Expand is needed to simplify things like `i-(i+1)` to `-1`
    RCP<const Basic> diff = expand(sub(i, j));
    if (eq(*diff, *zero)) {
        return one;
    } else if (is_a_Number(*diff)) {
        return zero;
    } else {
        // SymPy uses default key sorting to return in order
        return make_rcp<const KroneckerDelta>(i, j);
    }
}

bool has_dup(const vec_basic &arg)
{
    map_basic_basic d;
    auto it = d.end();
    for (const auto &p : arg) {
        it = d.find(p);
        if (it == d.end()) {
            insert(d, p, one);
        } else {
            return true;
        }
    }
    return false;
}

LeviCivita::LeviCivita(const vec_basic &&arg) : arg_{std::move(arg)}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool LeviCivita::is_canonical(const vec_basic &arg) const
{
    bool are_int = true;
    for (const auto &p : arg) {
        if (not(is_a_Number(*p))) {
            are_int = false;
            break;
        }
    }
    if (are_int) {
        return false;
    } else if (has_dup(arg)) {
        return false;
    } else {
        return true;
    }
}

bool LeviCivita::__eq__(const Basic &o) const
{
    if (is_a<LeviCivita>(o)
        and vec_basic_eq(arg_, static_cast<const LeviCivita &>(o).arg_))
        return true;
    else
        return false;
}

int LeviCivita::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<LeviCivita>(o))
    const LeviCivita &s = static_cast<const LeviCivita &>(o);
    return vec_basic_compare(arg_, s.arg_);
}

std::size_t LeviCivita::__hash__() const
{
    std::size_t seed = LEVICIVITA;
    for (const auto &p : arg_) {
        hash_combine<Basic>(seed, *p);
    }
    return seed;
}

RCP<const Basic> eval_levicivita(const vec_basic &arg, int len)
{
    int i, j;
    RCP<const Basic> res = one;
    for (i = 0; i < len; i++) {
        for (j = i + 1; j < len; j++) {
            res = mul(sub(arg[j], arg[i]), res);
        }
        res = div(res, factorial(i));
    }
    return res;
}

RCP<const Basic> levi_civita(const vec_basic &arg)
{
    bool are_int = true;
    int len = 0;
    for (const auto &p : arg) {
        if (not(is_a_Number(*p))) {
            are_int = false;
            break;
        } else {
            len++;
        }
    }
    if (are_int) {
        return eval_levicivita(arg, len);
    } else if (has_dup(arg)) {
        return zero;
    } else {
        return make_rcp<const LeviCivita>(std::move(arg));
    }
}

Zeta::Zeta(const RCP<const Basic> &s, const RCP<const Basic> &a) : s_{s}, a_{a}
{
    SYMENGINE_ASSERT(is_canonical(s_, a_))
}

Zeta::Zeta(const RCP<const Basic> &s) : s_{s}, a_{one}
{
    SYMENGINE_ASSERT(is_canonical(s_, a_))
}

bool Zeta::is_canonical(const RCP<const Basic> &s,
                        const RCP<const Basic> &a) const
{
    if (eq(*s, *zero))
        return false;
    if (eq(*s, *one))
        return false;
    if (is_a<Integer>(*s) and is_a<Integer>(*a)) {
        auto s_ = static_cast<const Integer &>(*s).as_int();
        if (s_ < 0 || s_ % 2 == 0)
            return false;
    }
    return true;
}

std::size_t Zeta::__hash__() const
{
    std::size_t seed = ZETA;
    hash_combine<Basic>(seed, *s_);
    hash_combine<Basic>(seed, *a_);
    return seed;
}

bool Zeta::__eq__(const Basic &o) const
{
    if (is_a<Zeta>(o) and eq(*s_, *(static_cast<const Zeta &>(o).s_))
        and eq(*a_, *(static_cast<const Zeta &>(o).a_)))
        return true;
    return false;
}

int Zeta::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Zeta>(o))
    return s_->__cmp__(*(static_cast<const Zeta &>(o).s_));
}

RCP<const Basic> zeta(const RCP<const Basic> &s, const RCP<const Basic> &a)
{
    if (is_a_Number(*s)) {
        if (rcp_static_cast<const Number>(s)->is_zero()) {
            return sub(div(one, i2), a);
        } else if (rcp_static_cast<const Number>(s)->is_one()) {
            throw std::runtime_error("Complex infinity is not yet implemented");
        } else if (is_a<Integer>(*s) and is_a<Integer>(*a)) {
            auto s_ = static_cast<const Integer &>(*s).as_int();
            auto a_ = static_cast<const Integer &>(*a).as_int();
            RCP<const Basic> zeta;
            if (s_ < 0) {
                RCP<const Number> res = (s_ % 2 == 0) ? one : minus_one;
                zeta
                    = mulnum(res, divnum(bernoulli(-s_ + 1), integer(-s_ + 1)));
            } else if (s_ % 2 == 0) {
                RCP<const Number> b = bernoulli(s_);
                RCP<const Number> f = factorial(s_);
                zeta = divnum(pownum(integer(2), integer(s_ - 1)), f);
                zeta = mul(zeta, mul(pow(pi, s), abs(b)));
            } else {
                return make_rcp<const Zeta>(s, a);
            }
            if (a_ < 0)
                return add(zeta, harmonic(-a_, s_));
            return sub(zeta, harmonic(a_ - 1, s_));
        }
    }
    return make_rcp<const Zeta>(s, a);
}

RCP<const Basic> zeta(const RCP<const Basic> &s)
{
    return zeta(s, one);
}

Dirichlet_eta::Dirichlet_eta(const RCP<const Basic> &s) : s_{s}
{
    SYMENGINE_ASSERT(is_canonical(s_))
}

bool Dirichlet_eta::is_canonical(const RCP<const Basic> &s) const
{
    if (eq(*s, *one))
        return false;
    if (not(is_a<Zeta>(*zeta(s))))
        return false;
    return true;
}

std::size_t Dirichlet_eta::__hash__() const
{
    std::size_t seed = DIRICHLET_ETA;
    hash_combine<Basic>(seed, *s_);
    return seed;
}

bool Dirichlet_eta::__eq__(const Basic &o) const
{
    if (is_a<Dirichlet_eta>(o)
        and eq(*s_, *(static_cast<const Dirichlet_eta &>(o).s_)))
        return true;
    return false;
}

int Dirichlet_eta::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Dirichlet_eta>(o))
    return s_->__cmp__(*(static_cast<const Dirichlet_eta &>(o).s_));
}

RCP<const Basic> Dirichlet_eta::rewrite_as_zeta() const
{
    return mul(sub(one, pow(i2, sub(one, s_))), zeta(s_));
}

RCP<const Basic> dirichlet_eta(const RCP<const Basic> &s)
{
    if (is_a_Number(*s) and rcp_static_cast<const Number>(s)->is_one()) {
        return log(i2);
    }
    RCP<const Basic> z = zeta(s);
    if (is_a<Zeta>(*z)) {
        return make_rcp<const Dirichlet_eta>(s);
    } else {
        return mul(sub(one, pow(i2, sub(one, s))), z);
    }
}

bool Erf::is_canonical(const RCP<const Basic> &arg) const
{
    if (is_a<Integer>(*arg) and rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    if (could_extract_minus(arg))
        return false;
    return true;
}

std::size_t Erf::__hash__() const
{
    std::size_t seed = ERF;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Erf::__eq__(const Basic &o) const
{
    if (is_a<Erf>(o) and eq(*arg_, *(static_cast<const Erf &>(o).arg_)))
        return true;
    return false;
}

int Erf::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Erf>(o))
    return arg_->__cmp__(*(static_cast<const Erf &>(o).arg_));
}

RCP<const Basic> erf(const RCP<const Basic> &arg)
{
    if (is_a<Integer>(*arg)
        and rcp_static_cast<const Integer>(arg)->is_zero()) {
        return zero;
    }
    if (could_extract_minus(arg)) {
        return neg(erf(neg(arg)));
    }
    return make_rcp<Erf>(arg);
}

RCP<const Basic> Erf::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
        return rcp_from_this();
    else
        return erf(arg);
}

Gamma::Gamma(const RCP<const Basic> &arg) : arg_{arg}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool Gamma::is_canonical(const RCP<const Basic> &arg) const
{
    if (is_a<Integer>(*arg))
        return false;
    if (is_a<Rational>(*arg)
        and (get_den(rcp_static_cast<const Rational>(arg)->i)) == 2) {
        return false;
    }
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }
    return true;
}

std::size_t Gamma::__hash__() const
{
    std::size_t seed = GAMMA;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Gamma::__eq__(const Basic &o) const
{
    if (is_a<Gamma>(o) and eq(*arg_, *(static_cast<const Gamma &>(o).arg_)))
        return true;
    return false;
}

int Gamma::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Gamma>(o))
    return arg_->__cmp__(*(static_cast<const Gamma &>(o).arg_));
}

RCP<const Basic> Gamma::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
        return rcp_from_this();
    else
        return gamma(arg);
}

RCP<const Basic> gamma_positive_int(const RCP<const Basic> &arg)
{
    SYMENGINE_ASSERT(is_a<Integer>(*arg))
    RCP<const Integer> arg_ = rcp_static_cast<const Integer>(arg);
    SYMENGINE_ASSERT(arg_->is_positive())
    return factorial((arg_->subint(*one))->as_int());
}

RCP<const Basic> gamma_multiple_2(const RCP<const Basic> &arg)
{
    SYMENGINE_ASSERT(is_a<Rational>(*arg))
    RCP<const Rational> arg_ = rcp_static_cast<const Rational>(arg);
    SYMENGINE_ASSERT(get_den(arg_->i) == 2)
    RCP<const Integer> n, k;
    RCP<const Number> coeff;
    n = quotient_f(*(integer(mp_abs(get_num(arg_->i)))),
                   *(integer(get_den(arg_->i))));
    if (arg_->is_positive()) {
        k = n;
        coeff = one;
    } else {
        n = n->addint(*one);
        k = n;
        if ((n->as_int() & 1) == 0) {
            coeff = one;
        } else {
            coeff = minus_one;
        }
    }
    int j = 1;
    for (int i = 3; i < 2 * k->as_int(); i = i + 2) {
        j = j * i;
    }
    coeff = mulnum(coeff, integer(j));
    if (arg_->is_positive()) {
        return div(mul(coeff, sqrt(pi)), pow(i2, n));
    } else {
        return div(mul(pow(i2, n), sqrt(pi)), coeff);
    }
}

RCP<const Basic> gamma(const RCP<const Basic> &arg)
{
    if (is_a<Integer>(*arg)) {
        RCP<const Integer> arg_ = rcp_static_cast<const Integer>(arg);
        if (arg_->is_positive()) {
            return gamma_positive_int(arg);
        } else {
            throw std::runtime_error("Complex Infinity not yet implemented");
        }
    } else if (is_a<Rational>(*arg)) {
        RCP<const Rational> arg_ = rcp_static_cast<const Rational>(arg);
        if ((get_den(arg_->i)) == 2) {
            return gamma_multiple_2(arg);
        } else {
            return make_rcp<const Gamma>(arg);
        }
    } else if (is_a_Number(*arg)
               and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().gamma(*arg);
    }
    return make_rcp<const Gamma>(arg);
}

LowerGamma::LowerGamma(const RCP<const Basic> &s, const RCP<const Basic> &x)
    : s_{s}, x_{x}
{
    SYMENGINE_ASSERT(is_canonical(s_, x_))
}

bool LowerGamma::is_canonical(const RCP<const Basic> &s,
                              const RCP<const Basic> &x) const
{
    // Only special values are evaluated
    if (eq(*s, *one))
        return false;
    if (is_a<Integer>(*s) and rcp_static_cast<const Integer>(s)->i > 1)
        return false;
    if (is_a<Integer>(*mul(i2, s)))
        return false;
    return true;
}

std::size_t LowerGamma::__hash__() const
{
    std::size_t seed = LOWERGAMMA;
    hash_combine<Basic>(seed, *s_);
    hash_combine<Basic>(seed, *x_);
    return seed;
}

bool LowerGamma::__eq__(const Basic &o) const
{
    if (is_a<LowerGamma>(o)
        and eq(*s_, *(static_cast<const LowerGamma &>(o).s_))
        and eq(*x_, *(static_cast<const LowerGamma &>(o).x_)))
        return true;
    return false;
}

int LowerGamma::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<LowerGamma>(o))
    const LowerGamma &lg = static_cast<const LowerGamma &>(o);
    if (neq(*s_, *(lg.s_))) {
        return s_->__cmp__(*(static_cast<const LowerGamma &>(o).s_));
    } else {
        return x_->__cmp__(*(static_cast<const LowerGamma &>(o).x_));
    }
}

RCP<const Basic> LowerGamma::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> s = s_->subs(subs_dict);
    RCP<const Basic> x = x_->subs(subs_dict);
    if (x == x_ and s == s_) {
        return rcp_from_this();
    } else {
        return lowergamma(s, x);
    }
}

RCP<const Basic> lowergamma(const RCP<const Basic> &s,
                            const RCP<const Basic> &x)
{
    // Only special values are being evaluated
    if (is_a<Integer>(*s)) {
        RCP<const Integer> s_int = rcp_static_cast<const Integer>(s);
        if (s_int->is_one()) {
            return sub(one, exp(mul(minus_one, x)));
        } else if (s_int->i > 1) {
            s_int = s_int->subint(*one);
            return sub(mul(s_int, lowergamma(s_int, x)),
                       mul(pow(x, s_int), exp(mul(minus_one, x))));
        } else {
            return make_rcp<const LowerGamma>(s, x);
        }
    } else if (is_a<Integer>(*(mul(i2, s)))) {
        // TODO: Implement `erf`. Currently the recursive expansion has
        // no base
        // case
        // when s is of form n/2 n is Integer
        RCP<const Number> s_num = rcp_static_cast<const Number>(s);
        s_num = subnum(s_num, one);
        if (s_num->is_positive()) {
            return sub(mul(s_num, lowergamma(s_num, x)),
                       mul(pow(x, s_num), exp(mul(minus_one, x))));
        } else {
            return add(lowergamma(add(s, one), x),
                       mul(pow(x, s), div(exp(mul(minus_one, x)), s)));
        }
    }
    return make_rcp<const LowerGamma>(s, x);
}

UpperGamma::UpperGamma(const RCP<const Basic> &s, const RCP<const Basic> &x)
    : s_{s}, x_{x}
{
    SYMENGINE_ASSERT(is_canonical(s_, x_))
}

bool UpperGamma::is_canonical(const RCP<const Basic> &s,
                              const RCP<const Basic> &x) const
{
    // Only special values are evaluated
    if (eq(*s, *one))
        return false;
    if (is_a<Integer>(*s) and rcp_static_cast<const Integer>(s)->i > 1)
        return false;
    if (is_a<Integer>(*mul(i2, s)))
        return false;
    return true;
}

std::size_t UpperGamma::__hash__() const
{
    std::size_t seed = UPPERGAMMA;
    hash_combine<Basic>(seed, *s_);
    hash_combine<Basic>(seed, *x_);
    return seed;
}

bool UpperGamma::__eq__(const Basic &o) const
{
    if (is_a<UpperGamma>(o)
        and eq(*s_, *(static_cast<const UpperGamma &>(o).s_))
        and eq(*x_, *(static_cast<const UpperGamma &>(o).x_)))
        return true;
    return false;
}

int UpperGamma::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<UpperGamma>(o))
    const UpperGamma &ug = static_cast<const UpperGamma &>(o);
    if (neq(*s_, *(ug.s_))) {
        return s_->__cmp__(*(static_cast<const UpperGamma &>(o).s_));
    } else {
        return x_->__cmp__(*(static_cast<const UpperGamma &>(o).x_));
    }
}

RCP<const Basic> UpperGamma::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> s = s_->subs(subs_dict);
    RCP<const Basic> x = x_->subs(subs_dict);
    if (s == s_ and x == x_) {
        return rcp_from_this();
    } else {
        return uppergamma(s, x);
    }
}

RCP<const Basic> uppergamma(const RCP<const Basic> &s,
                            const RCP<const Basic> &x)
{
    // Only special values are being evaluated
    if (is_a<Integer>(*s)) {
        RCP<const Integer> s_int = rcp_static_cast<const Integer>(s);
        if (s_int->is_one()) {
            return exp(mul(minus_one, x));
        } else if (s_int->i > 1) {
            s_int = s_int->subint(*one);
            return add(mul(s_int, uppergamma(s_int, x)),
                       mul(pow(x, s_int), exp(mul(minus_one, x))));
        } else {
            // TODO: implement unpolarfy to handle this case
            return make_rcp<const LowerGamma>(s, x);
        }
    } else if (is_a<Integer>(*(mul(i2, s)))) {
        // TODO: Implement `erf`. Currently the recursive expansion has
        // no base
        // case
        // when s is of form n/2 n is Integer
        RCP<const Number> s_num = rcp_static_cast<const Number>(s);
        s_num = subnum(s_num, one);
        if (s_num->is_positive()) {
            return add(mul(s_num, uppergamma(s_num, x)),
                       mul(pow(x, s_num), exp(mul(minus_one, x))));
        } else {
            return sub(uppergamma(add(s, one), x),
                       mul(pow(x, s), div(exp(mul(minus_one, x)), s)));
        }
    }
    return make_rcp<const UpperGamma>(s, x);
}

bool LogGamma::is_canonical(const RCP<const Basic> &arg) const
{
    if (is_a<Integer>(*arg)) {
        RCP<const Integer> arg_int = rcp_static_cast<const Integer>(arg);
        if (not arg_int->is_positive()) {
            return false;
        }
        if (eq(*integer(1), *arg_int) or eq(*integer(2), *arg_int)
            or eq(*integer(3), *arg_int)) {
            return false;
        }
    }
    return true;
}

std::size_t LogGamma::__hash__() const
{
    std::size_t seed = LOGGAMMA;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool LogGamma::__eq__(const Basic &o) const
{
    if (is_a<LogGamma>(o)
        and eq(*arg_, *(static_cast<const LogGamma &>(o).arg_)))
        return true;
    return false;
}

int LogGamma::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<LogGamma>(o))
    return arg_->__cmp__(*(static_cast<const LogGamma &>(o).arg_));
}

RCP<const Basic> LogGamma::rewrite_as_gamma() const
{
    return log(gamma(arg_));
}

RCP<const Basic> LogGamma::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
        return rcp_from_this();
    else
        return loggamma(arg);
}

RCP<const Basic> loggamma(const RCP<const Basic> &arg)
{
    if (is_a<Integer>(*arg)) {
        RCP<const Integer> arg_int = rcp_static_cast<const Integer>(arg);
        if (not arg_int->is_positive()) {
            throw std::runtime_error("Infinity not yet implemented");
        }
        if (eq(*integer(1), *arg_int) or eq(*integer(2), *arg_int)) {
            return zero;
        } else if (eq(*integer(3), *arg_int)) {
            return log(integer(2));
        }
    }
    return make_rcp<const LogGamma>(arg);
}

RCP<const Beta> Beta::from_two_basic(const RCP<const Basic> &x,
                                     const RCP<const Basic> &y)
{
    if (x->__cmp__(*y) == -1) {
        return make_rcp<const Beta>(y, x);
    }
    return make_rcp<const Beta>(x, y);
}

bool Beta::is_canonical(const RCP<const Basic> &x, const RCP<const Basic> &y)
{
    if (x->__cmp__(*y) == -1) {
        return false;
    }
    if (is_a<Integer>(*x)
        or (is_a<Rational>(*x)
            and (get_den(rcp_static_cast<const Rational>(x)->i)) == 2)) {
        if (is_a<Integer>(*y)
            or (is_a<Rational>(*y)
                and (get_den(rcp_static_cast<const Rational>(y)->i)) == 2)) {
            return false;
        }
    }
    return true;
}

std::size_t Beta::__hash__() const
{
    std::size_t seed = BETA;
    hash_combine<Basic>(seed, *x_);
    hash_combine<Basic>(seed, *y_);
    return seed;
}

bool Beta::__eq__(const Basic &o) const
{
    if (is_a<Beta>(o)) {
        if (eq(*x_, *(static_cast<const Beta &>(o).x_))
            and eq(*y_, *(static_cast<const Beta &>(o).y_))) {
            return true;
        }
    }
    return false;
}

int Beta::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Beta>(o))
    const Beta &b = static_cast<const Beta &>(o);
    if (eq(*x_, *(b.x_))) {
        return y_->__cmp__(*(static_cast<const Beta &>(o).y_));
    }
    return x_->__cmp__(*(static_cast<const Beta &>(o).x_));
}

RCP<const Basic> Beta::rewrite_as_gamma() const
{
    return div(mul(gamma(x_), gamma(y_)), add(x_, y_));
}

RCP<const Basic> Beta::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> x = x_->subs(subs_dict);
    RCP<const Basic> y = y_->subs(subs_dict);
    if ((x == x_ and y == y_) or (x == y_ and y == x_)) {
        return rcp_from_this();
    } else {
        return beta(x, y);
    }
}

RCP<const Basic> beta(const RCP<const Basic> &x, const RCP<const Basic> &y)
{
    // Only special values are being evaluated
    if (eq(*add(x, y), *one)) {
        throw std::runtime_error("Complex Infinity not yet implemented");
    }

    if (is_a<Integer>(*x)) {
        RCP<const Integer> x_int = rcp_static_cast<const Integer>(x);
        if (x_int->is_positive()) {
            if (is_a<Integer>(*y)) {
                RCP<const Integer> y_int = rcp_static_cast<const Integer>(y);
                if (y_int->is_positive()) {
                    return div(
                        mul(gamma_positive_int(x), gamma_positive_int(y)),
                        gamma_positive_int(add(x, y)));
                } else {
                    throw std::runtime_error(
                        "Complex Infinity not yet implemented");
                }
            } else if (is_a<Rational>(*y)) {
                RCP<const Rational> y_ = rcp_static_cast<const Rational>(y);
                if (get_den(y_->i) == 2) {
                    return div(mul(gamma_positive_int(x), gamma_multiple_2(y)),
                               gamma_multiple_2(add(x, y)));
                } else {
                    return Beta::from_two_basic(x, y);
                }
            }
        } else {
            throw std::runtime_error("Complex Infinity not yet implemented");
        }
    }

    if (is_a<Integer>(*y)) {
        RCP<const Integer> y_int = rcp_static_cast<const Integer>(y);
        if (y_int->is_positive()) {
            if (is_a<Rational>(*x)) {
                RCP<const Rational> x_ = rcp_static_cast<const Rational>(x);
                if (get_den(x_->i) == 2) {
                    return div(mul(gamma_positive_int(y), gamma_multiple_2(x)),
                               gamma_multiple_2(add(x, y)));
                } else {
                    return Beta::from_two_basic(x, y);
                }
            }
        } else {
            throw std::runtime_error("Complex Infinity not yet implemented");
        }
    }

    if (is_a<const Rational>(*x)
        and get_den(rcp_static_cast<const Rational>(x)->i) == 2) {
        if (is_a<Integer>(*y)) {
            RCP<const Integer> y_int = rcp_static_cast<const Integer>(y);
            if (y_int->is_positive()) {
                return div(mul(gamma_multiple_2(x), gamma_positive_int(y)),
                           gamma_multiple_2(add(x, y)));
            } else {
                throw std::runtime_error(
                    "Complex Infinity not yet implemented");
            }
        }
        if (is_a<const Rational>(*y)
            and get_den((rcp_static_cast<const Rational>(y))->i) == 2) {
            return div(mul(gamma_multiple_2(x), gamma_multiple_2(y)),
                       gamma_positive_int(add(x, y)));
        }
    }
    return Beta::from_two_basic(x, y);
}

bool PolyGamma::is_canonical(const RCP<const Basic> &n,
                             const RCP<const Basic> &x)
{
    if (is_a_Number(*x)
        and not(rcp_static_cast<const Number>(x))->is_positive()) {
        return false;
    }
    if (eq(*n, *zero)) {
        if (eq(*x, *one)) {
            return false;
        }
        if (is_a<Rational>(*x)) {
            auto x_ = rcp_static_cast<const Rational>(x);
            auto den = get_den(x_->i);
            if (den == 2 or den == 3 or den == 4) {
                return false;
            }
        }
    }
    return true;
}

std::size_t PolyGamma::__hash__() const
{
    std::size_t seed = POLYGAMMA;
    hash_combine<Basic>(seed, *n_);
    hash_combine<Basic>(seed, *x_);
    return seed;
}

bool PolyGamma::__eq__(const Basic &o) const
{
    if (is_a<PolyGamma>(o) and eq(*x_, *(static_cast<const PolyGamma &>(o).x_))
        and eq(*n_, *(static_cast<const PolyGamma &>(o).n_)))
        return true;
    return false;
}

int PolyGamma::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<PolyGamma>(o))
    const PolyGamma &pg = static_cast<const PolyGamma &>(o);
    if (neq(*n_, *(pg.n_))) {
        return n_->__cmp__(*(pg.n_));
    } else {
        return x_->__cmp__(*(pg.x_));
    }
}

RCP<const Basic> PolyGamma::rewrite_as_zeta() const
{
    if (not is_a<Integer>(*n_)) {
        return rcp_from_this();
    }
    RCP<const Integer> n = rcp_static_cast<const Integer>(n_);
    if (not(n->is_positive())) {
        return rcp_from_this();
    }
    if ((n->as_int() & 1) == 0) {
        return neg(mul(factorial(n->as_int()), zeta(add(n_, one), x_)));
    } else {
        return mul(factorial(n->as_int()), zeta(add(n_, one), x_));
    }
}

RCP<const Basic> PolyGamma::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp_from_this());
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> n1 = n_->subs(subs_dict);
    RCP<const Basic> x1 = x_->subs(subs_dict);
    if (n1 == n_ and x1 == x_) {
        return rcp_from_this();
    } else {
        return polygamma(n1, x1);
    }
}

RCP<const Basic> polygamma(const RCP<const Basic> &n_,
                           const RCP<const Basic> &x_)
{
    // Only special values are being evaluated
    if (is_a_Number(*x_)
        and not(rcp_static_cast<const Number>(x_))->is_positive()) {
        throw std::runtime_error("Complex Infinity not yet implemented");
    }
    if (is_a<Integer>(*n_) and is_a<Integer>(*x_)) {
        auto n = static_cast<const Integer &>(*n_).as_int();
        auto x = static_cast<const Integer &>(*x_).as_int();
        if (n == 0) {
            return sub(harmonic(x - 1, 1), EulerGamma);
        } else if (n % 2 == 1) {
            return mul(factorial(n), zeta(add(n_, one), x_));
        }
    }
    if (eq(*n_, *zero)) {
        if (eq(*x_, *one)) {
            return neg(EulerGamma);
        }
        if (is_a<Rational>(*x_)) {
            RCP<const Rational> x = rcp_static_cast<const Rational>(x_);
            const auto den = get_den(x->i);
            const auto num = get_num(x->i);
            const auto r = num % den;
            RCP<const Basic> res;
            if (den == 2) {
                res = sub(mul(im2, log(i2)), EulerGamma);
            } else if (den == 3) {
                if (num == 1) {
                    res = add(neg(div(div(pi, i2), sqrt(i3))),
                              sub(div(mul(im3, log(i3)), i2), EulerGamma));
                } else {
                    res = add(div(div(pi, i2), sqrt(i3)),
                              sub(div(mul(im3, log(i3)), i2), EulerGamma));
                }
            } else if (den == 4) {
                if (num == 1) {
                    res = add(div(pi, im2), sub(mul(im3, log(i2)), EulerGamma));
                } else {
                    res = add(div(pi, i2), sub(mul(im3, log(i2)), EulerGamma));
                }
            } else {
                return make_rcp<const PolyGamma>(n_, x_);
            }
            rational_class a(0), f(r, den);
            for (unsigned long i = 0; i < (num - r) / den; ++i) {
                a += 1 / (f + i);
            }
            return add(Rational::from_mpq(a), res);
        }
    }
    return make_rcp<const PolyGamma>(n_, x_);
}

Abs::Abs(const RCP<const Basic> &arg) : arg_{arg}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool Abs::is_canonical(const RCP<const Basic> &arg) const
{
    if (is_a<Integer>(*arg) or is_a<Rational>(*arg) or is_a<Complex>(*arg))
        return false;
    if (is_a_Number(*arg)
        and not static_cast<const Number &>(*arg).is_exact()) {
        return false;
    }

    if (could_extract_minus(arg)) {
        return false;
    }

    return true;
}

std::size_t Abs::__hash__() const
{
    std::size_t seed = ABS;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Abs::__eq__(const Basic &o) const
{
    if (is_a<Abs>(o) and eq(*arg_, *(static_cast<const Abs &>(o).arg_)))
        return true;
    return false;
}

int Abs::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Abs>(o))
    return arg_->__cmp__(*(static_cast<const Abs &>(o).arg_));
}

RCP<const Basic> abs(const RCP<const Basic> &arg)
{
    if (is_a<Integer>(*arg)) {
        RCP<const Integer> arg_ = rcp_static_cast<const Integer>(arg);
        if (arg_->is_negative()) {
            return arg_->neg();
        } else {
            return arg_;
        }
    } else if (is_a<Rational>(*arg)) {
        RCP<const Rational> arg_ = rcp_static_cast<const Rational>(arg);
        if (arg_->is_negative()) {
            return arg_->neg();
        } else {
            return arg_;
        }
    } else if (is_a<Complex>(*arg)) {
        RCP<const Complex> arg_ = rcp_static_cast<const Complex>(arg);
        return sqrt(Rational::from_mpq(arg_->real_ * arg_->real_
                                       + arg_->imaginary_ * arg_->imaginary_));
    } else if (is_a_Number(*arg)
               and not static_cast<const Number &>(*arg).is_exact()) {
        return static_cast<const Number &>(*arg).get_eval().abs(*arg);
    }

    if (could_extract_minus(arg)) {
        return abs(neg(arg));
    }

    return make_rcp<const Abs>(arg);
}

Max::Max(const vec_basic &&arg) : arg_{std::move(arg)}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool Max::is_canonical(const vec_basic &arg) const
{
    if (arg.size() < 2)
        return false;

    bool non_number_exists = false;

    for (const auto &p : arg) {
        if (is_a<Complex>(*p) or is_a<Max>(*p))
            return false;
        if (not is_a_Number(*p))
            non_number_exists = true;
    }
    if (not std::is_sorted(arg.begin(), arg.end(), RCPBasicKeyLess()))
        return false;

    return non_number_exists; // all arguments cant be numbers
}

bool Max::__eq__(const Basic &o) const
{
    if (is_a<Max>(o)
        and vec_basic_eq_perm(arg_, static_cast<const Max &>(o).arg_))
        return true;
    else
        return false;
}

int Max::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Max>(o))
    const Max &s = static_cast<const Max &>(o);
    return vec_basic_compare(arg_, s.arg_);
}

std::size_t Max::__hash__() const
{
    std::size_t seed = MAX;
    for (const auto &p : arg_) {
        hash_combine<Basic>(seed, *p);
    }
    return seed;
}

RCP<const Basic> max(const vec_basic &arg)
{
    bool number_set = false;
    RCP<const Number> max_number, difference;
    set_basic new_args;

    for (const auto &p : arg) {
        if (is_a<Complex>(*p))
            throw std::runtime_error("Complex can't be passed to max!");

        if (is_a_Number(*p)) {
            if (not number_set) {
                max_number = rcp_static_cast<const Number>(p);

            } else {
                difference = rcp_static_cast<const Number>(p)->sub(*max_number);

                if (difference->is_zero() and not difference->is_exact()) {
                    if (max_number->is_exact())
                        max_number = rcp_static_cast<const Number>(p);
                } else if (difference->is_positive()) {
                    max_number = rcp_static_cast<const Number>(p);
                }
            }
            number_set = true;

        } else if (is_a<Max>(*p)) {
            for (const auto &l : rcp_static_cast<const Max>(p)->get_args()) {
                if (is_a_Number(*l)) {
                    if (not number_set) {
                        max_number = rcp_static_cast<const Number>(l);

                    } else {
                        difference = rcp_static_cast<const Number>(l)
                                         ->sub(*max_number);

                        if (difference->is_zero()
                            and not difference->is_exact()) {
                            if (max_number->is_exact())
                                max_number = rcp_static_cast<const Number>(l);
                        } else if (difference->is_positive()) {
                            max_number = rcp_static_cast<const Number>(l);
                        }
                    }
                    number_set = true;
                } else {
                    new_args.insert(l);
                }
            }
        } else {
            new_args.insert(p);
        }
    }

    if (number_set)
        new_args.insert(max_number);

    vec_basic final_args(new_args.size());
    std::copy(new_args.begin(), new_args.end(), final_args.begin());

    if (final_args.size() > 1) {
        return make_rcp<const Max>(std::move(final_args));
    } else if (final_args.size() == 1) {
        return final_args[0];
    } else {
        throw std::runtime_error("Empty vec_basic passed to max!");
    }
}

Min::Min(const vec_basic &&arg) : arg_{std::move(arg)}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool Min::is_canonical(const vec_basic &arg) const
{
    if (arg.size() < 2)
        return false;

    bool non_number_exists = false;

    for (const auto &p : arg) {
        if (is_a<Complex>(*p) or is_a<Min>(*p))
            return false;
        if (not is_a_Number(*p))
            non_number_exists = true;
    }
    if (not std::is_sorted(arg.begin(), arg.end(), RCPBasicKeyLess()))
        return false;

    return non_number_exists; // all arguments cant be numbers
}

bool Min::__eq__(const Basic &o) const
{
    if (is_a<Min>(o)
        and vec_basic_eq_perm(arg_, static_cast<const Min &>(o).arg_))
        return true;
    else
        return false;
}

int Min::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Min>(o))
    const Min &s = static_cast<const Min &>(o);
    return vec_basic_compare(arg_, s.arg_);
}

std::size_t Min::__hash__() const
{
    std::size_t seed = MIN;
    for (const auto &p : arg_) {
        hash_combine<Basic>(seed, *p);
    }
    return seed;
}

RCP<const Basic> min(const vec_basic &arg)
{
    bool number_set = false;
    RCP<const Number> min_number, difference;
    set_basic new_args;

    for (const auto &p : arg) {
        if (is_a<Complex>(*p))
            throw std::runtime_error("Complex can't be passed to min!");

        if (is_a_Number(*p)) {
            if (not number_set) {
                min_number = rcp_static_cast<const Number>(p);

            } else {
                difference = min_number->sub(*rcp_static_cast<const Number>(p));

                if (difference->is_zero() and not difference->is_exact()) {
                    if (min_number->is_exact())
                        min_number = rcp_static_cast<const Number>(p);
                } else if (difference->is_positive()) {
                    min_number = rcp_static_cast<const Number>(p);
                }
            }
            number_set = true;

        } else if (is_a<Min>(*p)) {
            for (const auto &l : rcp_static_cast<const Min>(p)->get_args()) {
                if (is_a_Number(*l)) {
                    if (not number_set) {
                        min_number = rcp_static_cast<const Number>(l);

                    } else {
                        difference = min_number->sub(
                            *rcp_static_cast<const Number>(l));

                        if (difference->is_zero()
                            and not difference->is_exact()) {
                            if (min_number->is_exact())
                                min_number = rcp_static_cast<const Number>(l);
                        } else if (difference->is_positive()) {
                            min_number = rcp_static_cast<const Number>(l);
                        }
                    }
                    number_set = true;
                } else {
                    new_args.insert(l);
                }
            }
        } else {
            new_args.insert(p);
        }
    }

    if (number_set)
        new_args.insert(min_number);

    vec_basic final_args(new_args.size());
    std::copy(new_args.begin(), new_args.end(), final_args.begin());

    if (final_args.size() > 1) {
        return make_rcp<const Min>(std::move(final_args));
    } else if (final_args.size() == 1) {
        return final_args[0];
    } else {
        throw std::runtime_error("Empty vec_basic passed to min!");
    }
}

} // SymEngine
