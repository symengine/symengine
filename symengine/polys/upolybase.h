/**
 *  \file polynomial_int.h
 *  Class for Univariate Polynomial Base
 **/
#ifndef SYMENGINE_UINT_BASE_H
#define SYMENGINE_UINT_BASE_H

#include <symengine/basic.h>

#ifdef HAVE_SYMENGINE_FLINT
#include <flint/flint.h>
#include <flint/fmpzxx.h>
#endif
#ifdef HAVE_SYMENGINE_PIRANHA
#include <piranha/mp_integer.hpp>
#include <piranha/mp_rational.hpp>
#endif


 
namespace SymEngine
{

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

    static Wrapper from_vec(const std::vector<Value> &v)
    {
        Wrapper x;
        x.dict_ = {};
        for (unsigned int i = 0; i < v.size(); i++) {
            if (v[i] != Value(0)) {
                x.dict_[i] = v[i];
            }
        }
        return x;
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
        if (dict_.empty())
            return static_cast<Wrapper &>(*this);

        if (other.dict_.empty()) {
            *this = other;
            return static_cast<Wrapper &>(*this);
        }

        // ! other is a just constant term
        if (other.dict_.size() == 1
            and other.dict_.find(0) != other.dict_.end()) {
            for (const auto &i1 : dict_)
                for (const auto &i2 : other.dict_)
                    dict_[i1.first] = i1.second * i2.second;
            return static_cast<Wrapper &>(*this);
        }

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

    Value get_coeff(Key x) const
    {
        auto ite = dict_.find(x);
        if (ite != dict_.end())
            return ite->second;
        return Value(0);
    }
};

template <typename Container, typename Poly>
class UPolyBase : public Basic
{
protected:
    RCP<const Symbol> var_;
    Container poly_;

public:
    UPolyBase(const RCP<const Symbol> &var, Container &&container)
        : var_{var}, poly_{container}
    {
    }

    //! \returns `-1`,`0` or `1` after comparing
    virtual int compare(const Basic &o) const = 0;
    virtual std::size_t __hash__() const = 0;

    //! \returns `true` if two objects are equal
    inline bool __eq__(const Basic &o) const
    {
        return eq(*var_, *(static_cast<const Poly &>(o).var_))
               and poly_ == static_cast<const Poly &>(o).poly_;
    }

    inline const RCP<const Symbol> &get_var() const
    {
        return var_;
    }

    inline const Container &get_poly() const
    {
        return poly_;
    }

    // TODO add as_symbolic()
    inline vec_basic get_args() const
    {
        return {};
    }

    static RCP<const Poly> from_container(const RCP<const Symbol> &var,
                                          Container &&d)
    {
        return make_rcp<const Poly>(var, std::move(d));
    }
};

template <typename Container, typename Poly>
class UIntPolyBase : public UPolyBase<Container, Poly>
{
public:
    UIntPolyBase(const RCP<const Symbol> &var, Container &&container)
        : UPolyBase<Container, Poly>(var, std::move(container))
    {
    }

    inline unsigned int get_degree() const
    {
        return this->poly_.degree();
    }

    virtual integer_class get_coeff(unsigned int i) const = 0;
    virtual integer_class eval(const integer_class &x) const = 0;
};

template <typename Poly>
RCP<const Poly> add_upoly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict += b.get_poly();
    return Poly::from_container(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> neg_upoly(const Poly &a)
{
    auto dict = a.get_poly();
    dict = -dict;
    return Poly::from_container(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> sub_upoly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict -= b.get_poly();
    return Poly::from_container(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> mul_upoly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict *= b.get_poly();
    return Poly::from_container(a.get_var(), std::move(dict));
}

// misc methods
#if SYMENGINE_INTEGER_CLASS == SYMENGINE_GMPXX                                 \
    || SYMENGINE_INTEGER_CLASS == SYMENGINE_GMP
#ifdef HAVE_SYMENGINE_FLINT
inline integer_class to_integer_class(const flint::fmpzxx &i)
{
    integer_class x;
    fmpz_get_mpz(x.get_mpz_t(), i._data().inner);
    return x;
}
#endif

#ifdef HAVE_SYMENGINE_PIRANHA
inline integer_class to_integer_class(const piranha::integer &i)
{
    integer_class x;
    mpz_set(x.get_mpz_t(), i.get_mpz_view());
    return x;
}
#endif

#elif SYMENGINE_INTEGER_CLASS == SYMENGINE_PIRANHA
#ifdef HAVE_SYMENGINE_FLINT
inline integer_class to_integer_class(const flint::fmpzxx &i)
{
    integer_class x;
    fmpz_get_mpz(x.get_mpz_t(), i._data().inner);
    return x;
}
#endif

#elif SYMENGINE_INTEGER_CLASS == SYMENGINE_FLINT
#ifdef HAVE_SYMENGINE_PIRANHA
inline integer_class to_integer_class(const piranha::integer &x)
{
    return integer_class(x.get_mpz_view());
}
#endif

inline integer_class to_integer_class(const flint::fmpzxx &i)
{
    return integer_class(i._data().inner);
}
#endif
}

#endif // SYMENGINE_UINT_BASE_H