#include <symengine/visitor.h>

namespace SymEngine
{

class ApartVisitor : public BaseVisitor<ApartVisitor>
{
private:
    bool full;

public:
    ApartVisitor(bool full_ = true) : full(full_)
    {
    }
    RCP<const Basic> apply(const Basic &b)
    {
        b.accept(*this);
    }
}

// Apart `self`
RCP<const Basic> apart(const RCP<const Basic> &self, bool full)
{
    ApartVisitor v(full);
    return v.apply(*self);
}
}
