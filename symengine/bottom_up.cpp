#include <symengine/visitor.h>

namespace SymEngine
{

inline RCP<const Basic> identity(const RCP<const Basic> &arg)
{
    return arg;
}

template <typename Func>
class BottomUpVisitor : public BaseVisitor<BottomUpVisitor<Func>>
{
protected:
    RCP<const Basic> result_;
    Func f;

public:
    BottomUpVisitor(const Func &f_) : f(f_)
    {
    }

    RCP<const Basic> apply(const Basic &x)
    {
        return apply(x.rcp_from_this());
    }

    RCP<const Basic> apply(const RCP<const Basic> &x)
    {
        x->accept(*this);
        return result_;
    }

    void bvisit(const Basic &x)
    {
        result_ = f(x.rcp_from_this());
    }

    void bvisit(const Add &x)
    {
        vec_basic newargs;
        for (const auto &a : x.get_args()) {
            newargs.push_back(apply(a));
        }
        result_ = add(newargs);
    }

    void bvisit(const Mul &x)
    {
        vec_basic newargs;
        for (const auto &a : x.get_args()) {
            newargs.push_back(apply(a));
        }
        result_ = mul(newargs);
    }

    void bvisit(const Pow &x)
    {
        auto base_ = x.get_base(), exp_ = x.get_exp();
        auto newarg1 = apply(base_), newarg2 = apply(exp_);
        if (base_ != newarg1 or exp_ != newarg2)
            result_ = pow(newarg1, newarg2);
        else
            result_ = x.rcp_from_this();
    }

    void bvisit(const OneArgFunction &x)
    {
        auto farg = x.get_arg();
        auto newarg = apply(farg);
        if (eq(*newarg, *farg))
            result_ = x.rcp_from_this();
        else
            result_ = x.create(newarg);
    }

    template <class T>
    void bvisit(const TwoArgBasic<T> &x)
    {
        auto farg1 = x.get_arg1(), farg2 = x.get_arg2();
        auto newarg1 = apply(farg1), newarg2 = apply(farg2);
        if (farg1 != newarg1 or farg2 != newarg2)
            result_ = x.create(newarg1, newarg2);
        else
            result_ = x.rcp_from_this();
    }

    void bvisit(const MultiArgFunction &x)
    {
        auto fargs = x.get_args();
        vec_basic newargs;
        for (const auto &a : fargs) {
            newargs.push_back(apply(a));
        }
        auto nbarg = x.create(newargs);
        result_ = nbarg;
    }
};

class ExpandasExp
    : public BaseVisitor<ExpandasExp, BottomUpVisitor<decltype(&identity)>>
{
public:
    using BottomUpVisitor::bvisit;

    ExpandasExp()
        : BaseVisitor<ExpandasExp, BottomUpVisitor<decltype(&identity)>>(
              &identity)
    {
    }

    template <typename T,
              typename
              = enable_if_t<std::is_base_of<TrigFunction, T>::value
                            or std::is_base_of<HyperbolicFunction, T>::value>>
    void bvisit(const T &x)
    {
        auto farg = x.get_arg();
        auto newarg = apply(farg);
        if (eq(*newarg, *farg))
            result_ = x.expand_as_exp();
        else
            result_ = (x.create(newarg))->expand_as_exp();
    }
};

template <typename Func>
RCP<const Basic> bottom_up(const RCP<const Basic> &x, const Func &f)
{
    BottomUpVisitor<Func> b(f);
    return b.apply(x);
}

RCP<const Basic> expand_as_exp(const RCP<const Basic> &x)
{
    ExpandasExp b;
    return b.apply(x);
}
}
