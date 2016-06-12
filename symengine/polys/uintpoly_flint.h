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

class UIntPolyFlint : public UIntPolyBase<flint::fmpz_polyxx, UIntPolyFlint>
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

    integer_class eval(const integer_class &x) const;
    integer_class get_coeff(unsigned int x) const;

    typedef ContainerForIter<UIntPolyFlint> iterator;
    typedef ContainerRevIter<UIntPolyFlint> reverse_iterator;
    iterator begin() const
    {
        return iterator(rcp_from_this_cast<UIntPolyFlint>(), 0);
    }
    iterator end() const
    {
        return iterator(rcp_from_this_cast<UIntPolyFlint>(), size());
    }
    reverse_iterator obegin() const
    {
        return reverse_iterator(rcp_from_this_cast<UIntPolyFlint>(), (long)size() - 1);
    }
    reverse_iterator oend() const
    {
        return reverse_iterator(rcp_from_this_cast<UIntPolyFlint>(), -1);
    }

    unsigned int size() const
    {
        return poly_.length();
    }

}; // UIntPolyFLint
}

#endif // HAVE_SYMENGINE_FLINT

#endif // SYMENGINE_UINTPOLY_FLINT_H