/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UnivariateIntPolynomial and Polynomial
 **/
#ifndef SYMENGINE_POLYNOMIALS_H
#define SYMENGINE_POLYNOMIALS_H

#include <symengine/expression.h>
#include <symengine/monomials.h>

namespace SymEngine
{

class UnivariateIntPolynomial : public Basic
{
private:
    //! `degree` : Degree of UnivariateIntPolynomial
    //! `var_` : Variable of the uni-variate UnivariateIntPolynomial
    //! `dict_` : holds the UnivariateIntPolynomial
    // UnivariateIntPolynomial x**2 + 2*x + 1 has dict_ = {{0, 1}, {1, 2}, {2,
    // 1}} with var_ = "x"
    unsigned int degree_;
    RCP<const Symbol> var_;
    map_uint_mpz dict_;

public:
    IMPLEMENT_TYPEID(UNIVARIATEINTPOLYNOMIAL)
    //! Constructor of UnivariateIntPolynomial class
    UnivariateIntPolynomial(const RCP<const Symbol> &var,
                            const unsigned int &degree, map_uint_mpz &&dict);
    //! Constructor using a dense vector of integer_class coefficients

    UnivariateIntPolynomial(const RCP<const Symbol> &var,
                            const std::vector<integer_class> &v);

    static RCP<const UnivariateIntPolynomial>
    create(const RCP<const Symbol> &var, const std::vector<integer_class> &v)
    {
        return UnivariateIntPolynomial::from_vec(var, v);
    }

    //! \return true if canonical
    bool is_canonical(const unsigned int &degree,
                      const map_uint_mpz &dict) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    // creates a UnivariateIntPolynomial in cannonical form based on the
    // dictionary.
    static RCP<const UnivariateIntPolynomial>
    from_dict(const RCP<const Symbol> &var, map_uint_mpz &&d);
    // create a UnivariateIntPolynomial from a dense vector of integer_class
    // coefficients
    static RCP<const UnivariateIntPolynomial>
    from_vec(const RCP<const Symbol> &var, const std::vector<integer_class> &v);

    /*!
    * Adds coef*var_**n to the dict_
    */
    integer_class max_abs_coef() const;
    //! Evaluates the UnivariateIntPolynomial at value x
    integer_class eval(const integer_class &x) const;
    //! Evaluates the UnivariateIntPolynomial at value 2**x
    integer_class eval_bit(const int &x) const;

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
    inline unsigned int get_degree() const
    {
        return degree_;
    }
    inline RCP<const Symbol> get_var() const
    {
        return var_;
    }
    inline const map_uint_mpz &get_dict() const
    {
        return dict_;
    }
}; // UnivariateIntPolynomial

//! Adding two UnivariateIntPolynomial a and b
RCP<const UnivariateIntPolynomial> add_poly(const UnivariateIntPolynomial &a,
                                            const UnivariateIntPolynomial &b);
//! Negative of a UnivariateIntPolynomial
RCP<const UnivariateIntPolynomial> neg_poly(const UnivariateIntPolynomial &a);
//! Subtracting two UnivariateIntPolynomial a and b
RCP<const UnivariateIntPolynomial> sub_poly(const UnivariateIntPolynomial &a,
                                            const UnivariateIntPolynomial &b);
//! Multiplying two UnivariateIntPolynomial a and b
RCP<const UnivariateIntPolynomial> mul_poly(const UnivariateIntPolynomial &a,
                                            const UnivariateIntPolynomial &b);

inline RCP<const UnivariateIntPolynomial>
univariate_int_polynomial(RCP<const Symbol> i, map_uint_mpz &&dict)
{
    return UnivariateIntPolynomial::from_dict(i, std::move(dict));
}

class UnivariateExprPolynomial
{
public:
    //! Holds the dictionary for a UnivariatePolynomial
    map_int_Expr dict_;

public:
    UnivariateExprPolynomial()
    {
    }
    ~UnivariateExprPolynomial() SYMENGINE_NOEXCEPT
    {
    }
    UnivariateExprPolynomial(const UnivariateExprPolynomial &) = default;
    UnivariateExprPolynomial(UnivariateExprPolynomial &&other)
        SYMENGINE_NOEXCEPT : dict_(std::move(other.dict_))
    {
    }
    UnivariateExprPolynomial(const int &i)
    {
        if (i != 0)
            dict_ = {{0, Expression(i)}};
    }
    UnivariateExprPolynomial(const std::string &s) : dict_({{1, Expression(1)}})
    {
    }
    UnivariateExprPolynomial(const map_int_Expr &p)
    {
        dict_ = p;
        auto iter = dict_.begin();
        while (iter != dict_.end()) {
            if (Expression(0) == iter->second) {
                auto toErase = iter;
                iter++;
                dict_.erase(toErase);
            } else
                iter++;
        }
    }
    UnivariateExprPolynomial(const Expression &expr)
    {
        if (expr != Expression(0))
            dict_ = {{0, std::move(expr)}};
    }

