/**
 *  \file fields.h
 *
 **/
#ifndef SYMENGINE_FIELDS_H
#define SYMENGINE_FIELDS_H

#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/polys/upolybase.h>
namespace SymEngine
{

class GaloisFieldDict : public ODictWrapper<unsigned int, integer_class, GaloisFieldDict>
{
public:
    integer_class modulo_;
public:
    GaloisFieldDict() SYMENGINE_NOEXCEPT
    {
    }
    ~GaloisFieldDict() SYMENGINE_NOEXCEPT
    {
    }
    inline integer_class mod_function (const integer_class &a)
    {
        integer_class temp;
        mp_fdiv_r(temp, a, modulo_);
        return std::move(temp);
    }
    inline void dict_add_val(const unsigned int &pos, const integer_class &a)
    {
        integer_class temp;
        mp_fdiv_r(temp, a, modulo_);
        if (temp != integer_class(0)) {
            auto t = dict_.lower_bound(pos);
            if (t != dict_.end() and t->first == pos) {
                t->second = temp;
            } else {
                dict_.insert(t, {pos, temp});
            }
        }
    }
    GaloisFieldDict(GaloisFieldDict &&other) SYMENGINE_NOEXCEPT
        : ODictWrapper(std::move(other.dict_)), modulo_(std::move(other.modulo_))
    {
    }
    GaloisFieldDict(const int &i, const integer_class &mod) : modulo_(mod)
    {
        integer_class temp;
        mp_fdiv_r(temp, integer_class(i), modulo_);
        if (temp != integer_class(0)) {
            dict_.insert({0, temp});
        }
    }
    GaloisFieldDict(const map_uint_mpz &p, const integer_class &mod) : modulo_(mod)
    {
        for (auto &iter : p) {
            integer_class temp;
            mp_fdiv_r(temp, iter.second, modulo_);
            if (temp != integer_class(0)) {
                dict_.insert(dict_.end(), {iter.first, temp});
            }
        }
    }
    GaloisFieldDict(const integer_class &i, const integer_class &mod) : modulo_(mod)
    {
        integer_class temp;
        mp_fdiv_r(temp, i, modulo_);
        if (temp != integer_class(0)) {
            dict_.insert({0, temp});
        }
    }

    static GaloisFieldDict from_vec(const std::vector<integer_class> &v, const integer_class &modulo)
    {
        GaloisFieldDict x;
        for (unsigned int i = 0; i < v.size(); ++i) {
            integer_class a;
            mp_fdiv_r(a, v[i], modulo);
            if (a != integer_class(0)) {
                x.dict_.insert({i, a});
            }
        }
        x.modulo_ = modulo;
        return x;
    }

    GaloisFieldDict(const GaloisFieldDict &) = default;
    GaloisFieldDict &operator=(const GaloisFieldDict &) = default;
    void gf_div(const GaloisFieldDict &o,
                const Ptr<GaloisFieldDict> &quo,
                const Ptr<GaloisFieldDict> &rem) const;

    GaloisFieldDict gf_lshift(const integer_class n) const;
    void gf_rshift(const integer_class n,
                   const Ptr<GaloisFieldDict> &quo,
                   const Ptr<GaloisFieldDict> &rem) const;
    GaloisFieldDict gf_sqr() const;
    GaloisFieldDict gf_pow(const integer_class n) const;
    void gf_monic(integer_class &res, const Ptr<GaloisFieldDict> &monic) const;
    GaloisFieldDict gf_gcd(const GaloisFieldDict &o) const;
    GaloisFieldDict gf_lcm(const GaloisFieldDict &o) const;

