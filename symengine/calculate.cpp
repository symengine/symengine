#include <symengine/calculate.h>

namespace SymEngine
{

extern RCP<const Basic> &i2;

RCP<const Basic> gamma_positive_int(const RCP<const Basic> &arg)
{
    SYMENGINE_ASSERT(is_a<Integer>(*arg))
    RCP<const Integer> arg_ = rcp_static_cast<const Integer>(arg);
    SYMENGINE_ASSERT(arg_->is_positive())
    return factorial((arg_->subint(*one))->as_int());
}

RCP<const Basic> gamma_multiple_2(const RCP<const Basic> &arg)
{
    SYMENGINE_ASSERT(is_a<Rational>(*arg))
    RCP<const Rational> arg_ = rcp_static_cast<const Rational>(arg);
    SYMENGINE_ASSERT(get_den(arg_->as_rational_class()) == 2)
    RCP<const Integer> n, k;
    RCP<const Number> coeff;
    n = quotient_f(*(integer(mp_abs(get_num(arg_->as_rational_class())))),
                   *(integer(get_den(arg_->as_rational_class()))));
    if (arg_->is_positive()) {
        k = n;
        coeff = one;
    } else {
        n = n->addint(*one);
        k = n;
        if ((n->as_int() & 1) == 0) {
            coeff = one;
        } else {
            coeff = minus_one;
        }
    }
    int j = 1;
    for (int i = 3; i < 2 * k->as_int(); i = i + 2) {
        j = j * i;
    }
    coeff = mulnum(coeff, integer(j));
    if (arg_->is_positive()) {
        return div(mul(coeff, sqrt(pi)), pow(i2, n));
    } else {
        return div(mul(pow(i2, n), sqrt(pi)), coeff);
    }
}

void CalculateVisitor::bvisit(const Gamma &x)
{
    auto farg = x.get_arg();
    auto arg = apply(farg);

    if (is_a<Integer>(*arg)) {
        RCP<const Integer> arg_ = rcp_static_cast<const Integer>(arg);
        if (arg_->is_positive()) {
            result_ = gamma_positive_int(arg);
        } else {
            result_ = ComplexInf;
        }
    } else if (is_a<Rational>(*arg)) {
        RCP<const Rational> arg_ = rcp_static_cast<const Rational>(arg);
        if ((get_den(arg_->as_rational_class())) == 2) {
            result_ = gamma_multiple_2(arg);
        } else {
            result_ = make_rcp<const Gamma>(arg);
        }
    } else if (is_a_Number(*arg)
               and not down_cast<const Number &>(*arg).is_exact()) {
        result_ = down_cast<const Number &>(*arg).get_eval().gamma(*arg);
    } else {
        result_ = gamma(arg);
    }
}

RCP<const Basic> calculate(const RCP<const Basic> &x)
{
    CalculateVisitor b;
    return b.apply(x);
}

} // namespace SymEngine
