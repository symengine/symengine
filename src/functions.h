#ifndef CSYMPY_FUNCTIONS_H
#define CSYMPY_FUNCTIONS_H

#include <gmpxx.h>

#include "basic.h"

namespace CSymPy {

class Function : public Basic {
};

class Sin : public Function {
private:
    RCP<const Basic> arg_; // The 'arg' in sin(arg)

public:
    Sin(const RCP<const Basic> &arg);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }

    bool is_canonical(const RCP<const Basic> &arg);

    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

// This uses canonicalize:
RCP<const Basic> sin(const RCP<const Basic> &arg);


class Cos : public Function {
private:
    RCP<const Basic> arg_; // The 'arg' in sin(arg)

public:
    Cos(const RCP<const Basic> &arg);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }

    bool is_canonical(const RCP<const Basic> &arg);

    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

// This uses canonicalize:
RCP<const Basic> cos(const RCP<const Basic> &arg);

class FunctionSymbol : public Function {
private:
    std::string name_; // The 'f' in f(x+y)
    RCP<const Basic> arg_; // The 'x+y' in f(x+y)

public:
    FunctionSymbol(std::string name, const RCP<const Basic> &arg);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;
    inline std::string get_name() const {
        return name_;
    }
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }

    bool is_canonical(const RCP<const Basic> &arg);

    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

RCP<const Basic> function_symbol(std::string name,
        const RCP<const Basic> &arg);

} // CSymPy

#endif
