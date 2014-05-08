#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "rational.h"
#include "functions.h"


namespace CSymPy {

RCP<const Basic> i2 = rcp(new Integer(2));
RCP<const Basic> i3 = rcp(new Integer(3));

RCP<const Basic> sqrt(RCP<const Basic>& arg)
{
	return pow(arg, div(one, i2));
}

// TODO: Update to actual sqrt from integer sqrt.
RCP<const Basic> sq3 = sqrt(i3);
RCP<const Basic> sq2 = sqrt(i2);

RCP<const Basic> C0 = div(sub(sq3, one), mul(i2, sq2));
RCP<const Basic> C1 = div(one, i2);
RCP<const Basic> C2 = div(sq2, i2);
RCP<const Basic> C3 = div(sq3, i2);
RCP<const Basic> C4 = div(add(sq3, one), mul(i2, sq2));

RCP<const Basic> mC0 = mul(minus_one, C0);
RCP<const Basic> mC1 = mul(minus_one, C1);
RCP<const Basic> mC2 = mul(minus_one, C2);
RCP<const Basic> mC3 = mul(minus_one, C3);
RCP<const Basic> mC4 = mul(minus_one, C4);

// sin_table[n] represents the value of sin(2*pi*n/24) for n = 0..23
RCP<const Basic> sin_table[] = {
        zero, C0, C1, C2, C3, C4, one, C4, C3, C2, C1, C0,
        zero, mC0, mC1, mC2, mC3, mC4, minus_one, mC4, mC3, mC2, mC1, mC0
    };

bool get_pi_shift(const RCP<const Basic> &arg,
     RCP<const Integer> &n)
{
    // it should return n if arg = n*pi/12
    if (is_a<Mul>(*arg)) {
        
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

            n = rcp_dynamic_cast<const Integer>(coef);
            return true;    
        }
        else
            return false;
    }
    else
        return false;
}

Sin::Sin(const RCP<const Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool Sin::is_canonical(const RCP<const Basic> &arg)
{
    // e.g. sin(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;

    RCP<const Integer> r;
    bool b = get_pi_shift(arg, r);
    if (b)
        return false;

    return true;
}

std::size_t Sin::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Sin::__eq__(const Basic &o) const
{
    if (is_a<Sin>(o) &&
        eq(arg_, static_cast<const Sin &>(o).arg_))
        return true;
    return false;
}

int Sin::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Sin>(o))
    const Sin &s = static_cast<const Sin &>(o);
    return arg_->__cmp__(s);
}


std::string Sin::__str__() const
{
    std::ostringstream o;
    o << "sin(" << *arg_ << ")";
    return o.str();
}

RCP<const Basic> sin(const RCP<const Basic> &arg)
{
    if (eq(arg, zero)) return zero;
    RCP<const Integer> r;
    bool check;
    check = get_pi_shift(arg, r);
    if (check) {
        int index;
        index = r->as_int();
        return sin_table[index % 24];
    }
    else
        return rcp(new Sin(arg));
}

/* ---------------------------- */

Cos::Cos(const RCP<const Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool Cos::is_canonical(const RCP<const Basic> &arg)
{
    // e.g. cos(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // TODO: add things like cos(k*pi) etc.
    return true;
}

std::size_t Cos::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Cos::__eq__(const Basic &o) const
{
    if (is_a<Cos>(o) &&
        eq(arg_, static_cast<const Cos &>(o).arg_))
        return true;
    return false;
}

int Cos::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Cos>(o))
    const Cos &s = static_cast<const Cos &>(o);
    return arg_->__cmp__(s);
}

std::string Cos::__str__() const
{
    std::ostringstream o;
    o << "cos(" << *arg_ << ")";
    return o.str();
}

RCP<const Basic> cos(const RCP<const Basic> &arg)
{
    if (eq(arg, zero)) return one;
    return rcp(new Cos(arg));
}

/* ---------------------------- */

Tan::Tan(const RCP<const Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool Tan::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases 
    // and extend to tan(k*pi)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    return true;
}

std::size_t Tan::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Tan::__eq__(const Basic &o) const
{
    if (is_a<Tan>(o) &&
        eq(arg_, static_cast<const Tan &>(o).arg_))
        return true;
    else
        return false;
}

int Tan::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Tan>(o))
    const Tan &s = static_cast<const Tan &>(o);
    return arg_->__cmp__(s);
}


std::string Tan::__str__() const
{
    std::ostringstream o;
    o << "tan(" << *arg_ << ")";
    return o.str();
}

