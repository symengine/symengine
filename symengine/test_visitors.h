#ifndef SYMENGINE_TEST_VISITORS_H
#define SYMENGINE_TEST_VISITORS_H

#include <symengine/visitor.h>

namespace SymEngine
{

class ZeroVisitor : public BaseVisitor<ZeroVisitor>
{
private:
    tribool is_zero_;

public:
    void bvisit(const Basic &x)
    {
        is_zero_ = tribool::indeterminate;
    };
    void bvisit(const Symbol &x)
    {
        is_zero_ = tribool::indeterminate;
    };
    void bvisit(const Number &x);
    void bvisit(const Set &x)
    {
        is_zero_ = tribool::trifalse;
    };
    void bvisit(const Relational &x)
    {
        is_zero_ = tribool::trifalse;
    };
    void bvisit(const Boolean &x)
    {
        is_zero_ = tribool::trifalse;
    };
    void bvisit(const Constant &x)
    {
        is_zero_ = tribool::trifalse;
    };

    tribool apply(const Basic &b);
};
}

#endif // SYMENGINE_TEST_VISITORS_H
