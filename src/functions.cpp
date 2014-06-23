#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "rational.h"
#include "functions.h"


namespace CSymPy {

static RCP<const Basic> i2 = rcp(new Integer(2));
static RCP<const Basic> i3 = rcp(new Integer(3));
static RCP<const Basic> i5 = rcp(new Integer(5));
static RCP<const Basic> im2 = rcp(new Integer(-2));
static RCP<const Basic> im3 = rcp(new Integer(-3));
static RCP<const Basic> im5 = rcp(new Integer(-5));

RCP<const Basic> sqrt(RCP<const Basic>& arg)
{
    return pow(arg, div(one, i2));
}

static RCP<const Basic> sq3 = sqrt(i3);
static RCP<const Basic> sq2 = sqrt(i2);
static RCP<const Basic> sq5 = sqrt(i5);

static RCP<const Basic> C0 = div(sub(sq3, one), mul(i2, sq2));
static RCP<const Basic> C1 = div(one, i2);
static RCP<const Basic> C2 = div(sq2, i2);
static RCP<const Basic> C3 = div(sq3, i2);
static RCP<const Basic> C4 = div(add(sq3, one), mul(i2, sq2));
static RCP<const Basic> C5 = div(sqrt(sub(i5, sqrt(i5))), integer(8));
static RCP<const Basic> C6 = div(sub(sqrt(i5), one), integer(4));

static RCP<const Basic> mC0 = mul(minus_one, C0);
static RCP<const Basic> mC1 = mul(minus_one, C1);
static RCP<const Basic> mC2 = mul(minus_one, C2);
static RCP<const Basic> mC3 = mul(minus_one, C3);
static RCP<const Basic> mC4 = mul(minus_one, C4);
static RCP<const Basic> mC5 = mul(minus_one, C5);
static RCP<const Basic> mC6 = mul(minus_one, C6);

// sin_table[n] represents the value of sin(2*pi*n/24) for n = 0..23
static RCP<const Basic> sin_table[] = {
        zero, C0, C1, C2, C3, C4, one, C4, C3, C2, C1, C0,
        zero, mC0, mC1, mC2, mC3, mC4, minus_one, mC4, mC3, mC2, mC1, mC0
    };

static umap_basic_basic inverse_cst = {
    {C3, i3},
    {mC3, im3},
    {C2, mul(i2, i2)},
    {mC2, mul(im2, i2)},
    {C4, integer(12)},
    {mC4, integer(-12)},
    {C5, i5},
    {mC5, im5},
    {C6, integer(10)},
    {mC6, integer(-10)},
    {div(one, i2), integer(6)},
    {div(minus_one, i2), integer(-6)},
};

static umap_basic_basic inverse_tct = {
    {div(one, sq3), mul(i2, i3)},
    {div(minus_one, sq3), mul(im2, i3)},
    {sq3, i3},
    {mul(minus_one, sq3), im3},
    {add(one, sq2), div(pow(i2, i3), i3)},
    {mul(minus_one, add(one, sq2)), div(pow(i2, i3), im3)},
    {sub(sq2, one), pow(i2, i3)},
    {sub(one, sq2), pow(im2, i3)},
    {sub(i2, sq3), mul(mul(i2,i2), i3)},
    {sub(sq3, i2), mul(mul(im2,i2), i3)},
    {sqrt(add(i5, mul(i2, sqrt(i5)))), div(i5, i2)},
    {mul(minus_one, sqrt(add(i5, mul(i2, sqrt(i5))))), div(im5, i2)},
    {one, pow(i2, i2)},
    {minus_one, mul(minus_one, pow(i2, i2))},
};

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
                if (is_a<Symbol>(*p.first) &&
                    eq(rcp_static_cast<const Symbol>(p.first), pi)
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
            if (is_a<Symbol>(*p->first) &&
                eq(rcp_static_cast<const Symbol>(p->first), pi) &&
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
        if (s.dict_.size() == 1 && is_a<Symbol>(*p->first) &&
                eq(rcp_static_cast<const Symbol>(p->first), pi) &&
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
    else if (is_a<Symbol>(*arg) &&
             eq(rcp_static_cast<const Symbol>(arg), pi)) {
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
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

Sin::Sin(const RCP<const Basic> &arg)
    : TrigFunction(arg)
{
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Sin>(o))
    const Sin &s = static_cast<const Sin &>(o);
    return get_arg()->__cmp__(s);
}

std::string Sin::__str__() const
{
    std::ostringstream o;
    o << "sin(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Cos>(o))
    const Cos &s = static_cast<const Cos &>(o);
    return get_arg()->__cmp__(s);
}

std::string Cos::__str__() const
{
    std::ostringstream o;
    o << "cos(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Tan>(o))
    const Tan &s = static_cast<const Tan &>(o);
    return get_arg()->__cmp__(s);
}


std::string Tan::__str__() const
{
    std::ostringstream o;
    o << "tan(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Cot>(o))
    const Cot &s = static_cast<const Cot &>(o);
    return get_arg()->__cmp__(s);
}


std::string Cot::__str__() const
{
    std::ostringstream o;
    o << "cot(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Csc>(o))
    const Csc &s = static_cast<const Csc &>(o);
    return get_arg()->__cmp__(s);
}

std::string Csc::__str__() const
{
    std::ostringstream o;
    o << "csc(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
{    CSYMPY_ASSERT(is_a<Sec>(o))
    const Sec &s = static_cast<const Sec &>(o);
    return get_arg()->__cmp__(s);
}

std::string Sec::__str__() const
{
    std::ostringstream o;
    o << "sec(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<ASin>(o))
    const ASin &s = static_cast<const ASin &>(o);
    return get_arg()->__cmp__(s);
}


std::string ASin::__str__() const
{
    std::ostringstream o;
    o << "asin(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<ACos>(o))
    const ACos &s = static_cast<const ACos &>(o);
    return get_arg()->__cmp__(s);
}


std::string ACos::__str__() const
{
    std::ostringstream o;
    o << "acos(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<ASec>(o))
    const ASec &s = static_cast<const ASec &>(o);
    return get_arg()->__cmp__(s);
}


std::string ASec::__str__() const
{
    std::ostringstream o;
    o << "asec(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<ACsc>(o))
    const ACsc &s = static_cast<const ACsc &>(o);
    return get_arg()->__cmp__(s);
}


std::string ACsc::__str__() const
{
    std::ostringstream o;
    o << "acsc(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<ATan>(o))
    const ATan &s = static_cast<const ATan &>(o);
    return get_arg()->__cmp__(s);
}


std::string ATan::__str__() const
{
    std::ostringstream o;
    o << "atan(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<ACot>(o))
    const ACot &s = static_cast<const ACot &>(o);
    return get_arg()->__cmp__(s);
}


std::string ACot::__str__() const
{
    std::ostringstream o;
    o << "acot(" << *get_arg() << ")";
    return o.str();
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
    CSYMPY_ASSERT(is_canonical(num, den))
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
    CSYMPY_ASSERT(is_a<ATan2>(o))
    const ATan2 &s = static_cast<const ATan2 &>(o);
    return div(num_, den_)->__cmp__(s);
}


std::string ATan2::__str__() const
{
    std::ostringstream o;
    o << "atan2(" << *num_ << "/" << *den_ << ")";
    return o.str();
}

std::size_t ATan2::__hash__() const
{
    std::size_t seed = 0;
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
        // numbers in CSymPy sense and when num and den are positive.
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

RCP<const Basic> Sin::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Sin> self = rcp_const_cast<Sin>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return sin(arg);
}

RCP<const Basic> Cos::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Cos> self = rcp_const_cast<Cos>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return cos(arg);
}

RCP<const Basic> Tan::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Tan> self = rcp_const_cast<Tan>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return tan(arg);
}

RCP<const Basic> Cot::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Cot> self = rcp_const_cast<Cot>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return cot(arg);
}

RCP<const Basic> Csc::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Csc> self = rcp_const_cast<Csc>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return csc(arg);
}

RCP<const Basic> Sec::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Sec> self = rcp_const_cast<Sec>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return sec(arg);
}

