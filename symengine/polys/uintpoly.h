/**
 *  \file uintpoly.h
 *  Class for sparse Polynomial: UIntPoly
 **/
#ifndef SYMENGINE_UINTPOLY_H
#define SYMENGINE_UINTPOLY_H

#include <symengine/polys/upolybase.h>

namespace SymEngine
{
// Calculates bit length of number, used in UIntDict*= only
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

class UIntDict : public ODictWrapper<unsigned int, integer_class, UIntDict>
{

public:
    UIntDict() SYMENGINE_NOEXCEPT
    {
    }
    ~UIntDict() SYMENGINE_NOEXCEPT
    {
    }
    UIntDict(UIntDict &&other) SYMENGINE_NOEXCEPT
        : ODictWrapper(std::move(other))
    {
    }
    UIntDict(const int &i) : ODictWrapper(i)
    {
    }
    UIntDict(const map_uint_mpz &p) : ODictWrapper(p)
    {
    }
    UIntDict(const integer_class &i) : ODictWrapper(i)
    {
    }

    UIntDict(const UIntDict &) = default;
    UIntDict &operator=(const UIntDict &) = default;

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

    integer_class get_lc()
    {
        if (dict_.empty())
            return integer_class(0);
        return dict_.rbegin()->second;
    }

    static UIntDict mul(const UIntDict &a, const UIntDict &b)
    {
        int mul = 1;

        unsigned int N = bit_length(std::min(a.degree() + 1, b.degree() + 1))
                         + bit_length(a.max_abs_coef())
                         + bit_length(b.max_abs_coef());

        integer_class full = integer_class(1), temp, res;
        full <<= N;
        integer_class thresh = full / 2;
        integer_class mask = full - 1;
        integer_class s_val = a.eval_bit(N) * b.eval_bit(N);
        if (s_val < 0)
            mul = -1;
        s_val = mp_abs(s_val);

        unsigned int deg = 0, carry = 0;
        UIntDict r;

        while (s_val != 0 or carry != 0) {
            mp_and(temp, s_val, mask);
            if (temp < thresh) {
                res = mul * (temp + carry);
                if (res != 0)
                    r.dict_[deg] = res;
                carry = 0;
            } else {
                res = mul * (temp - full + carry);
                if (res != 0)
                    r.dict_[deg] = res;
                carry = 1;
            }
            s_val >>= N;
            deg++;
        }
        return r;
    }

    int compare(const UIntDict &other) const
    {
        if (dict_.size() != other.dict_.size())
            return (dict_.size() < other.dict_.size()) ? -1 : 1;
        return unified_compare(dict_, other.dict_);
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

}; // UIntDict

class UIntPoly : public UIntPolyBase<UIntDict, UIntPoly>
{
public:
    IMPLEMENT_TYPEID(UINTPOLY)
    //! Constructor of UIntPoly class
    UIntPoly(const RCP<const Basic> &var, UIntDict &&dict);

    //! \return true if canonical
    bool is_canonical(const UIntDict &dict) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    int compare(const Basic &o) const;

    // creates a UIntPoly in cannonical form based on the
    // dictionary.
    static RCP<const UIntPoly> from_dict(const RCP<const Basic> &var,
                                         map_uint_mpz &&d);
    static RCP<const UIntPoly> from_vec(const RCP<const Basic> &var,
                                        const vec_integer_class &v);
    //! Evaluates the UIntPoly at value x
    integer_class eval(const integer_class &x) const;
    vec_integer_class multieval(const vec_integer_class &v) const;

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

    inline const map_uint_mpz &get_dict() const
    {
        return poly_.dict_;
    }

    inline integer_class get_coeff(unsigned int x) const
    {
        return poly_.get_coeff(x);
    }

    typedef map_uint_mpz::const_iterator iterator;
    typedef map_uint_mpz::const_reverse_iterator reverse_iterator;
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

    unsigned int size() const
    {
        if (is_zero())
            return 0;
        return get_degree() + 1;
    }

}; // UIntPoly

RCP<const UIntPoly> pow_upoly(const UIntPoly &a, unsigned int p);
// true & sets `out` to b/a if a exactly divides b, otherwise false & undefined
bool divides_upoly(const UIntPoly &a, const UIntPoly &b,
                   const Ptr<RCP<const UIntPoly>> &res);

} // SymEngine

#endif
