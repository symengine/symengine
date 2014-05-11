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

/*! \return `true` if `arg` is of form `theta + n*pi/12`
 * `n` is the n in `n*pi/12`
 * `x` is `theta`
 * */
bool get_pi_shift(const RCP<const Basic> &arg,
			  RCP<const Integer> &n,
			  RCP<const Basic> &x);

//! \return `sqrt` of the `arg`
RCP<const Basic> sqrt(const RCP<const Basic> &arg);
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

class Tan : public Function {
private:
    RCP<const Basic> arg_; //! The `arg` in `tan(arg)`

public:
    //! Tan Constructor
    Tan(const RCP<const Basic> &arg);
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
//! Canonicalize Tan:
RCP<const Basic> tan(const RCP<const Basic> &arg);

class Cot : public Function {
private:
    RCP<const Basic> arg_; //! The `arg` in `cot(arg)`

public:
    //! Cot Constructor
    Cot(const RCP<const Basic> &arg);
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
//! Canonicalize Cot:
RCP<const Basic> cot(const RCP<const Basic> &arg);

class Csc: public Function {
private:
    RCP<const Basic> arg_; //! The `arg` in `Csc(arg)`

public:
    //! Csc Constructor
    Csc(const RCP<const Basic> &arg);
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
//! Canonicalize Csc:
RCP<const Basic> csc(const RCP<const Basic> &arg);

class Sec: public Function {
private:
    RCP<const Basic> arg_; //! The `arg` in `sec(arg)`

public:
    //! Sec Constructor
    Sec(const RCP<const Basic> &arg);
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
//! Canonicalize Sec:
RCP<const Basic> sec(const RCP<const Basic> &arg);


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
    bool is_canonical(const RCP<const Basic> &arg, const vec_basic &x) const;
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

} // CSymPy

#endif