RCP<const Basic> tan(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    if (eq(arg, zero)) return zero;
    return rcp(new Tan(arg));
}

/* ---------------------------- */

Cot::Cot(const RCP<const Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool Cot::is_canonical(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases 
    // and extend to cot(k*pi) 
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    return true;
}

std::size_t Cot::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Cot::__eq__(const Basic &o) const
{
    if (is_a<Cot>(o) &&
        eq(arg_, static_cast<const Cot &>(o).arg_))
        return true;
    else
        return false;
}

int Cot::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Cot>(o))
    const Cot &s = static_cast<const Cot &>(o);
    return arg_->__cmp__(s);
}


std::string Cot::__str__() const
{
    std::ostringstream o;
    o << "cot(" << *arg_ << ")";
    return o.str();
}

RCP<const Basic> cot(const RCP<const Basic> &arg)
{
    // TODO: Add further checks for +inf -inf cases
    return rcp(new Cot(arg));
}

/* ---------------------------- */

Csc::Csc(const RCP<const Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool Csc::is_canonical(const RCP<const Basic> &arg)
{
    // e.g. Csc(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // TODO: add things like Csc(k*pi) etc.
    // Update for +inf/-inf constraints
    return true;
}

std::size_t Csc::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Csc::__eq__(const Basic &o) const
{
    if (is_a<Csc>(o) &&
        eq(arg_, static_cast<const Csc &>(o).arg_))
        return true;
    return false;
}

int Csc::compare(const Basic &o) const
{    
    CSYMPY_ASSERT(is_a<Csc>(o))
    const Csc &s = static_cast<const Csc &>(o);
    return arg_->__cmp__(s);
}

std::string Csc::__str__() const
{
    std::ostringstream o;
    o << "csc(" << *arg_ << ")";
    return o.str();
}

RCP<const Basic> csc(const RCP<const Basic> &arg)
{
    return rcp(new Csc(arg));
}

/* ---------------------------- */

Sec::Sec(const RCP<const Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool Sec::is_canonical(const RCP<const Basic> &arg)
{
    // e.g. Sec(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<const Integer>(arg)->is_zero())
        return false;
    // TODO: add things like Sec(k*pi) etc.
    // Update for +inf/-inf constraints
    return true;
}

std::size_t Sec::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Sec::__eq__(const Basic &o) const
{
    if (is_a<Sec>(o) &&
        eq(arg_, static_cast<const Sec &>(o).arg_))
        return true;
    return false;
}

int Sec::compare(const Basic &o) const
{    CSYMPY_ASSERT(is_a<Sec>(o))
    const Sec &s = static_cast<const Sec &>(o);
    return arg_->__cmp__(s);
}

std::string Sec::__str__() const
{
    std::ostringstream o;
    o << "sec(" << *arg_ << ")";
    return o.str();
}

RCP<const Basic> sec(const RCP<const Basic> &arg)
{
    if (eq(arg, zero)) return one;
    return rcp(new Sec(arg));
}

/* ---------------------------- */

RCP<const Basic> Sin::diff(const RCP<const Symbol> &x) const
{
    return mul(cos(arg_), arg_->diff(x));
}

RCP<const Basic> Cos::diff(const RCP<const Symbol> &x) const
{
    return mul(mul(minus_one, sin(arg_)), arg_->diff(x));
}

RCP<const Basic> Tan::diff(const RCP<const Symbol> &x) const
{
    RCP<const Integer> two = rcp(new Integer(2));
    return mul(add(one, pow(tan(arg_), two)), arg_->diff(x));
}

RCP<const Basic> Cot::diff(const RCP<const Symbol> &x) const
{
    RCP<const Integer> two = rcp(new Integer(2));
    return mul(mul(add(one, pow(cot(arg_), two)), minus_one), arg_->diff(x));
}

RCP<const Basic> Csc::diff(const RCP<const Symbol> &x) const
{
    return mul(mul(mul(cot(arg_), csc(arg_)), minus_one), arg_->diff(x));
}

RCP<const Basic> Sec::diff(const RCP<const Symbol> &x) const
{
    return mul(mul(tan(arg_), sec(arg_)), arg_->diff(x));
}

RCP<const Basic> Sin::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Sin> self = rcp_const_cast<Sin>(rcp(this));
    auto it = subs_dict.find(self);
    if (it != subs_dict.end())
        return it->second;
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
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
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
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
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
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
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
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
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
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
    RCP<const Basic> arg = arg_->subs(subs_dict);
    if (arg == arg_)
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
