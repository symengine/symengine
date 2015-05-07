#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "complex.h"
#include "functions.h"
#include "constants.h"
#include "visitor.h"


namespace SymEngine {

extern RCP<const Basic> i2;
extern RCP<const Basic> i3;
extern RCP<const Basic> i5;
extern RCP<const Basic> im2;
extern RCP<const Basic> im3;
extern RCP<const Basic> im5;

RCP<const Basic> sqrt(RCP<const Basic>& arg)
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

bool get_pi_shift(const RCP<const Basic> &arg,
        const Ptr<RCP<const Integer>> &n,
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
            for (auto &p: s.dict_) {
                temp = mul(p.second, integer(12));
                if (is_a<Constant>(*p.first) &&
                    eq(rcp_static_cast<const Constant>(p.first), pi)
                    && is_a<Integer>(*temp)) {
                    check_pi = true;
                    *n = rcp_dynamic_cast<const Integer>(temp);
                }
                else {
                    *x = add( mul(p.first, p.second), *x);
                }
            }
            if (check_pi)
                return true;
            else // No term with `pi` found
                return false;
        }
        else if (size == 1) {
            // arg should be of form `a + n*pi/12`
            // where `a` is a `Number`.
            auto p = s.dict_.begin();
            RCP<const Basic> temp = mul(p->second, integer(12));
            if (is_a<Constant>(*p->first) &&
                eq(rcp_static_cast<const Constant>(p->first), pi) &&
                is_a<Integer>(*temp)) {

                *n = rcp_dynamic_cast<const Integer>(temp);
                *x = coef;
                return true;
            }
            else {
                return false;
            }
        }
        else { // Should never reach here though!
            // Dict of size < 1
            return false;
        }
    }
    else if (is_a<Mul>(*arg)) {
        // `arg` is of the form `k*pi/12`
        const Mul &s = static_cast<const Mul &>(*arg);
        RCP<const Basic> coef = s.coef_;
        coef = mul(coef, integer(12));
        auto p = s.dict_.begin();
        // dict should contain symbol `pi` only
        // and coeff should be a multiple of 12
        if (s.dict_.size() == 1 && is_a<Constant>(*p->first) &&
                eq(rcp_static_cast<const Constant>(p->first), pi) &&
                eq(rcp_static_cast<const Number>(p->second), one) &&
                is_a<Integer>(*coef)) {

            *n = rcp_dynamic_cast<const Integer>(coef);
            *x = zero;
            return true;
        }
        else {
            return false;
        }
    }
    else if (is_a<Constant>(*arg) &&
             eq(rcp_static_cast<const Constant>(arg), pi)) {
        *n = integer(12);
        *x = zero;
        return true;
    }
    else {
        return false;
    }
}

bool could_extract_minus(const RCP<const Basic> &arg)
{
    if (is_a<Mul>(*arg)) {
        const Mul &s = static_cast<const Mul &>(*arg);
        RCP<const Basic> coef = s.coef_;
        if (is_a<Integer>(*coef) &&
              rcp_static_cast<const Integer>(coef)->is_negative())
            return true;

        else if (is_a<Rational>(*coef) &&
              rcp_static_cast<const Rational>(coef)->is_negative())
            return true;
        else
            return false;
    }
    else if (is_a<Add>(*arg)) {
        const Add &s = static_cast<const Add &>(*arg);
        for (auto &p: s.dict_) {
            if (is_a<Integer>(*p.second)) {
                if (!(rcp_static_cast<const Integer>(p.second)->is_negative()))
                    return false;
            }
            else if (is_a<Rational>(*p.second)) {
                if (!(rcp_static_cast<const Rational>(p.second)->is_negative()))
                    return false;
            }
            else
                return false;
        }
        return true;
    }
    else
        return false;
}

bool handle_minus(const RCP<const Basic> &arg,
                const Ptr<RCP<const Basic>> &rarg)
{
    if (could_extract_minus(arg)) {
        *rarg = mul(minus_one, arg);
        return true;
    }
    else {
        *rarg = arg;
        return false;
    }
}

// \return true of conjugate has to be returned finally else false
bool eval(const RCP<const Basic> &arg, int period, bool odd, bool conj_odd, //input
            const Ptr<RCP<const Basic>>& rarg,int& index, int& sign) //output
{
    bool check;
    RCP<const Integer> n;
    RCP<const Basic> r;
    RCP<const Basic> ret_arg;
    check = get_pi_shift(arg, outArg(n), outArg(r));
    if (check) {
        int m = n->as_int();
        m = m % (12*period);
        sign = 1;
        if (eq(r, zero)) {
            index = m;
            *rarg = zero;
            return false;
        }
        else if ((m % (12*period)) == 0) {
            index = 0;
            bool b = handle_minus(r, outArg(ret_arg));
            *rarg = ret_arg;
            if (odd && b)
                sign = -1;
            return false;
        }
        else if ((m % 12) == 0) {
            sign = -1;
            bool b = handle_minus(r, outArg(ret_arg));
            *rarg = ret_arg;
            if (odd && b)
                sign = -1*sign;
            return false;
        }
        else if ((m % 6) == 0) {
            if (m == 6)
                sign = 1;
            else
                sign = -1;
            bool b = handle_minus(r, outArg(ret_arg));
            *rarg = ret_arg;
            if (!b && conj_odd)
                sign = -sign;
            return true;
        }
        else {
            *rarg = r;
            index = -1;
            return false;
        }
    }
    else {
        bool b = handle_minus(arg, outArg(ret_arg));
        *rarg = ret_arg;
        index = -1;
        if (odd && b)
            sign = -1;
        else
            sign = 1;
        return false;
    }
}

bool inverse_lookup(umap_basic_basic &d, const RCP<const Basic> &t,
                   const Ptr<RCP<const Basic>>& index)
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
    auto it = subs_dict.find(rcp(this));
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
        return rcp(this);
    else
        return this->create(arg);
}

