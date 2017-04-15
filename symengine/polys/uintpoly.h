/**
 *  \file uintpoly.h
 *  Class for sparse Polynomial: UIntPoly
 **/
#ifndef SYMENGINE_UINTPOLY_H
#define SYMENGINE_UINTPOLY_H

#include <symengine/polys/usymenginepoly.h>

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

    static UIntDict mul(const UIntDict &a, const UIntDict &b)
    {
        if (a.empty())
            return a;
        if (b.empty())
            return b;
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
    // Reduces the polynomial modulo a constant `mod` in `Z[x]`
    // All the coefficients are coerced to range `[-mod/2, mod/2]`
    void itrunc(const integer_class &mod);
    // `res` is set to the primitive part of polynomial
    // returns the content of polynomial
    integer_class primitive(const Ptr<UIntDict> &res) const;
    //! \return l1 norm of the polynomial
    integer_class l1_norm() const;
    // Divides a polynomial in integer domain
    static void zz_divide(const UIntDict &a, const UIntDict &b,
                          const Ptr<UIntDict> &quo, const Ptr<UIntDict> &rem);
}; // UIntDict

class UIntPoly : public USymEnginePoly<UIntDict, UIntPolyBase, UIntPoly>
{
public:
    IMPLEMENT_TYPEID(UINTPOLY)
    //! Constructor of UIntPoly class
    UIntPoly(const RCP<const Basic> &var, UIntDict &&dict);

    //! \return size of the hash
    hash_t __hash__() const;
    // Factors a primitive suare free polynomial in Z[x] using Zassenhaus's
    // algorithm
    // returns a set of factors
    // References :
    //     1.) J. von zur Gathen, J. Gerhard, Modern Computer Algebra, 1999,
    //     page no: 453-454
    std::set<RCP<const UIntPoly>, RCPBasicKeyLess> zz_zassenhaus() const;
    // Factors a square free(non-primitive) polynomial in Z[x]
    // returns a pair, where first is the content of polynomial and second is
    // the set of factors
    // References :
    //     1.) J. von zur Gathen, J. Gerhard, Modern Computer Algebra, 1999
    std::pair<integer_class, std::set<RCP<const UIntPoly>, RCPBasicKeyLess>>
    zz_factor_sqf() const;
}; // UIntPoly

// Divides a polynomial in integer domain
inline void divide_upoly_int(const UIntPoly &a, const UIntPoly &b,
                             const Ptr<RCP<const UIntPoly>> &quo,
                             const Ptr<RCP<const UIntPoly>> &rem)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");
    UIntDict q, r;
    UIntDict::zz_divide(a.get_poly(), b.get_poly(), outArg(q), outArg(r));
    *quo = UIntPoly::from_dict(a.get_var(), std::move(q.dict_));
    *rem = UIntPoly::from_dict(a.get_var(), std::move(r.dict_));
}

} // SymEngine

#endif
