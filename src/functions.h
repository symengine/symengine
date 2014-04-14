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
    std::vector<RCP<const Symbol>> x_; //! x, y, ...

public:
    Derivative(const RCP<const Basic> &arg,
            const std::vector<RCP<const Symbol>> &x);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;
    inline RCP<const Basic> get_arg() const {
        return arg_;
    }
    bool is_canonical(const RCP<const Basic> &arg);
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

} // CSymPy

#endif
