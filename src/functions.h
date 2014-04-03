/**
 *  \file functions.h
 *  Includes various trignometric functions
 *
 **/

#ifndef CSYMPY_FUNCTIONS_H
#define CSYMPY_FUNCTIONS_H

#include <gmpxx.h>

#include "basic.h"

namespace CSymPy {

class Function : public Basic {
};

class Sin : public Function {
private:
    RCP<const Basic> arg_; //! The `arg` in `sin(arg)`

public:
	//! Sin Constructor
    Sin(const RCP<const Basic> &arg);
    //! \return Size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    
    virtual int compare(const Basic &o) const;
    //! \return stringify version
    virtual std::string __str__() const;
    //! \return `arg_`
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }
	//! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
	//! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! Substitute with `subs_dict`
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

//! Canonicalize Sin:
RCP<const Basic> sin(const RCP<const Basic> &arg);


class Cos : public Function {
private:
    RCP<const Basic> arg_; //! The `arg` in `cos(arg)`

public:
	//! Cos Constructor
    Cos(const RCP<const Basic> &arg);
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
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }
	//! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
	//! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    //! Substitute with `subs_dict`
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

//! Canonicalize Cos:
RCP<const Basic> cos(const RCP<const Basic> &arg);

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
    inline std::string get_name() const {
        return name_;
    }
    //! \return `arg_`
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }
	//! \return `true` if canonical
    bool is_canonical(const RCP<const Basic> &arg);
	//! Differentiate w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

//! Canonicalize FunctionSymbol :
RCP<const Basic> function_symbol(std::string name,
        const RCP<const Basic> &arg);

} // CSymPy

#endif
