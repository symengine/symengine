/**
 *  \file constants.h
 *  Declare all the special constants in this file
 *
 **/

#ifndef SYMENGINE_CONSTANTS_H
#define SYMENGINE_CONSTANTS_H

#include "basic.h"
#include "number.h"
#include "integer.h"
#include "symbol.h"

namespace SymEngine {

class Constant : public Basic {
private:
    //! name of Constant
    std::string name_;

public:
    IMPLEMENT_TYPEID(CONSTANT)
    //! Constant Constructor
    Constant(const std::string &name);
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
    //! \return name of the Constant.
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

//! inline version to return `Constant`
inline RCP<const Constant> constant(const std::string &name)
{
    return rcp(new Constant(name));
}

// Constant Numbers
extern RCP<const Integer> zero;
extern RCP<const Integer> one;
extern RCP<const Integer> minus_one;
extern RCP<const Number> I;

// Symbolic Constants
extern RCP<const Constant> pi;
extern RCP<const Constant> E;

} // SymEngine

#endif
