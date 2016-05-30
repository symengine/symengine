/**
 *  \file polynomial_int.h
 *  Class for Univariate Polynomial Base
 **/
#ifndef SYMENGINE_UINT_BASE_H
#define SYMENGINE_UINT_BASE_H

#include <symengine/basic.h>

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

template <typename Container, typename Poly, typename Coeff>
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

    // unify these two constructor? another template would be required
    // may solve some more problems, like `get_degree` virtualization
    UPolyBase(const RCP<const Symbol> &var, const std::vector<Coeff> &v)
        : var_{var}
    {
    }

    // create a Poly from a vector of Coeff coefficients
    static RCP<const Poly> from_vec(const RCP<const Symbol> &var,
                                        const std::vector<Coeff> &v)
    {
        return make_rcp<const Poly>(var, std::move(v));
    }

    // TODO think of something to make this purely virtual
    //! \returns the degree of the polynomial
    // virtual unsigned int get_degree() const = 0;

    //! \returns `-1`,`0` or `1` after comparing
    virtual int compare(const Basic &o) const = 0;

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
};

template <typename Poly>
RCP<const Poly> add_upoly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict += b.get_poly();
    return Poly::from_dict(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> neg_upoly(const Poly &a)
{
    auto dict = -(a.get_poly());
    return Poly::from_dict(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> sub_upoly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict -= b.get_poly();
    return Poly::from_dict(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> mul_upoly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict *= b.get_poly();
    return Poly::from_dict(a.get_var(), std::move(dict));
}
}

#endif