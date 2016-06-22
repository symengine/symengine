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

            } else {
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
