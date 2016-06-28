#include <symengine/visitor.h>

namespace SymEngine
{
// all throughout Number refers to either a Rational or an Integer only
umap_basic_num _find_gens_poly(const RCP<const Basic> &x);
umap_basic_num _find_gens_poly_pow(const RCP<const Basic> &x, 
                                   const RCP<const Basic> &base);

class PolyGeneratorVisitor : public BaseVisitor<PolyGeneratorVisitor>
{
private:
    // the generators are pow(it.first, it.second)
    // those which are not Pow are stored as (x, one)
    // numbers must always be positive
    umap_basic_num gen_set;

public:
    umap_basic_num apply(const Basic &b)
    {
        b.accept(*this);
        return gen_set;
    }

    // adds curr to gen_set, or updates already existing gen
    void update_gens(const RCP<const Basic> &base, const RCP<const Number> &exp)
    {
        RCP<const Basic> map_exp;

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
            if (rcp_static_cast<const Integer>(x.get_exp())->i > 0) {
                gen_set = _find_gens_poly(x.get_base());
            } else {
                gen_set[pow(x.get_base(), minus_one)] = one;
            }
        } else {
            umap_basic_num pow_pairs = _find_gens_poly_pow(x.get_exp(), x.get_base());
            for (auto it : pow_pairs)
                gen_set[pow(x.get_base(), it.first)] = it.second;
        }
    }

    void bvisit(const Add &x)
    {
        for (auto it : x.dict_) {
            umap_basic_num curr_set = _find_gens_poly(it.first);
            for (auto init : curr_set)
                update_gens(init.first, init.second);
        }
    }

    void bvisit(const Mul &x)
    {
        for (auto it : x.dict_) {
            umap_basic_num curr_set = _find_gens_poly(it.first);
            for (auto init : curr_set)
                update_gens(init.first, init.second);
        }
    }

    void bvisit(const Number &x)
    {
        // intentionally blank
    }

    void bvisit(const Basic &x)
    {
        gen_set[x.rcp_from_this()] = one;
    }
};

class PolyGeneratorVisitorPow : public BaseVisitor<PolyGeneratorVisitorPow>
{
private:
    // the generators are mul(it.first, it.second) not Pow
    // the_base is the base of the Pow (whose exp we are currently dealing)
    umap_basic_num gen_set;
    RCP<const Basic> the_base;

public:
    umap_basic_num apply(const Basic &b, const RCP<const Basic> &base)
    {
        the_base = base;
        b.accept(*this);
        return gen_set;
    }

    void bvisit(const Number &x)
    {
        if (not is_a_Number(*pow(the_base, x.rcp_from_this()))) {
            if (x.is_positive())
                gen_set[one] = x.rcp_from_this_cast<const Number>();
            else
                gen_set[minus_one] = x.rcp_from_this_cast<const Number>();
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


} // SymEngine