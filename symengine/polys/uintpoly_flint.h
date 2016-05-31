/**
 *  \file uintpoly_flint.h
 *  Class for Polynomial: UIntPolyFlint
 **/
#ifndef SYMENGINE_UINTPOLY_FLINT_H
#define SYMENGINE_UINTPOLY_FLINT_H

#include <symengine/polys/upolybase.h>
#include <symengine/dict.h>

#ifdef HAVE_SYMENGINE_FLINT

#include <flint/flint.h>
#include <flint/fmpz_polyxx.h>

namespace SymEngine
{

class UIntPolyFlint : public UPolyBase<flint::fmpz_polyxx, UIntPolyFlint>
{
public:
    IMPLEMENT_TYPEID(UINTPOLYFLINT)
    //! Constructor of UIntPolyFlint class
    UIntPolyFlint(const RCP<const Symbol> &var, flint::fmpz_polyxx &&dict);
    //! \return size of the hash
    std::size_t __hash__() const;
    int compare(const Basic &o) const;

    static RCP<const UIntPolyFlint> from_dict(const RCP<const Symbol> &var,
                                              map_uint_mpz &&d);
    static RCP<const UIntPolyFlint>
    from_vec(const RCP<const Symbol> &var, const std::vector<integer_class> &v);

    virtual vec_basic get_args() const;
    inline unsigned int get_degree() const
    {
        return poly_.degree();
    }

}; // UIntPolyFLint
}

#endif // HAVE_SYMENGINE_FLINT

#endif // SYMENGINE_UINTPOLY_FLINT_H