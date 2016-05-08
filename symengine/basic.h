/**
 *  \file basic.h
 *  Includes the base class
 *
 **/

#ifndef SYMENGINE_BASIC_H
#define SYMENGINE_BASIC_H

// Include all C++ headers here:
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include <cassert>
#include <ciso646>
#include <cmath>
#include <complex>
#include <vector>
#include <type_traits>
#include <functional>

#include <symengine/symengine_config.h>

#ifdef WITH_SYMENGINE_THREAD_SAFE
#include <atomic>
#endif

#include <symengine/symengine_assert.h>
#include <symengine/symengine_rcp.h>
#include <symengine/dict.h>

namespace SymEngine
{

class Visitor;
class Symbol;

/*!
    Any Basic class can be used in a "dictionary", due to the methods:

        __hash__()
        __eq__(o)
    Subclasses must implement these.

*/
/*  Classes like Add, Mul, Pow are initialized through their constructor using
    their internal representation. Add, Mul have a 'coeff' and 'dict', while
    Pow has 'base' and 'exp'. There are restrictions on what 'coeff' and
    'dict' can be (for example 'coeff' cannot be zero in Mul, and if Mul is
    used inside Add, then Mul's coeff must be one, etc.). All these
    restrictions are checked when SYMENGINE_ASSERT is enabled inside the
    constructors using the is_canonical() method. That way, you don't have to
    worry about creating Add/Mul/Pow with wrong arguments, as it will be caught
    by the tests. In the Release mode no checks are done, so you can construct
    Add/Mul/Pow very quickly. The idea is that depending on the algorithm, you
    sometimes know that things are already canonical, so you simply pass it
    directly to Add/Mul/Pow and you avoid expensive type checking and
    canonicalization. At the same time, you need to make sure that tests are
    still running with SYMENGINE_ASSERT enabled, so that Add/Mul/Pow are never
   in
    an inconsistent state.

    Summary: always try to construct the expressions Add/Mul/Pow directly using
    their constructors and all the knowledge that you have for the given
    algorithm, that way things will be very fast. If you want slower but
    simpler code, you can use the add(), mul(), pow() functions that peform
    general and possibly slow canonicalization first.
*/

enum TypeID {
#define SYMENGINE_INCLUDE_ALL
#define SYMENGINE_ENUM(type, Class) type,
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
#undef SYMENGINE_INCLUDE_ALL
    // The 'TypeID_Count' returns the number of elements in 'TypeID'. For this
    // to work, do not assign numbers to the elements above (or if you do, you
    // must assign the correct count below).
    TypeID_Count
};

#include "basic-methods.inc"

class Basic : public EnableRCPFromThis<Basic>
{
private:
//! Private variables
// The hash_ is defined as mutable, because its value is initialized to 0
// in the constructor and then it can be changed in Basic::hash() to the
// current hash (which is always the same for the given instance). The
// state of the instance does not change, so we define hash_ as mutable.
#if defined(WITH_SYMENGINE_THREAD_SAFE)
    mutable std::atomic<std::size_t> hash_; // This holds the hash value
#else
    mutable std::size_t hash_; // This holds the hash value
#endif // WITH_SYMENGINE_THREAD_SAFE
public:
    virtual TypeID get_type_code() const = 0;
    //! Constructor
    Basic() : hash_{0}
    {
    }
    //! Destructor must be explicitly defined as virtual here to avoid problems
    //! with undefined behavior while deallocating derived classes.
    virtual ~Basic()
    {
    }

    //! Delete the copy constructor and assignment
    Basic(const Basic &) = delete;
    //! Assignment operator in continuation with above
    Basic &operator=(const Basic &) = delete;

    //! Delete the move constructor and assignment
    Basic(Basic &&) = delete;
    //! Assignment operator in continuation with above
    Basic &operator=(Basic &&) = delete;

    /*!  Implements the hash of the given SymEngine class.
         Use `std::hash` to get the hash. Example:
             RCP<const Symbol> x = symbol("x");
             std::hash<Basic> hash_fn;
             std::cout << hash_fn(*x);
    */
    virtual std::size_t __hash__() const = 0;

    //! This caches the hash:
    std::size_t hash() const;

    //! true if `this` is equal to `o`.
    virtual bool __eq__(const Basic &o) const = 0;

    //! true if `this` is not equal to `o`.
    bool __neq__(const Basic &o) const;

    //! Comparison operator.
    int __cmp__(const Basic &o) const;

