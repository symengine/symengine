#include <symengine/visitor.h>

namespace SymEngine
{

RCP<const Basic> find_generator(const RCP<const Basic> &x);

class UIntPolyGenaratorVisitor : public BaseVisitor<UIntPolyGenaratorVisitor>
{
private:
    RCP<const Basic> res;

public:
    RCP<const Basic> apply(const Basic &b)
    {
        b.accept(*this);
        return res;
    }

    void bvisit(const Pow &x)
    {
        if (is_a_Number(*x.get_base())) {
                res = pow(x.get_base(), find_generator(x.get_exp()));
        } else if (is_a_Number(*x.get_exp())) {
            if (is_a<const Integer>(*x.get_exp())) {
                integer_class i = rcp_static_cast<const Integer>(x.get_exp())->i;
                if (i > 0) {
                    res = find_generator(x.get_base());
                    return;
                }
            }
            if (is_a<const Integer>(*x.get_exp()) or is_a<const Rational>(*x.get_exp()))
                res = x.rcp_from_this();
            else
                throw std::runtime_error("Could not extract generator");
        } else {
            // there are more cases not handled like genarator = x**x
            throw std::runtime_error("Could not extract generator");
        }
    }

    void bvisit(const Add &x)
    {
        RCP<const Basic> min_gen = null, curr_gen;
        std::string min_gen_type;

        for (const auto &it : x.dict_) {
            curr_gen = find_generator(it.first);

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
                            min_gen = pow(curr_gen, gcd(one, rcp_static_cast<const Number>(powx->get_exp())));
                        }
                    }
                    throw std::runtime_error("Could not extract generator");
                }else if (is_a<const Pow>(*curr_gen)) {

                    //
                } else {
                    throw std::runtime_error("Internal Error : Wrong generator type");
                }

            } else {
                SYMENGINE_ASSERT(is_a<const Symbol>(*curr_gen) or is_a<const Pow>(*curr_gen)
                     or is_a_sub<const Function>(*curr_gen))
                min_gen = curr_gen;
            }
        }

        if (x.dict_.empty())
            throw std::runtime_error("Could not extract generator");
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

RCP<const Basic> find_generator(const RCP<const Basic> &x)
{
    UIntPolyGenaratorVisitor v;
    return v.apply(*x);
}

RCP<const UIntPoly> basic_to_uintpoly(const RCP<const Basic> &x)
{
    BasicToUIntPolyVisitor v;
    return v.apply(*x);
}

} // SymEngine