Sin::Sin(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Sin::is_canonical(const RCP<const Basic> &arg)
{
    // e.g. sin(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // e.g sin(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    bool b = get_pi_shift(arg, outArg(n), outArg(r));
    if (b)
        return false;
    return true;
}

bool Sin::__eq__(const Basic &o) const
{
    if (is_a<Sin>(o) &&
        eq(get_arg(), static_cast<const Sin &>(o).get_arg()))
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
    if (eq(arg, zero)) return zero;
    RCP<const Basic> ret_arg;
    int index;
    int sign;
    bool conjugate = eval(arg, 2, 1, 0, //input
                          outArg(ret_arg), index, sign); //output

    if (conjugate) {
        // cos has to be returned
        if (sign == 1)
            return rcp(new Cos(ret_arg));
        else
            return mul(minus_one, rcp(new Cos(ret_arg)));
    }
    else {
        if (eq(ret_arg, zero)) {
            return mul(integer(sign), sin_table[index]);
        }
        else {
            if (sign == 1)
                return rcp(new Sin(ret_arg));
            else
                return mul(minus_one, rcp(new Sin(ret_arg)));
        }
    }
}

/* ---------------------------- */

Cos::Cos(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Cos::is_canonical(const RCP<const Basic> &arg)
{
    // e.g. cos(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // e.g cos(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    bool b = get_pi_shift(arg, outArg(n), outArg(r));
    if (b)
        return false;
    return true;
}

bool Cos::__eq__(const Basic &o) const
{
    if (is_a<Cos>(o) &&
        eq(get_arg(), static_cast<const Cos &>(o).get_arg()))
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
    if (eq(arg, zero)) return one;
    RCP<const Basic> ret_arg;
    int index;
    int sign;
    bool conjugate =  eval(arg, 2, 0, 1, //input
                      outArg(ret_arg), index, sign); //output

    if (conjugate) {
        // cos has to be returned
        if (sign == 1)
            return rcp(new Sin(ret_arg));
        else
            return mul(minus_one, rcp(new Sin(ret_arg)));
    }
    else {
        if (eq(ret_arg, zero)) {
            return mul(integer(sign), sin_table[(index + 6) % 24]);
        }
        else {
            if (sign == 1)
                return rcp(new Cos(ret_arg));
            else
                return mul(minus_one, rcp(new Cos(ret_arg)));
        }
    }
}

/* ---------------------------- */

Tan::Tan(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Tan::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // e.g tan(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    bool b = get_pi_shift(arg, outArg(n), outArg(r));
    if (b)
        return false;
    return true;
}

bool Tan::__eq__(const Basic &o) const
{
    if (is_a<Tan>(o) &&
        eq(get_arg(), static_cast<const Tan &>(o).get_arg()))
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
    if (eq(arg, zero)) return zero;
    RCP<const Basic> ret_arg;
    int index;
    int sign;
    bool conjugate =  eval(arg, 1, 1, 1, //input
                      outArg(ret_arg), index, sign); //output

    if (conjugate) {
        // cos has to be returned
        if (sign == 1)
            return rcp(new Cot(ret_arg));
        else
            return mul(minus_one, rcp(new Cot(ret_arg)));
    }
    else {
        if (eq(ret_arg, zero)) {
            return mul(integer(sign),
                   div(sin_table[index], sin_table[(index + 6) % 24]));
        }
        else {
            if (sign == 1)
                return rcp(new Tan(ret_arg));
            else
                return mul(minus_one, rcp(new Tan(ret_arg)));
        }
    }
}

/* ---------------------------- */

Cot::Cot(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Cot::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // e.g cot(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    bool b = get_pi_shift(arg, outArg(n), outArg(r));
    if (b)
        return false;
    return true;
}

bool Cot::__eq__(const Basic &o) const
{
    if (is_a<Cot>(o) &&
        eq(get_arg(), static_cast<const Cot &>(o).get_arg()))
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
    RCP<const Basic> ret_arg;
    int index;
    int sign;
    bool conjugate =  eval(arg, 1, 1, 1, //input
                      outArg(ret_arg), index, sign); //output

    if (conjugate) {
        // cos has to be returned
        if (sign == 1)
            return rcp(new Tan(ret_arg));
        else
            return mul(minus_one, rcp(new Tan(ret_arg)));
    }
    else {
        if (eq(ret_arg, zero)) {
            return mul(integer(sign),
                   div(sin_table[(index + 6) % 24], sin_table[index]));
        }
        else {
            if (sign == 1)
                return rcp(new Cot(ret_arg));
            else
                return mul(minus_one, rcp(new Cot(ret_arg)));
        }
    }
}

/* ---------------------------- */

Csc::Csc(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Csc::is_canonical(const RCP<const Basic> &arg)
{
    // e.g. Csc(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // Update for +inf/-inf constraints
    // e.g csc(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    bool b = get_pi_shift(arg, outArg(n), outArg(r));
    if (b)
        return false;
    return true;
}

bool Csc::__eq__(const Basic &o) const
{
    if (is_a<Csc>(o) &&
        eq(get_arg(), static_cast<const Csc &>(o).get_arg()))
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
    RCP<const Basic> ret_arg;
    int index;
    int sign;
    bool conjugate =  eval(arg, 2, 1, 0, //input
                      outArg(ret_arg), index, sign); //output

    if (conjugate) {
        // cos has to be returned
        if (sign == 1)
            return rcp(new Sec(ret_arg));
        else
            return mul(minus_one, rcp(new Sec(ret_arg)));
    }
    else {
        if (eq(ret_arg, zero)) {
            return mul(integer(sign),
                   div(one, sin_table[index]));
        }
        else {
            if (sign == 1)
                return rcp(new Csc(ret_arg));
            else
                return mul(minus_one, rcp(new Csc(ret_arg)));
        }
    }
}


/* ---------------------------- */

Sec::Sec(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Sec::is_canonical(const RCP<const Basic> &arg)
{
    // e.g. Sec(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // TODO: Update for +inf/-inf constraints
    // e.g sec(k*pi/12)
    RCP<const Integer> n;
    RCP<const Basic> r;
    bool b = get_pi_shift(arg, outArg(n), outArg(r));
    if (b)
        return false;
    return true;
}

bool Sec::__eq__(const Basic &o) const
{
    if (is_a<Sec>(o) &&
        eq(get_arg(), static_cast<const Sec &>(o).get_arg()))
        return true;
    return false;
}

int Sec::compare(const Basic &o) const
{    SYMENGINE_ASSERT(is_a<Sec>(o))
    const Sec &s = static_cast<const Sec &>(o);
    return get_arg()->__cmp__(*(s.get_arg()));
}

RCP<const Basic> sec(const RCP<const Basic> &arg)
{
    RCP<const Basic> ret_arg;
    int index;
    int sign;
    bool conjugate =  eval(arg, 2, 0, 1, //input
                      outArg(ret_arg), index, sign); //output

    if (conjugate) {
        // cos has to be returned
        if (sign == 1)
            return rcp(new Csc(ret_arg));
        else
            return mul(minus_one, rcp(new Csc(ret_arg)));
    }
    else {
        if (eq(ret_arg, zero)) {
            return mul(integer(sign),
                   div(one, sin_table[(index + 6)% 24]));
        }
        else {
            if (sign == 1)
                return rcp(new Sec(ret_arg));
            else
                return mul(minus_one, rcp(new Sec(ret_arg)));
        }
    }
}

/* ---------------------------- */
ASin::ASin(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ASin::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero) || eq(arg, one) || eq(arg, minus_one))
        return false;
    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, get_arg(), outArg(index));
    if (b)
        return false;
    else
        return true;
}

bool ASin::__eq__(const Basic &o) const
{
    if (is_a<ASin>(o) &&
        eq(get_arg(), static_cast<const ASin &>(o).get_arg()))
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
    if (eq(arg, zero)) return zero;
    else if (eq(arg, one)) return div(pi, i2);
    else if (eq(arg, minus_one)) return mul(minus_one, div(pi, i2));

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, arg, outArg(index));
    if (b) {
        return div(pi, index);
    } else {
        return rcp(new ASin(arg));
    }
}