    /*! Returns -1, 0, 1 for `this < o, this == o, this > o`. This method is
     used
     when you want to sort things like `x+y+z` into canonical order. This
     function assumes that `o` is the same type as `this`. Use ` __cmp__` if you
     want general comparison.
     */
    virtual int compare(const Basic &o) const = 0;

    /*! Returns string representation of `self`.
     */
    std::string __str__() const;

    //! Substitutes 'subs_dict' into 'self'.
    RCP<const Basic> subs(const map_basic_basic &subs_dict) const;

    //! expands the special function in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const
    {
        throw std::runtime_error("Not implemented.");
    }

    //! Returns the list of arguments
    virtual vec_basic get_args() const = 0;

    SYMENGINE_INCLUDE_METHODS(= 0)
};

//! Our hash:
struct RCPBasicHash {
    //! Returns the hashed value.
    long operator()(const RCP<const Basic> &k) const
    {
        return k->hash();
    }
};

//! Our comparison `(==)`
struct RCPBasicKeyEq {
    //! Comparison Operator `==`
    bool operator()(const RCP<const Basic> &x, const RCP<const Basic> &y) const
    {
        return x->__eq__(*y);
    }
};

//! Our less operator `(<)`:
struct RCPBasicKeyLess {
    //! true if `x < y`, false otherwise
    bool operator()(const RCP<const Basic> &x, const RCP<const Basic> &y) const
    {
        std::size_t xh = x->hash(), yh = y->hash();
        if (xh != yh)
            return xh < yh;
        if (x->__eq__(*y))
            return false;
        return x->__cmp__(*y) == -1;
    }
};

//! Less operator `(<)` using cmp:
struct RCPBasicKeyLessCmp {
    //! true if `x < y`, false otherwise
    bool operator()(const RCP<const Basic> &x, const RCP<const Basic> &y) const
    {
        if (x->__eq__(*y))
            return false;
        return x->__cmp__(*y) == -1;
    }
};

// Convenience functions
//! Checks equality for `a` and `b`
bool eq(const Basic &a, const Basic &b);

//! Checks inequality for `a` and `b`
bool neq(const Basic &a, const Basic &b);

/*! Returns true if `b` is exactly of type `T`. Example:
  `is_a<Symbol>(b)` : true if "b" is of type Symbol
*/
template <class T>
bool is_a(const Basic &b);

/*! Returns true if `b` is of type T or any of its subclasses.
 * Example:

        is_a_sub<Symbol>(b)  // true if `b` is of type `Symbol` or any Symbol's
 subclass
*/
template <class T>
bool is_a_sub(const Basic &b);

//! Returns true if `a` and `b` are exactly the same type `T`.
bool is_same_type(const Basic &a, const Basic &b);

//! Expands `self`
RCP<const Basic> expand(const RCP<const Basic> &self);
void as_numer_denom(const RCP<const Basic> &x,
                    const Ptr<RCP<const Basic>> &numer,
                    const Ptr<RCP<const Basic>> &denom);

/*! This `<<` overloaded function simply calls `p.__str__`, so it allows any
   Basic
    type to be printed.

    This prints using: `std::cout << *x;`
*/
std::ostream &operator<<(std::ostream &out, const SymEngine::Basic &p);

} // SymEngine

//! Specialise `std::hash` for Basic.
namespace std
{
template <>
struct hash<SymEngine::Basic>;
}

/*! Standard `hash_combine()` function. Example of usage:

        std::size_t seed1 = 0;
        hash_combine<std::string>(seed1, "x");
        hash_combine<std::string>(seed1, "y");

     You can use it with any SymEngine class:


        RCP<const Symbol> x = symbol("x");
        RCP<const Symbol> y = symbol("y");
        std::size_t seed2 = 0;
        hash_combine<Basic>(seed2, *x);
        hash_combine<Basic>(seed2, *y);
*/
template <class T>
void hash_combine(std::size_t &seed, const T &v);

//! Inline members and functions
#include "basic-inl.h"

// Macro to define the type_code_id variable and its getter method
#define IMPLEMENT_TYPEID(ID)                                                   \
    /*! Type_code_id shared by all instances */                                \
    const static TypeID type_code_id = ID;                                     \
    /*! Virtual function that gives the type_code_id of the object */          \
    virtual TypeID get_type_code() const                                       \
    {                                                                          \
        return type_code_id;                                                   \
    };                                                                         \
    SYMENGINE_INCLUDE_METHODS(;)

#endif