    GaloisFieldDict &operator=(GaloisFieldDict &&other) SYMENGINE_NOEXCEPT
    {
        if (this != &other) {
            dict_ = std::move(other.dict_);
            modulo_ = std::move(other.modulo_);
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator+=(const GaloisFieldDict &other)
    {
        SYMENGINE_ASSERT(modulo_ == other.modulo_);
        for (auto &iter : other.dict_) {
            auto t = dict_.lower_bound(iter.first);
            if (t != dict_.end() and t->first == iter.first) {
                t->second += iter.second;
                mp_fdiv_r(t->second, t->second, modulo_);
                if (t->second == integer_class(0)) {
                    dict_.erase(t);
                }
            } else {
                integer_class temp;
                mp_fdiv_r(temp, iter.second, modulo_);
                dict_.insert(t, {iter.first, temp});
            }
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict operator-() const
    {
        GaloisFieldDict o(*this);
        for (auto &a : o.dict_) {
            a.second *= -1;
            a.second += modulo_;
        }
        return o;
    }

    GaloisFieldDict &operator-=(const GaloisFieldDict &other)
    {
        SYMENGINE_ASSERT(modulo_ == other.modulo_);
        for (auto &iter : other.get_dict()) {
            auto t = dict_.lower_bound(iter.first);
            if (t != dict_.end() and t->first == iter.first) {
                mp_fdiv_r(t->second, t->second - iter.second, modulo_);
                if (t->second == 0) {
                    dict_.erase(t);
                }
            } else {
                integer_class temp;
                temp = modulo_ - iter.second;
                dict_.insert(t, {iter.first, temp});
            }
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    static GaloisFieldDict mul(const GaloisFieldDict &a, const GaloisFieldDict &b)
    {
        SYMENGINE_ASSERT(a.modulo_ == b.modulo_);
        if (a.get_dict().empty())
            return a;
        if (b.get_dict().empty())
            return b;

        GaloisFieldDict p;
        p.modulo_ = a.modulo_;
        for (const auto &i1 : a.dict_)
            for (const auto &i2 : b.dict_) {
                auto t = p.dict_[i1.first + i2.first];
                mp_addmul(t, i1.second, i2.second);
                mp_fdiv_r(p.dict_[i1.first + i2.first], t, a.modulo_);
            }

        for (auto it = p.dict_.cbegin(); it != p.dict_.cend();) {
            if (it->second == 0) {
                p.dict_.erase(it++);
            } else {
                ++it;
            }
        }
        return p;
    }

    template <class T>
    friend GaloisFieldDict operator*(const GaloisFieldDict &a, const T &b)
    {
        return GaloisFieldDict::mul(a, b);
    }

    GaloisFieldDict &operator*=(const integer_class &other)
    {
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);

        if (other == integer_class(0)) {
            dict_.clear();
            return static_cast<GaloisFieldDict &>(*this);
        }

        for (auto &arg : dict_) {
            arg.second *= other;
            mp_fdiv_r(arg.second, arg.second, modulo_);
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator*=(const GaloisFieldDict &other)
    {
        SYMENGINE_ASSERT(modulo_ == other.modulo_);
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);

        auto o_dict = other.get_dict();
        if (o_dict.empty()) {
            dict_.clear();
            return static_cast<GaloisFieldDict &>(*this);
        }

        // ! other is a just constant term
        if (o_dict.rbegin()->first == 0) {
            for (auto &arg : dict_) {
                arg.second *= o_dict.rbegin()->second;
                mp_fdiv_r(arg.second, arg.second, modulo_);
            }
            return static_cast<GaloisFieldDict &>(*this);
        }

        GaloisFieldDict res = GaloisFieldDict::mul(static_cast<GaloisFieldDict &>(*this),
                                                   other);
        res.dict_.swap(this->dict_);
        return static_cast<GaloisFieldDict &>(*this);
    }

    template <class T>
    friend GaloisFieldDict operator/(const GaloisFieldDict &a, const T &b)
    {
        GaloisFieldDict c = a;
        c /= b;
        return c;
    }

    GaloisFieldDict &operator/=(const integer_class &other)
    {
        if (other == integer_class(0)) {
            throw std::runtime_error("ZeroDivisionError");
        }
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);
        integer_class inv;
        mp_invert(inv, other, modulo_);
        for (auto &arg : dict_) {
            arg.second *= inv;
            mp_fdiv_r(arg.second, arg.second, modulo_);
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator/=(const GaloisFieldDict &other)
    {
        SYMENGINE_ASSERT(modulo_ == other.modulo_);
        auto dict_divisor = other.get_dict();
        if (dict_divisor.empty()) {
            throw std::runtime_error("ZeroDivisionError");
        }
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);
        integer_class inv;
        mp_invert(inv, dict_divisor.rbegin()->second, modulo_);

        // ! other is a just constant term
        if (dict_divisor.rbegin()->first == 0) {
            for (auto &iter : dict_) {
                iter.second *= inv;
                mp_fdiv_r(iter.second, iter.second, modulo_);
            }
            return static_cast<GaloisFieldDict &>(*this);
        }
        map_uint_mpz dict_out;
        size_t deg_dividend = dict_.rbegin()->first;
        size_t deg_divisor = dict_divisor.rbegin()->first;
        if (deg_dividend < deg_divisor) {
            dict_.clear();
            return static_cast<GaloisFieldDict &>(*this);
        }
        dict_out = std::move(dict_);
        dict_.clear();
        integer_class coeff;
        for (auto riter = dict_out.rbegin(); riter != dict_out.rend(); ++riter) {
            if (riter->first < deg_divisor)
                break;
            size_t it = riter->first;
            coeff = riter->second;
            size_t lb = deg_divisor + it > deg_dividend ? 
                        deg_divisor + it - deg_dividend : 0;
            size_t ub = std::min(it + 1, deg_divisor);
            for (size_t j = lb; j < ub; ++j) {
                mp_addmul(coeff, dict_out[it - j + deg_divisor], -dict_divisor[j]);
            }
            coeff *= inv;
            mp_fdiv_r(dict_out[it], coeff, modulo_);
            if (dict_out[it] != integer_class(0))
                dict_.insert({it - deg_divisor, dict_out[it]});
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    bool operator==(const GaloisFieldDict &other) const
    {
        return dict_ == other.get_dict() and modulo_ == other.modulo_;
    }

    bool operator!=(const GaloisFieldDict &other) const
    {
        return not(*this == other);
    }
};


class GaloisField : public UPolyBase<GaloisFieldDict, GaloisField>
{
public:
    IMPLEMENT_TYPEID(GALOISFIELD)

    //! Constructor of GaloisField class
    GaloisField(const RCP<const Symbol> &var, GaloisFieldDict &&dict);

    //! \return true if canonical
    bool is_canonical(const GaloisFieldDict &dict) const;
    //! \return size of the hash
    std::size_t __hash__() const;
    int compare(const Basic &o) const;

    // creates a GaloisField in cannonical form based on the
    // dictionary.
    static RCP<const GaloisField> from_dict(const RCP<const Symbol> &var,
                                         GaloisFieldDict &&d);
    static RCP<const GaloisField> from_vec(const RCP<const Symbol> &var,
                                        const std::vector<integer_class> &v,
                                        const integer_class &modulo);

    virtual vec_basic get_args() const;
    inline const map_uint_mpz &get_dict() const
    {
        return poly_.dict_;
    }

    inline unsigned int get_degree() const
    {
        return poly_.degree();
    }

};

inline RCP<const GaloisField> gf_poly(RCP<const Symbol> i, GaloisFieldDict &&dict)
{
    return GaloisField::from_dict(i, std::move(dict));
}

inline RCP<const GaloisField> gf_poly(RCP<const Symbol> i, map_uint_mpz &&dict, integer_class modulo_)
{
    GaloisFieldDict wrapper(dict, modulo_);
    return GaloisField::from_dict(i, std::move(wrapper));
}
}

#endif
