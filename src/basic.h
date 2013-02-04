#ifndef CSYMPY_BASIC_H
#define CSYMPY_BASIC_H

// Include all C++ headers here:
#include <cstddef>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <unordered_map>

// Include all Teuchos headers here:
#include "Teuchos_RCP.hpp"
#include "Teuchos_TypeNameTraits.hpp"

namespace CSymPy {

/*
    Any Basic class can be used in a "dictionary", due to the methods:
        __hash__()
        __eq__(o)
    Sublcasses must implement these.
*/
class Basic {
public:
    // Implements the hash of the given CSymPy class.
    // Use std::hash to get the hash. Example:
    //     RCP<Symbol> x = rcp(new Symbol("x"));
    //     std::hash<Basic> hash_fn;
    //     std::cout << hash_fn(*x);
    virtual std::size_t __hash__() const = 0;

    // true if "this" is equal to "o".
    virtual bool __eq__(const Basic &o) const = 0;

    // Returns string representation of self. Subclasses can override this to
    // provide custom printing.
    virtual std::string __str__() const {
        std::ostringstream s;
        s << "<" << Teuchos::typeName<Basic>(*this)
            << " instance at " << (const void*)this << ">";
        return s.str();
    }

    bool operator==(const Basic &o) const {
        return this->__eq__(o);
    }

    bool operator!=(const Basic &o) const {
        return !(this->__eq__(o));
    }
};

// Returns true if "b" is of type T or any of its subclasses. Example:
//     is_a<Symbol>(b)  // true if "b" is of type Symbol any Symbol's subclass
template <class T>
inline bool is_a(const Basic &b)
{
    return dynamic_cast<const T *>(&b) != NULL;
}


} // CSymPy

// This "<<" overloaded function simply calls p.__str__, so it allows any Basic
// type to be printed.
// This prints using: std::cout << *x;
inline std::ostream& operator<<(std::ostream& out, const CSymPy::Basic& p)
{
    out << p.__str__();
    return out;
}
// This prints using: std::cout << x;
inline std::ostream& operator<<(std::ostream& out,
        const Teuchos::RCP<CSymPy::Basic>& p)
{
    out << p->__str__();
    return out;
}

namespace std
{
    // Specialise std::hash for Basic. We just call Basic.__hash__()
    template<>
    class hash<CSymPy::Basic> : public unary_function<CSymPy::Basic, size_t>
    {
    public:
        size_t operator()(const CSymPy::Basic& b)
        {
            return b.__hash__();
        }
    };
}

// Standard hash_combine() function. Example of usage:
//
//    std::size_t seed1 = 0;
//    hash_combine<std::string>(seed1, "x");
//    hash_combine<std::string>(seed1, "y");
//
// You can use it with any CSymPy class:
//
//    RCP<Symbol> x = rcp(new Symbol("x"));
//    RCP<Symbol> y = rcp(new Symbol("y"));
//    std::size_t seed2 = 0;
//    hash_combine<Basic>(seed2, *x);
//    hash_combine<Basic>(seed2, *y);
//
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}



#endif
