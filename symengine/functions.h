/**
 *  \file functions.h
 *  Includes various trignometric functions
 *
 **/

#ifndef SYMENGINE_FUNCTIONS_H
#define SYMENGINE_FUNCTIONS_H

#include <symengine/basic.h>
#include <symengine/symengine_casts.h>
#include <symengine/constants.h>

namespace SymEngine
{

class SYMENGINE_EXPORT Function : public Basic
{
};

class SYMENGINE_EXPORT OneArgFunction : public Function
{
private:
    RCP<const Basic> arg_; //! The `arg` in `OneArgFunction(arg)`
public:
    //! Constructor
    OneArgFunction(const RCP<const Basic> &arg) : arg_{arg} {};
    //! \return the hash
    inline hash_t __hash__() const override
    {
        hash_t seed = this->get_type_code();
        hash_combine<Basic>(seed, *arg_);
        return seed;
    }
    //! \return `arg_`
    inline RCP<const Basic> get_arg() const
    {
        return arg_;
    }
    inline vec_basic get_args() const override
    {
        return {arg_};
    }
    //! Method to construct classes with canonicalization
    virtual RCP<const Basic> create(const RCP<const Basic> &arg) const = 0;

    inline RCP<const Basic> create(const vec_basic &b) const
    {
        SYMENGINE_ASSERT(b.size() == 1);
        return create(b[0]);
    }

    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    inline bool __eq__(const Basic &o) const override
    {
        return is_same_type(*this, o)
               and eq(*get_arg(),
                      *down_cast<const OneArgFunction &>(o).get_arg());
    }
    //! Structural equality comparator
    inline int compare(const Basic &o) const override
    {
        SYMENGINE_ASSERT(is_same_type(*this, o))
        return get_arg()->__cmp__(
            *(down_cast<const OneArgFunction &>(o).get_arg()));
    }
};

template <class BaseClass>
class SYMENGINE_EXPORT TwoArgBasic : public BaseClass
{
private:
    RCP<const Basic> a_; //! `a` in `TwoArgBasic(a, b)`
    RCP<const Basic> b_; //! `b` in `TwoArgBasic(a, b)`
public:
    //! Constructor
    TwoArgBasic(const RCP<const Basic> &a, const RCP<const Basic> &b)
        : a_{a}, b_{b} {};
    //! \return the hash
    inline hash_t __hash__() const override
    {
        hash_t seed = this->get_type_code();
        hash_combine<Basic>(seed, *a_);
        hash_combine<Basic>(seed, *b_);
        return seed;
    }
    //! \return `arg_`
    inline RCP<const Basic> get_arg1() const
    {
        return a_;
    }
    //! \return `arg_`
    inline RCP<const Basic> get_arg2() const
    {
        return b_;
    }
    inline vec_basic get_args() const override
    {
        return {a_, b_};
    }
    //! Method to construct classes with canonicalization
    virtual RCP<const Basic> create(const RCP<const Basic> &a,
                                    const RCP<const Basic> &b) const = 0;

