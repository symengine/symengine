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
using fz_t = SymEngine::fmpz_wrapper;

namespace SymEngine
{

template <typename D, template <typename X, typename Y> class BaseType,
          typename P>
class UFlintPoly : public BaseType<D, P>
{
public:
    using C = typename BaseType<D, P>::coef_type;

    UFlintPoly(const RCP<const Basic> &var, D &&dict)
        : BaseType<D, P>(var, std::move(dict))
    {
    }

    int compare(const Basic &o) const
    {
        SYMENGINE_ASSERT(is_a<P>(o))
        const P &s = static_cast<const P &>(o);

        if (this->poly_.degree() != s.poly_.degree())
            return (this->poly_.degree() < s.poly_.degree()) ? -1 : 1;

        int cmp = this->var_->compare(*s.var_);
        if (cmp != 0)
            return cmp;

        for (unsigned int i = 0; i < this->poly_.length(); ++i) {
            if (this->poly_.coeff(i) != s.poly_.coeff(i))
                return (this->poly_.coeff(i) < s.poly_.coeff(i)) ? -1 : 1;
        }
        return 0;
    }

    static D cont_from_dict(const RCP<const Basic> &var,
                            std::map<unsigned, C> &&d)
    {
        D f;
        for (auto const &p : d) {
            if (p.second != 0) {
                typename D::internal_coef_type r(get_mp_t(p.second));
                f.set_coeff(p.first, r);
            }
        }
        return std::move(f);
    }

    static RCP<const P> from_vec(const RCP<const Basic> &var,
                                 const std::vector<C> &v)
    {
        // TODODO improve this (we already know the degree)
        D f;
        for (unsigned int i = 0; i < v.size(); i++) {
            if (v[i] != 0) {
                typename D::internal_coef_type r(get_mp_t(v[i]));
                f.set_coeff(i, r);
            }
        }
        return make_rcp<const P>(var, std::move(f));
    }

    C eval(const C &x) const
    {
        typename D::internal_coef_type r(get_mp_t(x));
        return to_mp_class(this->poly_.eval(r));
    }

    std::vector<C> multieval(const std::vector<C> &v) const
    {
        std::vector<C> res(v.size());
        for (unsigned int i = 0; i < v.size(); ++i)
            res[i] = eval(v[i]);
        return res;
    }

    C get_coeff(unsigned int x) const
    {
        return to_mp_class(this->poly_.coeff(x));
    }

    // TODODO chage this to reference
    typename D::internal_coef_type get_coeff_ref(unsigned int x) const
    {
        static typename D::internal_coef_type FZERO(0);
        if (x > this->poly_.degree())
            return FZERO;
        return this->poly_.coeff(x);
    }

    typedef ContainerForIter<P, typename D::internal_coef_type> iterator;
    typedef ContainerRevIter<P, typename D::internal_coef_type> r_iterator;
    iterator begin() const
    {
        return iterator(this->template rcp_from_this_cast<P>(), 0);
    }
    iterator end() const
    {
        return iterator(this->template rcp_from_this_cast<P>(), size());
    }
    r_iterator obegin() const
    {
        return r_iterator(this->template rcp_from_this_cast<P>(),
                          (long)size() - 1);
    }
    r_iterator oend() const
    {
        return r_iterator(this->template rcp_from_this_cast<P>(), -1);
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
    std::size_t __hash__() const;
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

    fzp_t divres;
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
