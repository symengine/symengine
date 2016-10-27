/**
 *  \file uintpoly_flint.h
 *  Class for Polynomial: UIntPolyFlint
 **/
#ifndef SYMENGINE_UINTPOLY_FLINT_H
#define SYMENGINE_UINTPOLY_FLINT_H

#include <symengine/polys/upolybase.h>

#ifdef HAVE_SYMENGINE_FLINT
#include <symengine/flint_wrapper.h>
using fzp_t = SymEngine::fmpz_poly_wrapper;
using fqp_t = SymEngine::fmpq_poly_wrapper;

namespace SymEngine
{

template <typename Container, template <typename X, typename Y> class BaseType,
          typename Poly>
class UFlintPoly : public BaseType<Container, Poly>
{
public:
    using Cf = typename BaseType<Container, Poly>::coef_type;

    UFlintPoly(const RCP<const Basic> &var, Container &&dict)
        : BaseType<Container, Poly>(var, std::move(dict))
    {
    }

    int compare(const Basic &o) const
    {
        SYMENGINE_ASSERT(is_a<Poly>(o))
        const Poly &s = static_cast<const Poly &>(o);

        if (this->poly_.degree() != s.poly_.degree())
            return (this->poly_.degree() < s.poly_.degree()) ? -1 : 1;

        int cmp = this->var_->compare(*s.var_);
        if (cmp != 0)
            return cmp;

        for (unsigned int i = 0; i < this->poly_.length(); ++i) {
            if (this->poly_.get_coeff(i) != s.poly_.get_coeff(i))
                return (this->poly_.get_coeff(i) < s.poly_.get_coeff(i)) ? -1
                                                                         : 1;
        }
        return 0;
    }

    static Container container_from_dict(const RCP<const Basic> &var,
                                         std::map<unsigned, Cf> &&d)
    {
        Container f;
        for (auto const &p : d) {
            if (p.second != 0) {
                typename Container::internal_coef_type r(get_mp_t(p.second));
                f.set_coeff(p.first, r);
            }
        }
        return std::move(f);
    }

    static RCP<const Poly> from_vec(const RCP<const Basic> &var,
                                    const std::vector<Cf> &v)
    {
        // TODO improve this (we already know the degree)
        Container f;
        for (unsigned int i = 0; i < v.size(); i++) {
            if (v[i] != 0) {
                typename Container::internal_coef_type r(get_mp_t(v[i]));
                f.set_coeff(i, r);
            }
        }
        return make_rcp<const Poly>(var, std::move(f));
    }

    template <typename FromPoly>
    static enable_if_t<is_a_UPoly<FromPoly>::value, RCP<const Poly>>
    from_poly(const FromPoly &p)
    {
        Container f;
        for (auto it = p.begin(); it != p.end(); ++it) {
            typename Container::internal_coef_type r(get_mp_t(it->second));
            f.set_coeff(it->first, r);
        }
        return Poly::from_container(p.get_var(), std::move(f));
    }

    Cf eval(const Cf &x) const
    {
        typename Container::internal_coef_type r(get_mp_t(x));
        return to_mp_class(this->poly_.eval(r));
    }

    Cf get_coeff(unsigned int x) const
    {
        return to_mp_class(this->poly_.get_coeff(x));
    }

    // can't return by reference
    Cf get_coeff_ref(unsigned int x) const
    {
        return to_mp_class(this->poly_.get_coeff(x));
    }

    typedef ContainerForIter<Poly, Cf> iterator;
    typedef ContainerRevIter<Poly, Cf> r_iterator;
    iterator begin() const
    {
        return iterator(this->template rcp_from_this_cast<Poly>(), 0);
    }
    iterator end() const
    {
        return iterator(this->template rcp_from_this_cast<Poly>(), size());
    }
    r_iterator obegin() const
    {
        return r_iterator(this->template rcp_from_this_cast<Poly>(),
                          (long)size() - 1);
    }
    r_iterator oend() const
    {
        return r_iterator(this->template rcp_from_this_cast<Poly>(), -1);
    }

    unsigned int size() const
    {
        return this->poly_.length();
    }
};

class UIntPolyFlint : public UFlintPoly<fzp_t, UIntPolyBase, UIntPolyFlint>
{
public:
    IMPLEMENT_TYPEID(UINTPOLYFLINT)
    //! Constructor of UIntPolyFlint class
    UIntPolyFlint(const RCP<const Basic> &var, fzp_t &&dict);
    //! \return size of the hash
    hash_t __hash__() const;
}; // UIntPolyFlint

class URatPolyFlint : public UFlintPoly<fqp_t, URatPolyBase, URatPolyFlint>
{
public:
    IMPLEMENT_TYPEID(URATPOLYFLINT)
    //! Constructor of URatPolyFlint class
    URatPolyFlint(const RCP<const Basic> &var, fqp_t &&dict);
    //! \return size of the hash
    hash_t __hash__() const;
}; // URatPolyFlint

template <typename T>
enable_if_t<std::is_same<UIntPolyFlint, T>::value
                or std::is_same<URatPolyFlint, T>::value,
            RCP<const T>>
gcd_upoly(const T &a, const T &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw SymEngineException("Error: variables must agree.");
    return make_rcp<const T>(a.get_var(), a.get_poly().gcd(b.get_poly()));
}

template <typename T>
enable_if_t<std::is_same<UIntPolyFlint, T>::value
                or std::is_same<URatPolyFlint, T>::value,
            RCP<const T>>
lcm_upoly(const T &a, const T &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw SymEngineException("Error: variables must agree.");
    return make_rcp<const T>(a.get_var(), a.get_poly().lcm(b.get_poly()));
}

inline RCP<const UIntPolyFlint> pow_upoly(const UIntPolyFlint &a,
                                          unsigned int p)
{
    return make_rcp<const UIntPolyFlint>(a.get_var(), a.get_poly().pow(p));
}

// temporary, needs to be common ^
inline RCP<const URatPolyFlint> pow_upoly(const URatPolyFlint &a,
                                          unsigned int p)
{
    return make_rcp<const URatPolyFlint>(a.get_var(), a.get_poly().pow(p));
}

template <typename T>
enable_if_t<std::is_same<UIntPolyFlint, T>::value
                or std::is_same<URatPolyFlint, T>::value,
            bool>
divides_upoly(const T &a, const T &b, const Ptr<RCP<const T>> &res)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw SymEngineException("Error: variables must agree.");

    typename T::container_type q, r;

    b.get_poly().divrem(q, r, a.get_poly());
    if (r == 0) {
        *res = make_rcp<T>(a.get_var(), std::move(q));
        return true;
    } else {
        return false;
    }
}
}

#endif // HAVE_SYMENGINE_FLINT

#endif // SYMENGINE_UINTPOLY_FLINT_H
