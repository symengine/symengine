/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UnivariatePolynomial and Polynomial
 **/
#ifndef SYMENGINE_POLYNOMIALS_H
#define SYMENGINE_POLYNOMIALS_H

#include <symengine/monomials.h>
#include <symengine/dict.h>
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/symbol.h>
#include <symengine/expression.h>

namespace SymEngine {
//! UnivariatePolynomial Class
class UnivariatePolynomial : public Basic{
public:
    //! `degree` : Degree of UnivariatePolynomial
    //! `var_` : Variable of the uni-variate UnivariatePolynomial
    //! `dict_` : holds the UnivariatePolynomial
    // UnivariatePolynomial x**2 + 2*x + 1 has dict_ = {{0, 1}, {1, 2}, {2, 1}} with var_ = "x"
    unsigned int degree_;
    RCP<const Symbol> var_;
    map_uint_mpz dict_;
public:
    IMPLEMENT_TYPEID(UNIVARIATEPOLYNOMIAL)
    //! Constructor of UnivariatePolynomial class
    UnivariatePolynomial(const RCP<const Symbol> &var, const unsigned int &degree, map_uint_mpz&& dict);
    //! Constructor using a dense vector of mpz_class coefficients
    UnivariatePolynomial(const RCP<const Symbol> &var, const std::vector<mpz_class> &v);

    static RCP<const UnivariatePolynomial> create(const RCP<const Symbol> &var,
            const std::vector<mpz_class> &v) {
        return make_rcp<const UnivariatePolynomial>(var, v);
    }

    //! \return true if canonical
    bool is_canonical(const unsigned int &degree, const map_uint_mpz& dict) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;
    std::string __str__() const;
    
    /*! Creates appropriate instance (i.e Symbol, Integer,
    * Mul, Pow, UnivariatePolynomial) depending on the size of dictionary `d`.
    */
    static RCP<const Basic> from_dict(const RCP<const Symbol> &var, map_uint_mpz &&d);
    /*!
    * Adds coef*var_**n to the dict_
    */
    static void dict_add_term(map_uint_mpz &d,
            const mpz_class &coef, const unsigned int &n);
    mpz_class max_coef() const;
    //! Evaluates the UnivariatePolynomial at value x
    mpz_class eval(const mpz_class &x) const;
    //! Evaluates the UnivariatePolynomial at value 2**x
    mpz_class eval_bit(const int &x) const;

    //! \return `true` if `0`
    bool is_zero() const;
    //! \return `true` if `1`
    bool is_one() const;
    //! \return `true` if `-1`
    bool is_minus_one() const;
    //! \return `true` if integer
    bool is_integer() const;
    //! \return `true` if symbol
    bool is_symbol() const;
    //! \return `true` if mul
    bool is_mul() const;
    //! \return `true` if pow
    bool is_pow() const;

    virtual vec_basic get_args() const;

    inline RCP<const Symbol> get_var() const {
        return var_;
    }
    inline const map_uint_mpz& get_dict() const {
        return dict_;
    };

}; //UnivariatePolynomial

//! Adding two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> add_uni_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b);
//! Negative of a UnivariatePolynomial
RCP<const UnivariatePolynomial> neg_uni_poly(const UnivariatePolynomial &a);
//! Subtracting two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> sub_uni_poly(const UnivariatePolynomial &a, const UnivariatePolynomial &b);
//! Multiplying two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> mul_uni_poly(RCP<const UnivariatePolynomial> a, RCP<const UnivariatePolynomial> b);

inline RCP<const UnivariatePolynomial>  pow_exp(RCP<const UnivariatePolynomial> a, RCP<const UnivariatePolynomial> b);

inline RCP<const UnivariatePolynomial> univariate_polynomial(RCP<const Symbol> i, unsigned int deg, map_uint_mpz&& dict)
{
    return make_rcp<const UnivariatePolynomial>(i, deg, std::move(dict));
}

class UnivariateExprPolynomial {
private:
    RCP<const UnivariatePolynomial> upoly;
public:
    // IMPLEMENT_TYPEID(UNIVARIATEEXPRPOLYNOMIAL)
   