ACos::ACos(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACos::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero) || eq(arg, one) || eq(arg, minus_one))
        return false;
    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, get_arg(), outArg(index));
    if (b)
        return false;
    else
        return true;
}

bool ACos::__eq__(const Basic &o) const
{
    if (is_a<ACos>(o) &&
        eq(get_arg(), static_cast<const ACos &>(o).get_arg()))
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
    if (eq(arg, zero)) return div(pi, i2);
    else if (eq(arg, one)) return zero;
    else if (eq(arg, minus_one)) return pi;

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, arg, outArg(index));
    if (b) {
        if (could_extract_minus(index)) {
            return add(pi, div(pi, index));
        }
        else {
            return sub(div(pi, i2), div(pi, index));
        }
    } else {
        return rcp(new ACos(arg));
    }
}

ASec::ASec(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ASec::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, one) || eq(arg, minus_one))
        return false;
    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, div(one, get_arg()), outArg(index));
    if (b)
        return false;
    else
        return true;
}

bool ASec::__eq__(const Basic &o) const
{
    if (is_a<ASec>(o) &&
        eq(get_arg(), static_cast<const ASec &>(o).get_arg()))
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
    if (eq(arg, one)) return zero;
    else if (eq(arg, minus_one)) return pi;

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, div(one, arg), outArg(index));
    if (b) {
        if (could_extract_minus(index)) {
            return add(pi, div(pi, index));
        }
        else {
            return sub(div(pi, i2), div(pi, index));
        }
    } else {
        return rcp(new ASec(arg));
    }
}

ACsc::ACsc(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACsc::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, one) || eq(arg, minus_one))
        return false;
    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, div(one, get_arg()), outArg(index));
    if (b)
        return false;
    else
        return true;
}

bool ACsc::__eq__(const Basic &o) const
{
    if (is_a<ACsc>(o) &&
        eq(get_arg(), static_cast<const ASec &>(o).get_arg()))
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
    if (eq(arg, one)) return div(pi, i2);
    else if (eq(arg, minus_one)) return div(pi, im2);

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, div(one, arg), outArg(index));
    if (b) {
        return div(pi, index);
    } else {
        return rcp(new ACsc(arg));
    }
}

ATan::ATan(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ATan::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero) || eq(arg, one) || eq(arg, minus_one))
        return false;
    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_tct, get_arg(), outArg(index));
    if (b)
        return false;
    else
        return true;
}

bool ATan::__eq__(const Basic &o) const
{
    if (is_a<ATan>(o) &&
        eq(get_arg(), static_cast<const ATan &>(o).get_arg()))
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
    if (eq(arg, zero)) return zero;
    else if (eq(arg, one)) return div(pi, mul(i2, i2));
    else if (eq(arg, minus_one)) return mul(minus_one, div(pi, mul(i2, i2)));

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_tct, arg, outArg(index));
    if (b) {
        return div(pi, index);
    } else {
        return rcp(new ATan(arg));
    }
}

ACot::ACot(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACot::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero) || eq(arg, one) || eq(arg, minus_one))
        return false;
    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_cst, get_arg(), outArg(index));
    if (b)
        return false;
    else
        return true;
}

bool ACot::__eq__(const Basic &o) const
{
    if (is_a<ACot>(o) &&
        eq(get_arg(), static_cast<const ACot &>(o).get_arg()))
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
    if (eq(arg, zero)) return div(pi, i2);
    else if (eq(arg, one)) return div(pi, mul(i2, i2));
    else if (eq(arg, minus_one)) return mul(i3, div(pi, mul(i2, i2)));

    RCP<const Basic> index;
    bool b = inverse_lookup(inverse_tct, arg, outArg(index));
    if (b) {
        if (could_extract_minus(index)) {
            return add(pi, div(pi, index));
        }
        else {
            return sub(div(pi, i2), div(pi, index));
        }
    } else {
        return rcp(new ACot(arg));
    }
}

ATan2::ATan2(const RCP<const Basic> &num, const RCP<const Basic> &den)
    : num_{num}, den_{den}
{
    SYMENGINE_ASSERT(is_canonical(num, den))
}

bool ATan2::is_canonical(const RCP<const Basic> &num,
                        const RCP<const Basic> &den)
{
    if (eq(num, zero) || eq(num, den) || eq(num, mul(minus_one, den)))
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
        if(eq(num_, s.get_num()) && eq(den_, s.get_den()))
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
    if (eq(num, zero)) {
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
    }
    else if (eq(den, zero)) {
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
        // Ideally the answer should depend on the signs of `num` and `den`
        // Currently is_positive() and is_negative() is not implemented for
        // types other than `Number`
        // Hence this will give exact answers in case when num and den are
        // numbers in SymEngine sense and when num and den are positive.
        // for the remaining cases in which we just return the value from
        // the lookup table.
        // TODO: update once is_positive() and is_negative() is implemented
        // in `Basic`
        if (is_a_Number(*den) && is_a_Number(*num)) {
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
        return rcp(new ATan2(num, den));
    }
}

/* ---------------------------- */

RCP<const Basic> Sin::diff(const RCP<const Symbol> &x) const
{
    return mul(cos(get_arg()), get_arg()->diff(x));
}

RCP<const Basic> Cos::diff(const RCP<const Symbol> &x) const
{
    return mul(mul(minus_one, sin(get_arg())), get_arg()->diff(x));
}

RCP<const Basic> Tan::diff(const RCP<const Symbol> &x) const
{
    RCP<const Integer> two = rcp(new Integer(2));
    return mul(add(one, pow(tan(get_arg()), two)), get_arg()->diff(x));
}

RCP<const Basic> Cot::diff(const RCP<const Symbol> &x) const
{
    RCP<const Integer> two = rcp(new Integer(2));
    return mul(mul(add(one, pow(cot(get_arg()), two)), minus_one), get_arg()->diff(x));
}

RCP<const Basic> Csc::diff(const RCP<const Symbol> &x) const
{
    return mul(mul(mul(cot(get_arg()), csc(get_arg())), minus_one), get_arg()->diff(x));
}

RCP<const Basic> Sec::diff(const RCP<const Symbol> &x) const
{
    return mul(mul(tan(get_arg()), sec(get_arg())), get_arg()->diff(x));
}

RCP<const Basic> ASin::diff(const RCP<const Symbol> &x) const
{
    return mul(div(one, sqrt(sub(one, pow(get_arg(), i2)))), get_arg()->diff(x));
}

RCP<const Basic> ACos::diff(const RCP<const Symbol> &x) const
{
    return mul(div(minus_one, sqrt(sub(one, pow(get_arg(), i2)))), get_arg()->diff(x));
}

RCP<const Basic> ASec::diff(const RCP<const Symbol> &x) const
{
    return mul(div(one, mul(pow(get_arg(), i2), sqrt(sub(one, div(one, pow(get_arg(), i2)))))), get_arg()->diff(x));
}

RCP<const Basic> ACsc::diff(const RCP<const Symbol> &x) const
{
    return mul(div(minus_one, mul(pow(get_arg(), i2), sqrt(sub(one, div(one, pow(get_arg(), i2)))))), get_arg()->diff(x));
}

RCP<const Basic> ATan::diff(const RCP<const Symbol> &x) const
{
    return mul(div(one, add(one, pow(get_arg(), i2))), get_arg()->diff(x));
}

RCP<const Basic> ACot::diff(const RCP<const Symbol> &x) const
{
    return mul(div(minus_one, add(one, pow(get_arg(), i2))), get_arg()->diff(x));
}

RCP<const Basic> ATan2::diff(const RCP<const Symbol> &x) const
{
    return mul(div(pow(den_, i2), add(pow(den_, i2), pow(num_, i2))),
                div(num_, den_)->diff(x));
}

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
LambertW::LambertW(const RCP<const Basic> &arg)
    : arg_{arg}
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool LambertW::is_canonical(const RCP<const Basic> &arg)
{
    if (eq(arg, zero)) return false;
    if (eq(arg, E)) return false;
    if (eq(arg, div(one, E))) return false;
    if (eq(arg, div(log(i2), im2))) return false;
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
    if (is_a<LambertW>(o) &&
        eq(arg_, static_cast<const LambertW &>(o).arg_))
        return true;
    return false;
}

int LambertW::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<LambertW>(o))
    return arg_->__cmp__(*(static_cast<const LambertW &>(o).arg_));
}

