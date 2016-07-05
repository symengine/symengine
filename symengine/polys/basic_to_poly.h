#include <symengine/visitor.h>

namespace SymEngine
{

template <typename T>
enable_if_t<std::is_same<T, UExprDict>::value, UExprDict>
_b_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen);

template <typename T>
enable_if_t<std::is_same<T, UIntDict>::value, UIntDict>
_b_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen);

template <typename P, typename V>
class BasicToUPolyBase : public BaseVisitor<V>
{
public:
    RCP<const Basic> gen;
    using D = typename P::container_type;
    D dict;

    D apply(const Basic &b, const RCP<const Basic> &gen_)
    {
        gen = gen_;
        b.accept(*this);
        return dict;
    }

    virtual void d_add_term(unsigned int pow, const Basic &x) = 0;

    void bvisit(const Pow &x)
    {
        if (is_a<const Integer>(*x.get_exp())) {
            int i = rcp_static_cast<const Integer>(x.get_exp())->as_int();
            if (i > 0) {
                dict = D::pow(_b_to_upoly<D>(x.get_base(), gen), i);
                return;
            }
        }

        RCP<const Basic> genbase = gen, genpow = one, coef = one, tmp;
        if (is_a<const Pow>(*gen)) {
            genbase = rcp_static_cast<const Pow>(gen)->get_base();
            genpow = rcp_static_cast<const Pow>(gen)->get_exp();
        }

        if (eq(*genbase, *x.get_base())) {

            set_basic expos;

            if (is_a<const Add>(*x.get_exp())) {
                RCP<const Add> addx = rcp_static_cast<const Add>(x.get_exp());
                for (auto const &it : addx->dict_)
                    expos.insert(mul(it.first, it.second));
                if (not addx->coef_->is_zero())
                    expos.insert(addx->coef_);
            } else {
                expos.insert(x.get_exp());
            }

            int powr = 0;
            for (auto const &it : expos) {
                tmp = div(it, genpow);
                if (is_a<const Integer>(*tmp)) {
                    RCP<const Integer> i = rcp_static_cast<const Integer>(tmp);
                    if (i->is_positive()) {
                        powr = i->as_int();
                        continue;
                    }
                }
                coef = mul(coef, pow(genbase, it));
            }
            d_add_term(powr, *coef);

        } else {
            d_add_term(0, x);
        }
    }

    void bvisit(const Add &x)
    {
        dict = _b_to_upoly<D>(x.coef_, gen);
        for (auto const &it : x.dict_)
            dict += (_b_to_upoly<D>(it.first, gen)
                     * _b_to_upoly<D>(it.second, gen));
    }

    void bvisit(const Mul &x)
    {
        dict = _b_to_upoly<D>(x.coef_, gen);
        for (auto const &it : x.dict_)
            dict *= _b_to_upoly<D>(pow(it.first, it.second), gen);
    }

    void bvisit(const Integer &x)
    {
        integer_class i = x.i;
        if (i != 0)
            dict
                = P::container_from_dict(gen, {{0, typename D::value_type(i)}});
    }

    void bvisit(const Basic &x)
    {
        RCP<const Basic> genpow = one, genbase = gen, powr;
        if (is_a<const Pow>(*gen)) {
            genpow = rcp_static_cast<const Pow>(gen)->get_exp();
            genbase = rcp_static_cast<const Pow>(gen)->get_base();
        }

        if (eq(*genbase, x)) {
            powr = div(one, genpow);
            if (is_a<const Integer>(*powr)) {
                int i = rcp_static_cast<const Integer>(powr)->as_int();
                if (i > 0) {
                    dict = P::container_from_dict(
                        gen, {{i, typename D::value_type(1)}});
                    return;
                }
            }
        }

        d_add_term(0, x);
    }
};

class BasicToUIntPoly : public BasicToUPolyBase<UIntPoly, BasicToUIntPoly>
{
public:
    using BasicToUPolyBase<UIntPoly, BasicToUIntPoly>::bvisit;
    using BasicToUPolyBase<UIntPoly, BasicToUIntPoly>::apply;

    void bvisit(const Rational &x)
    {
        throw std::runtime_error("Non-integer found");
    }

    void d_add_term(unsigned int pow, const Basic &x)
    {
        if (is_a<const Integer>(x))
            dict = UIntDict({{pow, static_cast<const Integer &>(x).i}});
        else
            throw std::runtime_error("Non-integer found");
    }
};

class BasicToUExprPoly : public BasicToUPolyBase<UExprPoly, BasicToUExprPoly>
{
public:
    using BasicToUPolyBase<UExprPoly, BasicToUExprPoly>::bvisit;
    using BasicToUPolyBase<UExprPoly, BasicToUExprPoly>::apply;

    void bvisit(const Rational &x)
    {
        if (not x.is_zero())
            dict = UExprDict(x.rcp_from_this());
    }

    void d_add_term(unsigned int pow, const Basic &x)
    {
        dict = UExprDict({{pow, x.rcp_from_this()}});
    }
};

template <typename T>
enable_if_t<std::is_same<T, UExprDict>::value, UExprDict>
_b_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen)
{
    BasicToUExprPoly v;
    return v.apply(*basic, gen);
}

template <typename T>
enable_if_t<std::is_same<T, UIntDict>::value, UIntDict>
_b_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen)
{
    BasicToUIntPoly v;
    return v.apply(*basic, gen);
}
}