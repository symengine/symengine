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
        root1 = add(div(neg(b), mul(integer(2), a)), discriminant);
        root2 = sub(div(neg(b), mul(integer(2), a)), discriminant);
    }
    return set_intersection({domain, finiteset({root1, root2})});
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
        // case 3 : return solve_poly_cubic(f,sym,domain);
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