RCP<const Basic> LambertW::diff(const RCP<const Symbol> &x) const
{
    // check http://en.wikipedia.org/wiki/Lambert_W_function#Derivative
    // for the equation
    RCP<const Basic> lambertw_val = lambertw(arg_);
    return mul(div(lambertw_val, mul(arg_, add(lambertw_val, one))), arg_->diff(x));
}

RCP<const Basic> lambertw(const RCP<const Basic> &arg)
{
    if (eq(arg, zero)) return zero;
    if (eq(arg, E)) return one;
    if (eq(arg, div(one, E))) return minus_one;
    if (eq(arg, div(log(i2), im2))) return mul(minus_one, log(i2));
    return rcp(new LambertW(arg));
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

bool FunctionSymbol::is_canonical(const vec_basic &arg)
{
    return true;
}

std::size_t FunctionSymbol::__hash__() const
{
    std::size_t seed = FUNCTIONSYMBOL;
    for (auto &a : arg_)
        hash_combine<Basic>(seed, *a);
    hash_combine<std::string>(seed, name_);
    return seed;
}

bool FunctionSymbol::__eq__(const Basic &o) const
{
    if (is_a<FunctionSymbol>(o) &&
        name_ == static_cast<const FunctionSymbol &>(o).name_ &&
        vec_basic_eq(arg_, static_cast<const FunctionSymbol &>(o).arg_))
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

RCP<const Basic> FunctionSymbol::diff(const RCP<const Symbol> &x) const
{
    RCP<const Basic> diff = zero, t;
    RCP<const Basic> self = rcp(this);
    RCP<const Symbol> s;
    std::string name;
    unsigned count  = 0;
    bool found_x = false;
    for (auto &a : arg_) {
        if (eq(a, x)) {
            found_x = true;
            count++;
        } else if (count < 2 && neq(a->diff(x), zero)) {
            count++;
        }
    }
    if (count == 1 && found_x) {
        return rcp(new Derivative(self, {x}));
    }
    for (unsigned i = 0; i < arg_.size(); i++) {
        t = arg_[i]->diff(x);
        if (neq(t, zero)) {
            name = "x";
            do {
                name = "_" + name;
                s = symbol(name);
            } while (has_symbol(*self, s));
            vec_basic v = arg_;
            v[i] = s;
            map_basic_basic m;
            insert(m, v[i], arg_[i]);
            diff = add(diff, mul(t, rcp(new Subs(rcp(new Derivative(rcp(new FunctionSymbol(name_, v)), {v[i]})), m))));
        }
    }
    return diff;
}

RCP<const Basic> FunctionSymbol::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp(this));
    if (it != subs_dict.end())
        return it->second;
    vec_basic v = arg_;
    for (unsigned i = 0; i < v.size(); i++) {
        v[i] = v[i]->subs(subs_dict);
    }
    return rcp(new FunctionSymbol(name_, v));
}

RCP<const Basic> function_symbol(std::string name, const vec_basic &arg)
{
    return rcp(new FunctionSymbol(name, arg));
}

RCP<const Basic> function_symbol(std::string name, const RCP<const Basic> &arg)
{
    return rcp(new FunctionSymbol(name, arg));
}

FunctionWrapper::FunctionWrapper(void* obj, std::string name, std::string hash, const vec_basic &arg, 
    void(*dec_ref)(void *), int(*comp)(void *, void *))
    : FunctionSymbol(name, arg)
{
    obj_ = obj;
    hash_ = hash;
    dec_ref_ = dec_ref;
    comp_ = comp;
    SYMENGINE_ASSERT(is_canonical(arg_))
}

FunctionWrapper::~FunctionWrapper()
{
    (*dec_ref_)(obj_);
}

std::size_t FunctionWrapper::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<std::string>(seed, hash_);
    return seed;
}

int FunctionWrapper::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<FunctionWrapper>(o))
    const FunctionWrapper &s = static_cast<const FunctionWrapper &>(o);
    return (*comp_)(obj_, s.obj_);
}

bool FunctionWrapper::__eq__(const Basic &o) const
{
    if (is_a<FunctionWrapper>(o) &&
        (*comp_)(obj_, static_cast<const FunctionWrapper &>(o).obj_) == 0)
        return true;
    return false;
}

RCP<const Basic> FunctionWrapper::diff(const RCP<const Symbol> &x) const
{
    for (auto &a : arg_) {
        if (neq(a->diff(x), zero)) {
            return rcp(new Derivative(rcp(this), {x}));
        }
    }
    return zero;
}

/* ---------------------------- */

Derivative::Derivative(const RCP<const Basic> &arg, const vec_basic &x)
    : arg_{arg}, x_{x}
{
    SYMENGINE_ASSERT(is_canonical(arg, x))
}

