#include "catch.hpp"

#include <symengine/basic.h>
#include <symengine/parser.h>
#include <symengine/serialize-cereal.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/logic.h>
#include <symengine/sets.h>
#include <symengine/constants.h>
#include <symengine/polys/uratpoly.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/polys/uexprpoly.h>
#include <symengine/polys/msymenginepoly.h>
#include <symengine/polys/uintpoly_flint.h>
#include <symengine/polys/uintpoly_piranha.h>
#include <symengine/expression.h>
#include <cereal/archives/binary.hpp>

using std::string;

using SymEngine::add;
using SymEngine::Basic;
using SymEngine::complex_double;
using SymEngine::cos;
using SymEngine::dummy;
using SymEngine::Integer;
using SymEngine::is_a;
using SymEngine::Number;
using SymEngine::RCP;
using SymEngine::RCPBasicAwareInputArchive;
using SymEngine::RCPBasicAwareOutputArchive;
using SymEngine::sin;
using SymEngine::Symbol;
#ifdef HAVE_SYMENGINE_MPFR
using SymEngine::mpfr_class;
using SymEngine::RealMPFR;
#endif

namespace se = SymEngine;

template <typename T>
string dumps(RCP<const T> obj)
{
    std::ostringstream oss;
    RCPBasicAwareOutputArchive<cereal::BinaryOutputArchive>{oss}(obj);
    return oss.str();
}

template <typename T>
RCP<const T> loads(string sobj)
{
    RCP<const T> obj;
    std::istringstream iss(sobj);
    RCPBasicAwareInputArchive<cereal::BinaryInputArchive>{iss}(obj);
    return obj;
}

template <typename T>
void check_string_serialization_roundtrip(RCP<const T> basic1)
{
    RCP<const Basic> basic2 = loads<T>(dumps<T>(basic1));
    CAPTURE(*basic1);
    CAPTURE(*basic2);
    REQUIRE(eq(*basic1, *basic2));

    RCP<const Basic> basic3 = loads<Basic>(dumps<Basic>(basic1));
    CAPTURE(*basic3);
    REQUIRE(eq(*basic1, *basic3));
}