    UnivariateExprPolynomial &operator=(const UnivariateExprPolynomial &)
        = default;
    UnivariateExprPolynomial &
    operator=(UnivariateExprPolynomial &&other) SYMENGINE_NOEXCEPT
    {
        if (this != &other)
            this->dict_ = std::move(other.dict_);
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const UnivariateExprPolynomial &expr)
    {
        os << expr.dict_;
        return os;
    }

    friend UnivariateExprPolynomial operator+(const UnivariateExprPolynomial &a,
                                              const UnivariateExprPolynomial &b)
    {
        map_int_Expr dict;
        for (const auto &it : a.dict_)
            dict[it.first] = it.second;
        for (const auto &it : b.dict_)
            dict[it.first] += it.second;
        return UnivariateExprPolynomial(dict);
    }

    UnivariateExprPolynomial &operator+=(const UnivariateExprPolynomial &other)
    {
        for (auto &it : other.dict_)
            dict_[it.first] += it.second;
        return *this;
    }

    friend UnivariateExprPolynomial operator-(const UnivariateExprPolynomial &a,
                                              const UnivariateExprPolynomial &b)
    {
        map_int_Expr dict;
        for (const auto &it : a.dict_)
            dict[it.first] = it.second;
        for (const auto &it : b.dict_)
            dict[it.first] -= it.second;
        return UnivariateExprPolynomial(dict);
    }

    UnivariateExprPolynomial operator-() const
    {
        map_int_Expr dict;
        for (auto &it : dict_)
            dict[it.first] = -it.second;
        return UnivariateExprPolynomial(dict);
    }

    UnivariateExprPolynomial &operator-=(const UnivariateExprPolynomial &other)
    {
        for (auto &it : other.dict_)
            dict_[it.first] -= it.second;
        return *this;
    }

    friend UnivariateExprPolynomial operator*(const UnivariateExprPolynomial &a,
                                              const UnivariateExprPolynomial &b)
    {
        if (a.dict_.empty() or b.dict_.empty())
            return UnivariateExprPolynomial({{0, Expression(0)}});

        map_int_Expr dict;
        for (const auto &i1 : a.dict_)
            for (const auto &i2 : b.dict_)
                dict[i1.first + i2.first] += i1.second * i2.second;

        return UnivariateExprPolynomial(dict);
    }

    friend UnivariateExprPolynomial operator/(const UnivariateExprPolynomial &a,
                                              const Expression &b)
    {
        return UnivariateExprPolynomial(a * (1 / b));
    }

    UnivariateExprPolynomial &operator*=(const UnivariateExprPolynomial &other)
    {
        if (dict_.empty())
            return *this;

        //! other is a just constant term
        if (other.dict_.size() == 1
            and other.dict_.find(0) != other.dict_.end()) {
            for (const auto &i1 : dict_)
                for (const auto &i2 : other.dict_)
                    dict_[i1.first + i2.first] = i1.second * i2.second;
            return *this;
        }

        map_int_Expr p;
        for (const auto &i1 : dict_)
            for (const auto &i2 : other.dict_)
                p[i1.first + i2.first] += i1.second * i2.second;
        *this = UnivariateExprPolynomial(p);
        return *this;
    }

    UnivariateExprPolynomial &operator/=(const Expression &other)
    {
        dict_ = (UnivariateExprPolynomial(dict_)
                 * UnivariateExprPolynomial(1 / other))
                    .dict_;
        return *this;
    }

    bool operator==(const UnivariateExprPolynomial &other) const
    {
        return map_int_Expr_compare(dict_, other.dict_) == 0;
    }

