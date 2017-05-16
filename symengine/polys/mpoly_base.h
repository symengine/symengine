#ifndef SYMENGINE_MULTIVARIATE
#define SYMENGINE_MULTIVARIATE

#include <symengine/basic.h>

namespace SymEngine
{

template <typename Container, typename Poly>
class MPolyBase : public Basic
{
private:
    Container poly_;
    set_basic vars_;

public:
    typedef Container container_type;
    typedef typename Container::coef_type coef_type;

    MPolyBase(const set_basic &vars, Container &&dict)
        : poly_{dict}, vars_{vars}
    {
    }

    static RCP<const Poly> from_container(const set_basic &vars, Container &&d)
    {
        return make_rcp<const Poly>(vars, std::move(d));
    }

    //! \returns `-1`,`0` or `1` after comparing
    virtual int compare(const Basic &o) const = 0;
    virtual hash_t __hash__() const = 0;

    inline vec_basic get_args() const
    {
        return {};
    }

    inline const Container &get_poly() const
    {
        return poly_;
    }

    inline const set_basic &get_vars() const
    {
        return vars_;
    }
};
}

#endif