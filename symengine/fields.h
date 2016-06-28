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

class GaloisFieldDict
{
public:
    std::vector<integer_class> dict_;
    integer_class modulo_;

public:
    GaloisFieldDict() SYMENGINE_NOEXCEPT
    {
    }
    ~GaloisFieldDict() SYMENGINE_NOEXCEPT
    {
    }
    GaloisFieldDict(GaloisFieldDict &&other) SYMENGINE_NOEXCEPT
        : dict_(std::move(other.dict_)),
          modulo_(std::move(other.modulo_))
    {
    }
    GaloisFieldDict(const int &i, const integer_class &mod) : modulo_(mod)
    {
        integer_class temp;
        mp_fdiv_r(temp, integer_class(i), modulo_);
        if (temp != integer_class(0))
            dict_.insert(dict_.begin(), temp);
    }
    GaloisFieldDict(const map_uint_mpz &p, const integer_class &mod)
        : modulo_(mod)
    {
        if (p.size() != 0) {
            dict_.resize(p.rbegin()->first + 1, integer_class(0));
            for (auto &iter : p) {
                integer_class temp;
                mp_fdiv_r(temp, iter.second, modulo_);
                dict_[iter.first] = temp;
            }
            gf_istrip();
        }
    }
    GaloisFieldDict(const integer_class &i, const integer_class &mod)
        : modulo_(mod)
    {
        integer_class temp;
        mp_fdiv_r(temp, i, modulo_);
        if (temp != integer_class(0))
            dict_.insert(dict_.begin(), temp);
    }

    static GaloisFieldDict from_vec(const std::vector<integer_class> &v,
                                    const integer_class &modulo)
    {
        GaloisFieldDict x;
        x.modulo_ = modulo;
        x.dict_.resize(v.size());
        for (unsigned int i = 0; i < v.size(); ++i) {
            integer_class a;
            mp_fdiv_r(a, v[i], modulo);
            x.dict_[i] = a;
        }
        x.gf_istrip();
        return x;
    }

    GaloisFieldDict(const GaloisFieldDict &) = default;
    GaloisFieldDict &operator=(const GaloisFieldDict &) = default;
    void gf_div(const GaloisFieldDict &o, const Ptr<GaloisFieldDict> &quo,
                const Ptr<GaloisFieldDict> &rem) const;

    GaloisFieldDict gf_lshift(const integer_class n) const;
    void gf_rshift(const integer_class n, const Ptr<GaloisFieldDict> &quo,
                   const Ptr<GaloisFieldDict> &rem) const;
    GaloisFieldDict gf_sqr() const;
    GaloisFieldDict gf_pow(const integer_class n) const;
    void gf_monic(integer_class &res, const Ptr<GaloisFieldDict> &monic) const;
    GaloisFieldDict gf_gcd(const GaloisFieldDict &o) const;
    GaloisFieldDict gf_lcm(const GaloisFieldDict &o) const;
    GaloisFieldDict gf_diff() const;

    // Returns whether polynomial is squarefield in `modulo_`
    bool gf_is_sqf() const;

    // Returns the square free decomposition of polynomial's monic
    // representation in `modulo_`
    // A vector of pair is returned where each element is a factor and each
    // pair's first raised to power of second gives the factor.
    std::vector<std::pair<GaloisFieldDict, integer_class>> gf_sqf_list() const;

    // Returns the square free part of the polynomaial in `modulo_`
    GaloisFieldDict gf_sqf_part() const;
    // returns `x**(i * modullo_) % (*this)` for `i` in [0, n)
    // where n = this->degree()
    std::vector<GaloisFieldDict> gf_frobenius_monomial_base() const;
    // computes `f**n % (*this)` in modulo_
    GaloisFieldDict gf_pow_mod(const GaloisFieldDict &f,
                               const integer_class &n) const;
    // uses Frobenius Map to find g.gf_pow_mod(*this, modulo_)
    // i.e. `(*this)**modulo_ % g`
    GaloisFieldDict
    gf_frobenius_map(const GaloisFieldDict &g,
                     const std::vector<GaloisFieldDict> &b) const;
    // For a monic square-free polynomial in modulo_, it returns its distinct
    // degree factorization. Each element's first is a factor and second
    // is used by equal degree factorization.
    std::vector<std::pair<GaloisFieldDict, integer_class>>
    gf_ddf_zassenhaus() const;

