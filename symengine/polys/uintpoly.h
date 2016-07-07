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

template <typename D, template <typename X, typename Y> class BaseType,
          typename P>
class USymEnginePoly : public BaseType<D, P>
{
public:
    using C = typename BaseType<D, P>::coef_type;

    USymEnginePoly(const RCP<const Basic> &var, UIntDict &&dict)
        : UIntPolyBase<D, P>(var, std::move(dict))
    {
    }

    int compare(const Basic &o) const
    {
        const P &s = static_cast<const P &>(o);

        if (this->poly_.size() != s.poly_.size())
            return (this->poly_.size() < s.poly_.size()) ? -1 : 1;

        int cmp = unified_compare(this->var_, s.var_);
        if (cmp != 0)
            return cmp;

        return unified_compare(this->poly_.dict_, s.poly_.dict_);
    }

    bool is_canonical(const D &dict) const
    {
        // Check if dictionary contains terms with coeffienct 0
        for (auto iter : dict.dict_)
            if (iter.second == 0)
                return false;
        return true;
    }

    static RCP<const P> from_vec(const RCP<const Basic> &var,
                                 const std::vector<C> &v)
    {
        return make_rcp<const P>(var, D::from_vec(v));
    }

    static D cont_from_dict(const RCP<const Basic> &var,
                            std::map<unsigned, C> &&d)
    {
        return std::move(D(d));
    }

    C eval(const C &x) const
    {
        unsigned int last_deg = this->poly_.dict_.rbegin()->first;
        C result(0), x_pow;

        for (auto it = this->poly_.dict_.rbegin();
             it != this->poly_.dict_.rend(); ++it) {
            mp_pow_ui(x_pow, x, last_deg - (*it).first);
            last_deg = (*it).first;
            result = (*it).second + x_pow * result;
        }
        mp_pow_ui(x_pow, x, last_deg);
        result *= x_pow;

        return result;
    }

    std::vector<C> multieval(const std::vector<C> &v) const
    {
        // this is not the optimal algorithm
        std::vector<C> res(v.size());
        for (unsigned int i = 0; i < v.size(); ++i)
            res[i] = eval(v[i]);
        return res;
    }

    inline const std::map<unsigned, C> &get_dict() const
    {
        return this->poly_.dict_;
    }

    inline C get_coeff(unsigned int x) const
    {
        return this->poly_.get_coeff(x);
    }

    typedef typename std::map<unsigned, C>::const_iterator iterator;
    typedef typename std::map<unsigned, C>::const_reverse_iterator r_iterator;
    iterator begin() const
    {
        return this->poly_.dict_.begin();
    }
    iterator end() const
    {
        return this->poly_.dict_.end();
    }
    r_iterator obegin() const
    {
        return this->poly_.dict_.rbegin();
    }
    r_iterator oend() const
    {
        return this->poly_.dict_.rend();
    }

    unsigned int size() const
    {
        if (this->poly_.dict_.empty())
            return 0;
        return this->get_degree() + 1;
    }
};

class UIntPoly : public USymEnginePoly<UIntDict, UIntPolyBase, UIntPoly>
{
public:
    IMPLEMENT_TYPEID(UINTPOLY)
    //! Constructor of UIntPoly class
    UIntPoly(const RCP<const Basic> &var, UIntDict &&dict);

    //! \return size of the hash
    std::size_t __hash__() const;
}; // UIntPoly

// true & sets `out` to b/a if a exactly divides b, otherwise false & undefined
bool divides_upoly(const UIntPoly &a, const UIntPoly &b,
                   const Ptr<RCP<const UIntPoly>> &res);
} // SymEngine

#endif
