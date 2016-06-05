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
    inline integer_class modFunction (integer_class a)
    {
        integer_class temp = a % modulo_;
        if (temp < 0)
            temp += modulo_;
        return temp;
    }
    // GaloisFieldDict(GaloisFieldDict &&other) SYMENGINE_NOEXCEPT
    //     : ODictWrapper(std::move(other))
    // {
    // }
    GaloisFieldDict(const int &i, const integer_class &mod) : modulo_(mod)
    {
        integer_class val = modFunction(integer_class(i));
        if (val != integer_class(0))
            dict_[0] = val;
    }
    GaloisFieldDict(const map_uint_mpz &p, const integer_class &mod) : modulo_(mod)
    {
        for (auto &iter : p) {
            integer_class val = modFunction(iter.second);
            if (val != integer_class(0))
                dict_[iter.first] = val;
        }
    }
    GaloisFieldDict(const integer_class &i, const integer_class &mod) : modulo_(mod)
    {
        integer_class val = modFunction(i);
        if (val != integer_class(0))
            dict_[0] = val;
    }

    static GaloisFieldDict from_vec(const std::vector<integer_class> &v, const integer_class &modulo)
    {
        GaloisFieldDict x;
        x.dict_ = {};
        for (unsigned int i = 0; i < v.size(); i++) {
            integer_class a = v[i] % modulo;
            if (a < 0)
                a += modulo;
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
    GaloisFieldDict gf_add_ground(const integer_class a) const;
    GaloisFieldDict gf_sub_ground(const integer_class a) const;
    GaloisFieldDict gf_mul_ground(const integer_class a) const;
    GaloisFieldDict gf_quo_ground(const integer_class a) const;

    GaloisFieldDict gf_add(const GaloisFieldDict &o) const;
    GaloisFieldDict gf_sub(const GaloisFieldDict &o) const;
    GaloisFieldDict gf_mul(const GaloisFieldDict &o) const;
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
            modulo_ = other.modulo_;
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator+=(const GaloisFieldDict &other)
    {
        *this = gf_add(other);
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict operator-() const
    {
        return gf_neg();
    }

    GaloisFieldDict &operator-=(const GaloisFieldDict &other)
    {
        *this = gf_sub(other);
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator*=(const GaloisFieldDict &other)
    {
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);

        if (other.dict_.empty()) {
            *this = other;
            return static_cast<GaloisFieldDict &>(*this);
        }

        // ! other is a just constant term
        auto a = other.dict_.find(0);
        if (other.dict_.size() == 1
            and a != other.dict_.end()) {
            *this = gf_add_ground(a->second);
            return static_cast<GaloisFieldDict &>(*this);
        }

        *this = gf_mul(other);
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator/=(const GaloisFieldDict &other)
    {
        if (other.dict_.empty()) {
            throw std::runtime_error("ZeroDivisionError");
        }

        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);

        // ! other is a just constant term
        auto a = other.dict_.find(0);
        if (other.dict_.size() == 1
            and a != other.dict_.end()) {
            *this = gf_quo_ground(a->second);
            return static_cast<GaloisFieldDict &>(*this);
        }

        *this = gf_quo(other);
        return static_cast<GaloisFieldDict &>(*this);
    }

    bool operator==(const GaloisFieldDict &other) const
    {
        return dict_ == other.dict_ and modulo_ == other.modulo_;
    }

    bool operator!=(const GaloisFieldDict &other) const
    {
        return not(*this == other);
    }
};


class GaloisField : public UPolyBase<GaloisFieldDict, GaloisField> {
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