TEST_CASE("Test serialization using cereal", "[serialize-cereal]")
{
    RCP<const Symbol> symb_x_ori = se::symbol("x");
    string s_symb_x = dumps<Symbol>(symb_x_ori);
    RCP<const Symbol> symb_x_des = loads<Symbol>(s_symb_x);
    REQUIRE(symb_x_ori->__eq__(*symb_x_des));
    RCP<const Basic> basic_x_des = loads<Basic>(s_symb_x);
    REQUIRE(is_a<Symbol>(*basic_x_des));
    REQUIRE(!is_a<Integer>(*basic_x_des));

    // Symbol
    check_string_serialization_roundtrip(se::symbol("y"));
    // Dummy
    check_string_serialization_roundtrip(se::dummy("foo"));
    // Add
    check_string_serialization_roundtrip(
        se::add(se::symbol("y"), se::integer(3)));
    check_string_serialization_roundtrip(
        se::add(se::symbol("y"), se::integer(-3)));
    // Pow
    check_string_serialization_roundtrip(
        se::pow(se::symbol("y"), se::integer(2)));
    check_string_serialization_roundtrip(se::reals());
    check_string_serialization_roundtrip(
        complex_double(std::complex<double>(4, 5)));
#ifdef HAVE_SYMENGINE_MPFR
    check_string_serialization_roundtrip(
        real_mpfr(mpfr_class("0.35", 100, 10)));
#endif
    // Rational
    check_string_serialization_roundtrip(
        se::parse("x**3/6 + x**9/135 + x**15/600"));
    // Complex
    check_string_serialization_roundtrip(
        se::parse("(1 + 2*I)*x**2 + (3 + 4*I)*x**8 + (5 + 6*I)*x**14"));
    // Rational and Complex
    check_string_serialization_roundtrip(
        se::parse("sin(x)**3/6 + (2 + 3*I)*cos(y)/135 + log(x)/(600*sin(z))"));
    // Numbers
    check_string_serialization_roundtrip(se::parse("42"));   // Integer
    check_string_serialization_roundtrip(se::parse("1.25")); // RealDouble
    check_string_serialization_roundtrip(se::parse("oo"));   // Infty
    check_string_serialization_roundtrip(se::parse("nan"));  // NaN
    check_string_serialization_roundtrip(se::parse("pi"));   // Constant
#ifdef HAVE_SYMENGINE_MPC
    // ComplexMPC (real and imaginary parts at MPFR precision)
    check_string_serialization_roundtrip(
        se::add(se::real_mpfr(mpfr_class("0.35", 100, 10)),
                se::mul(se::I, se::real_mpfr(mpfr_class("0.6", 100, 10)))));
#endif

    // One-argument functions (trigonometric)
    check_string_serialization_roundtrip(
        se::parse("sin(x) + cos(x) + tan(x) + cot(x) + sec(x) + csc(x)"));
    // One-argument functions (inverse trigonometric)
    check_string_serialization_roundtrip(
        se::parse("asin(x) + acos(x) + atan(x) + acot(x) + asec(x) + acsc(x)"));
    // One-argument functions (hyperbolic)
    check_string_serialization_roundtrip(
        se::parse("sinh(x) + cosh(x) + tanh(x) + coth(x) + sech(x) + csch(x)"));
    // One-argument functions (inverse hyperbolic)
    check_string_serialization_roundtrip(se::parse(
        "asinh(x) + acosh(x) + atanh(x) + acoth(x) + asech(x) + acsch(x)"));
    // One-argument functions (other): Log, Gamma, Abs, Sign, Erf, Erfc,
    // LogGamma, LambertW, Dirichlet_eta, Floor, Ceiling, PrimePi, Primorial
    check_string_serialization_roundtrip(
        se::parse("log(x) + gamma(x) + abs(x) + sign(x) + erf(x) + erfc(x)"
                  " + loggamma(x) + lambertw(x) + dirichlet_eta(x) + floor(x)"
                  " + ceiling(x) + primepi(x) + primorial(x)"));
    // One-argument functions not reachable through the parser
    check_string_serialization_roundtrip(se::conjugate(se::symbol("x")));
    check_string_serialization_roundtrip(se::truncate(se::symbol("x")));
    check_string_serialization_roundtrip(se::unevaluated_expr(se::symbol("x")));

    // Two-argument functions: ATan2, Zeta, LowerGamma, UpperGamma, PolyGamma,
    // Beta, KroneckerDelta
    check_string_serialization_roundtrip(se::parse(
        "atan2(x, y) + zeta(x, y) + lowergamma(x, y) + uppergamma(x, y)"
        " + polygamma(x, y) + beta(x, y) + kronecker_delta(x, y)"));
    // Multi-argument functions: Max, Min, LeviCivita, FunctionSymbol
    check_string_serialization_roundtrip(
        se::parse("max(x, y, z) + min(x, y) + levi_civita(x, y, z) + f(x, y)"));

    // Relational
    check_string_serialization_roundtrip(se::parse("Eq(x, y)")); // Equality
    check_string_serialization_roundtrip(se::parse("Ne(x, y)")); // Unequality
    check_string_serialization_roundtrip(se::parse("Le(x, y)")); // LessThan
    check_string_serialization_roundtrip(
        se::parse("Lt(x, y)")); // StrictLessThan

    // Logic
    check_string_serialization_roundtrip(se::boolean(true)); // BooleanAtom
    check_string_serialization_roundtrip(se::parse("And(Lt(x, y), Lt(y, z))"));
    check_string_serialization_roundtrip(se::parse("Or(Lt(x, y), Lt(y, z))"));
    check_string_serialization_roundtrip(se::parse("Xor(Lt(x, y), Lt(y, z))"));
    check_string_serialization_roundtrip(
        se::logical_not(se::contains(se::symbol("x"), se::integers()))); // Not
    check_string_serialization_roundtrip(
        se::contains(se::symbol("x"), se::reals())); // Contains

    // Sets
    check_string_serialization_roundtrip(se::emptyset());
    check_string_serialization_roundtrip(se::universalset());
    check_string_serialization_roundtrip(se::integers());
    check_string_serialization_roundtrip(se::rationals());
    check_string_serialization_roundtrip(
        se::interval(se::integer(1), se::integer(5)));
    check_string_serialization_roundtrip(
        se::finiteset({se::integer(1), se::integer(2), se::integer(3)}));
    check_string_serialization_roundtrip(
        se::set_union({se::interval(se::integer(0), se::integer(1)),
                       se::interval(se::integer(2), se::integer(3))}));
    check_string_serialization_roundtrip(
        se::set_complement(se::reals(), se::finiteset({se::integer(0)})));
    check_string_serialization_roundtrip(
        se::imageset(se::symbol("x"), se::mul(se::integer(2), se::symbol("x")),
                     se::integers()));
    check_string_serialization_roundtrip(se::conditionset(
        se::symbol("x"),
        se::Eq(se::mul(se::symbol("x"), se::symbol("x")), se::integer(2))));

    // Piecewise
    check_string_serialization_roundtrip(se::piecewise(
        {{se::symbol("x"), se::contains(se::symbol("x"), se::reals())}}));

    // Derivative and Subs
    check_string_serialization_roundtrip(se::Derivative::create(
        se::function_symbol("f", se::vec_basic{se::symbol("x")}),
        {se::symbol("x")}));
    check_string_serialization_roundtrip(se::Subs::create(
        se::Derivative::create(
            se::function_symbol(
                "f", se::vec_basic{se::symbol("x"), se::symbol("y")}),
            {se::symbol("x")}),
        {{se::symbol("x"), se::add(se::symbol("x"), se::symbol("y"))}}));

    // UIntPoly: univariate polynomial with integer coefficients,
    // i.e. 2 - 5*x**3 + 11*x**7
    check_string_serialization_roundtrip(
        se::UIntPoly::from_dict(se::symbol("x"), {{0, se::integer_class(2)},
                                                  {3, se::integer_class(-5)},
                                                  {7, se::integer_class(11)}}));
    // URatPoly: univariate polynomial with rational coefficients,
    // i.e. 1/2 + 3/4*x**2 - 7/6*x**5
    check_string_serialization_roundtrip(se::URatPoly::from_dict(
        se::symbol("x"),
        {{0, se::rational_class(se::integer_class(1), se::integer_class(2))},
         {2, se::rational_class(se::integer_class(3), se::integer_class(4))},
         {5,
          se::rational_class(se::integer_class(-7), se::integer_class(6))}}));
    // UExprPoly: univariate polynomial with symbolic expression coefficients,
    // i.e. a + 3*x**2 + a*b*x**5
    check_string_serialization_roundtrip(se::UExprPoly::from_dict(
        se::symbol("x"),
        {{0, se::Expression(se::symbol("a"))},
         {2, se::Expression(se::integer(3))},
         {5, se::Expression(se::mul(se::symbol("a"), se::symbol("b")))}}));
    // MIntPoly: multivariate polynomial with integer coefficients,
    // i.e. 2*y + 3*x - 5*x**2*y**2
    check_string_serialization_roundtrip(se::MIntPoly::from_dict(
        {se::symbol("x"), se::symbol("y")}, {{{0, 1}, se::integer_class(2)},
                                             {{1, 0}, se::integer_class(3)},
                                             {{2, 2}, se::integer_class(-5)}}));
    // MExprPoly: multivariate polynomial with symbolic expression coefficients,
    // i.e. x + z*y + x*z*x**2*y**2
    check_string_serialization_roundtrip(se::MExprPoly::from_dict(
        {se::symbol("x"), se::symbol("y")},
        {{{1, 0}, se::Expression(se::integer(1))},
         {{0, 1}, se::Expression(se::symbol("z"))},
         {{2, 2}, se::Expression(se::mul(se::symbol("x"), se::symbol("z")))}}));
#ifdef HAVE_SYMENGINE_FLINT
    // UIntPolyFlint: Flint-backed univariate integer polynomial
    check_string_serialization_roundtrip(se::UIntPolyFlint::from_dict(
        se::symbol("x"), {{0, se::integer_class(2)},
                          {3, se::integer_class(-5)},
                          {7, se::integer_class(11)}}));
    // URatPolyFlint: Flint-backed univariate rational polynomial
    check_string_serialization_roundtrip(se::URatPolyFlint::from_dict(
        se::symbol("x"),
        {{0, se::rational_class(se::integer_class(1), se::integer_class(2))},
         {2, se::rational_class(se::integer_class(3), se::integer_class(4))},
         {5,
          se::rational_class(se::integer_class(-7), se::integer_class(6))}}));
#endif
#ifdef HAVE_SYMENGINE_PIRANHA
    // UIntPolyPiranha: Piranha-backed univariate integer polynomial
    check_string_serialization_roundtrip(se::UIntPolyPiranha::from_dict(
        se::symbol("x"), {{0, se::integer_class(2)},
                          {3, se::integer_class(-5)},
                          {7, se::integer_class(11)}}));
    // URatPolyPiranha: Piranha-backed univariate rational polynomial
    check_string_serialization_roundtrip(se::URatPolyPiranha::from_dict(
        se::symbol("x"),
        {{0, se::rational_class(se::integer_class(1), se::integer_class(2))},
         {2, se::rational_class(se::integer_class(3), se::integer_class(4))},
         {5,
          se::rational_class(se::integer_class(-7), se::integer_class(6))}}));
#endif
}

