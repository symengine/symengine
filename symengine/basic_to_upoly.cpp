#include <symengine/visitor.h>

namespace SymEngine
{

class UIntPolyGenaratorVisitor : public BaseVisitor<UIntPolyGenaratorVisitor>
{
private:
    RCP<const Basic> res;
    bool inpow;

public:
    RCP<const Basic> apply(const Basic &b, bool inpow_)
    {
        inpow = inpow_;
        b.accept(*this);
        return res;
    }

    void _update_min(RCP<const Basic> &min_gen, RCP<const Basic> &curr_gen)
    {
        if(min_gen != null) {
            if ((is_a_sub<const Function>(*min_gen) and is_a_sub<const Function>(*curr_gen)) or
               (is_a<const Symbol>(*min_gen) and is_a<const Symbol>(*curr_gen))) {

                if (not eq(*curr_gen, *min_gen))
                    throw std::runtime_error("Could not extract generator");
            
            } else if (is_a_sub<const Function>(*curr_gen) or is_a<const Symbol>(*curr_gen)) {

                if (is_a<const Pow>(*min_gen)) {
                    RCP<const Pow> powx = rcp_static_cast<const Pow>(min_gen);

                    if (eq(*(powx->get_base()), *curr_gen)) {

                        SYMENGINE_ASSERT(is_a_Number(*powx->get_exp()))
                        if (is_a<const Rational>(*powx->get_exp()))
                            min_gen = pow(curr_gen, div(one, rcp_static_cast<const Rational>(powx->get_exp())->get_den()));
                        return;
                    }
                }
                throw std::runtime_error("Could not extract generator");

            } else if (is_a<const Pow>(*curr_gen)) {

                RCP<const Pow> powx = rcp_static_cast<const Pow>(curr_gen);
                if (is_a<const Symbol>(*min_gen) or is_a_sub<const Function>(*min_gen)) {
                    if (eq(*(powx->get_base()), *min_gen)) {

                        SYMENGINE_ASSERT(is_a_Number(*powx->get_exp()))
                        if (is_a<const Rational>(*powx->get_exp()))
                            min_gen = pow(min_gen, div(one, rcp_static_cast<const Rational>(powx->get_exp())->get_den()));
                        return;
                    }
                } else if (is_a<const Pow>(*min_gen)) {
                    // won't handle cases like 4**x + 2**x
                    RCP<const Pow> powx2 = rcp_static_cast<const Pow>(min_gen);
                    if (eq(*(powx->get_base()), *(powx2->get_base()))) {
                        // won't handle things like 2**(3x/2) + 2**x, but will handle 2**(x/2) + 2**x
                        RCP<const Basic> tmp = div(powx->get_exp(), powx2->get_exp());
                        if(is_a<const Integer>(*tmp)) {
                            integer_class i = rcp_static_cast<const Integer>(tmp)->i;
                            if (i > 0)
                                return;
                        }

                        tmp = div(powx2->get_exp(), powx->get_exp());
                        if(is_a<const Integer>(*tmp)) {
                            integer_class i = rcp_static_cast<const Integer>(tmp)->i;
                            if (i > 0) {
                                min_gen = curr_gen;
                                return;
                            }
                        }
                    }
                }

                throw std::runtime_error("Could not extract generator");
            } else {
                throw std::runtime_error("Internal Error : Wrong generator type");
            }

        } else {
            SYMENGINE_ASSERT(is_a<const Symbol>(*curr_gen) or is_a<const Pow>(*curr_gen)
                 or is_a_sub<const Function>(*curr_gen))
            min_gen = curr_gen;
        }
    }

    void bvisit(const Pow &x)
    {
        if (is_a<const Rational>(*x.get_base()) or is_a<const Integer>(*x.get_base())) {
            res = pow(x.get_base(), find_generator(x.get_exp(), true));
        } else if (is_a<const Rational>(*x.get_exp()) or is_a<const Integer>(*x.get_exp())) {
            if (is_a<const Integer>(*x.get_exp())) {
                // this case should not be encountered, if `expand` was called
                integer_class i = rcp_static_cast<const Integer>(x.get_exp())->i;
                if (i > 0) {
                    res = find_generator(x.get_base());
                    return;
                } else {
                    res = x.rcp_from_this();
                }
            }
            if (is_a<const Rational>(*x.get_exp())) {
                RCP<const Rational> rat = rcp_static_cast<const Rational>(x.get_exp());
                if (rat->is_negative())
                    res = pow(x.get_base(), div(neg(one), rat->get_den()));
                else
                    res = pow(x.get_base(), div(one, rat->get_den()));
            }
            else {
                throw std::runtime_error("Could not extract generator");
            }
        } else {
            // won't handle cases like genarator = x**x
            throw std::runtime_error("Could not extract generator");
        }
    }

    void bvisit(const Add &x)
    {
        RCP<const Basic> min_gen = null, curr_gen;

        if (not inpow)
            if (not is_a<const Integer>(*x.coef_))
                throw std::runtime_error("Non-integer coeff found");

        for (const auto &it : x.dict_) {
            if (not inpow)
                if (not is_a<const Integer>(*it.second))
                    throw std::runtime_error("Non-integer coeff found");

            curr_gen = find_generator(it.first);
            _update_min(min_gen, curr_gen);
        }

        if (x.dict_.empty())
            throw std::runtime_error("Could not extract generator");
        res = min_gen;
    }

    void bvisit(const Mul &x)
    {   
        RCP<const Basic> min_gen = null, curr_gen;

        if (not inpow)
            if (not is_a<const Integer>(*x.coef_))
                throw std::runtime_error("Non-integer coeff found");

        for (const auto &it : x.dict_) {
            curr_gen = find_generator(pow(it.first, it.second));
            _update_min(min_gen, curr_gen);
        }

        if (x.dict_.empty())
            throw std::runtime_error("Could not extract generator");
        if (is_a<const Rational>(*x.coef_))
            min_gen = mul(min_gen, div(one, rcp_static_cast<const Rational>(x.coef_)->get_den()));
        res = min_gen;
    }

    void bvisit(const Function &x)
    {
        res = x.rcp_from_this();
    }

    void bvisit(const Symbol &x)
    {
        res = x.rcp_from_this();
    }

    void bvisit(const Basic &x)
    {
        throw std::runtime_error("Could not extract generator");
    }
};


class BasicToUIntPolyVisitor : public BaseVisitor<BasicToUIntPolyVisitor>
{
private:
    RCP<const UIntPoly> res;

public:

    RCP<const UIntPoly> apply(const Basic &b)
    {
        b.accept(*this);
        return res;
    }

    void bvisit(const UIntPoly &x)
    {   
        auto dict = x.get_poly();
        res = UIntPoly::from_container(x.get_var(), std::move(dict));
    }

    void bvisit(const Basic &x)
    {
        throw std::runtime_error("Cannot convert to UIntPoly");
    }
};

//should be fed in after an `expand` call
RCP<const Basic> find_generator(const RCP<const Basic> &x, bool inpow_)
{
    UIntPolyGenaratorVisitor v;
    RCP<const Basic> tmp = expand(x);
    return v.apply(*tmp, inpow_);
}

RCP<const UIntPoly> basic_to_uintpoly(const RCP<const Basic> &x)
{
    BasicToUIntPolyVisitor v;
    return v.apply(*x);
}

} // SymEngine
