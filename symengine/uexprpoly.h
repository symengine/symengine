/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UExprPoly
 **/
#ifndef SYMENGINE_UEXPRPOLY_H
#define SYMENGINE_UEXPRPOLY_H

#include <symengine/expression.h>
#include <symengine/monomials.h>
#include <symengine/upolybase.h>

namespace SymEngine
{

class UExprODict : public ODictWrapper<int, Expression, UExprODict>
{

public:
    UExprODict() SYMENGINE_NOEXCEPT
    {
    }
    ~UExprODict() SYMENGINE_NOEXCEPT
    {
    }
    UExprODict(UExprODict &&other) SYMENGINE_NOEXCEPT
        : ODictWrapper(std::move(other))
    {
    }
    UExprODict(const int &i) : ODictWrapper(i)
    {
    }
    UExprODict(const map_int_Expr &p) : ODictWrapper(p)
    {
    }
    UExprODict(const Expression &expr) : ODictWrapper(expr)
    {
    }

    UExprODict(const std::string &s) : ODictWrapper(s)
    {
    }

    UExprODict(const UExprODict &) = default;
    UExprODict &operator=(const UExprODict &) = default;

    friend std::ostream &operator<<(std::ostream &os, const UExprODict &expr)
    {
        os << expr.dict_;
        return os;
    }

    friend UExprODict operator/(const UExprODict &a, const Expression &b)
    {
        return a * (1 / b);
    }

    UExprODict &operator/=(const Expression &other)
    {
        *this *= (1 / other);
        return *this;
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

    int compare(const UExprODict &other) const
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
}; // UExprODict

class UExprPolyO : public UPolyBase<UExprODict, UExprPolyO>
{
public:
    IMPLEMENT_TYPEID(UEXPRPOLYO)
    //! Constructor of UExprPolyO class
    UExprPolyO(const RCP<const Symbol> &var, UExprODict &&dict);
    //! Constructor using a dense vector of Expression
    UExprPolyO(const RCP<const Symbol> &var, const std::vector<Expression> &v);

    bool is_canonical(const UExprODict &dict) const;
    std::size_t __hash__() const;
    int compare(const Basic &o) const;

    /*! Creates appropriate instance (i.e Symbol, Integer,
    * Mul, Pow, UExprPolyO) depending on the size of dictionary `d`.
    */
    static RCP<const UExprPolyO> from_dict(const RCP<const Symbol> &var,
                                           UExprODict &&d);
    static RCP<const UExprPolyO> from_vec(const RCP<const Symbol> &var,
                                          const std::vector<Expression> &v);

    Expression max_coef() const;
    //! Evaluates the UExprPolyO at value x
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
        return poly_.degree();
    }
    inline const map_int_Expr &get_dict() const
    {
        return poly_.get_dict();
    }
}; // UExprPolyO

inline RCP<const UExprPolyO> uexpr_poly(RCP<const Symbol> i, UExprODict &&dict)
{
    return UExprPolyO::from_dict(i, std::move(dict));
}

inline RCP<const UExprPolyO> uexpr_poly(RCP<const Symbol> i,
                                        map_int_Expr &&dict)
{
    UExprODict wrapper(dict);
    return UExprPolyO::from_dict(i, std::move(wrapper));
}

} // SymEngine

#endif