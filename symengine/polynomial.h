/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UnivariatePolynomial and Polynomial
 *
 **/
#ifndef SYMENGINE_POLYNOMIALS_H
#define SYMENGINE_POLYNOMIALS_H

#include <symengine/monomials.h>
#include <symengine/dict.h>
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/symbol.h>

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

inline RCP<const UnivariatePolynomial> univariate_polynomial(RCP<const Symbol> i, unsigned int deg, map_uint_mpz&& dict)
{
    return make_rcp<const UnivariatePolynomial>(i, deg, std::move(dict));
}

 class vec_hash;
 class sym_hash;
 
class MultivariatePolynomial : public Basic{
public:
    //vars: set of variables for th polynomial
    //degrees: max degrees of the symbols
    //dict: dictionary for sparse represntation of polynomial, x**1 * y**2 + 3 * x**4 * y ** 5
    // is represented as {(1,2):1,(4,5):3}
    std::set<Symbol> vars_;
    std::unordered_map<Symbol, unsigned int,sym_hash> degrees_;
    std::unordered_map<std::vector<unsigned int>, mpz_class, vec_hash> dict_;  //map_uintvec_mpz
public:
    //constructor from components
    MultivariatePolynomial(std::unordered_map<Symbol, unsigned int> &degrees, std::set<Symbol> &var, std::map<std::vecotr<unsigned int>, mpz_class> &dict);
    bool is_canonical(std::unorderd_map<Symbol, unsigened int> &degrees, std::map<std::vector<unsigned int>,mpz_class> &dict);
    std::size_t __hash__();
    bool __eq__(const Basic &o);
    int compare(const Basic &o);
    mpz_class eval(std::map<Symbol, mpz_class> &vals);   
};

RCP<const MultivariatePolynomial> add_mult_poly(const MultivariatePolynomial &a, const MultivariatePolynomial &b);
RCP<const MultivariatePolynomial> neg_mult_poly(const MultivariatePolynomial &a);
RCP<const MultivariatePolynomial> sub_mult_poly(const MultivariatePolynomial &a, const MultivariatePolynomial &b);
RCP<const MultivariatePolynomial> mul_mult_poly(const MultivariatePolynomial &a, const MultivariatePolynomial &b);

class vec_hash{
public:
  size_t operator()(std::vector<unsigned int> &v) const{
    unsigned int count = 0;
    for(int i = 0; i < v.size(); i++){
        count ^= v[i]
    }
    return count;
    }  
};

class sym_hash{
public:
  size_t operator()(Symbol &s) const{
    return s.__hash__();
  }
}
 
//transfer to dict.cpp before issueing pull request
 bool map_uintvec_mpz_eq(const std::unordered_map<std::vector<unsigned int>, mpz_class, vec_hash> &a, const std::unordered_map<std::vector<unsigned int>, mpz_class, vec_hash> &b){
   //same logic as map_uint_mpz_eq
   if (a.size() != b.size())
     return false;
   for(const auto &p : a){
     auto f = b.find(p.first);
     if (f == b.end()) return false; // no such element in "b"
     if (p.second != f->second) return false; // values not equal
   }
   return true;
}

 bool map_uintvec_mpz_compare(const std::unordered_map<std::vector<unsigned int>, mpz_class, vec_hash> &a, const std::unordered_map<std::vector<unsigned int>, mpz_class, vec_hash> &b){
    //copied from map_uinit_mpz_compare
    if (A.size() != B.size())
        return (A.size() < B.size()) ? -1 : 1;
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (a->first != b->first)
            return (a->first < b->first) ? -1 : 1;
        if (a->second != b->second)
            return (a->second < b->second) ? -1 : 1;
    }
    return 0;
 }
 
}  //SymEngine

#endif
