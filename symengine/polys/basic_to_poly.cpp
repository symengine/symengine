#include <symengine/visitor.h>

namespace SymEngine
{
// all throughout Number refers to either a Rational or an Integer only
umap_basic_num _find_gens_poly_pow(const RCP<const Basic> &x, 
                                   const RCP<const Basic> &base);

class PolyGeneratorVisitor : public BaseVisitor<PolyGeneratorVisitor>
{
private:
    // the generators are pow(it.first, it.second)
    // those which are not Pow are stored as (x, one)
    // numbers must always be positive and of the form (1/d, d belongs to N)
    umap_basic_num gen_set;

public:
    umap_basic_num apply(const Basic &b)
    {
        b.accept(*this);
        return gen_set;
    }

    // adds curr to gen_set, or updates already existing gen
    void add_to_gen_set(const RCP<const Basic> &base, const RCP<const Number> &exp)
    {
        auto it = gen_set.find(base);
        if (it == gen_set.end()) {
            gen_set[base] = exp;
            return;
        }

        if (is_a<const Rational>(*exp) and is_a<const Rational>(*it->second)) {
            gen_set[base] = rcp_static_cast<const Rational>(div(one, 
                            lcm(*rcp_static_cast<const Rational>(exp)->get_den(),
                                *rcp_static_cast<const Rational>(it->second)->get_den())));
        } else if (is_a<const Rational>(*exp)) {
            gen_set[base] = rcp_static_cast<const Rational>(div(one, 
                            rcp_static_cast<const Rational>(exp)->get_den()));
        }
    }

    void bvisit(const Pow &x)
    {
        if (is_a<const Integer>(*x.get_exp())) {
            if (rcp_static_cast<const Integer>(x.get_exp())->is_positive()) {
                x.get_base()->accept(*this);
            } else {
                add_to_gen_set(pow(x.get_base(), minus_one), one);
            }

        } else if (is_a<const Rational>(*x.get_exp())) {

            RCP<const Basic> mulx = one;
            if (rcp_static_cast<const Rational>(x.get_exp())->is_negative())
                mulx = minus_one;
            RCP<const Rational> den = rcp_static_cast<const Rational>(div(one, rcp_static_cast<const Rational>(x.get_exp())->get_den()));
            add_to_gen_set(pow(x.get_base(), mulx), den);

        } else {
            umap_basic_num pow_pairs = _find_gens_poly_pow(x.get_exp(), x.get_base());
            for (auto it : pow_pairs)
                add_to_gen_set(pow(x.get_base(), it.first), it.second);
        }
    }

    void bvisit(const Add &x)
    {
        for (auto it : x.dict_)
            it.first->accept(*this);
    }

    void bvisit(const Mul &x)
    {
        for (auto it : x.dict_)
            it.first->accept(*this);
    }

    void bvisit(const Number &x)
    {
        // intentionally blank
    }

    void bvisit(const Basic &x)
    {
        add_to_gen_set(x.rcp_from_this(), one);
    }
};

class PolyGeneratorVisitorPow : public BaseVisitor<PolyGeneratorVisitorPow>
{
private:
    // the generators are mul(it.first, it.second) not Pow
    // the_base is the base of the Pow (whose exp we are currently dealing)
    // numbers must always be positive and of the form (1/d, d belongs to N)
    umap_basic_num gen_set;
    RCP<const Basic> the_base;

public:
    umap_basic_num apply(const Basic &b, const RCP<const Basic> &base)
    {
        the_base = base;
        b.accept(*this);
        return gen_set;
    }

    void bvisit(const Pow &x)
    {   
        umap_basic_num pow_pairs = _find_gens_poly_pow(x.get_exp(), x.get_base());
        for (auto it : pow_pairs)
            gen_set[pow(x.get_base(), it.first)] = it.second;
    }

    void bvisit(const Add &x)
    {   
        if (not x.coef_->is_zero())
            x.coef_->accept(*this);

        for (auto it : x.dict_) {
            RCP<const Basic> mulx = one, divx = one;

            if (it.second->is_negative())
                mulx = minus_one;

            if (is_a<const Rational>(*it.second))
                divx = rcp_static_cast<const Rational>(it.second)->get_den();

            gen_set[mul(mulx, it.first)] = rcp_static_cast<const Number>(div(one, divx));
        }
    }

    void bvisit(const Mul &x)
    {   
        // won't handle cases like 2**((x+1)(x+2))
        // needs `expand` to have been called
        RCP<const Number> mulx = one, divx = one;

        if (x.coef_->is_negative())
            mulx = minus_one;

        if (is_a<const Rational>(*x.coef_))
            divx = rcp_static_cast<const Rational>(x.coef_)->get_den();

        auto dict = x.dict_;
        gen_set[Mul::from_dict(mulx, std::move(dict))] = rcp_static_cast<const Number>(div(one, divx));
    }