    inline RCP<const Basic> create(const vec_basic &b) const
    {
        SYMENGINE_ASSERT(b.size() == 2);
        return create(b[0], b[1]);
    }

    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    inline bool __eq__(const Basic &o) const override
    {
        return is_same_type(*this, o)
               and eq(*get_arg1(),
                      *down_cast<const TwoArgBasic &>(o).get_arg1())
               and eq(*get_arg2(),
                      *down_cast<const TwoArgBasic &>(o).get_arg2());
    }
    //! Structural equality comparator
    inline int compare(const Basic &o) const override
    {
        SYMENGINE_ASSERT(is_same_type(*this, o))
        const TwoArgBasic &t = down_cast<const TwoArgBasic &>(o);
        if (neq(*get_arg1(), *(t.get_arg1()))) {
            return get_arg1()->__cmp__(
                *(down_cast<const TwoArgBasic &>(o).get_arg1()));
        } else {
            return get_arg2()->__cmp__(
                *(down_cast<const TwoArgBasic &>(o).get_arg2()));
        }
    }
};

typedef TwoArgBasic<Function> TwoArgFunction;

class SYMENGINE_EXPORT MultiArgFunction : public Function
{
private:
    vec_basic arg_;

public:
    //! Constructor
    MultiArgFunction(const vec_basic &arg) : arg_{arg} {};
    //! \return the hash
    inline hash_t __hash__() const override
    {
        hash_t seed = this->get_type_code();
        for (const auto &a : arg_)
            hash_combine<Basic>(seed, *a);
        return seed;
    }
    inline vec_basic get_args() const override
    {
        return arg_;
    }
    inline const vec_basic &get_vec() const
    {
        return arg_;
    }
    //! Method to construct classes with canonicalization
    virtual RCP<const Basic> create(const vec_basic &v) const = 0;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    inline bool __eq__(const Basic &o) const override
    {
        return is_same_type(*this, o)
               and unified_eq(get_vec(),
                              down_cast<const MultiArgFunction &>(o).get_vec());
    }
    //! Structural equality comparator
    inline int compare(const Basic &o) const override
    {
        SYMENGINE_ASSERT(is_same_type(*this, o))
        return unified_compare(
            get_vec(), down_cast<const MultiArgFunction &>(o).get_vec());
    }
};

class SYMENGINE_EXPORT Sign : public OneArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_SIGN)
    //! Sign constructor
    Sign(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized sign
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Sign
SYMENGINE_EXPORT RCP<const Basic> sign(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Floor : public OneArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_FLOOR)
    //! Floor Constructor
    Floor(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized floor
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Floor:
SYMENGINE_EXPORT RCP<const Basic> floor(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Ceiling : public OneArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_CEILING)
    //! Ceiling Constructor
    Ceiling(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized ceiling
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Ceiling:
SYMENGINE_EXPORT RCP<const Basic> ceiling(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Truncate : public OneArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_TRUNCATE)
    //! Truncate Constructor
    Truncate(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized truncate
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Truncate:
SYMENGINE_EXPORT RCP<const Basic> truncate(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Conjugate : public OneArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_CONJUGATE)
    //! Conjugate constructor
    Conjugate(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized conjugate
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Conjugate
SYMENGINE_EXPORT RCP<const Basic> conjugate(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT TrigBase : public OneArgFunction
{
public:
    //! Constructor
    TrigBase(RCP<const Basic> arg) : OneArgFunction(arg){};
};

class SYMENGINE_EXPORT TrigFunction : public TrigBase
{
public:
    //! Constructor
    TrigFunction(RCP<const Basic> arg) : TrigBase(arg){};
};

class SYMENGINE_EXPORT InverseTrigFunction : public TrigBase
{
public:
    //! Constructor
    InverseTrigFunction(RCP<const Basic> arg) : TrigBase(arg){};
};

/*! \return `true` if `arg` is of form `m + n*pi` where `n` is a rational
 * */
SYMENGINE_EXPORT
bool get_pi_shift(const RCP<const Basic> &arg, const Ptr<RCP<const Number>> &n,
                  const Ptr<RCP<const Basic>> &m);

//! \return `true` if `arg` contains a negative sign.
SYMENGINE_EXPORT bool could_extract_minus(const Basic &arg);

SYMENGINE_EXPORT
bool handle_minus(const RCP<const Basic> &arg,
                  const Ptr<RCP<const Basic>> &rarg);

/*! returns `true` if the given argument `t` is found in the
 *   lookup table `d`. It also returns the value in `index`
 **/
SYMENGINE_EXPORT
bool inverse_lookup(const umap_basic_basic &d, const RCP<const Basic> &t,
                    const Ptr<RCP<const Basic>> &index);

// \return true of conjugate has to be returned finally else false
SYMENGINE_EXPORT
bool trig_simplify(const RCP<const Basic> &arg, unsigned period, bool odd,
                   bool conj_odd, // input
                   const Ptr<RCP<const Basic>> &rarg, int &index,
                   int &sign); // output

//! \return `sqrt` of the `arg`
SYMENGINE_EXPORT RCP<const Basic> sqrt(const RCP<const Basic> &arg);

//! \return `cbrt` of the `arg`
SYMENGINE_EXPORT RCP<const Basic> cbrt(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Sin : public TrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_SIN)
    //! Sin Constructor
    Sin(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized sin
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Sin:
SYMENGINE_EXPORT RCP<const Basic> sin(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Cos : public TrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_COS)
    //! Cos Constructor
    Cos(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized cos
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Cos:
SYMENGINE_EXPORT RCP<const Basic> cos(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Tan : public TrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_TAN)
    //! Tan Constructor
    Tan(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized tan
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};
//! Canonicalize Tan:
SYMENGINE_EXPORT RCP<const Basic> tan(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Cot : public TrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_COT)
    //! Cot Constructor
    Cot(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized cot
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};
//! Canonicalize Cot:
SYMENGINE_EXPORT RCP<const Basic> cot(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Csc : public TrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_CSC)
    //! Csc Constructor
    Csc(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized csc
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};
//! Canonicalize Csc:
SYMENGINE_EXPORT RCP<const Basic> csc(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Sec : public TrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_SEC)
    //! Sec Constructor
    Sec(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized sec
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};
//! Canonicalize Sec:
SYMENGINE_EXPORT RCP<const Basic> sec(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ASin : public InverseTrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_ASIN)
    //! ASin Constructor
    ASin(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized asin
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ASin:
SYMENGINE_EXPORT RCP<const Basic> asin(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ACos : public InverseTrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_ACOS)
    //! ACos Constructor
    ACos(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized acos
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ACos:
SYMENGINE_EXPORT RCP<const Basic> acos(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ASec : public InverseTrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_ASEC)
    //! ASec Constructor
    ASec(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized asec
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ASec:
SYMENGINE_EXPORT RCP<const Basic> asec(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ACsc : public InverseTrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_ACSC)
    //! ACsc Constructor
    ACsc(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized acsc
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ACsc:
SYMENGINE_EXPORT RCP<const Basic> acsc(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ATan : public InverseTrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_ATAN)
    //! ATan Constructor
    ATan(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized atan
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ATan:
SYMENGINE_EXPORT RCP<const Basic> atan(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ACot : public InverseTrigFunction
{

public:
    IMPLEMENT_TYPEID(SYMENGINE_ACOT)
    //! ACot Constructor
    ACot(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized acot
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ACot:
SYMENGINE_EXPORT RCP<const Basic> acot(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ATan2 : public TwoArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_ATAN2)
    //! ATan2 Constructor
    ATan2(const RCP<const Basic> &num, const RCP<const Basic> &den);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &num,
                      const RCP<const Basic> &den) const;
    //! \return `y` in `atan2(y, x)`
    inline RCP<const Basic> get_num() const
    {
        return get_arg1();
    }
    //! \return `x` in `atan2(y, x)`
    inline RCP<const Basic> get_den() const
    {
        return get_arg2();
    }
    //! \return canonicalized `atan2`
    RCP<const Basic> create(const RCP<const Basic> &a,
                            const RCP<const Basic> &b) const override;
};

//! Canonicalize ATan2:
SYMENGINE_EXPORT RCP<const Basic> atan2(const RCP<const Basic> &num,
                                        const RCP<const Basic> &den);

class SYMENGINE_EXPORT Log : public OneArgFunction
{
    // Logarithms are taken with the natural base, `e`. To get
    // a logarithm of a different base `b`, use `log(x, b)`,
    // which is essentially short-hand for `log(x)/log(b)`.
public:
    IMPLEMENT_TYPEID(SYMENGINE_LOG)
    //! Log Constructor
    Log(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return canonicalized `log`
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Returns the Natural Logarithm from argument `arg`
SYMENGINE_EXPORT RCP<const Basic> log(const RCP<const Basic> &arg);
//! \return Log from argument `arg` wrt base `b`
SYMENGINE_EXPORT RCP<const Basic> log(const RCP<const Basic> &arg,
                                      const RCP<const Basic> &b);

class SYMENGINE_EXPORT LambertW : public OneArgFunction
{
    // Lambert W function, defined as the inverse function of
    // x*exp(x). This function represents the principal branch
    // of this inverse function, which is multivalued.
    // For more information, see:
    // http://en.wikipedia.org/wiki/Lambert_W_function
public:
    IMPLEMENT_TYPEID(SYMENGINE_LAMBERTW)
    //! LambertW Constructor
    LambertW(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return canonicalized lambertw
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Create a new LambertW instance:
SYMENGINE_EXPORT RCP<const Basic> lambertw(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Zeta : public TwoArgFunction
{
    // Hurwitz zeta function (or Riemann zeta function).
    //
    // For `\operatorname{Re}(a) > 0` and `\operatorname{Re}(s) > 1`, this
    // function is defined as
    //
    // .. math:: \zeta(s, a) = \sum_{n=0}^\infty \frac{1}{(n + a)^s},
    //
    // where the standard choice of argument for :math:`n + a` is used.
    // If no value is passed for :math:`a`, by this function assumes a default
    // value
    // of :math:`a = 1`, yielding the Riemann zeta function.
public:
    using TwoArgFunction::create;
    IMPLEMENT_TYPEID(SYMENGINE_ZETA)
    //! Zeta Constructor
    Zeta(const RCP<const Basic> &s, const RCP<const Basic> &a);
    //! Zeta Constructor
    Zeta(const RCP<const Basic> &s);
    //! \return `s_`
    inline RCP<const Basic> get_s() const
    {
        return get_arg1();
    }
    //! \return `a_`
    inline RCP<const Basic> get_a() const
    {
        return get_arg2();
    }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s,
                      const RCP<const Basic> &a) const;
    //! \return canonicalized `zeta`
    RCP<const Basic> create(const RCP<const Basic> &a,
                            const RCP<const Basic> &b) const override;
};

//! Create a new Zeta instance:
SYMENGINE_EXPORT RCP<const Basic> zeta(const RCP<const Basic> &s,
                                       const RCP<const Basic> &a);
SYMENGINE_EXPORT RCP<const Basic> zeta(const RCP<const Basic> &s);

class SYMENGINE_EXPORT Dirichlet_eta : public OneArgFunction
{
    // See http://en.wikipedia.org/wiki/Dirichlet_eta_function
public:
    IMPLEMENT_TYPEID(SYMENGINE_DIRICHLET_ETA)
    //! Dirichlet_eta Constructor
    Dirichlet_eta(const RCP<const Basic> &s);
    //! \return `s`
    inline RCP<const Basic> get_s() const
    {
        return get_arg();
    }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s) const;
    //! Rewrites in the form of zeta
    RCP<const Basic> rewrite_as_zeta() const;
    //! \return Canonicalized zeta
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
    using OneArgFunction::create;
};

//! Create a new Dirichlet_eta instance:
SYMENGINE_EXPORT RCP<const Basic> dirichlet_eta(const RCP<const Basic> &s);

class SYMENGINE_EXPORT FunctionSymbol : public MultiArgFunction
{
protected:
    std::string name_; //! The `f` in `f(x+y, z)`

public:
    IMPLEMENT_TYPEID(SYMENGINE_FUNCTIONSYMBOL)
    //! FunctionSymbol Constructors
    FunctionSymbol(std::string name, const vec_basic &arg);
    FunctionSymbol(std::string name, const RCP<const Basic> &arg);
    //! \return Size of the hash
    hash_t __hash__() const override;
    /*! Equality comparator
     * \param o  Object to be compared with
     * \return whether the 2 objects are equal
     * */
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    //! \return `name_`
    inline const std::string &get_name() const
    {
        return name_;
    }
    //! \return `true` if canonical
    bool is_canonical(const vec_basic &arg) const;
    RCP<const Basic> create(const vec_basic &x) const override;
};

//! Create a new FunctionSymbol instance:
SYMENGINE_EXPORT RCP<const Basic> function_symbol(std::string name,
                                                  const RCP<const Basic> &arg);
SYMENGINE_EXPORT RCP<const Basic> function_symbol(std::string name,
                                                  const vec_basic &arg);

/*! Use this class SYMENGINE_EXPORT to define custom functions by overriding
 *  the defaut behaviour for create, eval, diff, __eq__, compare etc.
 * */

class SYMENGINE_EXPORT FunctionWrapper : public FunctionSymbol
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_FUNCTIONWRAPPER)
    FunctionWrapper(std::string name, const vec_basic &arg);
    FunctionWrapper(std::string name, const RCP<const Basic> &arg);
    RCP<const Basic> create(const vec_basic &v) const override = 0;
    virtual RCP<const Number> eval(long bits) const = 0;
    virtual RCP<const Basic> diff_impl(const RCP<const Symbol> &s) const = 0;
};

/*! Derivative operator
 *  Derivative(f, [x, y, ...]) represents a derivative of `f` with respect to
 *  `x`, `y`, and so on.
 * */
class SYMENGINE_EXPORT Derivative : public Basic
{
private:
    RCP<const Basic> arg_; //! The expression to be differentiated
    // The symbols are declared as Basic (and checked by is_canonical() below),
    // to avoid issues with converting vector<RCP<Symbol>> to
    // vector<RCP<Basic>>, see [1], [2]. The problem is that even though Symbol
    // inherits from Basic, vector<RCP<Symbol>> does not inherit from
    // vector<RCP<Basic>>, so the compiler can't cast the derived type to the
    // base type when calling functions like unified_eq() that are only
    // defined for the base type vector<RCP<Basic>>.
    // [1]
    // http://stackoverflow.com/questions/14964909/how-to-cast-a-vector-of-shared-ptrs-of-a-derived-class-to-a-vector-of-share-ptrs
    // [2]
    // http://stackoverflow.com/questions/114819/getting-a-vectorderived-into-a-function-that-expects-a-vectorbase
    multiset_basic x_; //! x, y, ...

public:
    IMPLEMENT_TYPEID(SYMENGINE_DERIVATIVE)
    Derivative(const RCP<const Basic> &arg, const multiset_basic &x);

    static RCP<const Derivative> create(const RCP<const Basic> &arg,
                                        const multiset_basic &x)
    {
        return make_rcp<const Derivative>(arg, x);
    }

    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    inline RCP<const Basic> get_arg() const
    {
        return arg_;
    }
    inline const multiset_basic &get_symbols() const
    {
        return x_;
    }
    vec_basic get_args() const override
    {
        vec_basic args = {arg_};
        args.insert(args.end(), x_.begin(), x_.end());
        return args;
    }
    bool is_canonical(const RCP<const Basic> &arg,
                      const multiset_basic &x) const;
};

/*! Subs operator
 *  Subs(f, {x1 : x2, y1: y2, ...}) represents `f` after substituting
 *  `x1` with `x2`, `y1` with `y2`, and so on.
 * */
class SYMENGINE_EXPORT Subs : public Basic
{
private:
    RCP<const Basic> arg_;
    map_basic_basic dict_;

public:
    IMPLEMENT_TYPEID(SYMENGINE_SUBS)
    Subs(const RCP<const Basic> &arg, const map_basic_basic &x);

    static RCP<const Subs> create(const RCP<const Basic> &arg,
                                  const map_basic_basic &x)
    {
        return make_rcp<const Subs>(arg, x);
    }

    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    inline const RCP<const Basic> &get_arg() const
    {
        return arg_;
    }
    inline const map_basic_basic &get_dict() const
    {
        return dict_;
    };
    virtual vec_basic get_variables() const;
    virtual vec_basic get_point() const;
    vec_basic get_args() const override;

    bool is_canonical(const RCP<const Basic> &arg,
                      const map_basic_basic &x) const;
};

class SYMENGINE_EXPORT HyperbolicBase : public OneArgFunction
{
public:
    //! Constructor
    HyperbolicBase(RCP<const Basic> arg) : OneArgFunction{arg} {};
};

class SYMENGINE_EXPORT HyperbolicFunction : public HyperbolicBase
{
public:
    //! Constructor
    HyperbolicFunction(RCP<const Basic> arg) : HyperbolicBase{arg} {};
};

class SYMENGINE_EXPORT InverseHyperbolicFunction : public HyperbolicBase
{
public:
    //! Constructor
    InverseHyperbolicFunction(RCP<const Basic> arg) : HyperbolicBase{arg} {};
};

class SYMENGINE_EXPORT Sinh : public HyperbolicFunction
{
    //! The hyperbolic sine function, `\frac{e^x - e^{-x}}{2}`.
public:
    IMPLEMENT_TYPEID(SYMENGINE_SINH)
    //! Sinh Constructor
    Sinh(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized sinh
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Sinh:
SYMENGINE_EXPORT RCP<const Basic> sinh(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Csch : public HyperbolicFunction
{
    //! The hyperbolic cosecant function, `\frac{2}{e^x - e^{-x}}`.
public:
    IMPLEMENT_TYPEID(SYMENGINE_CSCH)
    //! Csch Constructor
    Csch(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized csch
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Csch:
SYMENGINE_EXPORT RCP<const Basic> csch(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Cosh : public HyperbolicFunction
{
    //! The hyperbolic cosine function, `\frac{e^x + e^{-x}}{2}`.
public:
    IMPLEMENT_TYPEID(SYMENGINE_COSH)
    //! Cosh Constructor
    Cosh(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized cosh
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Cosh:
SYMENGINE_EXPORT RCP<const Basic> cosh(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Sech : public HyperbolicFunction
{
    //! The hyperbolic secant function, `\frac{2}{e^x + e^{-x}}`.
public:
    IMPLEMENT_TYPEID(SYMENGINE_SECH)
    //! Sech Constructor
    Sech(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized sech
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Sech:
SYMENGINE_EXPORT RCP<const Basic> sech(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Tanh : public HyperbolicFunction
{
    //! The hyperbolic tangent function, `\frac{\sinh(x)}{\cosh(x)}`.
public:
    IMPLEMENT_TYPEID(SYMENGINE_TANH)
    //! Tanh Constructor
    Tanh(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized tanh
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Tanh:
SYMENGINE_EXPORT RCP<const Basic> tanh(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Coth : public HyperbolicFunction
{
    //! The hyperbolic tangent function, `\frac{\cosh(x)}{\sinh(x)}`.
public:
    IMPLEMENT_TYPEID(SYMENGINE_COTH)
    //! Coth Constructor
    Coth(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized coth
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Coth:
SYMENGINE_EXPORT RCP<const Basic> coth(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ASinh : public InverseHyperbolicFunction
{
    //! The inverse hyperbolic sine function.
public:
    IMPLEMENT_TYPEID(SYMENGINE_ASINH)
    //! ASinh Constructor
    ASinh(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized asinh
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ASinh:
SYMENGINE_EXPORT RCP<const Basic> asinh(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ACsch : public InverseHyperbolicFunction
{
    //! The inverse hyperbolic cosecant function.
public:
    IMPLEMENT_TYPEID(SYMENGINE_ACSCH)
    //! ACsch Constructor
    ACsch(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized acsch
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ACsch:
SYMENGINE_EXPORT RCP<const Basic> acsch(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ACosh : public InverseHyperbolicFunction
{
    //! The inverse hyperbolic cosine function.
public:
    IMPLEMENT_TYPEID(SYMENGINE_ACOSH)
    //! ACosh Constructor
    ACosh(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized acosh
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ACosh:
SYMENGINE_EXPORT RCP<const Basic> acosh(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ATanh : public InverseHyperbolicFunction
{
    //! The inverse hyperbolic tangent function.
public:
    IMPLEMENT_TYPEID(SYMENGINE_ATANH)
    //! ATanh Constructor
    ATanh(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized atanh
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ATanh:
SYMENGINE_EXPORT RCP<const Basic> atanh(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ACoth : public InverseHyperbolicFunction
{
    //! The inverse hyperbolic cotangent function.
public:
    IMPLEMENT_TYPEID(SYMENGINE_ACOTH)
    //! ACoth Constructor
    ACoth(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized acoth
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ACoth:
SYMENGINE_EXPORT RCP<const Basic> acoth(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT ASech : public InverseHyperbolicFunction
{
    //! The inverse hyperbolic secant function.
public:
    IMPLEMENT_TYPEID(SYMENGINE_ASECH)
    //! ASech Constructor
    ASech(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized asech
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize ASech:
SYMENGINE_EXPORT RCP<const Basic> asech(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT KroneckerDelta : public TwoArgFunction
{
    /*! The discrete, or Kronecker, delta function.
     * A function that takes in two integers `i` and `j`. It returns `0` if `i`
     *and `j` are
     * not equal or it returns `1` if `i` and `j` are equal.
     * http://en.wikipedia.org/wiki/Kronecker_delta
     **/
public:
    using TwoArgFunction::create;
    IMPLEMENT_TYPEID(SYMENGINE_KRONECKERDELTA)
    //! KroneckerDelta Constructor
    KroneckerDelta(const RCP<const Basic> &i, const RCP<const Basic> &j);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &i,
                      const RCP<const Basic> &j) const;
    //! \return canonicalized `KroneckerDelta`
    RCP<const Basic> create(const RCP<const Basic> &a,
                            const RCP<const Basic> &b) const override;
};

//! Canonicalize KroneckerDelta:
SYMENGINE_EXPORT RCP<const Basic> kronecker_delta(const RCP<const Basic> &i,
                                                  const RCP<const Basic> &j);

class SYMENGINE_EXPORT LeviCivita : public MultiArgFunction
{
    /*! Represent the Levi-Civita symbol.
     *  For even permutations of indices it returns 1, for odd permutations -1,
     *and
     *  for everything else (a repeated index) it returns 0.
     *
     *  Thus it represents an alternating pseudotensor.
     **/
public:
    IMPLEMENT_TYPEID(SYMENGINE_LEVICIVITA)
    //! LeviCivita Constructor
    LeviCivita(const vec_basic &&arg);
    //! \return `true` if canonical
    bool is_canonical(const vec_basic &arg) const;
    //! \return canonicalized Max
    RCP<const Basic> create(const vec_basic &arg) const override;
};

//! Canonicalize LeviCivita:
SYMENGINE_EXPORT RCP<const Basic> levi_civita(const vec_basic &arg);

class SYMENGINE_EXPORT Erf : public OneArgFunction
{
    /*   The Gauss error function. This function is defined as:
     *
     *   .. math::
     *      \mathrm{erf}(x) = \frac{2}{\sqrt{\pi}} \int_0^x e^{-t^2}
     *\mathrm{d}t.
     **/
public:
    IMPLEMENT_TYPEID(SYMENGINE_ERF)
    //! Erf Constructor
    Erf(const RCP<const Basic> &arg) : OneArgFunction(arg)
    {
        SYMENGINE_ASSIGN_TYPEID()
        SYMENGINE_ASSERT(is_canonical(arg))
    }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized erf
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Erf:
SYMENGINE_EXPORT RCP<const Basic> erf(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Erfc : public OneArgFunction
{
    /*   The complementary error function. This function is defined as:
     *
     *   .. math::
     *      \mathrm{erfc}(x) = 1 - \frac{2}{\sqrt{\pi}} \int_0^x e^{-t^2}
     *	 	 \mathrm{d}t.
     **/
public:
    IMPLEMENT_TYPEID(SYMENGINE_ERFC)
    //! Erfc Constructor
    Erfc(const RCP<const Basic> &arg) : OneArgFunction(arg)
    {
        SYMENGINE_ASSIGN_TYPEID()
        SYMENGINE_ASSERT(is_canonical(arg))
    }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized erfc
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Erfc:
SYMENGINE_EXPORT RCP<const Basic> erfc(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Gamma : public OneArgFunction
{
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
public:
    IMPLEMENT_TYPEID(SYMENGINE_GAMMA)
    //! Gamma Constructor
    Gamma(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized gamma
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Gamma:
SYMENGINE_EXPORT RCP<const Basic> gamma(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT LowerGamma : public TwoArgFunction
{
    //! The lower incomplete gamma function.
public:
    using TwoArgFunction::create;
    IMPLEMENT_TYPEID(SYMENGINE_LOWERGAMMA)
    //! LowerGamma Constructor
    LowerGamma(const RCP<const Basic> &s, const RCP<const Basic> &x);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s,
                      const RCP<const Basic> &x) const;
    //! \return canonicalized `LowerGamma`
    RCP<const Basic> create(const RCP<const Basic> &a,
                            const RCP<const Basic> &b) const override;
};

//! Canonicalize LowerGamma:
SYMENGINE_EXPORT RCP<const Basic> lowergamma(const RCP<const Basic> &s,
                                             const RCP<const Basic> &x);

class SYMENGINE_EXPORT UpperGamma : public TwoArgFunction
{
    //! The upper incomplete gamma function.
public:
    using TwoArgFunction::create;
    IMPLEMENT_TYPEID(SYMENGINE_UPPERGAMMA)
    //! UpperGamma Constructor
    UpperGamma(const RCP<const Basic> &s, const RCP<const Basic> &x);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s,
                      const RCP<const Basic> &x) const;
    //! \return canonicalized `UpperGamma`
    RCP<const Basic> create(const RCP<const Basic> &a,
                            const RCP<const Basic> &b) const override;
};

//! Canonicalize UpperGamma:
SYMENGINE_EXPORT RCP<const Basic> uppergamma(const RCP<const Basic> &s,
                                             const RCP<const Basic> &x);

class SYMENGINE_EXPORT LogGamma : public OneArgFunction
{
    /*!    The loggamma function
        The `loggamma` function implements the logarithm of the
        gamma function i.e, `\log\Gamma(x)`.
     **/
public:
    IMPLEMENT_TYPEID(SYMENGINE_LOGGAMMA)
    //! LogGamma Constructor
    LogGamma(const RCP<const Basic> &arg) : OneArgFunction(arg)
    {
        SYMENGINE_ASSIGN_TYPEID()
        SYMENGINE_ASSERT(is_canonical(arg))
    }
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    RCP<const Basic> rewrite_as_gamma() const;
    //! \return canonicalized loggamma
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize LogGamma:
SYMENGINE_EXPORT RCP<const Basic> loggamma(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Beta : public TwoArgFunction
{
    /*!    The beta function, also called the Euler integral
     *     of the first kind, is a special function defined by
     *
     *   .. math::
     *      \Beta(x, y) := \int^{1}_{0} t^{x-1} (1-t)^{y-1} \mathrm{d}t.
     **/
public:
    using TwoArgFunction::create;
    IMPLEMENT_TYPEID(SYMENGINE_BETA)
    //! Beta Constructor
    Beta(const RCP<const Basic> &x, const RCP<const Basic> &y)
        : TwoArgFunction(x, y)
    {
        SYMENGINE_ASSIGN_TYPEID()
        SYMENGINE_ASSERT(is_canonical(x, y))
    }
    //! return `Beta` with ordered arguments
    static RCP<const Beta> from_two_basic(const RCP<const Basic> &x,
                                          const RCP<const Basic> &y);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &s, const RCP<const Basic> &x);
    RCP<const Basic> rewrite_as_gamma() const;
    //! \return canonicalized `Beta`
    RCP<const Basic> create(const RCP<const Basic> &a,
                            const RCP<const Basic> &b) const override;
};

//! Canonicalize Beta:
SYMENGINE_EXPORT RCP<const Basic> beta(const RCP<const Basic> &x,
                                       const RCP<const Basic> &y);

class SYMENGINE_EXPORT PolyGamma : public TwoArgFunction
{
    /*!    The polygamma function
     *
     *     It is a meromorphic function on `\mathbb{C}` and defined as the
     *(n+1)-th
     *     derivative of the logarithm of the gamma function:
     *
     *  .. math::
     *  \psi^{(n)} (z) := \frac{\mathrm{d}^{n+1}}{\mathrm{d} z^{n+1}}
     *\log\Gamma(z).
     **/
public:
    using TwoArgFunction::create;
    IMPLEMENT_TYPEID(SYMENGINE_POLYGAMMA)
    //! PolyGamma Constructor
    PolyGamma(const RCP<const Basic> &n, const RCP<const Basic> &x)
        : TwoArgFunction(n, x)
    {
        SYMENGINE_ASSIGN_TYPEID()
        SYMENGINE_ASSERT(is_canonical(n, x))
    }
    bool is_canonical(const RCP<const Basic> &n, const RCP<const Basic> &x);
    RCP<const Basic> rewrite_as_zeta() const;
    //! \return canonicalized `PolyGamma`
    RCP<const Basic> create(const RCP<const Basic> &a,
                            const RCP<const Basic> &b) const override;
};

//! Canonicalize PolyGamma
SYMENGINE_EXPORT RCP<const Basic> polygamma(const RCP<const Basic> &n,
                                            const RCP<const Basic> &x);

SYMENGINE_EXPORT RCP<const Basic> digamma(const RCP<const Basic> &x);

SYMENGINE_EXPORT RCP<const Basic> trigamma(const RCP<const Basic> &x);

class SYMENGINE_EXPORT Abs : public OneArgFunction
{
    /*!    The absolute value function
     **/
public:
    IMPLEMENT_TYPEID(SYMENGINE_ABS)
    //! Abs Constructor
    Abs(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return canonicalized abs
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

//! Canonicalize Abs:
SYMENGINE_EXPORT RCP<const Basic> abs(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT Max : public MultiArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_MAX)
    //! Max Constructor
    Max(const vec_basic &&arg);
    //! \return `true` if canonical
    bool is_canonical(const vec_basic &arg) const;
    //! \return canonicalized Max
    RCP<const Basic> create(const vec_basic &arg) const override;
};

//! Canonicalize Max:
SYMENGINE_EXPORT RCP<const Basic> max(const vec_basic &arg);

class SYMENGINE_EXPORT Min : public MultiArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_MIN)
    //! Min Constructor
    Min(const vec_basic &&arg);
    //! \return `true` if canonical
    bool is_canonical(const vec_basic &arg) const;
    //! \return canonicalized Max
    RCP<const Basic> create(const vec_basic &arg) const override;
};

//! Canonicalize Min:
SYMENGINE_EXPORT RCP<const Basic> min(const vec_basic &arg);

//! \return simplified form if possible
SYMENGINE_EXPORT RCP<const Basic> trig_to_sqrt(const RCP<const Basic> &arg);

class SYMENGINE_EXPORT UnevaluatedExpr : public OneArgFunction
{
public:
    IMPLEMENT_TYPEID(SYMENGINE_UNEVALUATED_EXPR)
    //! UnevaluatedExpr Constructor
    UnevaluatedExpr(const RCP<const Basic> &arg);
    //! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg) const;
    //! \return Canonicalized UnevaluatedExpr
    RCP<const Basic> create(const RCP<const Basic> &arg) const override;
};

SYMENGINE_EXPORT RCP<const Basic> unevaluated_expr(const RCP<const Basic> &arg);

} // namespace SymEngine

#endif
