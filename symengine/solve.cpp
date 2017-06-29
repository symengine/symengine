#include <symengine/solve.h>
#include <symengine/polys/basic_conversions.h>
#include <symengine/logic.h>
#include <symengine/mul.h>
namespace SymEngine
{

RCP<const Set> solve_poly_linear(const vec_basic &coeffs,
                                 const RCP<const Symbol> &sym,
                                 const RCP<const Set> &domain)
{
    if (coeffs.size() != 2) {
        throw SymEngineException("Expected a polynomial of degree 1. Try with "
                                 "solve() or solve_poly()");
    }
    auto root = neg(div(coeffs[0], coeffs[1]));
    return set_intersection({domain, finiteset({root})});
}

RCP<const Set> solve_poly_quadratic(const vec_basic &coeffs,
                                    const RCP<const Symbol> &sym,
                                    const RCP<const Set> &domain)
{
    if (coeffs.size() != 3) {
        throw SymEngineException("Expected a polynomial of degree 2. Try with "
                                 "solve() or solve_poly()");
    }

    auto a = coeffs[2];
    auto b = div(coeffs[1], a), c = div(coeffs[0], a);
    RCP<const Basic> root1, root2;
    if (eq(*c, *zero)) {
        root1 = neg(b);
        root2 = zero;
    } else if (eq(*b, *zero)) {
        root1 = sqrt(neg(c));
        root2 = neg(root1);
    } else {
        auto discriminant = sub(mul(b, b), mul(integer(4), c));
        auto lterm = div(neg(b), integer(2));
        auto rterm = div(sqrt(discriminant), integer(2));
        root1 = add(lterm, rterm);
        root2 = sub(lterm, rterm);
    }
    return set_intersection({domain, finiteset({root1, root2})});
}

RCP<const Set> solve_poly_cubic(const vec_basic &coeffs,
                                const RCP<const Symbol> &sym,
                                const RCP<const Set> &domain)
{
    if (coeffs.size() != 4) {
        throw SymEngineException("Expected a polynomial of degree 3. Try with "
                                 "solve() or solve_poly()");
    }

    auto a = coeffs[3];
    auto b = div(coeffs[2], a), c = div(coeffs[1], a), d = div(coeffs[0], a);

    // ref :
    // https://en.wikipedia.org/wiki/Cubic_function#General_solution_to_the_cubic_equation_with_real_coefficients
    auto i2 = integer(2), i3 = integer(3), i4 = integer(4), i9 = integer(9),
         i27 = integer(27);
    auto delta0 = sub(mul(b, b), mul(i3, c));
    auto delta1 = add(sub(mul(pow(b, i3), i2), mul({i9, b, c})), mul(i27, d));
    auto delta = div(sub(mul(i4, pow(delta0, i3)), pow(delta1, i2)), i27);
    RCP<const Basic> root1, root2, root3;
    if (eq(*delta, *zero)) {
        if (eq(*delta0, *zero)) {
            root1 = root2 = root3 = div(neg(b), i3);
        } else {
            root1 = root2 = div(sub(mul(i9, d), mul(b, c)), mul(i2, delta0));
            root3 = div(sub(mul({i4, b, c}), add(mul(d, i9), pow(b, i3))),
                        delta0);
        }
    } else {
        auto temp = sqrt(mul(neg(i27), delta));
        auto Cexpr = div(add(delta1, temp), i2);
        if (eq(*Cexpr, *zero)) {
            Cexpr = div(sub(delta1, temp), i2);
        }
        auto C = pow(Cexpr, div(one, i3));
        root1 = neg(div(add(b, add(C, div(delta0, C))), i3));

        auto coef = div(mul(I, sqrt(i3)), i2);
        temp = neg(div(one, i2));
        auto cbrt1 = add(temp, coef);
        auto cbrt2 = sub(temp, coef);
        root2 = neg(
            div(add(b, add(mul(cbrt1, C), div(delta0, mul(cbrt1, C)))), i3));
        root3 = neg(
            div(add(b, add(mul(cbrt2, C), div(delta0, mul(cbrt2, C)))), i3));
    }

    return set_intersection({domain, finiteset({root1, root2, root3})});
}

RCP<const Set> solve_poly_quartic(const vec_basic &coeffs,
                                  const RCP<const Symbol> &sym,
                                  const RCP<const Set> &domain)
{
    if (coeffs.size() != 5) {
        throw SymEngineException("Expected a polynomial of degree 4. Try with "
                                 "solve() or solve_poly()");
    }

    auto i2 = integer(2), i3 = integer(3), i4 = integer(4), i8 = integer(8),
         i16 = integer(16), i64 = integer(64), i256 = integer(256);

    // ref : http://mathforum.org/dr.math/faq/faq.cubic.equations.html
    auto lc = coeffs[4];
    auto a = div(coeffs[3], lc), b = div(coeffs[2], lc), c = div(coeffs[1], lc),
         d = div(coeffs[0], lc);
    set_basic roots;

    if (eq(*d, *zero)) {
        vec_basic newcoeffs(4);
        newcoeffs[0] = c, newcoeffs[1] = b, newcoeffs[2] = a,
        newcoeffs[3] = one;
        auto rcubic = solve_poly_cubic(newcoeffs, sym, domain);
        SYMENGINE_ASSERT(is_a<FiniteSet>(*rcubic));
        roots = down_cast<const FiniteSet &>(*rcubic).get_container();
        roots.insert(zero);
    } else {
        // substitute x = y-a/4 to get equation of the form y**4 + e*y**2 + f*y
        // + g = 0
        auto sqa = mul(a, a);
        auto cba = mul(sqa, a);
        auto aby4 = div(a, i4);
        auto e = sub(b, div(mul(i3, sqa), i8));
        auto ff = sub(add(c, div(cba, i8)), div(mul(a, b), i2));
        auto g = sub(add(d, div(mul(sqa, b), i16)),
                     add(div(mul(a, c), i4), div(mul({i3, cba, a}), i256)));

        // two special cases
        if (eq(*g, *zero)) {
            vec_basic newcoeffs(4);
            newcoeffs[0] = ff, newcoeffs[1] = e, newcoeffs[2] = zero,
            newcoeffs[3] = one;
            auto rcubic = solve_poly_cubic(newcoeffs, sym, domain);
            SYMENGINE_ASSERT(is_a<FiniteSet>(*rcubic));
            auto rtemp = down_cast<const FiniteSet &>(*rcubic).get_container();
            SYMENGINE_ASSERT(rtemp.size() > 0 and rtemp.size() <= 3);
            for (auto &r : rtemp) {
                roots.insert(sub(r, aby4));
            }
            roots.insert(neg(aby4));
        } else if (eq(*ff, *zero)) {
            vec_basic newcoeffs(3);
            newcoeffs[0] = g, newcoeffs[1] = e, newcoeffs[2] = one;
            auto rquad = solve_poly_quadratic(newcoeffs, sym, domain);
            SYMENGINE_ASSERT(is_a<FiniteSet>(*rquad));
            auto rtemp = down_cast<const FiniteSet &>(*rquad).get_container();
            SYMENGINE_ASSERT(rtemp.size() > 0 and rtemp.size() <= 2);
            for (auto &r : rtemp) {
                auto sqrtr = sqrt(r);
                roots.insert(sub(sqrtr, aby4));
                roots.insert(sub(neg(sqrtr), aby4));
            }
        } else {
            // Leonhard Euler's method
            vec_basic newcoeffs(4);
            newcoeffs[0] = neg(div(mul(ff, ff), i64)),
            newcoeffs[1] = div(sub(mul(e, e), mul(i4, g)), i16),
            newcoeffs[2] = div(e, i2);
            newcoeffs[3] = one;

            auto rcubic = solve_poly_cubic(newcoeffs, sym);
            SYMENGINE_ASSERT(is_a<FiniteSet>(*rcubic));
            roots = down_cast<const FiniteSet &>(*rcubic).get_container();
            SYMENGINE_ASSERT(roots.size() > 0 and roots.size() <= 3);
            auto p = sqrt(*roots.begin());
            auto q = p;
            if (roots.size() > 1) {
                q = sqrt(*std::next(roots.begin()));
            }
            auto r = div(neg(ff), mul({i8, p, q}));
            roots.clear();
            roots.insert(add({p, q, r, neg(aby4)}));
            roots.insert(add({p, neg(q), neg(r), neg(aby4)}));
            roots.insert(add({neg(p), q, neg(r), neg(aby4)}));
            roots.insert(add({neg(p), neg(q), r, neg(aby4)}));
        }
    }
    return set_intersection({domain, finiteset(roots)});
}

RCP<const Set> solve_poly_heuristics(const vec_basic &coeffs,
                                     const RCP<const Symbol> &sym,
                                     const RCP<const Set> &domain)
{
    auto degree = coeffs.size() - 1;
    switch (degree) {
        case 0: {
            if (eq(*coeffs[0], *zero)) {
                return domain;
            } else {
                return emptyset();
            }
        }
        case 1:
            return solve_poly_linear(coeffs, sym, domain);
        case 2:
            return solve_poly_quadratic(coeffs, sym, domain);
        case 3:
            return solve_poly_cubic(coeffs, sym, domain);
        case 4:
            return solve_poly_quartic(coeffs, sym, domain);
        default:
            throw SymEngineException(
                "expected a polynomial of order between 0 to 4");
    }
}

inline RCP<const Basic> get_coeff_basic(const integer_class &i)
{
    return integer(i);
}

inline RCP<const Basic> get_coeff_basic(const Expression &i)
{
    return i.get_basic();
}

template <typename Poly>
inline vec_basic extract_coeffs(const RCP<const Poly> &f)
{
    int degree = f->get_degree();
    vec_basic coeffs;
    for (int i = 0; i <= degree; i++)
        coeffs.push_back(get_coeff_basic(f->get_coeff(i)));
    return coeffs;
}

RCP<const Set> solve_poly(const RCP<const Basic> &f,
                          const RCP<const Symbol> &sym,
                          const RCP<const Set> &domain)
{

#if defined(HAVE_SYMENGINE_FLINT) and __FLINT_RELEASE > 20502
    try {
        auto poly = from_basic<UIntPolyFlint>(f, sym);
        auto fac = factors(*poly);
        set_set solns;
        for (const auto &elem : fac) {
            auto uip = UIntPoly::from_poly(*elem.first);
            auto degree = uip->get_poly().degree();
            if (degree <= 4) {
                solns.insert(
                    solve_poly_heuristics(extract_coeffs(uip), sym, domain));
            } else {
                solns.insert(
                    conditionset(sym, logical_and({Eq(uip->as_symbolic(), zero),
                                                   domain->contains(sym)})));
            }
        }
        return SymEngine::set_union(solns);
    } catch (SymEngineException &x) {
        // Try next
    }
#endif
    auto uexp = from_basic<UExprPoly>(f, sym);
    auto degree = uexp->get_degree();
    if (degree <= 4) {
        return solve_poly_heuristics(extract_coeffs(uexp), sym, domain);
    } else {
        return conditionset(sym,
                            logical_and({Eq(f, zero), domain->contains(sym)}));
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
        // Solving inequalities is not implemented yet.
        return conditionset(sym, logical_and({rcp_static_cast<const Boolean>(f),
                                              domain->contains(sym)}));
    }
    RCP<const Basic> newf = f;
    if (is_a<Mul>(*f)) {
        auto args = f->get_args();
        set_set solns;
        for (auto &a : args) {
            solns.insert(solve(a, sym, domain));
        }
        return SymEngine::set_union(solns);
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