bool Derivative::is_canonical(const RCP<const Basic> &arg,
        const vec_basic &x) const
{
    // Check that 'x' are Symbols:
    for(auto &a: x)
        if (!is_a<Symbol>(*a)) return false;
    if (is_a<FunctionSymbol>(*arg)) {
        RCP<const Symbol> s = rcp_static_cast<const Symbol>(x[0]);
        RCP<const FunctionSymbol> f = rcp_static_cast<const FunctionSymbol>(arg);
        bool found_s = false;
        // 's' should be one of the args of the function
        // and should not appear anywhere else.
        for (auto &a : f->get_args()) {
            if (eq(a, s)) {
                if(found_s) {
                    return false;
                } else {
                    found_s = true;
                }
            } else if (neq(a->diff(s), zero)) {
                return false;
            }
        }
        return found_s;
    } else if (is_a<Abs>(*arg)) {
        return true;
    } else if (is_a<FunctionWrapper>(*arg)) {
        return true;
    }
    return false;
}

std::size_t Derivative::__hash__() const
{
    std::size_t seed = DERIVATIVE;
    hash_combine<Basic>(seed, *arg_);
    for(size_t i=0; i < x_.size(); i++)
        hash_combine<Basic>(seed, *x_[i]);
    return seed;
}

bool Derivative::__eq__(const Basic &o) const
{
    if (is_a<Derivative>(o) &&
            eq(arg_, static_cast<const Derivative &>(o).arg_) &&
            vec_basic_eq(x_, static_cast<const Derivative &>(o).x_))
        return true;
    return false;
}

int Derivative::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Derivative>(o))
    const Derivative &s = static_cast<const Derivative &>(o);
    int cmp = arg_->__cmp__(*(s.arg_));
    if (cmp != 0) return cmp;
    cmp = vec_basic_compare(x_, s.x_);
    return cmp;
}

RCP<const Basic> Derivative::diff(const RCP<const Symbol> &x) const
{
    if (eq(arg_->diff(x), zero)) return zero;
    vec_basic t = x_;
    t.push_back(x);
    return rcp(new Derivative(arg_, t));
}

RCP<const Basic> Derivative::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Symbol> s;
    map_basic_basic m, n;
    bool subs;
    auto it = subs_dict.find(rcp(this));
    if (it != subs_dict.end())
        return it->second;
    for (auto &p: subs_dict) {
        subs = true;
        if (eq(arg_->subs({{p.first, p.second}}), arg_))
            continue;
        // If p.first and p.second are symbols and arg_ is
        // independent of p.second, p.first can be replaced
        if (is_a<Symbol>(*p.first) && is_a<Symbol>(*p.second)
                && eq(arg_->diff(rcp_static_cast<const Symbol>(p.second)), zero)) {
            insert(n, p.first, p.second);
            continue;
        }
        for (auto &d: x_) {
            if (is_a<Symbol>(*d)) {
                s = rcp_static_cast<const Symbol>(d);
                // If p.first or p.second has non zero derivates wrt to s
                // p.first cannot be replaced
                if (neq(zero, p.first->diff(s)) ||
                        neq(zero, p.second->diff(s))) {
                    subs = false;
                    break;
                }
            } else {
                return rcp(new Subs(rcp(this), subs_dict));
            }
        }
        if (subs) {
            insert(n, p.first, p.second);
        } else {
            insert(m, p.first, p.second);
        }
    }
    vec_basic sym = x_;
    for (unsigned i = 0; i < sym.size(); i++) {
        sym[i] = sym[i]->subs(n);
    }
    if (m.empty()) {
        return rcp(new Derivative(arg_->subs(n), sym));
    } else {
         return rcp(new Subs(rcp(new Derivative(arg_->subs(n), sym)), m));
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
    for (auto &p: dict_) {
        hash_combine<Basic>(seed, *p.first);
        hash_combine<Basic>(seed, *p.second);
    }
    return seed;
}

bool Subs::__eq__(const Basic &o) const
{
    if (is_a<Subs>(o) &&
            eq(arg_, static_cast<const Subs &>(o).arg_) &&
            map_basic_basic_eq(dict_, static_cast<const Subs &>(o).dict_))
        return true;
    return false;
}

int Subs::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Subs>(o))
    const Subs &s = static_cast<const Subs &>(o);
    int cmp = arg_->__cmp__(*(arg_));
    if (cmp != 0) return cmp;
    cmp = map_basic_basic_compare(dict_, s.dict_);
    return cmp;
}

vec_basic Subs::get_variables() const {
    vec_basic v;
    for (auto &p: dict_) {
        v.push_back(p.first);
    }
    return v;
}

vec_basic Subs::get_point() const {
    vec_basic v;
    for (auto &p: dict_) {
        v.push_back(p.second);
    }
    return v;
}

vec_basic Subs::get_args() const {
    vec_basic v = {arg_};
    for (auto &p: dict_) {
        v.push_back(p.first);
    }
    for (auto &p: dict_) {
        v.push_back(p.second);
    }
    return v;
}

RCP<const Basic> Subs::diff(const RCP<const Symbol> &x) const
{
    RCP<const Basic> diff = zero, t;
    if (dict_.count(x) == 0) {
        diff = arg_->diff(x)->subs(dict_);
    }
    for (auto &p: dict_) {
        t = p.second->diff(x);
        if (neq(t, zero)) {
            if (is_a<Symbol>(*p.first)) {
                diff = add(diff, mul(t, arg_->diff(rcp_static_cast<const Symbol>(p.first))->subs(dict_)));
            } else {
                return rcp(new Derivative(rcp(this), {x}));
            }
        }
    }
    return diff;
}

RCP<const Basic> Subs::subs(const map_basic_basic &subs_dict) const
{
    map_basic_basic m, n;
    for (auto &p: subs_dict) {
        bool found = false;
        for (auto &s: dict_) {
            if (neq(s.first->subs({{p.first, p.second}}), s.first)) {
                found = true;
                break;
            }
        }
        // If p.first is not replaced in arg_ by dict_,
        // store p.first in n to replace in arg_
        if (!found) {
            insert(n, p.first, p.second);
        }
    }
    for (auto &s: dict_) {
        insert(m, s.first, s.second->subs(subs_dict));
    }
    return rcp(new Subs(arg_->subs(n), m));
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

RCP<const Basic> HyperbolicFunction::subs(const map_basic_basic &subs_dict) const
{
    auto it = subs_dict.find(rcp(this));
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
        return rcp(this);
    else
        return this->create(arg);
}

Sinh::Sinh(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Sinh::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero))
        return false;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return false;
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Sinh::__eq__(const Basic &o) const
{
    if (is_a<Sinh>(o) &&
        eq(get_arg(), static_cast<const Sinh &>(o).get_arg()))
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
    if (eq(arg, zero)) return zero;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return mul(minus_one, sinh(mul(minus_one, arg)));
    }
    if (could_extract_minus(arg)) {
        return mul(minus_one, sinh(mul(minus_one, arg)));
    }
    return rcp(new Sinh(arg));
}

