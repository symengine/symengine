#ifndef SYMENGINE_USYMENGINEPOLY_H
#define SYMENGINE_USYMENGINEPOLY_H

#include <symengine/polys/upolybase.h>

namespace SymEngine
{

template <typename D, template <typename X, typename Y> class BaseType,
          typename P>
class USymEnginePoly : public BaseType<D, P>
{
public:
    using C = typename BaseType<D, P>::coef_type;

    USymEnginePoly(const RCP<const Basic> &var, D &&dict)
        : BaseType<D, P>(var, std::move(dict))
    {
    }

    int compare(const Basic &o) const
    {
        SYMENGINE_ASSERT(is_a<P>(o))
        const P &s = static_cast<const P &>(o);

        if (this->poly_.size() != s.poly_.size())
            return (this->poly_.size() < s.poly_.size()) ? -1 : 1;

        int cmp = unified_compare(this->var_, s.var_);
        if (cmp != 0)
            return cmp;

        return unified_compare(this->poly_.dict_, s.poly_.dict_);
    }

    bool is_canonical(const D &dict) const
    {
        // Check if dictionary contains terms with coeffienct 0
        for (auto iter : dict.dict_)
            if (iter.second == 0)
                return false;
        return true;
    }

    static RCP<const P> from_vec(const RCP<const Basic> &var,
                                 const std::vector<C> &v)
    {
        return make_rcp<const P>(var, D::from_vec(v));
    }

    static D cont_from_dict(const RCP<const Basic> &var,
                            std::map<unsigned, C> &&d)
    {
        return std::move(D(d));
    }

    C eval(const C &x) const
    {
        unsigned int last_deg = this->poly_.dict_.rbegin()->first;
        C result(0), x_pow;

        for (auto it = this->poly_.dict_.rbegin();
             it != this->poly_.dict_.rend(); ++it) {
            mp_pow_ui(x_pow, x, last_deg - (*it).first);
            last_deg = (*it).first;
            result = (*it).second + x_pow * result;
        }
        mp_pow_ui(x_pow, x, last_deg);
        result *= x_pow;

        return result;
    }

    std::vector<C> multieval(const std::vector<C> &v) const
    {
        // this is not the optimal algorithm
        std::vector<C> res(v.size());
        for (unsigned int i = 0; i < v.size(); ++i)
            res[i] = eval(v[i]);
        return res;
    }

    inline const std::map<unsigned, C> &get_dict() const
    {
        return this->poly_.dict_;
    }

    inline C get_coeff(unsigned int x) const
    {
        return this->poly_.get_coeff(x);
    }

    typedef typename std::map<unsigned, C>::const_iterator iterator;
    typedef typename std::map<unsigned, C>::const_reverse_iterator r_iterator;
    iterator begin() const
    {
        return this->poly_.dict_.begin();
    }
    iterator end() const
    {
        return this->poly_.dict_.end();
    }
    r_iterator obegin() const
    {
        return this->poly_.dict_.rbegin();
    }
    r_iterator oend() const
    {
        return this->poly_.dict_.rend();
    }

    unsigned int size() const
    {
        if (this->poly_.dict_.empty())
            return 0;
        return this->get_degree() + 1;
    }
};
}

#endif