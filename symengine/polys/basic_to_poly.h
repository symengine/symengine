#include <symengine/visitor.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/polys/uexprpoly.h>

namespace SymEngine 
{

template<typename T>
enable_if_t<std::is_same<T, UExprDict>::value, UExprDict> 
_b_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen);

template<typename T>
enable_if_t<std::is_same<T, UIntDict>::value, UIntDict> 
_b_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen);

template<typename D, typename V>
class BasicToUPolyBase: public BaseVisitor<V>
{
public:
    RCP<const Basic> gen;
    D dict;

    D apply(const Basic &b, const RCP<const Basic> &gen_)
    {   
        gen = gen_;
        b.accept(*this);
        return dict;
    }

    // virtual void dict_add_term(unsigned int pow, const RCP<const Basic> &x) = 0;

    void bvisit(const Add &x)
    {
        dict = _b_to_upoly<D>(x.coef_, gen);
        for (auto const &it : x.dict_)
            dict += (_b_to_upoly<D>(it.first, gen) * _b_to_upoly<D>(it.second, gen));
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
            dict = D(typename D::value_type(i));
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
                    dict = D({{i, typename D::value_type(1)}});
                    return;
                }
            }
        }

        // dict_add_term(0, x);
    }
};

class BasicToUIntPoly: public BasicToUPolyBase<UIntDict, BasicToUIntPoly>
{
public:
    using BasicToUPolyBase<UIntDict, BasicToUIntPoly>::bvisit;
    using BasicToUPolyBase<UIntDict, BasicToUIntPoly>::apply;

    void bvisit(const Rational &x)
    {
        throw std::runtime_error("Non-integer found");
    }

    // void dict_add_term(unsigned int pow, )
};

class BasicToUExprPoly: public BasicToUPolyBase<UExprDict, BasicToUExprPoly>
{   
public:
    using BasicToUPolyBase<UExprDict, BasicToUExprPoly>::bvisit;
    using BasicToUPolyBase<UExprDict, BasicToUExprPoly>::apply;

    void bvisit(const Rational &x)
    {
        if (not x.is_zero())
            dict = UExprDict(x.rcp_from_this());
    }
};

template<typename T>
enable_if_t<std::is_same<T, UExprDict>::value, UExprDict> 
_b_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen)
{
    BasicToUExprPoly v;
    return v.apply(*basic, gen);
}

template<typename T>
enable_if_t<std::is_same<T, UIntDict>::value, UIntDict> 
_b_to_upoly(const RCP<const Basic> &basic, const RCP<const Basic> &gen)
{
    BasicToUIntPoly v;
    return v.apply(*basic, gen);
}

}