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

    void bvisit(const Pow &x)
    {
        if (is_a<const Rational>(*x.get_base()) or is_a<const Integer>(*x.get_base())) {
            res = pow(x.get_base(), _find_gen_uintpoly(x.get_exp(), true));
        } else if (is_a<const Rational>(*x.get_exp()) or is_a<const Integer>(*x.get_exp())) {

            int tmp = positive_integer(x.get_exp());
            if (tmp != 0) {
                if (tmp > 0)
                    res = _find_gen_uintpoly(x.get_base());
                else
                    res = x.rcp_from_this();
                return;
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

            curr_gen = _find_gen_uintpoly(it.first);

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
                            continue;
                        }
                    }
                    throw std::runtime_error("Could not extract generator");

                } else if (is_a<const Pow>(*curr_gen)) {

                    RCP<const Pow> powx = rcp_static_cast<const Pow>(curr_gen);
                    if (is_a<const Symbol>(*min_gen) or is_a_sub<const Function>(*min_gen)) {
                        if (eq(*(powx->get_base()), *min_gen)) {

                            SYMENGINE_ASSERT(is_a_Number(*powx->get_exp()))
                            int i = positive_integer(powx->get_exp());
                            if (i < 0)
                                throw std::runtime_error("Could not extract generator");
                            else if (i == 0)
                                min_gen = pow(min_gen, div(one, rcp_static_cast<const Rational>(powx->get_exp())->get_den()));
                            // else was a positive integer, nothing to be done
                            continue;
                        }
                    } else if (is_a<const Pow>(*min_gen)) {
                        // won't handle cases like 4**x + 2**x
                        RCP<const Pow> powx2 = rcp_static_cast<const Pow>(min_gen);
                        if (eq(*(powx->get_base()), *(powx2->get_base()))) {
                            // won't handle things like 2**(3x/2) + 2**x, but will handle 2**(x/2) + 2**x
                            // TODO : can be modified to handle, will need some work 
                            // expand should not be needed here
                            RCP<const Basic> tmp = expand(div(powx->get_exp(), powx2->get_exp()));
                            int i = positive_integer(tmp);
                            if (i > 0)
                                continue;

                            tmp = expand(div(powx2->get_exp(), powx->get_exp()));
                            i = positive_integer(tmp);
                            if (i > 0) {
                                min_gen = curr_gen;
                                continue;
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

        res = min_gen;
    }

    void bvisit(const Mul &x)
    {
        RCP<const Basic> gen;

        if (not inpow)
            if (not is_a<const Integer>(*x.coef_))
                throw std::runtime_error("Non-integer coeff found");

        if (x.dict_.size() > 1)
            // this case should not be encountered, if `expand` was called
            // apart from cases having more than one viable generator
            throw std::runtime_error("Expand Error / Could not extract generator");

        gen = _find_gen_uintpoly(pow(x.dict_.begin()->first, x.dict_.begin()->second));

        if (is_a<const Rational>(*x.coef_))
            gen = mul(gen, div(one, rcp_static_cast<const Rational>(x.coef_)->get_den()));
        res = gen;
    }

    void bvisit(const UIntPoly &x)
    {
        res = x.get_var();
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
    map_uint_mpz res;
    RCP<const Basic> gen;

public:

    RCP<const UIntPoly> apply(const Basic &b, const Basic &gen_)
    {   
        gen = gen_.rcp_from_this();
        b.accept(*this);
        return UIntPoly::from_dict(gen, std::move(res));
    }

    void _update_dict_pow(const RCP<const Basic> &base, const RCP<const Basic> &exp,
                          integer_class multi = integer_class(1))
    {
        if(is_a_sub<const Function>(*gen) or is_a<const Symbol>(*gen)){
            if (eq(*base, *gen)) {
                int i = positive_integer(exp);
                if (i > 0) {
                    // this degree has not been encountered before
                    SYMENGINE_ASSERT(res.find(i) == res.end());
                    res[i] = multi;
                    return;
                }
            }
            throw std::runtime_error("Could not convert to UIntPoly");

        } else if (is_a<const Pow>(*gen)) {
            RCP<const Pow> powx = rcp_static_cast<const Pow>(gen);
            // won't handle cases like (4**x + 2**x, 2**x)
            if(eq(*base, *powx->get_base())) {
                // `expand` should not really be necessary here
                RCP<const Basic> tmp = expand(div(exp, powx->get_exp()));
                int i = positive_integer(tmp);
                if (i > 0) {
                    // this degree has not been encountered before
                    SYMENGINE_ASSERT(res.find(i) == res.end());
                    res[i] = multi;
                    return;
                }
            }
            throw std::runtime_error("Could not convert to UIntPoly");
        }
        // change this? Poly((x+y)**2 + 1, (x+y))
        throw std::runtime_error("Unsupported generator type");
    }

    void _update_dict_mul(const RCP<const Mul> &x, integer_class multi = integer_class(1))
    {
        if (not is_a<const Integer>(*x->coef_))
            throw std::runtime_error("Non-integer coeff found");

        if (x->dict_.size() > 1)
            // this case should not be encountered, if `expand` was called
            // apart from cases having more than one viable generator
            throw std::runtime_error("Expand Error / Could not extract generator");

        _update_dict_pow(x->dict_.begin()->first, x->dict_.begin()->second,
                         rcp_static_cast<const Integer>(x->coef_)->i * multi);
    }

    void bvisit(const Pow &x)
    {
        _update_dict_pow(x.get_base(), x.get_exp());
    }

    void bvisit(const Mul &x)
    {
        _update_dict_mul(rcp_static_cast<const Mul>(x.rcp_from_this()));
    }

    void bvisit(const Add &x)
    {
        if (not is_a<const Integer>(*x.coef_))
            throw std::runtime_error("Non-integer coeff found");
        else
            res[0] = rcp_static_cast<const Integer>(x.coef_)->i;

        integer_class multi;
        for (const auto &it : x.dict_) {
            if (not is_a<const Integer>(*it.second))
                throw std::runtime_error("Non-integer coeff found");
            multi = rcp_static_cast<const Integer>(it.second)->i;

            if (is_a_sub<const Function>(*it.first) or is_a<const Symbol>(*it.first)) {
                if (eq(*it.first, *gen)) {
                    SYMENGINE_ASSERT(res.find(1) == res.end())
                    res[1] = multi;
                    continue;
                }
                throw std::runtime_error("Could not extract generator");

            } else if (is_a<const Mul>(*it.first))  {
                _update_dict_mul(rcp_static_cast<const Mul>(it.first), multi);
            } else if (is_a<const Pow>(*it.first)) {
                RCP<const Pow> powx = rcp_static_cast<const Pow>(it.first);
                _update_dict_pow(powx->get_base(), powx->get_exp(), multi);
            } else {
                throw std::runtime_error("Unexpected type encountered");
            }
        }
    }

    void bvisit(const UIntPoly &x)
    {   
        if (not eq(*gen, *x.get_var()))
            throw std::runtime_error("Generator doesn't match");
        res = x.get_dict();
    }

    void bvisit(const Basic &x)
    {
        throw std::runtime_error("Cannot convert to UIntPoly");
    }
};

//should be fed in after an `expand` call
RCP<const Basic> _find_gen_uintpoly(const RCP<const Basic> &x, bool inpow_, bool exp)
{
    UIntPolyGenaratorVisitor v;
    if (exp)
    {
        RCP<const Basic> tmp = expand(x);
        return v.apply(*tmp, inpow_);
    }
    return v.apply(*x, inpow_);
}
//should be fed in after an `expand` call
RCP<const UIntPoly> _basic_to_uintpoly(const RCP<const Basic> &x, const RCP<const Basic> &gen)
{
    BasicToUIntPolyVisitor v;
    return v.apply(*x, *gen);
}

} // SymEngine
