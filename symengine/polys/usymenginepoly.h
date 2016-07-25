#ifndef SYMENGINE_USYMENGINEPOLY_H
#define SYMENGINE_USYMENGINEPOLY_H

#include <symengine/polys/upolybase.h>

namespace SymEngine
{

template <typename Container, template <typename X, typename Y> class BaseType,
          typename Poly, typename Key>
class USymEnginePoly : public BaseType<Container, Poly>
{
public:
    using Cf = typename BaseType<Container, Poly>::coef_type;

    USymEnginePoly(const RCP<const Basic> &var, Container &&dict)
        : BaseType<Container, Poly>(var, std::move(dict))
    {
    }

    int compare(const Basic &o) const
    {
        SYMENGINE_ASSERT(is_a<Poly>(o))
        const Poly &s = static_cast<const Poly &>(o);

        if (this->poly_.size() != s.poly_.size())
            return (this->poly_.size() < s.poly_.size()) ? -1 : 1;

        int cmp = unified_compare(this->var_, s.var_);
        if (cmp != 0)
            return cmp;

        return unified_compare(this->poly_.dict_, s.poly_.dict_);
    }

    bool is_canonical(const Container &dict) const
    {
        // Check if dictionary contains terms with coeffienct 0
        for (auto iter : dict.dict_)
            if (iter.second == 0)
                return false;
        return true;
    }

    static RCP<const Poly> from_vec(const RCP<const Basic> &var,
                                    const std::vector<Cf> &v)
    {
        return make_rcp<const Poly>(var, Container::from_vec(v));
    }

    static Container container_from_dict(const RCP<const Basic> &var,
                                    std::map<Key, Cf> &&d)
    {
        return std::move(Container(d));
    }

    Cf eval(const Cf &x) const
    {
        Key last_deg = this->poly_.dict_.rbegin()->first;
        Cf result(0), x_pow;

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

    std::vector<Cf> multieval(const std::vector<Cf> &v) const
    {
        // this is not the optimal algorithm
        std::vector<Cf> res(v.size());
        for (Key i = 0; i < v.size(); ++i)
            res[i] = eval(v[i]);
        return res;
    }

    inline const std::map<Key, Cf> &get_dict() const
    {
        return this->poly_.dict_;
    }

    inline Cf get_coeff(Key x) const
    {
        return this->poly_.get_coeff(x);
    }

    typedef typename std::map<Key, Cf>::const_iterator iterator;
    typedef typename std::map<Key, Cf>::const_reverse_iterator r_iterator;
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

// true & sets `out` to b/a if a exactly divides b, otherwise false & undefined
template <typename Poly>
bool divides_upoly(const Poly &a, const Poly &b,
                   const Ptr<RCP<const Poly>> &out)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto a_poly = a.get_poly();
    auto b_poly = b.get_poly();
    if (a_poly.size() == 0)
        return false;

    using C = typename Poly::container_type;
    using Coef = typename Poly::coef_type;

    std::map<unsigned int, Coef> res;
    C tmp;
    Coef q, r;
    unsigned int a_deg, b_deg;

    while (b_poly.size() >= a_poly.size()) {
        a_deg = a_poly.degree();
        b_deg = b_poly.degree();

        mp_tdiv_qr(q, r, b_poly.get_lc(), a_poly.get_lc());
        if (r != 0)
            return false;

        res[b_deg - a_deg] = q;
        C tmp = C({{b_deg - a_deg, q}});
        b_poly -= (a_poly * tmp);
    }

    if (b_poly.empty()) {
        *out = Poly::from_dict(a.get_var(), std::move(res));
        return true;
    } else {
        return false;
    }
}
}

#endif