    bool operator==(int i) const
    {
        return map_int_Expr_compare(dict_, UnivariateExprPolynomial(i).dict_)
               == 0;
    }

    bool operator!=(const UnivariateExprPolynomial &other) const
    {
        return not(*this == other) != 0;
    }

    /*!
    * Adds coef*var_**n to the dict_
    */
    void dict_add_term(const Expression &coef, const int &n)
    {
        auto it = dict_.find(n);
        if (it == dict_.end())
            dict_[n] = coef;
        // return *this;
    }

    //! Method to get UnivariatePolynomial's dictionary
    const map_int_Expr &get_dict() const
    {
        return dict_;
    }

    int size() const
    {
        return dict_.size();
    }

    bool empty() const
    {
        return dict_.empty();
    }

    std::size_t __hash__() const
    {
        std::size_t seed = UNIVARIATEPOLYNOMIAL;
        for (const auto &it : dict_) {
            std::size_t temp = UNIVARIATEPOLYNOMIAL;
            hash_combine<unsigned int>(temp, it.first);
            hash_combine<Basic>(temp, *(it.second.get_basic()));
            seed += temp;
        }
        return seed;
    }

    const umap_int_basic get_basic() const
    {
        umap_int_basic p;
        for (const auto &it : dict_)
            if (it.second != 0)
                p[it.first] = it.second.get_basic();
        return p;
    }

    int compare(const UnivariateExprPolynomial &other)
    {
        return map_int_Expr_compare(dict_, other.dict_);
    }

    Expression find_cf(int deg) const
    {
        if (dict_.find(deg) != dict_.end())
            return dict_.at(deg);
        else
            return Expression(0);
    }
}; // UnivariateExprPolynomial

class UnivariatePolynomial : public Basic
{
private:
    int degree_;
    RCP<const Symbol> var_;
    UnivariateExprPolynomial dict_;

public:
    IMPLEMENT_TYPEID(UNIVARIATEPOLYNOMIAL)
    //! Constructor of UnivariatePolynomial class
    UnivariatePolynomial(const RCP<const Symbol> &var, const int &degree,
                         const map_int_Expr &&dict);
    //! Constructor using a dense vector of Expression
    UnivariatePolynomial(const RCP<const Symbol> &var,
                         const std::vector<Expression> &v);

    static RCP<const UnivariatePolynomial>
    create(const RCP<const Symbol> &var, const std::vector<Expression> &v)
    {
        return UnivariatePolynomial::from_vec(var, v);
    }

    bool is_canonical(const int &degree,
                      const UnivariateExprPolynomial &dict) const;
    std::size_t __hash__() const;
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    /*! Creates appropriate instance (i.e Symbol, Integer,
    * Mul, Pow, UnivariatePolynomial) depending on the size of dictionary `d`.
    */
    static RCP<const UnivariatePolynomial>
    from_dict(const RCP<const Symbol> &var, map_int_Expr &&d);
    static RCP<const UnivariatePolynomial>
    from_vec(const RCP<const Symbol> &var, const std::vector<Expression> &v);

    Expression max_coef() const;
    //! Evaluates the UnivariatePolynomial at value x
    Expression eval(const Expression &x) const;

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

    inline int get_degree() const
    {
        return degree_;
    }
    inline RCP<const Symbol> get_var() const
    {
        return var_;
    }
    inline const map_int_Expr &get_dict() const
    {
        return dict_.get_dict();
    }
    const UnivariateExprPolynomial &get_dict2() const
    {
        return dict_;
    }

}; // UnivariatePolynomial

//! Adding two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> add_uni_poly(const UnivariatePolynomial &a,
                                             const UnivariatePolynomial &b);
//! Negative of a UnivariatePolynomial
RCP<const UnivariatePolynomial> neg_uni_poly(const UnivariatePolynomial &a);
//! Subtracting two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> sub_uni_poly(const UnivariatePolynomial &a,
                                             const UnivariatePolynomial &b);
//! Multiplying two UnivariatePolynomial a and b
RCP<const UnivariatePolynomial> mul_uni_poly(const UnivariatePolynomial &a,
                                             const UnivariatePolynomial &b);

inline RCP<const UnivariatePolynomial>
univariate_polynomial(RCP<const Symbol> i, map_int_Expr &&dict)
{
    return UnivariatePolynomial::from_dict(i, std::move(dict));
}

} // SymEngine

#endif
