/**
 *  \file uintpoly_piranha.h
 *  Class for Polynomial: UIntPolyPiranha
 **/
#ifndef SYMENGINE_UINTPOLY_PIRANHA_H
#define SYMENGINE_UINTPOLY_PIRANHA_H

#include <symengine/polys/upolybase.h>
#include <symengine/dict.h>

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
template <typename T, typename U>
struct pow_impl<T, U,
                SymEngine::
                    enable_if_t<std::is_same<T, SymEngine::integer_class>::value
                                && std::is_integral<U>::value>> {
    template <typename T2>
    SymEngine::integer_class operator()(const SymEngine::integer_class &r,
                                        const T2 &x) const
    {
        SymEngine::integer_class res;
        mp_pow_ui(res, r, x);
        return res;
    }
};
}
}
#endif

namespace SymEngine
{
using pmonomial = piranha::monomial<unsigned int>;
using pintpoly = piranha::polynomial<integer_class, pmonomial>;
using pterm = pintpoly::term_type;

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
    static RCP<const UIntPolyPiranha>
    from_vec(const RCP<const Symbol> &var, const std::vector<integer_class> &v);

    integer_class eval(const integer_class &x) const;
    integer_class get_coeff(unsigned int x) const;

    inline unsigned int get_degree() const
    {
        return poly_.degree();
    }

    unsigned int size() const
    {   
        if (poly_.size() == 0)
            return 0;
        return get_degree() + 1;
    }

}; // UIntPolyPiranha
}

#endif // HAVE_SYMENGINE_PIRANHA

#endif // SYMENGINE_UINTPOLY_PIRANHA_H