    GaloisFieldDict &operator=(GaloisFieldDict &&other) SYMENGINE_NOEXCEPT
    {
        if (this != &other) {
            dict_ = std::move(other.dict_);
            modulo_ = std::move(other.modulo_);
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    friend GaloisFieldDict operator+(const GaloisFieldDict &a,
                                     const GaloisFieldDict &b)
    {
        GaloisFieldDict c = a;
        c += b;
        return c;
    }

    GaloisFieldDict &operator+=(const GaloisFieldDict &other)
    {
        if (modulo_ != other.modulo_)
            throw std::runtime_error("Error: field must be same.");
        if (other.dict_.size() == 0)
            return static_cast<GaloisFieldDict &>(*this);
        if (this->dict_.size() == 0) {
            *this = other;
            return static_cast<GaloisFieldDict &>(*this);
        }
        if (other.dict_.size() < this->dict_.size()) {
            for (unsigned int i = 0; i < other.dict_.size(); i++) {
                integer_class temp;
                temp += dict_[i];
                temp += other.dict_[i];
                if (temp != integer_class(0)) {
                    mp_fdiv_r(temp, temp, modulo_);
                }
                dict_[i] = temp;
            }
        } else {
            for (unsigned int i = 0; i < dict_.size(); i++) {
                integer_class temp;
                temp += dict_[i];
                temp += other.dict_[i];
                if (temp != integer_class(0)) {
                    mp_fdiv_r(temp, temp, modulo_);
                }
                dict_[i] = temp;
            }
            if (other.dict_.size() == this->dict_.size())
                gf_istrip();
            else
                dict_.insert(dict_.end(), other.dict_.begin() + dict_.size(),
                             other.dict_.end());
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator+=(const integer_class &other)
    {
        if (dict_.empty() or other == integer_class(0))
            return static_cast<GaloisFieldDict &>(*this);
        integer_class temp = dict_[0] + other;
        mp_fdiv_r(temp, temp, modulo_);
        dict_[0] = temp;
        if (dict_.size() == 1)
            gf_istrip();
        return static_cast<GaloisFieldDict &>(*this);
    }

    friend GaloisFieldDict operator-(const GaloisFieldDict &a,
                                     const GaloisFieldDict &b)
    {
        GaloisFieldDict c = a;
        c -= b;
        return c;
    }

    GaloisFieldDict operator-() const
    {
        GaloisFieldDict o(*this);
        for (auto &a : o.dict_) {
            a *= -1;
            a += modulo_;
        }
        return o;
    }

    GaloisFieldDict &negate()
    {
        for (auto &a : dict_) {
            a *= -1;
            a += modulo_;
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator-=(const integer_class &other)
    {
        return *this += (-1 * other);
    }

    GaloisFieldDict &operator-=(const GaloisFieldDict &other)
    {
        if (modulo_ != other.modulo_)
            throw std::runtime_error("Error: field must be same.");
        if (other.dict_.size() == 0)
            return static_cast<GaloisFieldDict &>(*this);
        if (this->dict_.size() == 0) {
            *this = -other;
            return static_cast<GaloisFieldDict &>(*this);
        }
        if (other.dict_.size() < this->dict_.size()) {
            for (unsigned int i = 0; i < other.dict_.size(); i++) {
                integer_class temp;
                temp += dict_[i];
                temp -= other.dict_[i];
                if (temp != integer_class(0)) {
                    mp_fdiv_r(temp, temp, modulo_);
                }
                dict_[i] = temp;
            }
        } else {
            for (unsigned int i = 0; i < dict_.size(); i++) {
                integer_class temp;
                temp += dict_[i];
                temp -= other.dict_[i];
                if (temp != integer_class(0)) {
                    mp_fdiv_r(temp, temp, modulo_);
                }
                dict_[i] = temp;
            }
            if (other.dict_.size() == this->dict_.size())
                gf_istrip();
            else {
                unsigned int orig_size = dict_.size();
                dict_.resize(other.dict_.size());
                for (unsigned int i = orig_size; i < other.dict_.size(); i++) {
                    dict_[i] = -other.dict_[i];
                    dict_[i] += modulo_;
                }
            }
        }
        return static_cast<GaloisFieldDict &>(*this);
    }

    static GaloisFieldDict mul(const GaloisFieldDict &a,
                               const GaloisFieldDict &b)
    {
        // TODO
        if (a.modulo_ != b.modulo_)
            throw std::runtime_error("Error: field must be same.");
        if (a.get_dict().empty())
            return a;
        if (b.get_dict().empty())
            return b;

        GaloisFieldDict p;
        p.dict_.resize(a.degree() + b.degree() + 1, integer_class(0));
        p.modulo_ = a.modulo_;
        for (unsigned int i = 0; i <= a.degree(); i++)
            for (unsigned int j = 0; j <= b.degree(); j++) {
                auto temp = a.dict_[i];
                temp *= b.dict_[j];
                if (temp != integer_class(0)) {
                    auto t = p.dict_[i + j];
                    t += temp;
                    mp_fdiv_r(t, t, a.modulo_);
                    p.dict_[i + j] = t;
                }
            }
        p.gf_istrip();
        return p;
    }

    friend GaloisFieldDict operator*(const GaloisFieldDict &a,
                                     const GaloisFieldDict &b)
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
            if (arg != integer_class(0)) {
                arg *= other;
                mp_fdiv_r(arg, arg, modulo_);
            }
        }
        gf_istrip();
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator*=(const GaloisFieldDict &other)
    {
        if (modulo_ != other.modulo_)
            throw std::runtime_error("Error: field must be same.");
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);

        auto o_dict = other.dict_;
        if (o_dict.empty()) {
            dict_.clear();
            return static_cast<GaloisFieldDict &>(*this);
        }

        // ! other is a just constant term
        if (o_dict.size() == 1) {
            for (auto &arg : dict_) {
                if (arg != integer_class(0)) {
                    arg *= o_dict[0];
                    mp_fdiv_r(arg, arg, modulo_);
                }
            }
            gf_istrip();
            return static_cast<GaloisFieldDict &>(*this);
        }
        // mul will return a stripped dict
        GaloisFieldDict res = GaloisFieldDict::mul(
            static_cast<GaloisFieldDict &>(*this), other);
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
            if (arg != integer_class(0)) {
                arg *= inv;
                mp_fdiv_r(arg, arg, modulo_);
            }
        }
        gf_istrip();
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator/=(const GaloisFieldDict &other)
    {
        if (modulo_ != other.modulo_)
            throw std::runtime_error("Error: field must be same.");
        auto dict_divisor = other.dict_;
        if (dict_divisor.empty()) {
            throw std::runtime_error("ZeroDivisionError");
        }
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);
        integer_class inv;
        mp_invert(inv, *(dict_divisor.rbegin()), modulo_);

        // ! other is a just constant term
        if (dict_divisor.size() == 1) {
            for (auto &iter : dict_) {
                if (iter != 0) {
                    iter *= inv;
                    mp_fdiv_r(iter, iter, modulo_);
                }
            }
            return static_cast<GaloisFieldDict &>(*this);
        }
        std::vector<integer_class> dict_out;
        size_t deg_dividend = this->degree();
        size_t deg_divisor = other.degree();
        if (deg_dividend < deg_divisor) {
            dict_.clear();
            return static_cast<GaloisFieldDict &>(*this);
        }
        dict_out.swap(dict_);
        dict_.resize(deg_dividend - deg_divisor + 1);
        integer_class coeff;
        for (auto riter = deg_dividend; riter >= deg_divisor; --riter) {
            coeff = dict_out[riter];
            auto lb = deg_divisor + riter > deg_dividend
                          ? deg_divisor + riter - deg_dividend
                          : 0;
            auto ub = std::min(riter + 1, deg_divisor);
            for (unsigned j = lb; j < ub; ++j) {
                mp_addmul(coeff, dict_out[riter - j + deg_divisor],
                          -dict_divisor[j]);
            }
            coeff *= inv;
            mp_fdiv_r(coeff, coeff, modulo_);
            dict_out[riter] = dict_[riter - deg_divisor] = coeff;
        }
        gf_istrip();
        return static_cast<GaloisFieldDict &>(*this);
    }

    template <class T>
    friend GaloisFieldDict operator%(const GaloisFieldDict &a, const T &b)
    {
        GaloisFieldDict c = a;
        c %= b;
        return c;
    }

    GaloisFieldDict &operator%=(const integer_class &other)
    {
        if (other == integer_class(0)) {
            throw std::runtime_error("ZeroDivisionError");
        }
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);
        dict_.clear();
        return static_cast<GaloisFieldDict &>(*this);
    }

    GaloisFieldDict &operator%=(const GaloisFieldDict &other)
    {
        if (modulo_ != other.modulo_)
            throw std::runtime_error("Error: field must be same.");
        auto dict_divisor = other.dict_;
        if (dict_divisor.empty()) {
            throw std::runtime_error("ZeroDivisionError");
        }
        if (dict_.empty())
            return static_cast<GaloisFieldDict &>(*this);
        integer_class inv;
        mp_invert(inv, *(dict_divisor.rbegin()), modulo_);

        // ! other is a just constant term
        if (dict_divisor.size() == 1) {
            dict_.clear();
            return static_cast<GaloisFieldDict &>(*this);
        }
        std::vector<integer_class> dict_out;
        size_t deg_dividend = this->degree();
        size_t deg_divisor = other.degree();
        if (deg_dividend < deg_divisor) {
            return static_cast<GaloisFieldDict &>(*this);
        }
        dict_out.swap(dict_);
        dict_.resize(deg_divisor);
        integer_class coeff;
        for (auto it = deg_dividend + 1; it-- != 0;) {
            coeff = dict_out[it];
            auto lb = deg_divisor + it > deg_dividend
                          ? deg_divisor + it - deg_dividend
                          : 0;
            auto ub = std::min(it + 1, deg_divisor);
            for (size_t j = lb; j < ub; ++j) {
                mp_addmul(coeff, dict_out[it - j + deg_divisor],
                          -dict_divisor[j]);
            }
            if (it >= deg_divisor) {
                coeff *= inv;
                mp_fdiv_r(coeff, coeff, modulo_);
                dict_out[it] = coeff;
            } else {
                mp_fdiv_r(coeff, coeff, modulo_);
                dict_out[it] = dict_[it] = coeff;
            }
        }
        gf_istrip();
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

    unsigned int size() const
    {
        return dict_.size();
    }

    bool empty() const
    {
        return dict_.empty();
    }

    unsigned degree() const
    {
        if (dict_.empty())
            return 0;
        return dict_.size() - 1;
    }

    const std::vector<integer_class> &get_dict() const
    {
        return dict_;
    }

    void gf_istrip()
    {
        for (auto i = dict_.size(); i-- != 0;) {
            if (dict_[i] == integer_class(0))
                dict_.pop_back();
            else
                break;
        }
    }

    bool is_one() const
    {
        if (dict_.size() == 1)
            if (dict_[0] == integer_class(1))
                return true;
        return false;
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
    inline const std::vector<integer_class> &get_dict() const
    {
        return poly_.dict_;
    }

    inline unsigned int get_degree() const
    {
        return poly_.degree();
    }
};

inline RCP<const GaloisField> gf_poly(RCP<const Symbol> i,
                                      GaloisFieldDict &&dict)
{
    return GaloisField::from_dict(i, std::move(dict));
}

inline RCP<const GaloisField> gf_poly(RCP<const Symbol> i, map_uint_mpz &&dict,
                                      integer_class modulo_)
{
    GaloisFieldDict wrapper(dict, modulo_);
    return GaloisField::from_dict(i, std::move(wrapper));
}
}

#endif