    //! Construct UnivariateExprPolynomial from UnivariatePolynomial
#if defined(HAVE_SYMENGINE_IS_CONSTRUCTIBLE)
    template <typename T, typename = typename std::enable_if<std::is_constructible<RCP<const UnivariatePolynomial>, T &&>::value>::type>
#else
    template <typename T>
#endif
    UnivariateExprPolynomial(T &&o) : upoly(std::forward<T>(o)) {}
    UnivariateExprPolynomial(const UnivariateExprPolynomial &) = default;
    UnivariateExprPolynomial(UnivariateExprPolynomial &&other) SYMENGINE_NOEXCEPT : upoly(std::move(other.upoly)) {}
    UnivariateExprPolynomial &operator=(const UnivariateExprPolynomial &) = default;
    UnivariateExprPolynomial &operator=(UnivariateExprPolynomial &&other) SYMENGINE_NOEXCEPT 
    {
        if (this != &other)
            this->upoly.ptr() = std::move(other.upoly.ptr());
        return *this;
    }
  
    ~UnivariateExprPolynomial() SYMENGINE_NOEXCEPT {}
     
    friend std::ostream &operator<<(std::ostream &os, const UnivariateExprPolynomial &expr)
    {
        os << expr.upoly.ptr()->__str__();
        return os;
    }
    
    friend UnivariateExprPolynomial operator+(const UnivariateExprPolynomial &a, const UnivariateExprPolynomial &b)
    {
        return UnivariateExprPolynomial(add_uni_poly(*(a.upoly.ptr()), *(b.upoly.ptr())));
    }
    
    UnivariateExprPolynomial &operator+=(const UnivariateExprPolynomial &other)
    {
        upoly = add_uni_poly(*(upoly.ptr()), *(other.upoly.ptr()));
        return *this;
    }
    
    friend UnivariateExprPolynomial operator-(const UnivariateExprPolynomial &a, const UnivariateExprPolynomial &b)
    {
        return UnivariateExprPolynomial(sub_uni_poly(*(a.upoly.ptr()), *(b.upoly.ptr())));
    }
    
    UnivariateExprPolynomial operator-() const
    {
        UnivariateExprPolynomial retval(*this);
        neg_uni_poly(*(retval.upoly.ptr()));
        return retval;
    }
    
    UnivariateExprPolynomial &operator-=(const UnivariateExprPolynomial &other)
    {
        upoly = sub_uni_poly(*(upoly.ptr()), *(other.upoly.ptr()));
        return *this;
    }
    
    friend UnivariateExprPolynomial operator*(const UnivariateExprPolynomial &a, const UnivariateExprPolynomial &b)
    {   
          return UnivariateExprPolynomial(mul_uni_poly(a.upoly, b.upoly));
    }   
    
    UnivariateExprPolynomial &operator*=(const UnivariateExprPolynomial &other)
    {   
        upoly = mul_uni_poly(upoly, other.upoly);
        return *this;
    }   
    
    bool operator==(const UnivariateExprPolynomial &other) const
    {   
        return eq(*upoly, *other.upoly);
    }   
    
    bool operator!=(const UnivariateExprPolynomial &other) const
    {   
        return not (*this == other);
    }   
    //! Method to get UnivariatePolynomial from UnivariateExprPolynomial
    const RCP<const UnivariatePolynomial> &get_univariate_poly() const
    {   
        return upoly;
    }   
 
}; //UnivariateExprPolynomial


class sym_hash{
public:
  size_t operator()(const Symbol &s) const{
    return s.__hash__();
  }
};

class sym_compare{
  public:
  size_t operator()(const Symbol &a, const Symbol &b){
    return a.compare(b);
  }
};

class sym_eq{
 public:
  bool operator()(const Symbol &a, const Symbol &b){
    return a.__eq__(b);
  }
};

int umap_vec_mpz_compare(umap_vec_mpz &a, umap_vec_mpz &b){
  if(a.size() < b.size())
    return (a.size() < b.size()) ? -1 : 1;
  return 0;
};

}  //SymEngine

#endif
