#include <symengine/solve.h>
#include <symengine/polys/basic_conversions.h>
#include <symengine/polys/uintpoly_flint.h>
#include <symengine/logic.h>
#include <symengine/mul.h>
namespace SymEngine
{

RCP<const Set> solve_poly_linear(const RCP<const Basic> &f,
                                 const RCP<const Symbol> &sym,
                                 const RCP<const Set> &domain)
{
    auto upf = from_basic<UIntPoly>(f, sym);
    if (upf->get_poly().degree() != 1) {
        throw SymEngineException("Expected a polynomial of degree 1. Try with "
                                 "solve() or solve_poly()");
    }
    auto root = div(integer(-upf->get_poly().get_coeff(0)),
                    integer(upf->get_poly().get_coeff(1)));
    return set_intersection({domain, finiteset({root})});
}

RCP<const Set> solve_poly_quadratic(const RCP<const Basic> &f,
                                    const RCP<const Symbol> &sym,
                                    const RCP<const Set> &domain)
{
    auto upf = from_basic<UIntPoly>(f, sym);
    if (upf->get_poly().degree() != 2) {
        throw SymEngineException("Expected a polynomial of degree 2. Try with "
                                 "solve() or solve_poly()");
    }

    auto a = integer(upf->get_poly().get_coeff(2)),
         b = integer(upf->get_poly().get_coeff(1)),
         c = integer(upf->get_poly().get_coeff(0));
    RCP<const Basic> root1, root2;
    if (eq(*c, *zero)) {
        root1 = div(neg(b), a);
        root2 = zero;
    } else if (eq(*b, *zero)) {
        root1 = sqrt(div(neg(c), a));
        root2 = neg(root1);
    } else {
        auto discriminant = sub(mul(b, b), mul(mul(integer(4), a), c));
        auto lterm = div(neg(b), mul(integer(2), a));
        auto rterm = div(sqrt(discriminant), mul(integer(2), a));
        root1 = add(lterm, rterm);
        root2 = sub(lterm, rterm);
    }
    return set_intersection({domain, finiteset({root1, root2})});
}

RCP<const Set> solve_poly_cubic(const RCP<const Basic> &f,
                                const RCP<const Symbol> &sym,
                                const RCP<const Set> &domain)
{
    auto upf = from_basic<UIntPoly>(f, sym);
    if (upf->get_poly().degree() != 3) {
        throw SymEngineException("Expected a polynomial of degree 3. Try with "
                                 "solve() or solve_poly()");
    }

    auto a = integer(upf->get_poly().get_coeff(3)),
         b = integer(upf->get_poly().get_coeff(2)),
         c = integer(upf->get_poly().get_coeff(1)),
         d = integer(upf->get_poly().get_coeff(0));

    // ref :
    // https://en.wikipedia.org/wiki/Cubic_function#General_solution_to_the_cubic_equation_with_real_coefficients
    auto i2 = integer(2), i3 = integer(3), i4 = integer(4), i9 = integer(9),
         i27 = integer(27);
    auto delta0 = sub(mul(b, b), mul(mul(i3, a), c));
    auto delta1
        = add(sub(mul(mul(b, b), mul(i2, b)), mul(mul(i9, a), mul(b, c))),
              mul(mul(i27, d), mul(a, a)));
    auto delta = div(sub(mul(i4, pow(delta0, i3)), pow(delta1, i2)),
                     mul(i27, pow(a, i2)));
    RCP<const Basic> root1, root2, root3;
    if (eq(*delta, *zero)) {
        if (eq(*delta0, *zero)) {
            root1 = root2 = root3 = div(neg(b), mul(i3, a));
        } else {
            root1 = root2
                = div(sub(mul(i9, mul(a, d)), mul(b, c)), mul(i2, delta0));
            root3 = div(sub(mul(mul(i4, a), mul(b, c)),
                            add(mul(mul(pow(a, i2), d), i9), pow(b, i3))),
                        mul(delta0, a));
        }
    } else {
        auto C = pow(
            div(add(delta1, sqrt(mul(mul(neg(i27), delta), mul(a, a)))), i2),
            div(one, i3));
        root1 = neg(div(add(b, add(C, div(delta0, C))), mul(i3, a)));
        auto coef = div(mul(I, sqrt(i3)), i2);
        auto cbrt1 = add(neg(div(one, i2)), coef);
        auto cbrt2 = sub(neg(div(one, i2)), coef);
        root2 = neg(div(add(b, add(mul(cbrt1, C), div(delta0, mul(cbrt1, C)))),
                        mul(i3, a)));
        root3 = neg(div(add(b, add(mul(cbrt2, C), div(delta0, mul(cbrt2, C)))),
                        mul(i3, a)));
    }

    return set_intersection({domain, finiteset({root1, root2, root3})});
}

RCP<const Set> solve_poly(const RCP<const Basic> &f,
                          const RCP<const Symbol> &sym,
                          const RCP<const Set> &domain)
{
    auto degree = from_basic<UIntPoly>(f, sym)->get_poly().degree();
    switch (degree) {
        case 0: {
            if (eq(*f, *zero)) {
                return domain;
            } else {
                return emptyset();
            }
        }
        case 1:
            return solve_poly_linear(f, sym, domain);
        case 2:
            return solve_poly_quadratic(f, sym, domain);
        case 3:
            return solve_poly_cubic(f, sym, domain);
        // case 4 : return solve_poly_quartic(f,sym,domain);
        default: {
        }
    }
}

RCP<const Set> solve_rational(const RCP<const Basic> &f,
                              const RCP<const Symbol> &sym,
                              const RCP<const Set> &domain)
{
    RCP<const Basic> num, den;
    as_numer_denom(f, outArg(num), outArg(den));
    if (has_symbol(*den, *sym)) {
        auto numsoln = solve(num, sym, domain);
        auto densoln = solve(den, sym, domain);
        return set_complement(numsoln, densoln);
    }
    return solve_poly(num, sym, domain);
}

RCP<const Set> solve(const RCP<const Basic> &f, const RCP<const Symbol> &sym,
                     const RCP<const Set> &domain)
{
    if (eq(*f, *boolTrue))
        return domain;
    if (eq(*f, *boolFalse))
        return emptyset();
    if (is_a<Equality>(*f)) {
        return solve(sub(down_cast<const Relational &>(*f).get_arg1(),
                         down_cast<const Relational &>(*f).get_arg2()),
                     sym, domain);
    } else if (is_a<Unequality>(*f)) {
        auto soln = solve(sub(down_cast<const Relational &>(*f).get_arg1(),
                              down_cast<const Relational &>(*f).get_arg2()),
                          sym, domain);
        return set_complement(domain, soln);
    } else if (is_a_Relational(*f)) {
        // Solving inequalities not implemented yet.
        // return conditionset(sym,f,domain);
    }
    RCP<const Basic> newf = f;
    if (is_a<Mul>(*f)
        and not eq(*down_cast<const Mul &>(*f).get_coef(), *one)) {
        map_basic_basic d = down_cast<const Mul &>(*f).get_dict();
        newf = Mul::from_dict(one, std::move(d));
    }
    if (is_a_Number(*newf)) {
        if (eq(*newf, *zero)) {
            return domain;
        } else {
            return emptyset();
        }
    }
    if (not has_symbol(*newf, *sym))
        return emptyset();
    // TODO - Trig solver
    return solve_rational(newf, sym, domain);
}
}