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

RCP<const Basic> sqrt(RCP<const Basic>& arg)
{
  return pow(arg, div(one, i2));
}

static RCP<const Basic> sq3 = sqrt(i3);
static RCP<const Basic> sq2 = sqrt(i2);

static RCP<const Basic> C0 = div(sub(sq3, one), mul(i2, sq2));
static RCP<const Basic> C1 = div(one, i2);
static RCP<const Basic> C2 = div(sq2, i2);
static RCP<const Basic> C3 = div(sq3, i2);
static RCP<const Basic> C4 = div(add(sq3, one), mul(i2, sq2));

static RCP<const Basic> mC0 = mul(minus_one, C0);
static RCP<const Basic> mC1 = mul(minus_one, C1);
static RCP<const Basic> mC2 = mul(minus_one, C2);
static RCP<const Basic> mC3 = mul(minus_one, C3);
static RCP<const Basic> mC4 = mul(minus_one, C4);

// sin_table[n] represents the value of sin(2*pi*n/24) for n = 0..23
static RCP<const Basic> sin_table[] = {
        zero, C0, C1, C2, C3, C4, one, C4, C3, C2, C1, C0,
        zero, mC0, mC1, mC2, mC3, mC4, minus_one, mC4, mC3, mC2, mC1, mC0
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
/* ---------------------------- */

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

} // CSymPy
