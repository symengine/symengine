/**
 *  \file symbol.h
 *  Class Symbol
 *
 **/
#ifndef SYMENGINE_SYMBOL_H
#define SYMENGINE_SYMBOL_H

#include "basic.h"

namespace SymEngine {

class Symbol : public Basic {
private:
    //! name of Symbol
    std::string name_;

public:
    IMPLEMENT_TYPEID(SYMBOL)
    //! Symbol Constructor
    Symbol(const std::string &name);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    
    /*! Comparison operator
     * \param o - Object to be compared with
     * \return `0` if equal, `-1` , `1` according to string compare
     * */
    virtual int compare(const Basic &o) const;
    //! \return name of the Symbol.
    inline std::string get_name() const {
        return name_;
    }
    /*! Differentiate w.r.t other symbol.
     * \param x - Symbol to be differentiated with.
     * \return `1` if `name_` are equal, else `0`
     * */
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;

    virtual vec_basic get_args() const { return {}; }

    virtual void accept(Visitor &v) const;
};

//! inline version to return `Symbol`
inline RCP<const Symbol> symbol(const std::string &name)
{
    return rcp(new Symbol(name));
}

} // SymEngine

#endif
