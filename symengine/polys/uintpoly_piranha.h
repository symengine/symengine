/**
 *  \file uintpoly_piranha.h
 *  Class for Polynomial: UIntPolyPiranha
 **/
#ifndef SYMENGINE_UINTPOLY_PIRANHA_H
#define SYMENGINE_UINTPOLY_PIRANHA_H

#include <symengine/polys/upolybase.h>
#include <symengine/dict.h>
#include <memory>

#ifdef HAVE_SYMENGINE_PIRANHA
#include <piranha/monomial.hpp>
#include <piranha/polynomial.hpp>
#include <piranha/mp_rational.hpp>
#include <piranha/mp_integer.hpp>
#include <piranha/math.hpp>
#include <piranha/type_traits.hpp>

#if SYMENGINE_INTEGER_CLASS != SYMENGINE_PIRANHA
namespace piranha
{

// overloading pow for pirahna::math::evaluate
namespace math
{
template <typename U>
struct pow_impl<SymEngine::integer_class, U, 
                SymEngine::enable_if_t<std::is_integral<U>::value>>
{
    template <typename T2>
    SymEngine::integer_class operator()(const SymEngine::integer_class &r,
                                        const T2 &x) const
    {
        SymEngine::integer_class res;
        mp_pow_ui(res, r, x);
        return res;
    }
};

template <>
struct gcd_impl<SymEngine::integer_class, SymEngine::integer_class>
{
    SymEngine::integer_class operator()(const SymEngine::integer_class &r,
                                        const SymEngine::integer_class &x) const
    {
        SymEngine::integer_class res;
        mp_gcd(res, r, x);
        return res;
    }
};

template <>
struct divexact_impl<SymEngine::integer_class>
{
    SymEngine::integer_class operator()(SymEngine::integer_class &r,
                                        const SymEngine::integer_class &x, const SymEngine::integer_class &y) const
    {
        mp_divexact(r, x, y);
        return r;
    }
};
}

template <>
struct has_exact_ring_operations<SymEngine::integer_class>
{
    static const bool value = true;
};
}
#endif

namespace SymEngine
{
using pmonomial = piranha::monomial<unsigned int>;
using pintpoly = piranha::polynomial<integer_class, pmonomial>;
using pterm = pintpoly::term_type;

class PiranhaForIter
{
    pintpoly::container_type::const_iterator ptr_;

public:
    PiranhaForIter(pintpoly::container_type::const_iterator ptr) : ptr_{ptr}
    {
    }

    bool operator==(const PiranhaForIter &rhs)
    {
        return (ptr_ == rhs.ptr_);
    }

    bool operator!=(const PiranhaForIter &rhs)
    {
        return (ptr_ != rhs.ptr_);
    }

    PiranhaForIter operator++()
    {
        ptr_++;
        return *this;
    }

    std::pair<unsigned int, const integer_class &> operator*()
    {
        return std::make_pair(*(ptr_->m_key.begin()), ptr_->m_cf);
    }

    std::shared_ptr<std::pair<unsigned int, const integer_class &>> operator->()
    {
        return std::make_shared<std::pair<unsigned int, const integer_class &>>(
            *(ptr_->m_key.begin()), ptr_->m_cf);
    }
};

class UIntPolyPiranha : public UIntPolyBase<pintpoly, UIntPolyPiranha>
{
public:
    IMPLEMENT_TYPEID(UINTPOLYPIRANHA)
    //! Constructor of UIntPolyPiranha class
    UIntPolyPiranha(const RCP<const Symbol> &var, pintpoly &&dict);
    //! \return size of the hash
    std::size_t __hash__() const;
    int compare(const Basic &o) const;

    static RCP<const UIntPolyPiranha> from_dict(const RCP<const Symbol> &var,
                                                map_uint_mpz &&d);
    static RCP<const UIntPolyPiranha> from_vec(const RCP<const Symbol> &var,
                                               const vec_integer_class &v);

    integer_class eval(const integer_class &x) const;
    integer_class get_coeff(unsigned int x) const;
    const integer_class &get_coeff_ref(unsigned int x) const;

    inline unsigned int get_degree() const
    {
        return poly_.degree();
    }

    // begin() and end() are unordered
    // obegin() and oend() are ordered, from highest degree to lowest
    typedef PiranhaForIter iterator;
    typedef ContainerRevIter<UIntPolyPiranha, const integer_class &>
        reverse_iterator;
    iterator begin() const
    {
        return iterator(poly_._container().begin());
    }
    iterator end() const
    {
        return iterator(poly_._container().end());
    }
    reverse_iterator obegin() const
    {
        return reverse_iterator(rcp_from_this_cast<UIntPolyPiranha>(),
                                (long)size() - 1);
    }
    reverse_iterator oend() const
    {
        return reverse_iterator(rcp_from_this_cast<UIntPolyPiranha>(), -1);
    }

    unsigned int size() const
    {
        if (poly_.size() == 0)
            return 0;
        return get_degree() + 1;
    }

}; // UIntPolyPiranha

inline RCP<const UIntPolyPiranha> gcd_upoly(const UIntPolyPiranha &a,
                                          const UIntPolyPiranha &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    pintpoly gcdx(std::get<0>(pintpoly::gcd(a.get_poly(), b.get_poly())));
    // following the convention, that leading coefficient should be positive
    if(gcdx.find_cf(pmonomial{gcdx.degree()}) < 0)
        gcdx = -gcdx;
    return make_rcp<const UIntPolyPiranha>(a.get_var(), std::move(gcdx));
}

inline RCP<const UIntPolyPiranha> lcm_upoly(const UIntPolyPiranha &a,
                                          const UIntPolyPiranha &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    pintpoly gcdx(std::get<0>(pintpoly::gcd(a.get_poly(), b.get_poly())));
    pintpoly mulx(a.get_poly() * b.get_poly());
    return make_rcp<const UIntPolyPiranha>(a.get_var(), std::move(pintpoly::udivrem(mulx, gcdx)).first);
}

inline RCP<const UIntPolyPiranha> pow_upoly(const UIntPolyPiranha &a,
                                          unsigned int p)
{
    return make_rcp<const UIntPolyPiranha>(a.get_var(), std::move(piranha::math::pow(a.get_poly(), p)));
}

}

#endif // HAVE_SYMENGINE_PIRANHA

#endif // SYMENGINE_UINTPOLY_PIRANHA_H