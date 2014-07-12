/**
 *  \file functions.h
 *  Includes various trignometric functions
 *
 **/

#ifndef CSYMPY_FUNCTIONS_H
#define CSYMPY_FUNCTIONS_H

#include <gmpxx.h>

#include "basic.h"
#include "dict.h"

namespace CSymPy {

class Function : public Basic {
};

class TrigFunction : public Function {

private:
    RCP<const Basic> arg_; //! The `arg` in `trigclass(arg)`
public:
    //! Constructor
    TrigFunction(RCP<const Basic> arg)
        :arg_{arg} {};
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `arg_`
    inline RCP<const Basic> get_arg() const { return arg_; }
    virtual vec_basic get_args() const { return {arg_}; }
    //! Method to construct classes with canonicalization
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! Substitute with `subs_dict`
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

/*! \return `true` if `arg` is of form `theta + n*pi/12`
 * `n` is the n in `n*pi/12`
 * `x` is `theta`
 * */
bool get_pi_shift(const RCP<const Basic> &arg,
        const Ptr<RCP<const Integer>> &n,
        const Ptr<RCP<const Basic>> &x);

//! \return `true` if `arg` contains a negative sign.
bool could_extract_minus(const RCP<const Basic> &arg);

bool handle_minus(const RCP<const Basic> &arg,
            const Ptr<RCP<const Basic>> &rarg);

/*! returns `true` if the given argument `t` is found in the
*   lookup table `d`. It also returns the value in `index`
**/
bool inverse_lookup(umap_basic_basic &d, const RCP<const Basic> &t,
                   const Ptr<RCP<const Basic>>& index);

// \return true of conjugate has to be returned finally else false
bool eval(const RCP<const Basic> &arg, int period, bool odd, bool conj_odd, //input
            const Ptr<RCP<const Basic>>& rarg,int& index, int& sign); //output

//! \return `sqrt` of the `arg`
RCP<const Basic> sqrt(const RCP<const Basic> &arg);
class Sin : public TrigFunction {

public:
    //! Sin Constructor
    Sin(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized sin
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize Sin:
RCP<const Basic> sin(const RCP<const Basic> &arg);


class Cos : public TrigFunction {

public:
    //! Cos Constructor
    Cos(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o  Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized cos
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

};

//! Canonicalize Cos:
RCP<const Basic> cos(const RCP<const Basic> &arg);

class Tan : public TrigFunction {

public:
    //! Tan Constructor
    Tan(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized tan
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};
//! Canonicalize Tan:
RCP<const Basic> tan(const RCP<const Basic> &arg);

class Cot : public TrigFunction {

public:
    //! Cot Constructor
    Cot(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized cot
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};
//! Canonicalize Cot:
RCP<const Basic> cot(const RCP<const Basic> &arg);

class Csc: public TrigFunction {

public:
    //! Csc Constructor
    Csc(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized csc
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};
//! Canonicalize Csc:
RCP<const Basic> csc(const RCP<const Basic> &arg);

class Sec: public TrigFunction {

public:
    //! Sec Constructor
    Sec(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized sec
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};
//! Canonicalize Sec:
RCP<const Basic> sec(const RCP<const Basic> &arg);

class ASin : public TrigFunction {

public:
    //! ASin Constructor
    ASin(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized asin
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ASin:
RCP<const Basic> asin(const RCP<const Basic> &arg);

class ACos : public TrigFunction {

public:
    //! ACos Constructor
    ACos(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acos
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ACos:
RCP<const Basic> acos(const RCP<const Basic> &arg);

class ASec : public TrigFunction {

public:
    //! ASec Constructor
    ASec(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized asec
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ASec:
RCP<const Basic> asec(const RCP<const Basic> &arg);

class ACsc : public TrigFunction {

public:
    //! ACsc Constructor
    ACsc(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acsc
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ACsc:
RCP<const Basic> acsc(const RCP<const Basic> &arg);

class ATan : public TrigFunction {

public:
    //! ATan Constructor
    ATan(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized atan
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ATan:
RCP<const Basic> atan(const RCP<const Basic> &arg);

class ACot : public TrigFunction {

public:
    //! ACot Constructor
    ACot(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acot
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ACot:
RCP<const Basic> acot(const RCP<const Basic> &arg);

class ATan2 : public Function {
private:
    RCP<const Basic> num_; //! The `y` in `atan2(y, x)`
    RCP<const Basic> den_; //! The `x` in `atan2(y, x)`
public:
    //! ATan2 Constructor
    ATan2(const RCP<const Basic> &num, const RCP<const Basic> &den);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &num, const RCP<const Basic> &den);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `y` in `atan2(y, x)`
    inline RCP<const Basic> get_num() const {
        return num_;
    }
    //! \return `x` in `atan2(y, x)`
    inline RCP<const Basic> get_den() const {
        return den_;
    }
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;

    virtual vec_basic get_args() const { return {num_, den_}; }
};

//! Canonicalize ATan2:
RCP<const Basic> atan2(const RCP<const Basic> &num,
                        const RCP<const Basic> &den);

class LambertW : public Function {
// Lambert W function, defined as the inverse function of
// x*exp(x). This function represents the principal branch
// of this inverse function, which is multivalued.
// For more information, see:
// http://en.wikipedia.org/wiki/Lambert_W_function
private:
    RCP<const Basic> arg_;

public:
    //! LambertW Constructor
    LambertW(const RCP<const Basic> &arg);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o  Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `arg_`
    inline RCP<const Basic> get_arg() const { return arg_; }
    virtual vec_basic get_args() const { return {arg_}; }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

//! Create a new LambertW instance:
RCP<const Basic> lambertw(const RCP<const Basic> &arg);

class FunctionSymbol : public Function {
private:
    std::string name_; //! The `f` in `f(x+y)`
    RCP<const Basic> arg_; //! The `x+y` in `f(x+y)`

public:
    //! FunctionSymbol Constructor
    FunctionSymbol(std::string name, const RCP<const Basic> &arg);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o  Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `name_`
    inline std::string get_name() const { return name_; }
    //! \return `arg_`
    inline RCP<const Basic> get_arg() const { return arg_; }
    virtual vec_basic get_args() const { return {arg_}; }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

//! Create a new FunctionSymbol instance:
RCP<const Basic> function_symbol(std::string name,
        const RCP<const Basic> &arg);

/*! Derivative operator
 *  Derivative(f, [x, y, ...]) represents a derivative of `f` with respect to
 *  `x`, `y`, and so on.
 * */
class Derivative : public Basic {
private:
    RCP<const Basic> arg_; //! The expression to be differentiated
    // The symbols are declared as Basic (and checked by is_canonical() below),
    // to avoid issues with converting vector<RCP<Symbol>> to
    // vector<RCP<Basic>>, see [1], [2]. The problem is that even though Symbol
    // inherits from Basic, vector<RCP<Symbol>> does not inherit from
    // vector<RCP<Basic>>, so the compiler can't cast the derived type to the
    // base type when calling functions like vec_basic_eq() that are only
    // defined for the base type vector<RCP<Basic>>.
    // [1] http://stackoverflow.com/questions/14964909/how-to-cast-a-vector-of-shared-ptrs-of-a-derived-class-to-a-vector-of-share-ptrs
    // [2] http://stackoverflow.com/questions/114819/getting-a-vectorderived-into-a-function-that-expects-a-vectorbase
    vec_basic x_; //! x, y, ...

public:
    Derivative(const RCP<const Basic> &arg, const vec_basic &x);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }
    inline vec_basic get_symbols() const {
        return x_;
    }
    virtual vec_basic get_args() const {
        vec_basic args = {arg_};
        args.insert(args.end(), x_.begin(), x_.end());
        return args;
    }
    bool is_canonical(const RCP<const Basic> &arg, const vec_basic &x) const;
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

class HyperbolicFunction : public Function {

private:
    RCP<const Basic> arg_; //! The `arg` in `hyperbolicclass(arg)`
public:
    //! Constructor
    HyperbolicFunction(RCP<const Basic> arg)
        :arg_{arg} {};
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `arg_`
    inline RCP<const Basic> get_arg() const { return arg_; }
    virtual vec_basic get_args() const { return {arg_}; }
    //! Method to construct classes with canonicalization
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! Substitute with `subs_dict`
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

class Sinh : public HyperbolicFunction {
//! The hyperbolic sine function, `\frac{e^x - e^{-x}}{2}`.
public:
    //! Sinh Constructor
    Sinh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized sinh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! expands sinh in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const;
};

//! Canonicalize Sinh:
RCP<const Basic> sinh(const RCP<const Basic> &arg);

class Cosh : public HyperbolicFunction {
//! The hyperbolic cosine function, `\frac{e^x + e^{-x}}{2}`.
public:
    //! Cosh Constructor
    Cosh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized cosh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! expands cosh in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const;
};

//! Canonicalize Cosh:
RCP<const Basic> cosh(const RCP<const Basic> &arg);

class Tanh : public HyperbolicFunction {
//! The hyperbolic tangent function, `\frac{\sinh(x)}{\cosh(x)}`.
public:
    //! Tanh Constructor
    Tanh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized tanh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! expands tanh in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const;
};

//! Canonicalize Tanh:
RCP<const Basic> tanh(const RCP<const Basic> &arg);

class Coth : public HyperbolicFunction {
//! The hyperbolic tangent function, `\frac{\cosh(x)}{\sinh(x)}`.
public:
    //! Coth Constructor
    Coth(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized coth
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! expands coth in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const;
};

//! Canonicalize Coth:
RCP<const Basic> coth(const RCP<const Basic> &arg);

class ASinh : public HyperbolicFunction {
//! The inverse hyperbolic sine function.
public:
    //! ASinh Constructor
    ASinh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized asinh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ASinh:
RCP<const Basic> asinh(const RCP<const Basic> &arg);

class ACosh: public HyperbolicFunction {
//! The inverse hyperbolic cosine function.
public:
    //! ACosh Constructor
    ACosh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acosh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ACosh:
RCP<const Basic> acosh(const RCP<const Basic> &arg);

class ATanh: public HyperbolicFunction {
//! The inverse hyperbolic tangent function.
public:
    //! ATanh Constructor
    ATanh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized atanh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ATanh:
RCP<const Basic> atanh(const RCP<const Basic> &arg);

class ACoth: public HyperbolicFunction {
//! The inverse hyperbolic cotangent function.
public:
    //! ACoth Constructor
    ACoth(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acoth
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

//! Canonicalize ACoth:
RCP<const Basic> acoth(const RCP<const Basic> &arg);
} // CSymPy

#endif
