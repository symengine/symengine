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

// can be removed once mpz_class constructors have noexempt
// they have been included in development, not yet released
#if SYMENGINE_INTEGER_CLASS == SYMENGINE_GMPXX
namespace piranha
{
template <typename T>
struct enable_noexcept_checks<T,typename std::enable_if<std::is_same<T,SymEngine::integer_class>::value>::type>
{
    static const bool value = false;
};
}
#endif

namespace SymEngine
{
using pmonomial = piranha::monomial<unsigned int>;
using pintpoly = piranha::polynomial<integer_class, pmonomial>;
using pterm = pintpoly::term_type;

class UIntPolyPiranha : public UPolyBase<pintpoly, UIntPolyPiranha>
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

    // integer_class eval(const integer_class &x) const;
    // integer_class get_coeff(unsigned int x) const;

    inline unsigned int get_degree() const
    {
        return poly_.degree();
    }

}; // UIntPolyPiranha
}

#endif // HAVE_SYMENGINE_PIRANHA

#endif // SYMENGINE_UINTPOLY_PIRANHA_H