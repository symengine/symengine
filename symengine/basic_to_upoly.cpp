#include <symengine/visitor.h>

namespace SymEngine
{

// returns the integer if `x` is a positive integer else
// returns `-1` if negative integer, `0` if not an integer
inline int positive_integer(const RCP<const Basic> &x)
{
    if (is_a<const Integer>(*x)) {
        int tmp = rcp_static_cast<const Integer>(x)->as_int();
        if (tmp > 0)
            return tmp;
        else
            return -1;
    }
    return 0;
}

class UIntPolyGeneratorVisitor : public BaseVisitor<UIntPolyGeneratorVisitor>
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
        if (is_a<const Rational>(*x.get_base())
            or is_a<const Integer>(*x.get_base())) {
            res = pow(x.get_base(), _find_gen_uintpoly(x.get_exp(), true));
        } else if (is_a<const Rational>(*x.get_exp())
                   or is_a<const Integer>(*x.get_exp())) {

            int tmp = positive_integer(x.get_exp());
            if (tmp != 0) {
                if (tmp > 0)
                    res = _find_gen_uintpoly(x.get_base());
                else
                    res = div(one, x.get_base());
                return;
            }

            if (is_a<const Rational>(*x.get_exp())) {
                RCP<const Rational> rat
                    = rcp_static_cast<const Rational>(x.get_exp());
                if (rat->is_negative())
                    res = pow(x.get_base(), div(neg(one), rat->get_den()));
                else
                    res = pow(x.get_base(), div(one, rat->get_den()));
            } else {
                throw std::runtime_error("Could not extract generator");
            }
        } else {
            // won't handle cases like genarator = x**x
            throw std::runtime_error("Could not extract generator");
        }
    }

    void _update_min_gen(RCP<const Basic> &min_gen, RCP<const Basic> &curr_gen, bool &min_gen_set)
    {
        if (min_gen_set) {
            if ((is_a_sub<const Function>(*min_gen)
                 and is_a_sub<const Function>(*curr_gen))
                or (is_a<const Symbol>(*min_gen)
                    and is_a<const Symbol>(*curr_gen))) {

                if (not eq(*curr_gen, *min_gen))
                    throw std::runtime_error("Could not extract generator");

            } else if (is_a_sub<const Function>(*curr_gen)
                       or is_a<const Symbol>(*curr_gen)) {

                if (is_a<const Pow>(*min_gen)) {
                    RCP<const Pow> powx
                        = rcp_static_cast<const Pow>(min_gen);

                    if (eq(*(powx->get_base()), *curr_gen)) {

                        SYMENGINE_ASSERT(is_a<const Rational>(*powx->get_exp()) or
                                         is_a<const Integer>(*powx->get_exp()))
                        int i = positive_integer(powx->get_exp());
                        if (i < 0)
                            throw std::runtime_error("Could not extract generator");
                        else if (i == 0)
                            min_gen = pow(
                                curr_gen,
                                div(one, rcp_static_cast<const Rational>(
                                             powx->get_exp())->get_den()));
                        return;
                    }
                }
                throw std::runtime_error("Could not extract generator");

            } else if (is_a<const Pow>(*curr_gen)) {

                RCP<const Pow> powx = rcp_static_cast<const Pow>(curr_gen);
                if (is_a<const Symbol>(*min_gen)
                    or is_a_sub<const Function>(*min_gen)) {
                    if (eq(*(powx->get_base()), *min_gen)) {

                        SYMENGINE_ASSERT(is_a_Number(*powx->get_exp()))
                        int i = positive_integer(powx->get_exp());
                        if (i < 0)
                            throw std::runtime_error(
                                "Could not extract generator");
                        else if (i == 0)
                            min_gen = pow(
                                min_gen,
                                div(one, rcp_static_cast<const Rational>(
                                             powx->get_exp())->get_den()));
                        // else was a positive integer, nothing to be done
                        return;
                    }
                } else if (is_a<const Pow>(*min_gen)) {
                    // won't handle cases like 4**x + 2**x
                    RCP<const Pow> powx2
                        = rcp_static_cast<const Pow>(min_gen);
                    if (eq(*(powx->get_base()), *(powx2->get_base()))) {
                        // won't handle things like 2**(3x/2) + 2**x, but
                        // will handle 2**(x/2) + 2**x
                        // TODO : can be modified to handle, will 
                        // need some work
                        // expand should not be needed here
                        RCP<const Basic> tmp = expand(
                            div(powx->get_exp(), powx2->get_exp()));
                        int i = positive_integer(tmp);
                        if (i > 0)
                            return;

                        tmp = expand(
                            div(powx2->get_exp(), powx->get_exp()));
                        i = positive_integer(tmp);
                        if (i > 0) {
                            min_gen = curr_gen;
                            return;
                        }
                    }
                }

                throw std::runtime_error("Could not extract generator");
            } else {
                throw std::runtime_error(
                    "Internal Error : Wrong generator type");
            }

        } else {
            SYMENGINE_ASSERT(is_a<const Symbol>(*curr_gen)
                             or is_a<const Pow>(*curr_gen)
                             or is_a_sub<const Function>(*curr_gen))
            min_gen = curr_gen;
            min_gen_set = true;
        }
    }

    void bvisit(const Add &x)
    {
        RCP<const Basic> min_gen, curr_gen;
        bool min_gen_set = false;

        if (not inpow)
            if (not is_a<const Integer>(*x.coef_))
                throw std::runtime_error("Non-integer coeff found");

        for (const auto &it : x.dict_) {
            if (not inpow)
                if (not is_a<const Integer>(*it.second))
                    throw std::runtime_error("Non-integer coeff found");

            curr_gen = _find_gen_uintpoly(it.first);
            _update_min_gen(min_gen, curr_gen, min_gen_set);
        }

        res = min_gen;
    }

    void bvisit(const Mul &x)
    {
        RCP<const Basic> min_gen, curr_gen;
        bool min_gen_set = false;

        if (not inpow)
            if (not is_a<const Integer>(*x.coef_))
                throw std::runtime_error("Non-integer coeff found");

        for (const auto &it : x.dict_) {
            curr_gen = _find_gen_uintpoly(pow(it.first, it.second));
            _update_min_gen(min_gen, curr_gen, min_gen_set);
        }

        if (is_a<const Rational>(*x.coef_))
            min_gen = mul(min_gen, div(one, rcp_static_cast<const Rational>(x.coef_)
                                        ->get_den()));
        res = min_gen;
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

    void bvisit(const Pow &x)
    {
        int i = positive_integer(x.get_exp());
        if (i > 0) {
            RCP<const UIntPoly> poly = _basic_to_uintpoly(x.get_base(), gen);
            res = (pow_upoly(*poly, i))->get_dict();
            return;
        } else {
            
            if (is_a<const Pow>(*gen)) {
                RCP<const Pow> powx = rcp_static_cast<const Pow>(gen);
                // won't handle cases like (4**x + 2**x, 2**x)
                if (eq(*x.get_base(), *powx->get_base())) {
                    // `expand` should not really be necessary here
                    RCP<const Basic> tmp = expand(div(x.get_exp(), powx->get_exp()));
                    int i = positive_integer(tmp);
                    if (i > 0) {
                        res[i] = integer_class(1);
                        return;
                    }
                }
            }
        }

        throw std::runtime_error("Could not convert to UIntPoly");
    }

    void bvisit(const Mul &x)
    {
        if (not is_a<const Integer>(*x.coef_))
            throw std::runtime_error("Non-integer coeff found");
        res[0] = rcp_static_cast<const Integer>(x.coef_)->i;

        UIntDict dict(res);
        for (const auto &it : x.dict_) {
            dict *= _basic_to_uintpoly(pow(it.first, it.second), gen)->get_poly();
        }
        res = dict.get_dict();
    }

    void bvisit(const Add &x)
    {
        if (not is_a<const Integer>(*x.coef_)) {
            throw std::runtime_error("Non-integer coeff found");
        } else {
            integer_class tmp = rcp_static_cast<const Integer>(x.coef_)->i;
            if (tmp != 0)
                res[0] = tmp;
        }

        UIntDict dict(res);
        integer_class multi;
        for (const auto &it : x.dict_) {
            if (not is_a<const Integer>(*it.second))
                throw std::runtime_error("Non-integer coeff found");

            multi = rcp_static_cast<const Integer>(it.second)->i;
            dict += (UIntDict(multi) * _basic_to_uintpoly(it.first, gen)->get_poly());
        }
        res = dict.get_dict();
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

RCP<const Basic> _find_gen_uintpoly(const RCP<const Basic> &x, bool inpow_)
{
    UIntPolyGeneratorVisitor v;
    return v.apply(*x, inpow_);
}

// should be fed in after an `expand` call
RCP<const UIntPoly> _basic_to_uintpoly(const RCP<const Basic> &x,
                                       const RCP<const Basic> &gen)
{
    BasicToUIntPolyVisitor v;
    return v.apply(*x, *gen);
}

} // SymEngine