RCP<const Basic> ASin::subs(const map_basic_basic &subs_dict) const
{
    RCP<const ASin> self = rcp_const_cast<ASin>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return asin(arg);
}

RCP<const Basic> ACos::subs(const map_basic_basic &subs_dict) const
{
    RCP<const ACos> self = rcp_const_cast<ACos>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return acos(arg);
}

RCP<const Basic> ASec::subs(const map_basic_basic &subs_dict) const
{
    RCP<const ASec> self = rcp_const_cast<ASec>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return asec(arg);
}

RCP<const Basic> ACsc::subs(const map_basic_basic &subs_dict) const
{
    RCP<const ACsc> self = rcp_const_cast<ACsc>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return acsc(arg);
}

RCP<const Basic> ATan::subs(const map_basic_basic &subs_dict) const
{
    RCP<const ATan> self = rcp_const_cast<ATan>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return atan(arg);
}

RCP<const Basic> ACot::subs(const map_basic_basic &subs_dict) const
{
    RCP<const ACot> self = rcp_const_cast<ACot>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return acot(arg);
}
/* ---------------------------- */
LambertW::LambertW(const RCP<const Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
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
    std::size_t seed = 0;
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
    CSYMPY_ASSERT(is_a<LambertW>(o))
    return arg_->__cmp__(*(static_cast<const LambertW &>(o).arg_));
}


std::string LambertW::__str__() const
{
    std::ostringstream o;
    o<< "lambertw(" << *arg_ << ")";
    return o.str();
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
    : name_{name}, arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool FunctionSymbol::is_canonical(const RCP<const Basic> &arg)
{
    return true;
}

std::size_t FunctionSymbol::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    hash_combine<std::string>(seed, name_);
    return seed;
}

bool FunctionSymbol::__eq__(const Basic &o) const
{
    if (is_a<FunctionSymbol>(o) &&
        name_ == static_cast<const FunctionSymbol &>(o).name_ &&
        eq(arg_, static_cast<const FunctionSymbol &>(o).arg_))
        return true;
    return false;
}

