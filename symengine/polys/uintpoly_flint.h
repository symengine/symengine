/**
 *  \file uintpoly_flint.h
 *  Class for Polynomial: UIntPolyFlint
 **/
#ifndef SYMENGINE_UINTPOLY_FLINT_H
#define SYMENGINE_UINTPOLY_FLINT_H

#include <symengine/polys/upolybase.h>

#ifdef HAVE_SYMENGINE_FLINT
#include <symengine/flint_wrapper.h>
using fp_t = SymEngine::fmpz_poly_wrapper;
using fz_t = SymEngine::fmpz_wrapper;

namespace SymEngine
{

class UIntPolyFlint : public UIntPolyBase<fp_t, UIntPolyFlint>
{
public:
    IMPLEMENT_TYPEID(UINTPOLYFLINT)
    //! Constructor of UIntPolyFlint class
    UIntPolyFlint(const RCP<const Symbol> &var, fp_t &&dict);
    //! \return size of the hash
    std::size_t __hash__() const;
    int compare(const Basic &o) const;

    static RCP<const UIntPolyFlint> from_dict(const RCP<const Symbol> &var,
                                              map_uint_mpz &&d);
    static RCP<const UIntPolyFlint> from_vec(const RCP<const Symbol> &var,
                                             const vec_integer_class &v);

    integer_class eval(const integer_class &x) const;
    vec_integer_class multieval(const vec_integer_class &v) const;

    integer_class get_coeff(unsigned int x) const;
    fz_t get_coeff_ref(unsigned int x) const;

    typedef ContainerForIter<UIntPolyFlint, fz_t> iterator;
    typedef ContainerRevIter<UIntPolyFlint, fz_t> reverse_iterator;
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
        return reverse_iterator(rcp_from_this_cast<UIntPolyFlint>(),
                                (long)size() - 1);
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

inline RCP<const UIntPolyFlint> gcd_upoly(const UIntPolyFlint &a,
                                          const UIntPolyFlint &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");
    return make_rcp<const UIntPolyFlint>(a.get_var(),
                                         a.get_poly().gcd(b.get_poly()));
}

inline RCP<const UIntPolyFlint> lcm_upoly(const UIntPolyFlint &a,
                                          const UIntPolyFlint &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");
    return make_rcp<const UIntPolyFlint>(a.get_var(),
                                         a.get_poly().lcm(b.get_poly()));
}

inline RCP<const UIntPolyFlint> pow_upoly(const UIntPolyFlint &a,
                                          unsigned int p)
{
    return make_rcp<const UIntPolyFlint>(a.get_var(), a.get_poly().pow(p));
}

inline bool divides_upoly(const UIntPolyFlint &a, const UIntPolyFlint &b,
                          const Ptr<RCP<const UIntPolyFlint>> &res)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    fp_t divres;
    bool div_f = a.get_poly().divides(b.get_poly(), divres);
    if (div_f) {
        *res = make_rcp<UIntPolyFlint>(a.get_var(), std::move(divres));
        return true;
    } else {
        return false;
    }
}
}

#endif // HAVE_SYMENGINE_FLINT

#endif // SYMENGINE_UINTPOLY_FLINT_H
