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

namespace SymEngine
{

class UIntPolyPiranha : public UPolyBase<piranha::polynomial, UIntPolyPiranha>
{
public:
    IMPLEMENT_TYPEID(UINTPOLYPIRANHA)
    // //! Constructor of UIntPolyPiranha class
    // UIntPolyPiranha(const RCP<const Symbol> &var, flint::fmpz_polyxx &&dict);
    // //! \return size of the hash
    // std::size_t __hash__() const;
    // int compare(const Basic &o) const;

    // static RCP<const UIntPolyPiranha> from_dict(const RCP<const Symbol> &var,
    //                                           map_uint_mpz &&d);
    // static RCP<const UIntPolyPiranha>
    // from_vec(const RCP<const Symbol> &var, const std::vector<integer_class> &v);

    // integer_class eval(const integer_class &x) const;
    // integer_class get_coeff(unsigned int x) const;

    // inline unsigned int get_degree() const
    // {
    //     return poly_.degree();
    // }

}; // UIntPolyPiranha
}

#endif // HAVE_SYMENGINE_PIRANHA

#endif // SYMENGINE_UINTPOLY_PIRANHA_H