/**
 *  \file functions.h
 *  Includes various trignometric functions
 *
 **/

#ifndef SYMENGINE_FUNCTIONS_H
#define SYMENGINE_FUNCTIONS_H

#include <symengine/basic.h>
#include <symengine/dict.h>

namespace SymEngine {

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
    IMPLEMENT_TYPEID(SIN)
    //! Sin Constructor
    Sin(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized sin
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize Sin:
RCP<const Basic> sin(const RCP<const Basic> &arg);


class Cos : public TrigFunction {

public:
    IMPLEMENT_TYPEID(COS)
    //! Cos Constructor
    Cos(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o  Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized cos
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize Cos:
RCP<const Basic> cos(const RCP<const Basic> &arg);

class Tan : public TrigFunction {

public:
    IMPLEMENT_TYPEID(TAN)
    //! Tan Constructor
    Tan(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized tan
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};
//! Canonicalize Tan:
RCP<const Basic> tan(const RCP<const Basic> &arg);

class Cot : public TrigFunction {

public:
    IMPLEMENT_TYPEID(COT)
    //! Cot Constructor
    Cot(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized cot
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};
//! Canonicalize Cot:
RCP<const Basic> cot(const RCP<const Basic> &arg);

class Csc: public TrigFunction {

public:
    IMPLEMENT_TYPEID(CSC)
    //! Csc Constructor
    Csc(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized csc
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};
//! Canonicalize Csc:
RCP<const Basic> csc(const RCP<const Basic> &arg);

class Sec: public TrigFunction {

public:
    IMPLEMENT_TYPEID(SEC)
    //! Sec Constructor
    Sec(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;

    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized sec
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};
//! Canonicalize Sec:
RCP<const Basic> sec(const RCP<const Basic> &arg);

class ASin : public TrigFunction {

public:
    IMPLEMENT_TYPEID(ASIN)
    //! ASin Constructor
    ASin(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized asin
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ASin:
RCP<const Basic> asin(const RCP<const Basic> &arg);

class ACos : public TrigFunction {

public:
    IMPLEMENT_TYPEID(ACOS)
    //! ACos Constructor
    ACos(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acos
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ACos:
RCP<const Basic> acos(const RCP<const Basic> &arg);

class ASec : public TrigFunction {

public:
    IMPLEMENT_TYPEID(ASEC)
    //! ASec Constructor
    ASec(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized asec
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ASec:
RCP<const Basic> asec(const RCP<const Basic> &arg);

class ACsc : public TrigFunction {

public:
    IMPLEMENT_TYPEID(ACSC)
    //! ACsc Constructor
    ACsc(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acsc
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ACsc:
RCP<const Basic> acsc(const RCP<const Basic> &arg);

class ATan : public TrigFunction {

public:
    IMPLEMENT_TYPEID(ATAN)
    //! ATan Constructor
    ATan(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized atan
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ATan:
RCP<const Basic> atan(const RCP<const Basic> &arg);

class ACot : public TrigFunction {

public:
    IMPLEMENT_TYPEID(ACOT)
    //! ACot Constructor
    ACot(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acot
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ACot:
RCP<const Basic> acot(const RCP<const Basic> &arg);

class ATan2 : public Function {
private:
    RCP<const Basic> num_; //! The `y` in `atan2(y, x)`
    RCP<const Basic> den_; //! The `x` in `atan2(y, x)`
public:
    IMPLEMENT_TYPEID(ATAN2)
    //! ATan2 Constructor
    ATan2(const RCP<const Basic> &num, const RCP<const Basic> &den);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
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

    virtual void accept(Visitor &v) const;
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
    IMPLEMENT_TYPEID(LAMBERTW)
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
    //! \return `arg_`
    inline RCP<const Basic> get_arg() const { return arg_; }
    virtual vec_basic get_args() const { return {arg_}; }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;

    virtual void accept(Visitor &v) const;
};

//! Create a new LambertW instance:
RCP<const Basic> lambertw(const RCP<const Basic> &arg);


class Zeta : public Function {
// Hurwitz zeta function (or Riemann zeta function).
//
// For `\operatorname{Re}(a) > 0` and `\operatorname{Re}(s) > 1`, this function is defined as
//
// .. math:: \zeta(s, a) = \sum_{n=0}^\infty \frac{1}{(n + a)^s},
//
// where the standard choice of argument for :math:`n + a` is used. 
// If no value is passed for :math:`a`, by this function assumes a default value
// of :math:`a = 1`, yielding the Riemann zeta function.

private:
    RCP<const Basic> s_;
    RCP<const Basic> a_;

public:
    IMPLEMENT_TYPEID(ZETA)
    //! Zeta Constructor
    Zeta(const RCP<const Basic> &s, const RCP<const Basic> &a);
    //! Zeta Constructor
    Zeta(const RCP<const Basic> &s);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o  Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `s_`
    inline RCP<const Basic> get_s() const { return s_; }
    //! \return `a_`
    inline RCP<const Basic> get_a() const { return a_; }
    virtual vec_basic get_args() const { return {s_, a_}; }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s, const RCP<const Basic> &a);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;

    virtual void accept(Visitor &v) const;
};

//! Create a new Zeta instance:
RCP<const Basic> zeta(const RCP<const Basic> &s, const RCP<const Basic> &a);
RCP<const Basic> zeta(const RCP<const Basic> &s);

class Dirichlet_eta : public Function {
// See http://en.wikipedia.org/wiki/Dirichlet_eta_function

private:
    RCP<const Basic> s_;

public:
    IMPLEMENT_TYPEID(DIRICHLET_ETA)
    //! Dirichlet_eta Constructor
    Dirichlet_eta(const RCP<const Basic> &s);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o  Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `s_`
    inline RCP<const Basic> get_s() const { return s_; }
    virtual vec_basic get_args() const { return {s_}; }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s);
    //! Rewrites in the form of zeta
    RCP<const Basic> rewrite_as_zeta() const;

    virtual void accept(Visitor &v) const;
};

//! Create a new Dirichlet_eta instance:
RCP<const Basic> dirichlet_eta(const RCP<const Basic> &s);

class FunctionSymbol : public Function {
protected:
    std::string name_; //! The `f` in `f(x+y, z)`
    vec_basic arg_; //! The `x+y`, `z` in `f(x+y, z)`

public:
    IMPLEMENT_TYPEID(FUNCTIONSYMBOL)
    //! FunctionSymbol Constructors
    FunctionSymbol(std::string name, const vec_basic &arg);
    FunctionSymbol(std::string name, const RCP<const Basic> &arg);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o  Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `name_`
    inline std::string get_name() const { return name_; }
    //! \return `arg_`
    virtual vec_basic get_args() const { return arg_; }
    //! \return `true` if canonical
    bool is_canonical(const vec_basic &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;

    virtual void accept(Visitor &v) const;
};

//! Create a new FunctionSymbol instance:
RCP<const Basic> function_symbol(std::string name,
        const RCP<const Basic> &arg);
RCP<const Basic> function_symbol(std::string name,
        const vec_basic &arg);

/*! Class to hold a pointer to a function object
*   FunctionWrapper can be used to wrap any C/C++ object
*   (eg: Python object through Python/C API) 
* */

class FunctionWrapper: public FunctionSymbol {
private:
    void* obj_;
    std::string hash_;
    void (*dec_ref_)(void *);
    int (*comp_)(void *, void *);

public:
    IMPLEMENT_TYPEID(FUNCTIONWRAPPER)
    /*! FunctionWrapper Constructor
     * \param obj - Pointer to the function object
     * \param name - Name of the function
     * \param hash - Hash value of obj
     * \param arg - Arguments of the function
     * \param dec_ref - Function pointer to decrease the reference count
     * \param comp - Function pointer to compare two function objects
     * */
    FunctionWrapper(void* obj, std::string name, std::string hash, const vec_basic &arg,
        void (*dec_ref)(void *), int (*comp)(void *, void *));
    ~FunctionWrapper();
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Pointer to the function object
    inline void* get_object() const { return obj_; }

    virtual void accept(Visitor &v) const;
};

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
    IMPLEMENT_TYPEID(DERIVATIVE)
    Derivative(const RCP<const Basic> &arg, const vec_basic &x);

    static RCP<const Derivative> create(const RCP<const Basic> &arg,
            const vec_basic &x) {
        return make_rcp<const Derivative>(arg, x);
    }

    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
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
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;

    virtual void accept(Visitor &v) const;
};

/*! Subs operator
 *  Subs(f, {x1 : x2, y1: y2, ...}) represents `f` after substituting
 *  `x1` with `x2`, `y1` with `y2`, and so on.
 * */
class Subs : public Basic {
public:
    RCP<const Basic> arg_;
    map_basic_basic dict_;

public:
    IMPLEMENT_TYPEID(SUBS)
    Subs(const RCP<const Basic> &arg, const map_basic_basic &x);

    static RCP<const Subs> create(const RCP<const Basic> &arg,
            const map_basic_basic &x) {
        return make_rcp<const Subs>(arg, x);
    }

    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }
    virtual vec_basic get_variables() const;
    virtual vec_basic get_point() const;
    virtual vec_basic get_args() const;

    bool is_canonical(const RCP<const Basic> &arg, const map_basic_basic &x) const;
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;

    virtual void accept(Visitor &v) const;
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
    IMPLEMENT_TYPEID(SINH)
    //! Sinh Constructor
    Sinh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized sinh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! expands sinh in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize Sinh:
RCP<const Basic> sinh(const RCP<const Basic> &arg);

class Cosh : public HyperbolicFunction {
//! The hyperbolic cosine function, `\frac{e^x + e^{-x}}{2}`.
public:
    IMPLEMENT_TYPEID(COSH)
    //! Cosh Constructor
    Cosh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized cosh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! expands cosh in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize Cosh:
RCP<const Basic> cosh(const RCP<const Basic> &arg);

class Tanh : public HyperbolicFunction {
//! The hyperbolic tangent function, `\frac{\sinh(x)}{\cosh(x)}`.
public:
    IMPLEMENT_TYPEID(TANH)
    //! Tanh Constructor
    Tanh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized tanh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! expands tanh in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize Tanh:
RCP<const Basic> tanh(const RCP<const Basic> &arg);

class Coth : public HyperbolicFunction {
//! The hyperbolic tangent function, `\frac{\cosh(x)}{\sinh(x)}`.
public:
    IMPLEMENT_TYPEID(COTH)
    //! Coth Constructor
    Coth(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized coth
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;
    //! expands coth in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize Coth:
RCP<const Basic> coth(const RCP<const Basic> &arg);

class ASinh : public HyperbolicFunction {
//! The inverse hyperbolic sine function.
public:
    IMPLEMENT_TYPEID(ASINH)
    //! ASinh Constructor
    ASinh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized asinh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ASinh:
RCP<const Basic> asinh(const RCP<const Basic> &arg);

class ACosh: public HyperbolicFunction {
//! The inverse hyperbolic cosine function.
public:
    IMPLEMENT_TYPEID(ACOSH)
    //! ACosh Constructor
    ACosh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acosh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ACosh:
RCP<const Basic> acosh(const RCP<const Basic> &arg);

class ATanh: public HyperbolicFunction {
//! The inverse hyperbolic tangent function.
public:
    IMPLEMENT_TYPEID(ATANH)
    //! ATanh Constructor
    ATanh(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized atanh
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ATanh:
RCP<const Basic> atanh(const RCP<const Basic> &arg);

class ACoth: public HyperbolicFunction {
//! The inverse hyperbolic cotangent function.
public:
    IMPLEMENT_TYPEID(ACOTH)
    //! ACoth Constructor
    ACoth(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized acoth
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ACoth:
RCP<const Basic> acoth(const RCP<const Basic> &arg);

class ASech: public HyperbolicFunction {
//! The inverse hyperbolic secant function.
public:
    IMPLEMENT_TYPEID(ASECH)
    //! ASech Constructor
    ASech(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    //! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! \return Canonicalized asech
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize ASech:
RCP<const Basic> asech(const RCP<const Basic> &arg);

class KroneckerDelta: public Function {
/*! The discrete, or Kronecker, delta function.
 * A function that takes in two integers `i` and `j`. It returns `0` if `i` and `j` are
 * not equal or it returns `1` if `i` and `j` are equal.
 * http://en.wikipedia.org/wiki/Kronecker_delta
 **/
private:
    RCP<const Basic> i_;
    RCP<const Basic> j_;
public:
    IMPLEMENT_TYPEID(KRONECKERDELTA)
    //! KroneckerDelta Constructor
    KroneckerDelta(const RCP<const Basic> &i, const RCP<const Basic> &j);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &i, const RCP<const Basic> &j);
    virtual vec_basic get_args() const { return {i_, j_}; }

    virtual void accept(Visitor &v) const;
};

//! Canonicalize KroneckerDelta:
RCP<const Basic> kronecker_delta(const RCP<const Basic> &i, const RCP<const Basic> &j);


class LeviCivita: public Function {
/*! Represent the Levi-Civita symbol.
 *  For even permutations of indices it returns 1, for odd permutations -1, and
 *  for everything else (a repeated index) it returns 0.
 *
 *  Thus it represents an alternating pseudotensor.
 **/
private:
    vec_basic arg_;
public:
    IMPLEMENT_TYPEID(LEVICIVITA)
    //! LeviCivita Constructor
    LeviCivita(const vec_basic&& arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `true` if canonical
    bool is_canonical(const vec_basic &arg);
    virtual vec_basic get_args() const { return arg_; }

    virtual void accept(Visitor &v) const;
};

//! Canonicalize LeviCivita:
RCP<const Basic> levi_civita(const vec_basic &arg);

class Gamma: public Function {
/*!    The gamma function
 *
 *   .. math::
 *      \Gamma(x) := \int^{\infty}_{0} t^{x-1} e^{t} \mathrm{d}t.
 *
 *  The ``gamma`` function implements the function which passes through the
 *  values of the factorial function, i.e. `\Gamma(n) = (n - 1)!` when n is
 *  an integer. More general, `\Gamma(z)` is defined in the whole complex
 *  plane except at the negative integers where there are simple poles.
 **/
private:
    RCP<const Basic> arg_;
public:
    IMPLEMENT_TYPEID(GAMMA)
    //! Gamma Constructor
    Gamma(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    virtual vec_basic get_args() const { return {arg_}; }

    virtual void accept(Visitor &v) const;
};

//! Canonicalize Gamma:
RCP<const Basic> gamma(const RCP<const Basic> &arg);

class LowerGamma: public Function {
//! The lower incomplete gamma function.
private:
    RCP<const Basic> s_;
    RCP<const Basic> x_;
public:
    IMPLEMENT_TYPEID(LOWERGAMMA)
    //! LowerGamma Constructor
    LowerGamma(const RCP<const Basic> &s, const RCP<const Basic> &x);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s, const RCP<const Basic> &x);
    virtual vec_basic get_args() const { return {s_, x_}; }

    virtual void accept(Visitor &v) const;
};

//! Canonicalize LowerGamma:
RCP<const Basic> lowergamma(const RCP<const Basic> &s, const RCP<const Basic> &x);


class UpperGamma: public Function {
//! The upper incomplete gamma function.
private:
    RCP<const Basic> s_;
    RCP<const Basic> x_;
public:
    IMPLEMENT_TYPEID(UPPERGAMMA)
    //! UpperGamma Constructor
    UpperGamma(const RCP<const Basic> &s, const RCP<const Basic> &x);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s, const RCP<const Basic> &x);
    virtual vec_basic get_args() const { return {s_, x_}; }

    virtual void accept(Visitor &v) const;
};

//! Canonicalize UpperGamma:
RCP<const Basic> uppergamma(const RCP<const Basic> &s, const RCP<const Basic> &x);


class Abs: public Function {
/*!    The absolute value function
 **/
private:
    RCP<const Basic> arg_;
public:
    IMPLEMENT_TYPEID(ABS)
    //! Abs Constructor
    Abs(const RCP<const Basic> &arg);
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
    inline RCP<const Basic> get_arg() const { return arg_; }
    virtual vec_basic get_args() const { return {arg_}; }
    RCP<const Basic> diff(const RCP<const Symbol> &x) const;

    virtual void accept(Visitor &v) const;
};

//! Canonicalize Abs:
RCP<const Basic> abs(const RCP<const Basic> &arg);


} // SymEngine

#endif
