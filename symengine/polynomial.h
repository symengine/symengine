/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UIntPolyO and Polynomial
 **/
#ifndef SYMENGINE_POLYNOMIALS_H
#define SYMENGINE_POLYNOMIALS_H

#include <symengine/expression.h>
#include <symengine/monomials.h>
#include <symengine/uint_base.h>

namespace SymEngine
{
// Calculates bit length of number, used in UIntODict*= only
template <typename T>
unsigned int bit_length(T t)
{
    unsigned int count = 0;
    while (t > 0) {
        count++;
        t = t >> 1;
    }
    return count;
}

// dict wrapper
template <typename Key, typename Value, typename Wrapper>
class ODictWrapper
{
public:
    std::map<Key, Value> dict_;

public:
    ODictWrapper() SYMENGINE_NOEXCEPT
    {
    }
    ~ODictWrapper() SYMENGINE_NOEXCEPT
    {
    }

    ODictWrapper(const int &i)
    {
        if (i != 0)
            dict_ = {{0, Value(i)}};
    }

    ODictWrapper(const std::map<Key, Value> &p)
    {
        for (auto &iter : p) {
            if (iter.second != Value(0))
                dict_[iter.first] = iter.second;
        }
    }

    ODictWrapper(const Value &p)
    {
        if (p != Value(0))
            dict_[0] = p;
    }

    ODictWrapper(std::string s)
    {
        dict_[1] = Value(1);
    }

    Wrapper &operator=(Wrapper &&other) SYMENGINE_NOEXCEPT
    {
        if (this != &other)
            dict_ = std::move(other.dict_);
        return static_cast<Wrapper &>(*this);
    }

    friend Wrapper operator+(const Wrapper &a, const Wrapper &b)
    {
        Wrapper c = a;
        c += b;
        return c;
    }

    Wrapper &operator+=(const Wrapper &other)
    {
        for (auto &iter : other.dict_) {
            auto t = dict_.lower_bound(iter.first);
            if (t != dict_.end() and t->first == iter.first) {
                t->second += iter.second;
                if (t->second == 0) {
                    dict_.erase(t);
                }
            } else {
                dict_.insert(t, {iter.first, iter.second});
            }
        }
        return static_cast<Wrapper &>(*this);
    }

    friend Wrapper operator-(const Wrapper &a, const Wrapper &b)
    {
        Wrapper c = a;
        c -= b;
        return c;
    }

    Wrapper operator-() const
    {
        ODictWrapper c = *this;
        for (auto &iter : c.dict_)
            iter.second *= -1;
        return static_cast<Wrapper &>(c);
    }

    Wrapper &operator-=(const Wrapper &other)
    {
        for (auto &iter : other.dict_) {
            auto t = dict_.lower_bound(iter.first);
            if (t != dict_.end() and t->first == iter.first) {
                t->second -= iter.second;
                if (t->second == 0) {
                    dict_.erase(t);
                }
            } else {
                dict_.insert(t, {iter.first, -iter.second});
            }
        }
        return static_cast<Wrapper &>(*this);
    }

    friend Wrapper operator*(const Wrapper &a, const Wrapper &b)
    {
        Wrapper c = a;
        c *= b;
        return c;
    }

    Wrapper &operator*=(const Wrapper &other)
    {
        std::map<Key, Value> p;
        for (const auto &i1 : dict_)
            for (const auto &i2 : other.dict_)
                p[i1.first + i2.first] += i1.second * i2.second;

        dict_ = {};
        for (const auto &ite : p)
            if (ite.second != Value(0))
                dict_[ite.first] = ite.second;

        return static_cast<Wrapper &>(*this);
    }

    bool operator==(const Wrapper &other) const
    {
        return dict_ == other.dict_;
    }

    bool operator!=(const Wrapper &other) const
    {
        return not(*this == other);
    }

    const std::map<Key, Value> &get_dict() const
    {
        return dict_;
    }

    unsigned int size() const
    {
        return dict_.size();
    }

    bool empty() const
    {
        return dict_.empty();
    }

