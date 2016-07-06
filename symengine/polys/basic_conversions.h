#ifndef SYMENGINE_BASIC_CONVERSIONS_H
#define SYMENGINE_BASIC_CONVERSIONS_H

#include <symengine/visitor.h>

namespace SymEngine
{

// convert a `basic`, to a UPoly `P` (eg. UIntPoly, UExprPoly, UIntPolyFlint)
// using `gen` as the genarator. Throws, if poly constructions not possible.
// `ex` is the optional parameter for expanding the given `basic` or not.
template <typename P>
RCP<const P> from_basic(const RCP<const Basic> &basic,
                        const RCP<const Basic> &gen, bool ex = false);
// convert a `basic`, to a UPoly `P` (eg. UIntPoly, UExprPoly, UIntPolyFlint)
// after finding out the generator automatically. Throws, if number
// of generators found != 1, or poly construction not possible.
// `ex` is the optional parameter for expanding the given `basic` or not.
template <typename P>
RCP<const P> from_basic(const RCP<const Basic> &basic, bool ex = false);

template <typename T, typename P>
enable_if_t<std::is_same<T, UExprDict>::value, T>
_basic_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen);

template <typename T, typename P>
enable_if_t<std::is_base_of<UIntPolyBase<T, P>, P>::value, T>
_basic_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen);

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

    void dict_set(unsigned int pow, const Basic &x)
    {
        static_cast<V *>(this)->dict_set(pow, x);
    }

    void bvisit(const Pow &x)
    {
        if (is_a<const Integer>(*x.get_exp())) {
            int i = rcp_static_cast<const Integer>(x.get_exp())->as_int();
            if (i > 0) {
                dict
                    = pow_upoly(*P::from_container(gen, _basic_to_upoly<D, P>(
                                                            x.get_base(), gen)),
                                i)
                          ->get_poly();
                return;
            }
        }

        RCP<const Basic> genbase = gen, genpow = one, coef = one, tmp;
        if (is_a<const Pow>(*gen)) {
            genbase = static_cast<const Pow &>(*gen).get_base();
            genpow = static_cast<const Pow &>(*gen).get_exp();
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
            dict_set(powr, *coef);

        } else {
            dict_set(0, x);
        }
    }

    void bvisit(const Add &x)
    {
        x.coef_->accept(*this);
        for (auto const &it : x.dict_)
            dict += (_basic_to_upoly<D, P>(it.first, gen)
                     * _basic_to_upoly<D, P>(it.second, gen));
    }

    void bvisit(const Mul &x)
    {
        x.coef_->accept(*this);
        for (auto const &it : x.dict_)
            dict *= _basic_to_upoly<D, P>(pow(it.first, it.second), gen);
    }

    void bvisit(const Integer &x)
    {
        integer_class i = x.i;
        if (i != 0)
            dict = P::container_from_dict(gen, {{0, i}});
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
                        gen, {{i, typename P::coef_type(1)}});
                    return;
                }
            }
        }

        dict_set(0, x);
    }
};

template <typename Poly>
class BasicToUIntPoly : public BasicToUPolyBase<Poly, BasicToUIntPoly<Poly>>
{
public:
    using BasicToUPolyBase<Poly, BasicToUIntPoly>::bvisit;
    using BasicToUPolyBase<Poly, BasicToUIntPoly>::apply;

    void bvisit(const Rational &x)
    {
        throw std::runtime_error("Non-integer found");
    }

    void dict_set(unsigned int pow, const Basic &x)
    {
        if (is_a<const Integer>(x))
            this->dict = Poly::container_from_dict(
                this->gen, {{pow, static_cast<const Integer &>(x).i}});
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
        dict = UExprDict(x.rcp_from_this());
    }

    void dict_set(unsigned int pow, const Basic &x)
    {
        dict = UExprDict({{pow, x.rcp_from_this()}});
    }
};

template <typename T, typename P>
enable_if_t<std::is_same<T, UExprDict>::value, T>
_basic_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen)
{
    BasicToUExprPoly v;
    return v.apply(*basic, gen);
}

template <typename T, typename P>
enable_if_t<std::is_base_of<UIntPolyBase<T, P>, P>::value, T>
_basic_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen)
{
    BasicToUIntPoly<P> v;
    return v.apply(*basic, gen);
}

template <typename P>
RCP<const P> from_basic(const RCP<const Basic> &basic,
                        const RCP<const Basic> &gen, bool ex)
{
    RCP<const Basic> exp = basic;
    if (ex)
        exp = expand(basic);
    return P::from_container(
        gen, _basic_to_upoly<typename P::container_type, P>(exp, gen));
}

template <typename P>
RCP<const P> from_basic(const RCP<const Basic> &basic, bool ex)
{
    RCP<const Basic> exp = basic;
    if (ex)
        exp = expand(basic);

    umap_basic_num tmp = _find_gens_poly(exp);

    if (tmp.size() != 1)
        throw std::runtime_error("Did not find exactly 1 generator");

    RCP<const Basic> gen = pow(tmp.begin()->first, tmp.begin()->second);
    return P::from_container(
        gen, _basic_to_upoly<typename P::container_type, P>(exp, gen));
}
}

#endif