int FunctionSymbol::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<FunctionSymbol>(o))
    const FunctionSymbol &s = static_cast<const FunctionSymbol &>(o);
    if (name_ == s.name_)
        return arg_->__cmp__(*(s.arg_));
    else
        return name_ < s.name_ ? -1 : 1;
}


std::string FunctionSymbol::__str__() const
{
    std::ostringstream o;
    o << name_ << "(" << *arg_ << ")";
    return o.str();
}

RCP<const Basic> FunctionSymbol::diff(const RCP<const Symbol> &x) const
{
    if (eq(arg_->diff(x), zero))
        return zero;
    else
        return rcp(new Derivative(rcp(this), {x}));
}

RCP<const Basic> function_symbol(std::string name, const RCP<const Basic> &arg)
{
    return rcp(new FunctionSymbol(name, arg));
}

/* ---------------------------- */

Derivative::Derivative(const RCP<const Basic> &arg, const vec_basic &x)
    : arg_{arg}, x_{x}
{
    CSYMPY_ASSERT(is_canonical(arg, x))
}

bool Derivative::is_canonical(const RCP<const Basic> &arg,
        const vec_basic &x) const
{
    // After we implement the Subs class, we will require simplifications like
    // f(x^2).diff(x) -> 2*x*Subs(Derivative(f(_xi_1), _xi_1), _xi_1, x**2).
    // This will be checked here for 'args'.

    // Check that 'x' are Symbols:
    for(auto &a: x)
        if (!is_a<Symbol>(*a)) return false;

    return true;
}

std::size_t Derivative::__hash__() const
{
    std::size_t seed = 0;
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
    CSYMPY_ASSERT(is_a<Derivative>(o))
    const Derivative &s = static_cast<const Derivative &>(o);
    int cmp = arg_->__cmp__(*(s.arg_));
    if (cmp != 0) return cmp;
    cmp = vec_basic_compare(x_, s.x_);
    return cmp;
}


std::string Derivative::__str__() const
{
    std::ostringstream o, tmp;
    tmp << x_;
    std::string vars = tmp.str();
    o << "Derivative(" << *arg_ << ", " << vars.substr(1, vars.size()-2) << ")";
    return o.str();
}

RCP<const Basic> Derivative::diff(const RCP<const Symbol> &x) const
{
    vec_basic t = x_;
    t.push_back(x);
    return rcp(new Derivative(arg_, t));
}

std::size_t HyperbolicFunction::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

Sinh::Sinh(const RCP<const Basic> &arg)
    : HyperbolicFunction(arg)
{
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Sinh>(o))
    const Sinh &s = static_cast<const Sinh &>(o);
    return get_arg()->__cmp__(s);
}


std::string Sinh::__str__() const
{
    std::ostringstream o;
    o << "sinh(" << *get_arg() << ")";
    return o.str();
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

RCP<const Basic> Sinh::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Sinh> self = rcp_const_cast<Sinh>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return sinh(arg);
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Cosh>(o))
    const Cosh &s = static_cast<const Cosh &>(o);
    return get_arg()->__cmp__(s);
}


std::string Cosh::__str__() const
{
    std::ostringstream o;
    o << "cosh(" << *get_arg() << ")";
    return o.str();
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

RCP<const Basic> Cosh::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Cosh> self = rcp_const_cast<Cosh>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return cosh(arg);
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Tanh>(o))
    const Tanh &s = static_cast<const Tanh &>(o);
    return get_arg()->__cmp__(s);
}


std::string Tanh::__str__() const
{
    std::ostringstream o;
    o << "tanh(" << *get_arg() << ")";
    return o.str();
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

RCP<const Basic> Tanh::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Tanh> self = rcp_const_cast<Tanh>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return tanh(arg);
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<Coth>(o))
    const Coth &s = static_cast<const Coth &>(o);
    return get_arg()->__cmp__(s);
}


std::string Coth::__str__() const
{
    std::ostringstream o;
    o << "coth(" << *get_arg() << ")";
    return o.str();
}

RCP<const Basic> coth(const RCP<const Basic> &arg)
{
    if (eq(arg, zero)) {
        // Answer is infinity. Yet to be implemented in CSymPy
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

RCP<const Basic> Coth::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Coth> self = rcp_const_cast<Coth>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return coth(arg);
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
    CSYMPY_ASSERT(is_canonical(arg))
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
    CSYMPY_ASSERT(is_a<ASinh>(o))
    const ASinh &s = static_cast<const ASinh &>(o);
    return get_arg()->__cmp__(s);
}


std::string ASinh::__str__() const
{
    std::ostringstream o;
    o << "asinh(" << *get_arg() << ")";
    return o.str();
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

RCP<const Basic> ASinh::subs(const map_basic_basic &subs_dict) const
{
    RCP<const ASinh> self = rcp_const_cast<ASinh>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = get_arg()->subs(subs_dict);
    if (arg == get_arg())
        return self;
    else
        return asinh(arg);
}

RCP<const Basic> ASinh::diff(const RCP<const Symbol> &x) const
{
    return mul(div(one, add(pow(x, i2), one)), get_arg()->diff(x));
}

} // CSymPy
