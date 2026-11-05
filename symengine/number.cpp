#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/real_double.h>
#include <symengine/real_mpfr.h>
#include <symengine/constants.h>
#include <symengine/infinity.h>
#include <symengine/nan.h>
#include <symengine/functions.h>
#include <symengine/eval.h>

namespace SymEngine
{

RCP<const Basic> Number::conjugate() const
{
    if (not is_complex()) {
        return this->rcp_from_this();
    }
    throw NotImplementedError("Not Implemented.");
}

RCP<const Number> Number::sub(const Number &other) const
{
    return add(*other.mul(*integer(-1)));
}

RCP<const Number> Number::rsub(const Number &other) const
{
    return mul(*integer(-1))->add(other);
}

RCP<const Number> Number::div(const Number &other) const
{
    return mul(*other.pow(*integer(-1)));
}

RCP<const Number> Number::rdiv(const Number &other) const
{
    return other.mul(*pow(*integer(-1)));
}

RCP<const Number> round(const Number &x)
{
    if (is_a<Infty>(x) or is_a<NaN>(x)) {
        return rcp_static_cast<const Number>(x.rcp_from_this());
    }
    if (is_a_sub<ComplexBase>(x)) {
        const ComplexBase &c = down_cast<const ComplexBase &>(x);
        return addnum(round(*c.real_part()),
                      mulnum(round(*c.imaginary_part()), I));
    }
    // Computed exactly from floor, with ties to even
    RCP<const Number> self = x.rcp_from_this_cast<const Number>();
    RCP<const Number> floored = rcp_static_cast<const Number>(floor(self));
    RCP<const Number> diff = subnum(subnum(self, floored), rational(1, 2));
    if (diff->is_positive()) {
        return addnum(floored, one);
    }
    if (diff->is_negative()) {
        return floored;
    }
    // A tie goes to the even neighbour
    const integer_class &i
        = down_cast<const Integer &>(*floored).as_integer_class();
    integer_class q, r;
    mp_fdiv_qr(q, r, i, integer_class(2));
    return addnum(floored, integer(std::move(r)));
}

RCP<const Number> round(const Number &x, long n)
{
    if (is_a<Infty>(x) or is_a<NaN>(x)) {
        return rcp_static_cast<const Number>(x.rcp_from_this());
    }
    if (is_a_sub<ComplexBase>(x)) {
        const ComplexBase &c = down_cast<const ComplexBase &>(x);
        return addnum(round(*c.real_part(), n),
                      mulnum(round(*c.imaginary_part(), n), I));
    }
    RCP<const Number> scale = pownum(integer(10), integer(n));
    RCP<const Number> result = divnum(
        round(*mulnum(x.rcp_from_this_cast<const Number>(), scale)), scale);
    if (is_a<RealDouble>(x)) {
        return rcp_static_cast<const Number>(
            evalf(*result, 53, EvalfDomain::Real));
    }
#ifdef HAVE_SYMENGINE_MPFR
    if (is_a<RealMPFR>(x)) {
        return rcp_static_cast<const Number>(
            evalf(*result, down_cast<const RealMPFR &>(x).i.get_prec(),
                  EvalfDomain::Real));
    }
#endif
    return result;
}

} // namespace SymEngine
