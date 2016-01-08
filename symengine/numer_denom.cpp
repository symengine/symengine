#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>

namespace SymEngine {

class NumerDenomVisitor : public BaseVisitor<NumerDenomVisitor> {
private:
    Ptr<RCP<const Basic>> numer_, denom_;
public:
    NumerDenomVisitor(const Ptr<RCP<const Basic>> &numer, const Ptr<RCP<const Basic>> &denom)
        : numer_{numer}, denom_{denom} { }

    void apply(const Basic &b) {
        b.accept(*this);
    }

    void bvisit(const Mul &x) {

        RCP<const Basic> curr_num = one;
        RCP<const Basic> curr_den = one;
        RCP<const Basic> arg_num, arg_den;

        for (const auto &arg: x.get_args()) {
            as_numer_denom(arg, outArg(arg_num), outArg(arg_den));
            curr_num = mul(curr_num, arg_num);
            curr_den = mul(curr_den, arg_den);
        }

        *numer_ = curr_num;
        *denom_ = curr_den;
    }

    void bvisit(const Add &x) {

        RCP<const Basic> curr_num = zero;
        RCP<const Basic> curr_den = one;
        RCP<const Basic> arg_num, arg_den, den_mul, divx;
        RCP<const Basic> divx_num, divx_den;

        for (const auto &arg: x.get_args()) {
            as_numer_denom(arg, outArg(arg_num), outArg(arg_den));

            divx = div(arg_den, curr_den);
            as_numer_denom(divx, outArg(divx_num), outArg(divx_den));
            if (eq(*divx_den, *one)) {
                // the curr_den completely divides the arg_den
                curr_den = arg_den;
                curr_num = add(mul(curr_num, divx), arg_num);
                continue;
            }

            divx = div(curr_den, arg_den);
            as_numer_denom(divx, outArg(divx_num), outArg(divx_den));
            // the below two lines, cover the general case, as well as the case
            // where arg_den completely divides curr_den
            curr_den = mul(curr_den, divx_den);
            curr_num = add(mul(curr_num, divx_den), mul(arg_num, divx_num));
        }

        *numer_ = curr_num;
        *denom_ = curr_den;
    }

    void bvisit(const Pow &x) {

        RCP<const Basic> base_, exp_, num, den;
        base_ = x.get_base();
        exp_ = x.get_exp();
        as_numer_denom(base_, outArg(num), outArg(den));

        // if the exp is a negative numer, or is intuitively 'negative'
        if ((is_a_Number(*exp_) and rcp_static_cast<const Number>(exp_)->is_negative()) or (could_extract_minus(exp_))) {
            exp_ = mul(integer(-1), exp_);
            *numer_ = pow(den, exp_);
            *denom_ = pow(num, exp_);
        } else {
            *numer_ = pow(num, exp_);
            *denom_ = pow(den, exp_);
        }
    }

    void bvisit(const Complex &x) {

        RCP<const Integer> den, den1, den2;
        RCP<const Integer> num1, num2;

        num1 = integer(x.real_.get_num());
        num2 = integer(x.imaginary_.get_num());

        den1 = integer(x.real_.get_den());
        den2 = integer(x.imaginary_.get_den());
        den = lcm(*den1, *den2);

        num1 = rcp_static_cast<const Integer>(mul(num1, div(den, den1)));
        num2 = rcp_static_cast<const Integer>(mul(num2, div(den, den2)));

        *numer_ = Complex::from_two_nums(*num1, *num2);
        *denom_ = den;
    }

    void bvisit(const Rational &x) {
        *numer_ = integer(x.i.get_num());
        *denom_ = integer(x.i.get_den());
    }

    void bvisit(const Basic &x) {
        *numer_ = x.rcp_from_this();
        *denom_ = one;
    }
};

void as_numer_denom(const RCP<const Basic> &x, const Ptr<RCP<const Basic>> &numer, const Ptr<RCP<const Basic>> &denom)
{
    NumerDenomVisitor v(numer, denom);
    v.apply(*x);
}

} // SymEngine
