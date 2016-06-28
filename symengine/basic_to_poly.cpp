#include <symengine/visitor.h>

namespace SymEngine
{

class PolyGeneratorVisitor : public BaseVisitor<PolyGeneratorVisitor>
{
private:
    // the generators are pow(it.first, it.second)
    // those which are not Pow are stored as (x, one)
    map_basic_basic gen_set;

public:
    map_basic_basic apply(const Basic &b)
    {
        b.accept(*this);
        return gen_set;
    }

    void bvisit(const Pow &x)
    {
    }

    void bvisit(const Basic &x)
    {
    }
};

class PolyGeneratorVisitorPow : public BaseVisitor<PolyGeneratorVisitorPow>
{
private:
    // the generators are pow(it.first, it.second)
    // those which are not Pow are stored as (x, one)
    // the_base is the base of the Pow (whose exp we are currently dealing)
    map_basic_basic gen_set;
    RCP<const Basic> the_base;

public:
    map_basic_basic apply(const Basic &b, const RCP<const Basic> &base)
    {
        the_base = base;
        b.accept(*this);
        return gen_set;
    }

    void bvisit(const Pow &x)
    {
    }

    void bvisit(const Basic &x)
    {
    }
};


map_basic_basic _find_gens_poly(const RCP<const Basic> &x)
{
    PolyGeneratorVisitor v;
    return v.apply(*x);
}

map_basic_basic _find_gens_poly_pow(const RCP<const Basic> &x, const RCP<const Basic> &base)
{
    PolyGeneratorVisitorPow v;
    return v.apply(*x, base);
}


} // SymEngine