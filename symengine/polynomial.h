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
        UnivariateExprPolynomial c = a;
        c += b;
        return c;
    }

    UnivariateExprPolynomial &operator+=(const UnivariateExprPolynomial &other)
    {
        for (auto &it : other.dict_) {
            auto t = dict_.lower_bound(it.first);
            if (t != dict_.end() and t->first == it.first) {
                t->second += it.second;
                if (t->second == 0) {
                    dict_.erase(t);
                }
            } else {
                dict_.insert(t, {it.first, it.second});
            }
        }
        return *this;
    }

    friend UnivariateExprPolynomial operator-(const UnivariateExprPolynomial &a,
                                              const UnivariateExprPolynomial &b)
    {
        UnivariateExprPolynomial c = a;
        c -= b;
        return c;
    }

    UnivariateExprPolynomial operator-() const
    {
        UnivariateExprPolynomial c = *this;
        for (auto &it : c.dict_)
            it.second *= -1;
        return c;
    }

    UnivariateExprPolynomial &operator-=(const UnivariateExprPolynomial &other)
    {
        for (auto &it : other.dict_) {
            auto t = dict_.lower_bound(it.first);
            if (t != dict_.end() and t->first == it.first) {
                t->second -= it.second;
                if (t->second == 0) {
                    dict_.erase(t);
                }
            } else {
                dict_.insert(t, {it.first, -it.second});
            }
        }
        return *this;
    }

    friend UnivariateExprPolynomial operator*(const UnivariateExprPolynomial &a,
                                              const UnivariateExprPolynomial &b)
    {
        UnivariateExprPolynomial c = a;
        c *= b;
        return c;
    }

    friend UnivariateExprPolynomial operator/(const UnivariateExprPolynomial &a,
                                              const Expression &b)
    {
        return a * (1 / b);
    }

    UnivariateExprPolynomial &operator*=(const UnivariateExprPolynomial &other)
    {
        if (dict_.empty())
            return *this;

        if (other.dict_.empty()) {
            *this = other;
            return *this;
        }

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
        *this *= (1 / other);
        return *this;
    }

    bool operator==(const UnivariateExprPolynomial &other) const
    {
        return dict_ == other.dict_;
    }

    bool operator!=(const UnivariateExprPolynomial &other) const
    {
        return not(*this == other);
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

    std::string __str__(const std::string name) const
    {
        std::ostringstream o;
        bool first = true;
        for (auto it = dict_.rbegin(); it != dict_.rend(); ++it) {
            std::string t;
            // if exponent is 0, then print only coefficient
            if (it->first == 0) {
                if (first) {
                    o << it->second;
                } else {
                    t = detail::poly_print(it->second);
                    if (t[0] == '-') {
                        o << " - " << t.substr(1);
                    } else {
                        o << " + " << t;
                    }
                }
                first = false;
                continue;
            }
            // if the coefficient of a term is +1 or -1
            if (it->second == 1 or it->second == -1) {
                // in cases of -x, print -x
                // in cases of x**2 - x, print - x
                if (first) {
                    if (it->second == -1)
                        o << "-";
                } else {
                    if (static_cast<const Integer &>(*it->second.get_basic())
                            .as_mpz()
                        < 0) {
                        o << " "
                          << "-"
                          << " ";
                    } else {
                        o << " "
                          << "+"
                          << " ";
                    }
                }
            }
            // if the coefficient of a term is 0, skip
            else if (it->second == 0)
                continue;
            // same logic is followed as above
            else {
                // in cases of -2*x, print -2*x
                // in cases of x**2 - 2*x, print - 2*x
                if (first) {
                    o << detail::poly_print(it->second) << "*";
                } else {
                    t = detail::poly_print(it->second);
                    if (t[0] == '-') {
                        o << " - " << t.substr(1);
                    } else {
                        o << " + " << t;
                    }
                    o << "*";
                }
            }
            o << name;
            // if exponent is not 1, print the exponent;
            if (it->first > 1) {
                o << "**" << it->first;
            } else if (it->first < 0) {
                o << "**(" << it->first << ")";
            }
            // corner cases of only first term handled successfully, switch the
            // bool
            first = false;
        }
        return o.str();
    }

    // const umap_int_basic get_basic() const
    const RCP<const Basic> get_basic(std::string var) const
    {
        RCP<const Symbol> x = symbol(var);
        umap_basic_num dict;
        RCP<const Number> coeff;
        for (const auto &it : dict_) {
            if (it.first != 0) {
                auto term = SymEngine::mul(
                    it.second.get_basic(),
                    pow_ex(Expression(x), Expression(it.first)).get_basic());
                RCP<const Number> coef;
                coef = zero;
                Add::coef_dict_add_term(outArg((coef)), dict, one, term);
            } else
                coeff = rcp_static_cast<const Number>(it.second.get_basic());
        }
        return Add::from_dict(coeff, std::move(dict));
    }

    int compare(const UnivariateExprPolynomial &other) const
    {
        if (dict_.size() != other.dict_.size())
            return (dict_.size() < other.dict_.size()) ? -1 : 1;
        auto p = dict_.begin();
        auto o = other.dict_.begin();
        for (; p != dict_.end(); ++p, ++o) {
            if (p->first != o->first)
                return (p->first < o->first) ? -1 : 1;
            if (p->second != o->second)
                return (p->second.get_basic()->__cmp__(*o->second.get_basic()))
                           ? -1
                           : 1;
        }
        return 0;
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
    UnivariateExprPolynomial expr_dict_;

public:
    IMPLEMENT_TYPEID(UNIVARIATEPOLYNOMIAL)
    //! Constructor of UnivariatePolynomial class
    UnivariatePolynomial(const RCP<const Symbol> &var, const int &degree,
                         const UnivariateExprPolynomial &&dict);
    //! Constructor using a dense vector of Expression
    UnivariatePolynomial(const RCP<const Symbol> &var,
                         const std::vector<Expression> &v);

    bool is_canonical(const int &degree,
                      const UnivariateExprPolynomial &dict) const;
    std::size_t __hash__() const;
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    /*! Creates appropriate instance (i.e Symbol, Integer,
    * Mul, Pow, UnivariatePolynomial) depending on the size of dictionary `d`.
    */
    static RCP<const UnivariatePolynomial>
    from_dict(const RCP<const Symbol> &var, UnivariateExprPolynomial &&d);
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
        return expr_dict_.get_dict();
    }
    const UnivariateExprPolynomial &get_expr_dict() const
    {
        return expr_dict_;
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
univariate_polynomial(RCP<const Symbol> i, UnivariateExprPolynomial &&dict)
{
    return UnivariatePolynomial::from_dict(i, std::move(dict));
}

} // SymEngine

#endif
