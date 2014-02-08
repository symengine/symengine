#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "rational.h"
#include "functions.h"


namespace CSymPy {

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
    // TODO: add things like sin(k*pi) etc.
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
    return rcp(new Sin(arg));
}


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


RCP<const Basic> Sin::diff(const RCP<const Symbol> &x) const
{
    return mul(cos(arg_), arg_->diff(x));
}

RCP<const Basic> Cos::diff(const RCP<const Symbol> &x) const
{
    return mul(mul(minus_one, sin(arg_)), arg_->diff(x));
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
        return arg_->__cmp__(s);
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
        throw std::runtime_error("f(x).diff(x) not implemented yet.");
}

RCP<const Basic> function_symbol(std::string name, const RCP<const Basic> &arg)
{
    return rcp(new FunctionSymbol(name, arg));
}

} // CSymPy
