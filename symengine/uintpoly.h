/**
 *  \file polynomial.h
 *  Class for sparse Polynomial: UIntPoly
 **/
#ifndef SYMENGINE_UINTPOLY_H
#define SYMENGINE_UINTPOLY_H

#include <symengine/expression.h>
#include <symengine/monomials.h>
#include <symengine/upolybase.h>

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

} // SymEngine

#endif