RCP<const Basic>  Sinh::expand_as_exp() const
{
    return div(sub(exp(get_arg()), exp(mul(get_arg(), minus_one))), i2);
}

RCP<const Basic> Sinh::diff(const RCP<const Symbol> &x) const
{
    return mul(cosh(get_arg()), get_arg()->diff(x));
}

Cosh::Cosh(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Cosh::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero))
        return false;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return false;
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Cosh::__eq__(const Basic &o) const
{
    if (is_a<Cosh>(o) &&
        eq(get_arg(), static_cast<const Cosh &>(o).get_arg()))
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
    if (eq(arg, zero)) return one;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return cosh(mul(minus_one, arg));
    }
    if (could_extract_minus(arg)) {
        return cosh(mul(minus_one, arg));
    }
    return rcp(new Cosh(arg));
}

RCP<const Basic>  Cosh::expand_as_exp() const
{
    return div(add(exp(get_arg()), exp(mul(get_arg(), minus_one))), i2);
}

RCP<const Basic> Cosh::diff(const RCP<const Symbol> &x) const
{
    return mul(sinh(get_arg()), get_arg()->diff(x));
}

Tanh::Tanh(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Tanh::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero))
        return false;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return false;
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Tanh::__eq__(const Basic &o) const
{
    if (is_a<Tanh>(o) &&
        eq(get_arg(), static_cast<const Tanh &>(o).get_arg()))
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
    if (eq(arg, zero)) return zero;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return mul(minus_one, tanh(mul(minus_one, arg)));
    }
    if (could_extract_minus(arg)) {
        return mul(minus_one, tanh(mul(minus_one, arg)));
    }
    return rcp(new Tanh(arg));
}

RCP<const Basic>  Tanh::expand_as_exp() const
{
    RCP<const Basic> pos_exp = exp(get_arg());
    RCP<const Basic> neg_exp = exp(mul(minus_one, get_arg()));
    return div(sub(pos_exp, neg_exp), add(pos_exp, neg_exp));
}

RCP<const Basic> Tanh::diff(const RCP<const Symbol> &x) const
{
    return mul(sub(one, pow(tanh(get_arg()), i2)), get_arg()->diff(x));
}

Coth::Coth(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Coth::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero))
        return false;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return false;
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool Coth::__eq__(const Basic &o) const
{
    if (is_a<Coth>(o) &&
        eq(get_arg(), static_cast<const Coth &>(o).get_arg()))
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
    if (eq(arg, zero)) {
        // Answer is infinity. Yet to be implemented in SymEngine
        throw std::runtime_error("Not implemented.");
    }
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return mul(minus_one, coth(mul(minus_one, arg)));
    }
    if (could_extract_minus(arg)) {
        return mul(minus_one, coth(mul(minus_one, arg)));
    }
    return rcp(new Coth(arg));
}

RCP<const Basic>  Coth::expand_as_exp() const
{
    RCP<const Basic> pos_exp = exp(get_arg());
    RCP<const Basic> neg_exp = exp(mul(minus_one, get_arg()));
    return div(add(pos_exp, neg_exp), sub(pos_exp, neg_exp));
}

RCP<const Basic> Coth::diff(const RCP<const Symbol> &x) const
{
    return mul(div(minus_one, pow(sinh(get_arg()), i2)), get_arg()->diff(x));
}

ASinh::ASinh(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ASinh::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero) || eq(arg, one) || eq(arg, minus_one))
        return false;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return false;
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool ASinh::__eq__(const Basic &o) const
{
    if (is_a<ASinh>(o) &&
        eq(get_arg(), static_cast<const ASinh &>(o).get_arg()))
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
    if (eq(arg, zero)) return zero;
    if (eq(arg, one)) return log(add(one, sq2));
    if (eq(arg, minus_one)) return log(sub(sq2, one));
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return mul(minus_one, asinh(mul(minus_one, arg)));
    }
    if (could_extract_minus(arg)) {
        return mul(minus_one, asinh(mul(minus_one, arg)));
    }
    return rcp(new ASinh(arg));
}

RCP<const Basic> ASinh::diff(const RCP<const Symbol> &x) const
{
    return mul(div(one, sqrt(add(pow(get_arg(), i2), one))), get_arg()->diff(x));
}

ACosh::ACosh(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACosh::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    // TODO: Lookup into a cst table once complex is implemented
    if (eq(arg, one))
        return false;
    return true;
}

bool ACosh::__eq__(const Basic &o) const
{
    if (is_a<ACosh>(o) &&
        eq(get_arg(), static_cast<const ACosh &>(o).get_arg()))
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
    if (eq(arg, one)) return zero;
    return rcp(new ACosh(arg));
}

RCP<const Basic> ACosh::diff(const RCP<const Symbol> &x) const
{
    return mul(div(one, sqrt(sub(pow(get_arg(), i2), one))), get_arg()->diff(x));
}

ATanh::ATanh(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ATanh::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero))
        return false;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return false;
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool ATanh::__eq__(const Basic &o) const
{
    if (is_a<ATanh>(o) &&
        eq(get_arg(), static_cast<const ATanh &>(o).get_arg()))
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
    if (eq(arg, zero)) return zero;
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return mul(minus_one, atanh(mul(minus_one, arg)));
    }
    if (could_extract_minus(arg)) {
        return mul(minus_one, atanh(mul(minus_one, arg)));
    }
    return rcp(new ATanh(arg));
}

RCP<const Basic> ATanh::diff(const RCP<const Symbol> &x) const
{
    return mul(div(one, sub(one, pow(get_arg(), i2))), get_arg()->diff(x));
}

ACoth::ACoth(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ACoth::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return false;
    }
    if (could_extract_minus(arg))
        return false;
    return true;
}

bool ACoth::__eq__(const Basic &o) const
{
    if (is_a<ACoth>(o) &&
        eq(get_arg(), static_cast<const ACoth &>(o).get_arg()))
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
    if (is_a_Number(*arg) &&
        rcp_static_cast<const Number>(arg)->is_negative()) {
        return mul(minus_one, acoth(mul(minus_one, arg)));
    }
    if (could_extract_minus(arg)) {
        return mul(minus_one, acoth(mul(minus_one, arg)));
    }
    return rcp(new ACoth(arg));
}

RCP<const Basic> ACoth::diff(const RCP<const Symbol> &x) const
{
    return mul(div(one, sub(one, pow(get_arg(), i2))), get_arg()->diff(x));
}

ASech::ASech(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool ASech::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    // TODO: Lookup into a cst table once complex is implemented
    if (eq(arg, one))
        return false;
    return true;
}

bool ASech::__eq__(const Basic &o) const
{
    if (is_a<ASech>(o) &&
        eq(get_arg(), static_cast<const ASech &>(o).get_arg()))
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
    if (eq(arg, one)) return zero;
    return rcp(new ASech(arg));
}

