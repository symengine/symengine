/**
 *  \file basic.h
 *  Includes the base class
 *
 **/

#ifndef CSYMPY_BASIC_H
#define CSYMPY_BASIC_H

// Include all C++ headers here:
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <atomic>

#include "csympy_config.h"
#include "csympy_assert.h"
#include "csympy_rcp.h"
#include "dict.h"

namespace CSymPy {

class Symbol;

/*!
    Any Basic class can be used in a "dictionary", due to the methods:
        
        __hash__()
        __eq__(o)
    Sublcasses must implement these.

*/
/*    Classes like Add, Mul, Pow are initialzied through their constructor using
    their internal representation. Add, Mul have a 'coeff' and 'dict', while
    Pow hase 'base' and 'exp'. There are restrictions on what 'coeff' and
    'dict' can be (for example 'coeff' cannot be zero in Mul, and if Mul is
    used inside Add, then Mul's coeff must be one, etc.). All these
    restrictions are checked when CSYMPY_ASSERT is enabled inside the
    constructors using the is_canonical() method. That way, you don't have to
    worry about creating Add/Mul/Pow with wrong arguments, as it will be caught
    be the tests. In the Release mode no checks are done, so you can construct
    Add/Mul/Pow very quickly. The idea is that depending on the algorithm, you
    sometimes know that things are already canonical, so you simply pass it
    directly to Add/Mul/Pow and you avoid expensive type checking and
    canonicalization. At the same time, you need to make sure that tests are
    still running with CSYMPY_ASSERT enabled, so that Add/Mul/Pow are never in
    an inconsistent state.

    Summary: always try to construct the expressions Add/Mul/Pow directly using
    their constructors and all the knowledge that you have for the given
    algorithm, that way things will be very fast. If you want slower but
    simpler code, you can use the add(), mul(), pow() functions that peform
    general and possibly slow canonicalization first.
*/
class Basic {
private:
    //! Private variables
    // The hash_ is defined as mutable, because its value is initialized to 0
    // in the constructor and then it can be changed in Basic::hash() to the
    // current hash (which is always the same for the given instance). The
    // state of the instance does not change, so we define hash_ as mutable.
#if defined(WITH_CSYMPY_THREAD_SAFE)
    mutable std::atomic<std::size_t> hash_; // This holds the hash value
#else
    mutable std::size_t hash_; // This holds the hash value
#endif // WITH_CSYMPY_THREAD_SAFE
#if defined(WITH_CSYMPY_RCP)
public:
    //! Public variables if defined with CSYMPY_RCP
    // The reference counter is defined either as "unsigned int" (faster, but
    // not thread safe) or as std::atomic<unsigned int> (slower, but thread
    // safe). Semantically they are almost equivalent, except that the
    // pre-decrement operator `operator--()` returns a copy for std::atomic
    // instead of a reference to itself.
    // The refcount_ is defined as mutable, because it does not change the
    // state of the instance, but changes when more copies
    // of the same instance are made.
#if defined(WITH_CSYMPY_THREAD_SAFE)
    mutable std::atomic<unsigned int> refcount_; // reference counter
#else
    mutable unsigned int refcount_; // reference counter
#endif // WITH_CSYMPY_THREAD_SAFE
#endif // WITH_CSYMPY_RCP
public:
    //! Constructor
    Basic() : hash_{0}
#if defined(WITH_CSYMPY_RCP)
        , refcount_(0)
#endif
        {}
    //! Destructor must be explicitly defined as virtual here to avoid problems
    //! with undefined behavior while deallocating derived classes.
    virtual ~Basic() {}

    //! Delete the copy constructor and assignment
    Basic(const Basic&) = delete;
    //! Assignment operator in continuation with above
    Basic& operator=(const Basic&) = delete;

    //! Delete the move constructor and assignment
    Basic(Basic&&) = delete;
    //! Assignment operator in continuation with above
    Basic& operator=(Basic&&) = delete;

    /*!  Implements the hash of the given CSymPy class.
         Use `std::hash` to get the hash. Example:
             RCP<const Symbol> x = rcp(new Symbol("x"));
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

    /*! Returns -1, 0, 1 for `this < o, this == o, this > o`. This method is used
     when you want to sort things like `x+y+z` into canonical order. This
     function assumes that `o` is the same type as `this`. Use ` __cmp__` if you
     want general comparison.
     */
    virtual int compare(const Basic &o) const = 0;

    /*! Returns string representation of `self`. Subclasses can override this to
     provide custom printing.
     */
    virtual std::string __str__() const;

    //! Returns the derivative of self
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const {
        throw std::runtime_error("Not implemented.");
    }

    //! Substitutes 'subs_dict' into 'self'.
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;

    //! expands the special function in terms of exp function
    virtual RCP<const Basic> expand_as_exp() const {
        throw std::runtime_error("Not implemented.");
    }
};

//! Our hash:
struct RCPBasicHash {
    //! Returns the hashed value.
    long operator() (const RCP<const Basic> &k) const {
        return k->hash();
    }
};

//! Our comparison `(==)`
struct RCPBasicKeyEq {
    //! Comparison Operator `==`
    bool operator() (const RCP<const Basic> &x, const RCP<const Basic> &y) const {
        return x->__eq__(*y);
    }
};

//! Our less operator `(<)`:
struct RCPBasicKeyLess {
    //! true if `x < y`, false otherwise
    bool operator() (const RCP<const Basic> &x, const RCP<const Basic> &y) const {
        std::size_t xh=x->hash(), yh=y->hash();
        if (xh != yh) return xh < yh;
        if (x->__eq__(*y)) return false;
        return x->__cmp__(*y) == -1;
    }
};

// Convenience functions
//! Checks equality for `a` and `b` 
bool eq(const RCP<const Basic> &a, const RCP<const Basic> &b);

//! Checks inequality for `a` and `b` 
bool neq(const RCP<const Basic> &a, const RCP<const Basic> &b);

/*! Returns true if `b` is exactly of type `T`. Example:
  `is_a<Symbol>(b)` : true if "b" is of type Symbol
*/
template <class T> bool is_a(const Basic &b);

/*! Returns true if `b` is of type T or any of its subclasses. 
 * Example:
     
        is_a_sub<Symbol>(b)  // true if `b` is of type `Symbol` or any Symbol's subclass 
*/
template <class T>
bool is_a_sub(const Basic &b);

//! Expands `self`
RCP<const Basic> expand(const RCP<const Basic> &self);

} // CSymPy

/*! This `<<` overloaded function simply calls `p.__str__`, so it allows any Basic
    type to be printed.

    This prints using: `std::cout << *x;`
*/
std::ostream& operator<<(std::ostream& out, const CSymPy::Basic& p);

//! Specialise `std::hash` for Basic.
namespace std {
    template<> struct hash<CSymPy::Basic>;
}

/*! Standard `hash_combine()` function. Example of usage:

        std::size_t seed1 = 0;
        hash_combine<std::string>(seed1, "x");
        hash_combine<std::string>(seed1, "y");

     You can use it with any CSymPy class:

        
        RCP<const Symbol> x = rcp(new Symbol("x"));
        RCP<const Symbol> y = rcp(new Symbol("y"));
        std::size_t seed2 = 0;
        hash_combine<Basic>(seed2, *x);
        hash_combine<Basic>(seed2, *y);
*/
template <class T>
void hash_combine(std::size_t& seed, const T& v);

//! Inline members and functions
#include "basic-inl.h"

#endif

