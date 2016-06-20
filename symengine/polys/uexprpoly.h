/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UExprPoly
 **/
#ifndef SYMENGINE_UEXPRPOLY_H
#define SYMENGINE_UEXPRPOLY_H

#include <symengine/expression.h>
#include <symengine/monomials.h>
#include <symengine/polys/upolybase.h>

namespace SymEngine
{

class UExprDict : public ODictWrapper<int, Expression, UExprDict>
{

public:
    UExprDict() SYMENGINE_NOEXCEPT
    {
    }
    ~UExprDict() SYMENGINE_NOEXCEPT
    {
    }
    UExprDict(UExprDict &&other) SYMENGINE_NOEXCEPT
        : ODictWrapper(std::move(other))
    {
    }
    UExprDict(const int &i) : ODictWrapper(i)
    {
    }
    UExprDict(const map_int_Expr &p) : ODictWrapper(p)
    {
    }
    UExprDict(const Expression &expr) : ODictWrapper(expr)
    {
    }

    UExprDict(const std::string &s) : ODictWrapper(s)
    {
    }

    UExprDict(const UExprDict &) = default;
    UExprDict &operator=(const UExprDict &) = default;

    friend std::ostream &operator<<(std::ostream &os, const UExprDict &expr)
    {
        os << expr.dict_;
        return os;
    }

    friend UExprDict operator/(const UExprDict &a, const Expression &b)
    {
        return a * (1 / b);
    }

    UExprDict &operator/=(const Expression &other)
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

    int compare(const UExprDict &other) const
    {
        return unified_compare(dict_, other.dict_);
    }

    Expression find_cf(int deg) const
    {
        if (dict_.find(deg) != dict_.end())
            return dict_.at(deg);
        else
            return Expression(0);
    }
}; // UExprDict

class UExprPoly : public UPolyBase<UExprDict, UExprPoly>
{
public:
    IMPLEMENT_TYPEID(UEXPRPOLY)
    //! Constructor of UExprPoly class
    UExprPoly(const RCP<const Basic> &var, UExprDict &&dict);

    bool is_canonical(const UExprDict &dict) const;
    std::size_t __hash__() const;
    int compare(const Basic &o) const;

    static RCP<const UExprPoly> from_dict(const RCP<const Basic> &var,
                                          map_int_Expr &&d);
    static RCP<const UExprPoly> from_vec(const RCP<const Basic> &var,
                                         const std::vector<Expression> &v);

    Expression max_coef() const;
    //! Evaluates the UExprPoly at value x
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

    inline Expression get_coeff(int x) const
    {
        return poly_.get_coeff(x);
    }

    typedef map_int_Expr::const_iterator iterator;
    typedef map_int_Expr::const_reverse_iterator reverse_iterator;
    iterator begin() const
    {
        return poly_.dict_.begin();
    }
    iterator end() const
    {
        return poly_.dict_.end();
    }
    reverse_iterator obegin() const
    {
        return poly_.dict_.rbegin();
    }
    reverse_iterator oend() const
    {
        return poly_.dict_.rend();
    }
}; // UExprPoly

inline RCP<const UExprPoly> uexpr_poly(RCP<const Basic> i, UExprDict &&dict)
{
    return UExprPoly::from_container(i, std::move(dict));
}

inline RCP<const UExprPoly> uexpr_poly(RCP<const Basic> i, map_int_Expr &&dict)
{
    return UExprPoly::from_dict(i, std::move(dict));
}

} // SymEngine

#endif