TEST_CASE("Test serialization exception", "[serialize-cereal]")
{
    RCP<const Basic> expr = se::parse("x + y");
    std::string orig_data = expr->dumps();

    // These positions were chosen because they do not try to create an object
    // that throws std::bad_alloc
    std::vector<int> positions
        = {29, 30, 31, 32, 56, 57, 58, 59, 75, 76, 77, 78, 94, 95, 96, 97};
#if defined(_MSC_VER) && defined(_DEBUG)
    size_t end = 55;
#else
    size_t end = orig_data.size();
#endif

    for (size_t pos = 0; pos < end; pos++) {
        if (std::find(positions.begin(), positions.end(), pos)
            != positions.end()) {
            continue;
        }
        std::string data = orig_data;
        data[pos] = char(9);
        // Corrupted data should either give an expr
        // or throw an error
        try {
            Basic::loads(data);
        } catch (se::SerializationError &e) {
        }
    }
}

TEST_CASE("Test serialization shared pointer", "[serialize-cereal]")
{
    RCP<const Basic> b = se::symbol("x");
    RCP<const Basic> expr = add(sin(b), cos(b));

    RCP<const Basic> new_expr = Basic::loads(expr->dumps());
    REQUIRE(new_expr->get_args()[0]->get_args()[0].get()
            == new_expr->get_args()[1]->get_args()[0].get());
}