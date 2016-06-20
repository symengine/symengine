#include <symengine/visitor.h>

namespace SymEngine
{

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

RCP<const UIntPoly> basic_to_uintpoly(const RCP<const Basic> &x)
{
    BasicToUIntPolyVisitor v;
    return v.apply(*x);
}

} // SymEngine
