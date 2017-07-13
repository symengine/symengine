#include <symengine/visitor.h>
#include <symengine/basic.h>

namespace SymEngine
{

class RealImagVisitor : public BaseVisitor<RealImagVisitor>
{
private:
    Ptr<RCP<const Basic>> real_, imag_;

public:
    RealImagVisitor(const Ptr<RCP<const Basic>> &real,
                    const Ptr<RCP<const Basic>> &imag)
        : real_{real}, imag_{imag}
    {
    }

    void apply(const Basic &b)
    {
        b.accept(*this);
    }

    void bvisit(const Mul &x)
    {
        RCP<const Basic> rest = one;
        RCP<const Basic> re_, im_;
        RCP<const Basic> coef = one;
        bool is_imag = false;

        for (const auto &arg : x.get_args()) {
            as_real_imag(arg, outArg(re_), outArg(im_));
            if (eq(*im_, *zero)) {
                coef = mul(coef, re_);
            } else if (eq(*re_, *zero)) {
                is_imag = !is_imag;
                coef = mul(coef, im_);
            } else {
                rest = mul(rest, arg);
            }
        }

        if (eq(*rest, *one)) {
            if (is_imag) {
                *real_ = zero;
                *imag_ = coef;
            } else {
                *real_ = coef;
                *imag_ = zero;
            }
        } else {
            if (eq(*rest, x)) {
                throw SymEngineException(
                    "Not Implemented classes for real and imaginary parts");
            }
            as_real_imag(rest, outArg(re_), outArg(im_));
            if (is_imag) {
                *real_ = neg(mul(coef, im_));
                *imag_ = mul(coef, re_);
            } else {
                *real_ = mul(coef, re_);
                *imag_ = mul(coef, im_);
            }
        }
    }

    void bvisit(const Add &x)
    {
        RCP<const Basic> re = zero, re_, im = zero, im_;

        for (const auto &arg : x.get_args()) {
            as_real_imag(arg, outArg(re_), outArg(im_));
            re = add(re, re_);
            im = add(im, im_);
        }

        *real_ = re;
        *imag_ = im;
    }

    void bvisit(const Pow &x)
    {
        RCP<const Basic> base_, exp_, re, im;
        base_ = x.get_base();
        exp_ = x.get_exp();
        as_real_imag(base_, outArg(re), outArg(im));

        if (is_a<Integer>(*exp_)) {
            if (eq(*im, *zero)) {
                *real_ = x.rcp_from_this();
                *imag_ = zero;
                return;
            }
            auto expx = expand(x.rcp_from_this());
            as_real_imag(expx, real_, imag_);
        } else if (is_a<Rational>(*exp_)) {
            auto magn = sqrt(add(mul(re, re), mul(im, im)));
            auto ang = atan2(im, re);
            magn = pow(magn, exp_);
            ang = mul(ang, exp_);
            *real_ = mul(magn, cos(ang));
            *imag_ = mul(magn, sin(ang));
        } else {
            throw SymEngineException("Not Implemented");
        }
    }

    void bvisit(const Complex &x)
    {
        *real_ = x.real_part();
        *imag_ = x.imaginary_part();
    }

    void bvisit(const Symbol &x)
    {
        throw SymEngineException("Not Implemented");
    }

    void bvisit(const Basic &x)
    {
        *real_ = x.rcp_from_this();
        *imag_ = zero;
    }
};

void as_real_imag(const RCP<const Basic> &x, const Ptr<RCP<const Basic>> &real,
                  const Ptr<RCP<const Basic>> &imag)
{
    RealImagVisitor v(real, imag);
    v.apply(*x);
}

} // SymEngine
