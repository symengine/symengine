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
    // GaloisFieldDict(GaloisFieldDict &&other) SYMENGINE_NOEXCEPT
    //     : ODictWrapper(std::move(other))
    // {
    // }
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
        x.dict_ = {};
        for (unsigned int i = 0; i < v.size(); i++) {
            integer_class a;
            mp_fdiv_r(a, v[i], modulo);
            if (a != integer_class(0)) {
                x.dict_[i] = a;
            }
        }
        x.modulo_ = modulo;
        return x;
    }

    GaloisFieldDict(const GaloisFieldDict &) = default;
    GaloisFieldDict &operator=(const GaloisFieldDict &) = default;
    GaloisFieldDict gf_neg() const;
    GaloisFieldDict gf_mul_ground(const integer_class a) const;
    void gf_imul_ground(const integer_class a);
    GaloisFieldDict gf_quo_ground(const integer_class a) const;

    GaloisFieldDict gf_quo(const GaloisFieldDict &o) const;
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
                mp_fdiv_r(t->second, t->second + iter.second, modulo_);
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
        return gf_neg();
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
                mp_fdiv_r(temp, -iter.second, modulo_);
                dict_.insert(t, {iter.first, temp});
            }
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
            *this = other;
            return static_cast<GaloisFieldDict &>(*this);
        }

        // ! other is a just constant term
        auto a = o_dict.find(0);
        if (o_dict.size() == 1
            and a != o_dict.end()) {
            gf_imul_ground(a->second);
            return static_cast<GaloisFieldDict &>(*this);
        }

        map_uint_mpz dict_out;
        for (auto &iter : o_dict) {
            for (auto &it : dict_) {
                mp_fdiv_r(dict_out[iter.first + it.first],
                          dict_out[iter.first + it.first] + iter.second * it.second,
                          modulo_);
            }
        }
        dict_ = {};
        for (auto &iter : dict_out) {
            if (iter.second != integer_class(0))
                dict_.insert(dict_.end(), {iter.first, iter.second});
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator/=(const GaloisFieldDict &other)
    {
        SYMENGINE_ASSERT(modulo_ == other.modulo_);
        auto o_dict = other.get_dict();
        if (o_dict.empty()) {
            throw std::runtime_error("ZeroDivisionError");
        }
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);

        // ! other is a just constant term
        auto a = o_dict.find(0);
        if (o_dict.size() == 1
            and a != o_dict.end()) {
            *this = gf_quo_ground(a->second);
            return static_cast<GaloisFieldDict &>(*this);
        }

        *this = gf_quo(other);
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