RCP<const Basic> ASech::diff(const RCP<const Symbol> &x) const
{
    return mul(div(minus_one, mul(sqrt(sub(one, pow(get_arg(), i2))), get_arg())), get_arg()->diff(x));
}

RCP<const Basic> Sinh::create(const RCP<const Basic> &arg) const
{
    return sinh(arg);
}

RCP<const Basic> Cosh::create(const RCP<const Basic> &arg) const
{
    return cosh(arg);
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

KroneckerDelta::KroneckerDelta(const RCP<const Basic> &i, const RCP<const Basic> &j)
    :i_{i}, j_{j}
{
    SYMENGINE_ASSERT(is_canonical(i_, j_))
}

bool KroneckerDelta::is_canonical(const RCP<const Basic> &i, const RCP<const Basic> &j)
{
    RCP<const Basic> diff = expand(sub(i, j));
    if (eq(diff, zero)) {
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
    if (is_a<KroneckerDelta>(o) &&
        eq(i_, static_cast<const KroneckerDelta &>(o).i_) &&
        eq(j_, static_cast<const KroneckerDelta &>(o).j_))
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

RCP<const Basic> kronecker_delta(const RCP<const Basic> &i, const RCP<const Basic> &j)
{
    // Expand is needed to simplify things like `i-(i+1)` to `-1`
    RCP<const Basic> diff = expand(sub(i, j));
    if (eq(diff, zero)) {
        return one;
    } else if (is_a_Number(*diff)) {
        return zero;
    } else {
        // SymPy uses default key sorting to return in order
        return rcp(new KroneckerDelta(i, j));
    }
}

bool has_dup(const vec_basic &arg)
{
    map_basic_basic d;
    auto it = d.end();
    for (auto &p: arg) {
        it = d.find(p);
        if (it == d.end()) {
            insert(d, p, one);
        } else {
            return true;
        }
    }
    return false;
}

LeviCivita::LeviCivita(const vec_basic&& arg)
    :arg_{std::move(arg)}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool LeviCivita::is_canonical(const vec_basic &arg)
{
    bool are_int = true;
    for (auto &p: arg) {
        if (!(is_a_Number(*p))) {
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
    if (is_a<LeviCivita>(o) &&
        vec_basic_eq(arg_, static_cast<const LeviCivita &>(o).arg_))
        return true;
    else
        return false;
}

int LeviCivita::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<LeviCivita>(o))
    const LeviCivita &s = static_cast<const LeviCivita &>(o);
    // # of elements
    if (arg_.size() != s.arg_.size())
        return (arg_.size() < s.arg_.size()) ? -1 : 1;
    return vec_basic_compare(arg_, s.arg_);
}

std::size_t LeviCivita::__hash__() const
{
    std::size_t seed = LEVICIVITA;
    for (auto &p: arg_) {
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
    for (auto &p: arg) {
        if (!(is_a_Number(*p))) {
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
        return rcp(new LeviCivita(std::move(arg)));
    }
}

Zeta::Zeta(const RCP<const Basic> &s, const RCP<const Basic> &a)
    : s_{s}, a_{a}
{
    SYMENGINE_ASSERT(is_canonical(s_, a_))
}

Zeta::Zeta(const RCP<const Basic> &s)
    : s_{s}, a_{one}
{
    SYMENGINE_ASSERT(is_canonical(s_, a_))
}

bool Zeta::is_canonical(const RCP<const Basic> &s, const RCP<const Basic> &a)
{
    if (eq(s, zero)) return false;
    if (eq(s, one)) return false;
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
    if (is_a<Zeta>(o) &&
        eq(s_, static_cast<const Zeta &>(o).s_) &&
        eq(a_, static_cast<const Zeta &>(o).a_))
        return true;
    return false;
}

int Zeta::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Zeta>(o))
    return s_->__cmp__(*(static_cast<const Zeta &>(o).s_));
}

RCP<const Basic> Zeta::diff(const RCP<const Symbol> &x) const
{
    // TODO: check if it is differentiated wrt s
    return mul(mul(mul(minus_one, s_), zeta(add(s_, one), a_)), a_->diff(x));
}

RCP<const Basic> zeta(const RCP<const Basic> &s, const RCP<const Basic> &a)
{
    if (is_a_Number(*s)) {
        if (rcp_static_cast<const Number>(s)->is_zero()) {
            if (is_a_Number(*a) &&
                rcp_static_cast<const Number>(a)->is_negative()) {
                return sub(div(minus_one, i2), a);
            } else {
                return sub(div(one, i2), a);
            }
        } else if (rcp_static_cast<const Number>(s)->is_one()) {
            throw std::runtime_error("Complex infinity is not yet implemented");
        } else if (is_a<Integer>(*s) && is_a<Integer>(*a)) {
            // Implement Harmonic and simplify this
            return rcp(new Zeta(s, a));
        }
    }
    return rcp(new Zeta(s, a));
}

RCP<const Basic> zeta(const RCP<const Basic> &s)
{
    return zeta(s, one);
}

Dirichlet_eta::Dirichlet_eta(const RCP<const Basic> &s)
    : s_{s}
{
    SYMENGINE_ASSERT(is_canonical(s_))
}

bool Dirichlet_eta::is_canonical(const RCP<const Basic> &s)
{
    if (eq(s, one)) return false;
    if (!(is_a<Zeta>(*zeta(s)))) return false;
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
    if (is_a<Dirichlet_eta>(o) &&
        eq(s_, static_cast<const Dirichlet_eta &>(o).s_))
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
    if (is_a_Number(*s) &&
        rcp_static_cast<const Number>(s)->is_one()) {
            return log(i2);
    }
    RCP<const Basic> z = zeta(s);
    if (is_a<Zeta>(*z)) {
        return rcp(new Dirichlet_eta(s));
    } else {
        return mul(sub(one, pow(i2, sub(one, s))), z);
    }
}

Gamma::Gamma(const RCP<const Basic> &arg)
    : arg_{arg}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool Gamma::is_canonical(const RCP<const Basic> &arg)
{
    if (is_a<Integer>(*arg)) return false;
    if (is_a<Rational>(*arg) &&
        (rcp_static_cast<const Rational>(arg)->i.get_den()) == 2) {
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
    if (is_a<Gamma>(o) &&
        eq(arg_, static_cast<const Gamma &>(o).arg_))
        return true;
    return false;
}

int Gamma::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Gamma>(o))
    return arg_->__cmp__(*(static_cast<const Gamma &>(o).arg_));
}

RCP<const Basic> gamma(const RCP<const Basic> &arg)
{
    if (is_a<Integer>(*arg)) {
        RCP<const Integer> arg_ = rcp_static_cast<const Integer>(arg);
        if (arg_->is_positive()) {
            return factorial((arg_->subint(*one))->as_int());
        } else {
            throw std::runtime_error("Complex Infinity not yet implemented");
        }
    } else if (is_a<Rational>(*arg)) {
        RCP<const Rational> arg_ = rcp_static_cast<const Rational>(arg);
        if ((arg_->i.get_den()) == 2) {
            RCP<const Integer> n, k;
            RCP<const Number> coeff;
            n = quotient_f(*(integer(abs(arg_->i.get_num()))), *(integer(arg_->i.get_den())));
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
            for (int i = 3; i < 2*k->as_int(); i = i + 2)
            {
                j = j * i;
            }
            coeff = mulnum(coeff, integer(j));
            if (arg_->is_positive()) {
                return div(mul(coeff, sqrt(pi)), pow(i2, n));
            } else {
                return div(mul(pow(i2, n), sqrt(pi)), coeff);
            }
        } else {
            return rcp(new Gamma(arg));
        }
    }
    return rcp(new Gamma(arg));
}

LowerGamma::LowerGamma(const RCP<const Basic> &s, const RCP<const Basic> &x)
    : s_{s}, x_{x}
{
    SYMENGINE_ASSERT(is_canonical(s_, x_))
}

bool LowerGamma::is_canonical(const RCP<const Basic> &s, const RCP<const Basic> &x)
{
    // Only special values are evaluated
    if (eq(s, one)) return false;
    if (is_a<Integer>(*s) &&
        rcp_static_cast<const Integer>(s)->i > 1)
        return false;
    if (is_a<Integer>(*mul(i2, s))) return false;
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
    if (is_a<LowerGamma>(o) &&
        eq(s_, static_cast<const LowerGamma &>(o).s_) &&
        eq(x_, static_cast<const LowerGamma &>(o).x_))
        return true;
    return false;
}

int LowerGamma::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<LowerGamma>(o))
    const LowerGamma &lg = static_cast<const LowerGamma &>(o);
    if (neq(s_, lg.s_)) {
        return s_->__cmp__(*(static_cast<const LowerGamma &>(o).s_));
    }
    else {
        return x_->__cmp__(*(static_cast<const LowerGamma &>(o).x_));
    }
}