    void bvisit(const Number &x)
    {
        if (not is_a_Number(*pow(the_base, x.rcp_from_this()))) {
            if (x.is_positive())
                gen_set[one] = x.rcp_from_this_cast<const Number>();
            else
                gen_set[minus_one] = rcp_static_cast<const Number>(neg(x.rcp_from_this()));
        }
    }

    void bvisit(const Basic &x)
    {
        gen_set[x.rcp_from_this()] = one;
    }
};

umap_basic_num _find_gens_poly(const RCP<const Basic> &x)
{
    PolyGeneratorVisitor v;
    return v.apply(*x);
}

umap_basic_num _find_gens_poly_pow(const RCP<const Basic> &x, 
                                   const RCP<const Basic> &base)
{
    PolyGeneratorVisitorPow v;
    return v.apply(*x, base);
}

class BasicToUPoly : public BaseVisitor<BasicToUPoly>
{
private:
    RCP<const Basic> gen;
    UIntDict dict;

public:
    UIntDict apply(const Basic &b, const RCP<const Basic> &gen_)
    {   
        gen = gen_;
        b.accept(*this);
        return dict;
    }

    void bvisit(const Pow &x) {

        // if positive integer power
        if (is_a<const Integer>(*x.get_exp())) {
            int i = rcp_static_cast<const Integer>(x.get_exp())->as_int();
            if (i > 0) {
                dict = UIntDict::pow(_basic_to_upoly(x.get_base(), gen), i);
                return;
            }
        }

        RCP<const Basic> base = gen, exp = one, coef = one, tmp;
        if(is_a<const Pow>(*gen)) {
            base = rcp_static_cast<const Pow>(gen)->get_base();
            exp = rcp_static_cast<const Pow>(gen)->get_exp();
        }

        // if not positive integer, the bases must match
        if (not eq(*base, *x.get_base()))
            throw std::runtime_error("Generator bases don't match");

        umap_basic_num add_dict, coef_dict;
        if (is_a<const Add>(*x.get_exp())) {
            RCP<const Add> adder = rcp_static_cast<const Add>(x.get_exp());
            add_dict = adder->dict_;
            if (not adder->coef_->is_zero())
                add_dict[adder->coef_] = one;
        } else {
            add_dict[x.get_exp()] = one;
        }

        unsigned int powr = 0;
        for (auto it : add_dict) {
            tmp = div(mul(it.first, it.second), exp);
            if (is_a<const Integer>(*tmp)) {
                RCP<const Integer> i = rcp_static_cast<const Integer>(tmp);
                if (i->is_positive()) {
                    powr = i->as_int();
                    continue;
                }
            }
            coef = mul(coef, pow(base, mul(it.first, it.second)));
        }
        if (is_a<const Integer>(*coef))
            dict = UIntDict({{powr, rcp_static_cast<const Integer>(coef)->i}});
        else
            throw std::runtime_error("Non-integer coeffienct found");

    }

    void bvisit(const Mul &x)
    {   
        if (not is_a<const Integer>(*x.coef_))
            throw std::runtime_error("Non-integer coeff found");

        dict = UIntDict(rcp_static_cast<const Integer>(x.coef_)->i);
        for (const auto &it : x.dict_)
            dict *= _basic_to_upoly(pow(it.first, it.second), gen);
    }

    void bvisit(const Add &x)
    {   
        x.coef_->accept(*this);

        integer_class multi;
        for (const auto &it : x.dict_) {
            if (not is_a<const Integer>(*it.second))
                throw std::runtime_error("Non-integer coeff found");

            multi = rcp_static_cast<const Integer>(it.second)->i;
            dict += UIntDict(multi) * _basic_to_upoly(it.first, gen);
        }
    }

    void bvisit(const Number &x)
    {
        if (not is_a<const Integer>(x))
            throw std::runtime_error("Non-integer coeff found");
        integer_class tmp = static_cast<const Integer&>(x).i;
        if (tmp != 0)
            dict = UIntDict(tmp);
    }

    void bvisit(const Basic &x)
    {   
        RCP<const Basic> divx = one;
        if (is_a<const Pow>(*gen))
            divx = rcp_static_cast<const Pow>(gen)->get_exp();
        RCP<const Basic> powx = div(one, divx);

        if (is_a<const Integer>(*powx)) {
            RCP<const Integer> tmp = rcp_static_cast<const Integer>(powx);            
            if (tmp->is_positive()) {
                dict = UIntDict({{tmp->as_int(), integer_class(1)}});
                return;
            }
        }
        throw std::runtime_error("Could not extract polynomial");
    }
};

UIntDict _basic_to_upoly(const RCP<const Basic> &basic,
                         const RCP<const Basic> &gen)
{
    BasicToUPoly v;
    return v.apply(*basic, gen);
}

} // SymEngine