    Key degree() const
    {
        if (dict_.empty())
            return Key(0);
        return dict_.rbegin()->first;
    }
};

class UIntODict : public ODictWrapper<unsigned int, integer_class, UIntODict>
{

public:
    UIntODict() SYMENGINE_NOEXCEPT
    {
    }
    ~UIntODict() SYMENGINE_NOEXCEPT
    {
    }
    UIntODict(UIntODict &&other) SYMENGINE_NOEXCEPT
        : ODictWrapper(std::move(other))
    {
    }
    UIntODict(const int &i) : ODictWrapper(i)
    {
    }
    UIntODict(const map_uint_mpz &p) : ODictWrapper(p)
    {
    }
    UIntODict(const integer_class &i) : ODictWrapper(i)
    {
    }

    UIntODict(const UIntODict &) = default;
    UIntODict &operator=(const UIntODict &) = default;

    //! Evaluates the dict_ at value 2**x
    integer_class eval_bit(const unsigned int &x) const
    {
        unsigned int last_deg = dict_.rbegin()->first;
        integer_class result(0);

        for (auto it = dict_.rbegin(); it != dict_.rend(); ++it) {
            result <<= x * (last_deg - (*it).first);
            result += (*it).second;
            last_deg = (*it).first;
        }
        result <<= x * last_deg;

        return result;
    }

    UIntODict &operator*=(const UIntODict &other)
    {
        int mul = 1;

        unsigned int N = bit_length(std::min(degree() + 1, other.degree() + 1))
                         + bit_length(max_abs_coef())
                         + bit_length(other.max_abs_coef());

        integer_class full = integer_class(1), temp, res;
        full <<= N;
        integer_class thresh = full / 2;
        integer_class mask = full - 1;
        integer_class s_val = eval_bit(N) * other.eval_bit(N);
        if (s_val < 0)
            mul = -1;
        s_val = mp_abs(s_val);

        unsigned int deg = 0, carry = 0;
        map_uint_mpz dict;

        while (s_val != 0 or carry != 0) {
            mp_and(temp, s_val, mask);
            if (temp < thresh) {
                res = mul * (temp + carry);
                if (res != 0)
                    dict[deg] = res;
                carry = 0;
            } else {
                res = mul * (temp - full + carry);
                if (res != 0)
                    dict[deg] = res;
                carry = 1;
            }
            s_val >>= N;
            deg++;
        }

        dict_ = dict;
        return *this;
    }

    int compare(const UIntODict &other) const
    {
        if (dict_.size() != other.dict_.size())
            return (dict_.size() < other.dict_.size()) ? -1 : 1;
        return map_uint_mpz_compare(dict_, other.dict_);
    }

    integer_class max_abs_coef() const
    {
        integer_class curr(mp_abs(dict_.begin()->second));
        for (const auto &it : dict_) {
            if (mp_abs(it.second) > curr)
                curr = mp_abs(it.second);
        }
        return curr;
    }

}; // UIntODict

class UIntPolyO : public UPolyBase<UIntODict, UIntPolyO>
{
public:
    IMPLEMENT_TYPEID(UINTPOLYO)
    //! Constructor of UIntPolyO class
    UIntPolyO(const RCP<const Symbol> &var, UIntODict &&dict);
    //! Constructor using a dense vector of integer_class coefficients

    UIntPolyO(const RCP<const Symbol> &var,
              const std::vector<integer_class> &v);

    //! \return true if canonical
    bool is_canonical(const UIntODict &dict) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    int compare(const Basic &o) const;

    // creates a UIntPolyO in cannonical form based on the
    // dictionary.
    static RCP<const UIntPolyO> from_dict(const RCP<const Symbol> &var,
                                          UIntODict &&d);
    // create a UIntPolyO from a dense vector of integer_class
    // coefficients
    static RCP<const UIntPolyO> from_vec(const RCP<const Symbol> &var,
                                         const std::vector<integer_class> &v);

    /*!
    * Adds coef*var_**n to the dict_
    */
    integer_class max_abs_coef() const;
    //! Evaluates the UIntPolyO at value x
    integer_class eval(const integer_class &x) const;

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
    inline const map_uint_mpz &get_dict() const
    {
        return poly_.dict_;
    }

    inline unsigned int get_degree() const
    {
        return poly_.degree();
    }

}; // UIntPolyO

inline RCP<const UIntPolyO> uint_poly(RCP<const Symbol> i, UIntODict &&dict)
{
    return UIntPolyO::from_dict(i, std::move(dict));
}

inline RCP<const UIntPolyO> uint_poly(RCP<const Symbol> i, map_uint_mpz &&dict)
{
    UIntODict wrapper(dict);
    return UIntPolyO::from_dict(i, std::move(wrapper));
}

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