RCP<const Basic> lowergamma(const RCP<const Basic> &s, const RCP<const Basic> &x)
{
    // Only special values are being evaluated
    if (is_a<Integer>(*s)) {
        RCP<const Integer> s_int = rcp_static_cast<const Integer>(s);
        if (s_int->is_one()) {
            return sub(one, exp(mul(minus_one, x)));
        } else if (s_int->i > 1) {
            s_int = s_int->subint(*one);
            return sub(mul(s_int, lowergamma(s_int, x)), mul(pow(x, s_int), exp(mul(minus_one, x))));
        } else {
            return rcp(new LowerGamma(s, x));
        }
    } else if (is_a<Integer>(*(mul(i2, s)))) {
        // TODO: Implement `erf`. Currently the recursive expansion has no base case
        // when s is of form n/2 n is Integer
        RCP<const Number> s_num = rcp_static_cast<const Number>(s);
        s_num = subnum(s_num, one);
        if (s_num->is_positive()) {
            return sub(mul(s_num, lowergamma(s_num, x)), mul(pow(x, s_num), exp(mul(minus_one, x))));
        } else {
            return add(lowergamma(add(s, one), x), mul(pow(x, s), div(exp(mul(minus_one, x)), s)));
        }
    }
    return rcp(new LowerGamma(s, x));
}

UpperGamma::UpperGamma(const RCP<const Basic> &s, const RCP<const Basic> &x)
    : s_{s}, x_{x}
{
    SYMENGINE_ASSERT(is_canonical(s_, x_))
}

bool UpperGamma::is_canonical(const RCP<const Basic> &s, const RCP<const Basic> &x)
{
    // Only special values are evaluated
    if (eq(s, one)) return false;
    if (is_a<Integer>(*s) &&
        rcp_static_cast<const Integer>(s)->i > 1)
        return false;
    if (is_a<Integer>(*mul(i2, s))) return false;
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
    if (is_a<UpperGamma>(o) &&
        eq(s_, static_cast<const UpperGamma &>(o).s_) &&
        eq(x_, static_cast<const UpperGamma &>(o).x_))
        return true;
    return false;
}

int UpperGamma::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<UpperGamma>(o))
    const UpperGamma &ug = static_cast<const UpperGamma &>(o);
    if (neq(s_, ug.s_)) {
        return s_->__cmp__(*(static_cast<const UpperGamma &>(o).s_));
    }
    else {
        return x_->__cmp__(*(static_cast<const UpperGamma &>(o).x_));
    }
}

RCP<const Basic> uppergamma(const RCP<const Basic> &s, const RCP<const Basic> &x)
{
    // Only special values are being evaluated
    if (is_a<Integer>(*s)) {
        RCP<const Integer> s_int = rcp_static_cast<const Integer>(s);
        if (s_int->is_one()) {
            return exp(mul(minus_one, x));
        } else if (s_int->i > 1) {
            s_int = s_int->subint(*one);
            return add(mul(s_int, uppergamma(s_int, x)), mul(pow(x, s_int), exp(mul(minus_one, x))));
        } else {
            // TODO: implement unpolarfy to handle this case
            return rcp(new LowerGamma(s, x));
        }
    } else if (is_a<Integer>(*(mul(i2, s)))) {
        // TODO: Implement `erf`. Currently the recursive expansion has no base case
        // when s is of form n/2 n is Integer
        RCP<const Number> s_num = rcp_static_cast<const Number>(s);
        s_num = subnum(s_num, one);
        if (s_num->is_positive()) {
            return add(mul(s_num, uppergamma(s_num, x)), mul(pow(x, s_num), exp(mul(minus_one, x))));
        } else {
            return sub(uppergamma(add(s, one), x), mul(pow(x, s), div(exp(mul(minus_one, x)), s)));
        }
    }
    return rcp(new UpperGamma(s, x));
}


Abs::Abs(const RCP<const Basic> &arg)
    : arg_{arg}
{
    SYMENGINE_ASSERT(is_canonical(arg_))
}

bool Abs::is_canonical(const RCP<const Basic> &arg)
{
    if (is_a<Integer>(*arg) || is_a<Rational>(*arg)) return false;
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
    if (is_a<Abs>(o) &&
        eq(arg_, static_cast<const Abs &>(o).arg_))
        return true;
    return false;
}

int Abs::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Abs>(o))
    return arg_->__cmp__(*(static_cast<const Abs &>(o).arg_));
}

RCP<const Basic> Abs::diff(const RCP<const Symbol> &x) const
{
    if (eq(arg_->diff(x), zero))
        return zero;
    else
        return rcp(new Derivative(rcp(this), {x}));
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
            // TODO: make this faster by implementing Rational::neg()
            return neg(arg_);
        } else {
            return arg_;
        }
    }
    return rcp(new Abs(arg));
}

} // SymEngine
