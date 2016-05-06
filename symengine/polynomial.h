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

class UnivariatePolynomial : public Basic
{
private:
    int degree_;
    RCP<const Symbol> var_;
    map_int_Expr dict_;

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

    bool is_canonical(const int &degree, const map_int_Expr &dict) const;
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
    /*!
    * Adds coef*var_**n to the dict_
    */
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
RCP<const UnivariatePolynomial> mul_uni_poly(RCP<const UnivariatePolynomial> a,
                                             RCP<const UnivariatePolynomial> b);

inline RCP<const UnivariatePolynomial>
univariate_polynomial(RCP<const Symbol> i, map_int_Expr &&dict)
{
    return UnivariatePolynomial::from_dict(i, std::move(dict));
}

class UnivariateExprPolynomial
{
private:
    RCP<const UnivariatePolynomial> poly_;

public:
//! Construct UnivariateExprPolynomial from UnivariatePolynomial
#if defined(HAVE_SYMENGINE_IS_CONSTRUCTIBLE)
    template <
        typename T,
        typename = typename std::
            enable_if<std::is_constructible<RCP<const UnivariatePolynomial>,
                                            T &&>::value>::type>
#else
    template <typename T>
#endif
    UnivariateExprPolynomial(T &&o)
        : poly_(std::forward<T>(o))
    {
    }
    UnivariateExprPolynomial()
    {
    }
    ~UnivariateExprPolynomial() SYMENGINE_NOEXCEPT
    {
    }
    UnivariateExprPolynomial(const UnivariateExprPolynomial &) = default;
    UnivariateExprPolynomial(UnivariateExprPolynomial &&other)
        SYMENGINE_NOEXCEPT : poly_(std::move(other.poly_))
    {
    }
    UnivariateExprPolynomial(int i)
        : poly_(UnivariatePolynomial::create(symbol(""), {Expression(i)}))
    {
    }
    UnivariateExprPolynomial(std::string varname)
        : poly_(UnivariatePolynomial::create(symbol(varname), {0, 1}))
    {
    }
    UnivariateExprPolynomial(RCP<const UnivariatePolynomial> p)
        : poly_(std::move(p))
    {
    }
    UnivariateExprPolynomial(Expression expr)
        : poly_(UnivariatePolynomial::create(symbol(""), {expr}))
    {
    }
    UnivariateExprPolynomial &operator=(const UnivariateExprPolynomial &)
        = default;
    UnivariateExprPolynomial &
    operator=(UnivariateExprPolynomial &&other) SYMENGINE_NOEXCEPT
    {
        if (this != &other)
            this->poly_ = std::move(other.poly_);
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const UnivariateExprPolynomial &expr)
    {
        os << expr.poly_->__str__();
        return os;
    }

    friend UnivariateExprPolynomial operator+(const UnivariateExprPolynomial &a,
                                              const UnivariateExprPolynomial &b)
    {
        return UnivariateExprPolynomial(add_uni_poly(*a.poly_, *b.poly_));
    }

    UnivariateExprPolynomial &operator+=(const UnivariateExprPolynomial &other)
    {
        poly_ = add_uni_poly(*poly_, *other.poly_);
        return *this;
    }

    friend UnivariateExprPolynomial operator-(const UnivariateExprPolynomial &a,
                                              const UnivariateExprPolynomial &b)
    {
        return UnivariateExprPolynomial(sub_uni_poly(*a.poly_, *b.poly_));
    }

    UnivariateExprPolynomial operator-() const
    {
        return neg_uni_poly(*this->poly_);
    }

    UnivariateExprPolynomial &operator-=(const UnivariateExprPolynomial &other)
    {
        poly_ = sub_uni_poly(*poly_, *other.poly_);
        return *this;
    }

    friend UnivariateExprPolynomial operator*(const UnivariateExprPolynomial &a,
                                              const UnivariateExprPolynomial &b)
    {
        return UnivariateExprPolynomial(mul_uni_poly(a.poly_, b.poly_));
    }

    friend UnivariateExprPolynomial operator/(const UnivariateExprPolynomial &a,
                                              const Expression &b)
    {
        return UnivariateExprPolynomial(
            mul_uni_poly(a.poly_, UnivariateExprPolynomial(1 / b).poly_));
    }

    UnivariateExprPolynomial &operator*=(const UnivariateExprPolynomial &other)
    {
        poly_ = mul_uni_poly(poly_, other.poly_);
        return *this;
    }

    UnivariateExprPolynomial &operator/=(const Expression &other)
    {
        poly_ = mul_uni_poly(poly_, UnivariateExprPolynomial(1 / other).poly_);
        return *this;
    }

    bool operator==(const UnivariateExprPolynomial &other) const
    {
        return eq(*poly_, *other.poly_);
    }

    bool operator==(int i) const
    {
        return eq(*poly_, *(UnivariateExprPolynomial(i).poly_));
    }

    bool operator!=(const UnivariateExprPolynomial &other) const
    {
        return not(*this == other);
    }

    //! Method to get UnivariatePolynomial from UnivariateExprPolynomial
    const RCP<const UnivariatePolynomial> &get_univariate_poly() const
    {
        return poly_;
    }

    std::size_t __hash__() const
    {
        return poly_->hash();
    }

    const RCP<const Basic> get_basic() const
    {
        RCP<const Symbol> x = poly_->get_var();
        umap_basic_num dict_;
        RCP<const Number> coeff;
        for (const auto &it : poly_->get_dict()) {
            if (it.first != 0) {
                auto term = mul(
                    it.second.get_basic(),
                    pow_ex(Expression(x), Expression(it.first)).get_basic());
                RCP<const Number> coef;
                coef = zero;
                Add::coef_dict_add_term(outArg((coef)), dict_, one, term);
            } else
                coeff = rcp_static_cast<const Number>(it.second.get_basic());
        }
        return Add::from_dict(coeff, std::move(dict_));
    }

    int compare(const UnivariateExprPolynomial &other)
    {
        return poly_->compare(*other.poly_);
    }

    Expression find_cf(int deg) const
    {
        if (poly_->get_dict().find(deg) != poly_->get_dict().end()) {
            return poly_->get_dict().at(deg);
        } else {
            return Expression(0);
        }
    }
}; // UnivariateExprPolynomial

} // SymEngine

#endif
