#include "catch.hpp"
#include <chrono>

#include <symengine/add.h>
#include <symengine/basic.h>
#include <symengine/complex.h>
#include <symengine/complex_double.h>
#include <symengine/complex_mpc.h>
#include <symengine/integer.h>
#include <symengine/matrix.h>
#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/pow.h>
#include <symengine/printer.h>
#include <symengine/rational.h>
#include <symengine/real_double.h>
#include <symengine/real_mpfr.h>
#include <symengine/symbol.h>

using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::div;
using SymEngine::Expression;
using SymEngine::pow;
using SymEngine::univariate_int_polynomial;
using SymEngine::univariate_polynomial;
using SymEngine::mul;
using SymEngine::integer;
using SymEngine::print_stack_on_segfault;
using SymEngine::symbol;
using SymEngine::Complex;
using SymEngine::Rational;
using SymEngine::Number;
using SymEngine::add;
using SymEngine::Symbol;
using SymEngine::erf;
using SymEngine::Integer;
using SymEngine::DenseMatrix;
using SymEngine::loggamma;
using SymEngine::Subs;
using SymEngine::Derivative;
using SymEngine::function_symbol;
using SymEngine::I;
using SymEngine::real_double;
using SymEngine::complex_double;
using SymEngine::BaseVisitor;
using SymEngine::StrPrinter;
using SymEngine::Sin;
using SymEngine::integer_class;
using SymEngine::map_uint_mpz;

using namespace SymEngine::literals;

namespace SymEngine
{
class MyStrPrinter : public BaseVisitor<MyStrPrinter, StrPrinter>
{
public:
    using StrPrinter::bvisit;

    void bvisit(const Sin &x)
    {
        str_ = "MySin(" + this->apply(x.get_arg()) + ")";
    }
};
}

TEST_CASE("test_printing(): printing", "[printing]")
{
    RCP<const Basic> r, r1, r2;
    RCP<const Integer> i = integer(-1);
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");

    r = div(integer(12), pow(integer(195), div(integer(1), integer(2))));
    REQUIRE(r->__str__() == "(4/65)*195**(1/2)");

    r = mul(integer(12), pow(integer(195), div(integer(1), integer(2))));
    REQUIRE(r->__str__() == "12*195**(1/2)");

    r = mul(integer(23), mul(pow(integer(5), div(integer(1), integer(2))),
                             pow(integer(7), div(integer(1), integer(2)))));
    REQUIRE(r->__str__() == "23*5**(1/2)*7**(1/2)");

    r = mul(integer(2), pow(symbol("x"), integer(2)));
    REQUIRE(r->__str__() == "2*x**2");

    r = mul(integer(23),
            mul(pow(div(integer(5), integer(2)), div(integer(1), integer(2))),
                pow(div(integer(7), integer(3)), div(integer(1), integer(2)))));
    REQUIRE(r->__str__() == "(23/6)*2**(1/2)*3**(1/2)*5**(1/2)*7**(1/2)");

    r = pow(div(symbol("x"), integer(2)), div(integer(1), integer(2)));
    REQUIRE(r->__str__() == "(1/2)*2**(1/2)*x**(1/2)");

    r = pow(div(integer(3), integer(2)), div(integer(1), integer(2)));
    REQUIRE(r->__str__() == "(1/2)*2**(1/2)*3**(1/2)");

    r1 = mul(integer(12), pow(integer(196), div(integer(-1), integer(2))));
    r2 = mul(integer(294), pow(integer(196), div(integer(-1), integer(2))));
    r = add(integer(-51), mul(r1, r2));
    REQUIRE(r->__str__() == "-33");

    r1 = mul(x, i);
    r2 = mul(r1, y);
    REQUIRE(r1->__str__() == "-x");
    REQUIRE(r1->__str__() != "-1x");
    REQUIRE(r2->__str__() == "-x*y");
    REQUIRE(r2->__str__() != "-1x*y");

    r = mul(integer(-1), pow(integer(195), div(integer(1), integer(2))));
    REQUIRE(r->__str__() == "-195**(1/2)");
    r = pow(integer(-6), div(integer(1), integer(2)));
    REQUIRE(r->__str__() == "I*6**(1/2)");

    RCP<const Number> rn1, rn2, rn3, c1, c2;
    rn1 = Rational::from_two_ints(*integer(2), *integer(4));
    rn2 = Rational::from_two_ints(*integer(5), *integer(7));
    rn3 = Rational::from_two_ints(*integer(-5), *integer(7));

    c1 = Complex::from_two_rats(static_cast<const Rational &>(*rn1),
                                static_cast<const Rational &>(*rn2));
    c2 = Complex::from_two_rats(static_cast<const Rational &>(*rn1),
                                static_cast<const Rational &>(*rn3));
    r1 = mul(c1, x);
    r2 = mul(c2, x);
    REQUIRE(c1->__str__() == "1/2 + 5/7*I");
    REQUIRE(c2->__str__() == "1/2 - 5/7*I");
    REQUIRE(r1->__str__() == "(1/2 + 5/7*I)*x");
    REQUIRE(r2->__str__() == "(1/2 - 5/7*I)*x");
    r1 = pow(x, c1);
    r2 = pow(x, c2);
    REQUIRE(r1->__str__() == "x**(1/2 + 5/7*I)");
    REQUIRE(r2->__str__() == "x**(1/2 - 5/7*I)");

    c1 = Complex::from_two_nums(*rn1, *rn2);
    c2 = Complex::from_two_nums(*rn1, *rn3);
    REQUIRE(c1->__str__() == "1/2 + 5/7*I");
    REQUIRE(c2->__str__() == "1/2 - 5/7*I");

    rn1 = Rational::from_two_ints(*integer(0), *integer(4));
    c1 = Complex::from_two_nums(*rn1, *rn2);
    c2 = Complex::from_two_nums(*rn1, *rn3);
    r1 = mul(c1, x);
    r2 = mul(c2, x);
    REQUIRE(c1->__str__() == "5/7*I");
    REQUIRE(c2->__str__() == "-5/7*I");
    REQUIRE(r1->__str__() == "5/7*I*x");
    REQUIRE(r2->__str__() == "-5/7*I*x");
    r1 = pow(x, c1);
    r2 = pow(x, c2);
    REQUIRE(r1->__str__() == "x**(5/7*I)");
    REQUIRE(r2->__str__() == "x**(-5/7*I)");

    c1 = Complex::from_two_nums(*rn2, *rn1);
    c2 = Complex::from_two_nums(*rn3, *rn1);
    r1 = mul(c1, x);
    r2 = mul(c2, x);
    REQUIRE(c1->__str__() == "5/7");
    REQUIRE(c2->__str__() == "-5/7");
    REQUIRE(r1->__str__() == "(5/7)*x");
    REQUIRE(r2->__str__() == "(-5/7)*x");
    r1 = pow(x, c1);
    r2 = pow(x, c2);
    REQUIRE(r1->__str__() == "x**(5/7)");
    REQUIRE(r2->__str__() == "x**(-5/7)");

    rn1 = Rational::from_two_ints(*integer(1), *integer(1));
    c1 = Complex::from_two_nums(*rn2, *rn1);
    REQUIRE(c1->__str__() == "5/7 + I");
    rn1 = Rational::from_two_ints(*integer(-1), *integer(1));
    c1 = Complex::from_two_nums(*rn2, *rn1);
    REQUIRE(c1->__str__() == "5/7 - I");

    r1 = mul(c1, x);
    REQUIRE(r1->__str__() == "(5/7 - I)*x");

    r1 = mul(integer(2), x);
    REQUIRE(r1->__str__() == "2*x");

    r1 = mul(mul(integer(2), pow(symbol("x"), div(integer(2), integer(3)))), y);
    REQUIRE(r1->__str__() == "2*x**(2/3)*y");

    r1 = mul(x, y);
    REQUIRE(r1->__str__() == "x*y");

    r = div(x, add(x, y));
    r1 = div(x, pow(add(x, y), div(integer(2), integer(3))));
    r2 = div(x, pow(add(x, y), div(integer(-2), integer(3))));
    REQUIRE(r->__str__() == "x/(x + y)");
    REQUIRE(r1->__str__() == "x/(x + y)**(2/3)");
    REQUIRE(r2->__str__() == "x*(x + y)**(2/3)");

    r = div(integer(1), mul(x, add(x, y)));
    r1 = div(mul(y, integer(-1)), mul(x, add(x, y)));
    r2 = mul(pow(y, x), pow(x, y));
    REQUIRE(r->__str__() == "1/(x*(x + y))");
    REQUIRE(r1->__str__() == "-y/(x*(x + y))");
    REQUIRE(r2->__str__() == "x**y*y**x");

    r = pow(y, pow(x, integer(2)));
    r1 = pow(integer(3), mul(integer(2), x));
    r2 = pow(integer(3), mul(integer(-1), x));
    REQUIRE(r->__str__() == "y**(x**2)");
    REQUIRE(r1->__str__() == "3**(2*x)");
    REQUIRE(r2->__str__() == "3**(-x)");

    r1 = pow(mul(integer(2), x), y);
    r2 = pow(mul(x, y), z);
    REQUIRE(r1->__str__() == "(2*x)**y");
    REQUIRE(r2->__str__() == "(x*y)**z");

    r1 = pow(pow(integer(2), x), y);
    r2 = pow(pow(x, y), z);
    REQUIRE(r1->__str__() == "(2**x)**y");
    REQUIRE(r2->__str__() == "(x**y)**z");

    r = pow(I, x);
    r1 = sub(sub(integer(2), x), y);
    REQUIRE(r->__str__() == "I**x");
    REQUIRE(r1->__str__() == "2 - x - y");

    RCP<const Basic> f = function_symbol("f", x);
    RCP<const Basic> g = function_symbol("g", x);
    r = f->diff(x);
    r1 = Derivative::create(f, {x});
    r2 = Derivative::create(g, {x});

    REQUIRE(r->__str__() == "Derivative(f(x), x)");
    REQUIRE(r1->__str__() == "Derivative(f(x), x)");
    REQUIRE(r2->__str__() == "Derivative(g(x), x)");

    r1 = f->diff(x)->diff(x);
    REQUIRE(r1->__str__() == "Derivative(f(x), x, x)");

    f = function_symbol("f", {x, y});
    r = f->diff(x)->diff(y);
    REQUIRE(r->__str__() == "Derivative(f(x, y), x, y)");
    r1 = Subs::create(Derivative::create(function_symbol("f", {y, x}), {x}),
                      {{x, add(x, y)}});
    REQUIRE(r1->__str__() == "Subs(Derivative(f(y, x), x), (x), (x + y))");
}

TEST_CASE("test_matrix(): printing", "[printing]")
{
    DenseMatrix A
        = DenseMatrix(2, 2, {integer(1), integer(0), integer(0), integer(1)});
    REQUIRE(A.__str__() == "[1, 0]\n[0, 1]\n");
}

TEST_CASE("test_univariate_int_polynomial(): printing", "[printing]")
{
    RCP<const Basic> p;
    RCP<const Symbol> x = symbol("x");

    p = univariate_int_polynomial(x, {{0, 0_z}});
    REQUIRE(p->__str__() == "0");

    p = univariate_int_polynomial(x, {{0, 1_z}});
    REQUIRE(p->__str__() == "1");

    p = univariate_int_polynomial(x, {{1, 1_z}});
    REQUIRE(p->__str__() == "x");

    p = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}});
    REQUIRE(p->__str__() == "2*x + 1");

    p = univariate_int_polynomial(x, {{0, -1_z}, {1, 2_z}});
    REQUIRE(p->__str__() == "2*x - 1");

    p = univariate_int_polynomial(x, {{0, -1_z}});
    REQUIRE(p->__str__() == "-1");

    p = univariate_int_polynomial(x, {{1, -1_z}});
    REQUIRE(p->__str__() == "-x");

    p = univariate_int_polynomial(x, {{0, -1_z}, {1, 1_z}});
    REQUIRE(p->__str__() == "x - 1");

    p = univariate_int_polynomial(x, {{0, 1_z}, {1, 1_z}, {2, 1_z}});
    REQUIRE(p->__str__() == "x**2 + x + 1");

    p = univariate_int_polynomial(x, {{0, 1_z}, {1, -1_z}, {2, 1_z}});
    REQUIRE(p->__str__() == "x**2 - x + 1");

    p = univariate_int_polynomial(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(p->__str__() == "x**2 + 2*x + 1");

    p = univariate_int_polynomial(x, {{1, 2_z}, {2, 1_z}});
    REQUIRE(p->__str__() == "x**2 + 2*x");

    p = univariate_int_polynomial(x, {{0, -1_z}, {1, -2_z}, {2, -1_z}});

    REQUIRE(p->__str__() == "-x**2 - 2*x - 1");
}

TEST_CASE("test_univariate_polynomial(): printing", "[printing]")
{
    RCP<const Basic> p;
    RCP<const Symbol> x = symbol("x");
    Expression a(symbol("a"));
    Expression b(symbol("b"));
    Expression c(symbol("c"));
    Expression d(symbol("d"));

    p = univariate_polynomial(x, {{0, 0}});
    REQUIRE(p->__str__() == "0");
    p = univariate_polynomial(x, {{0, 1}});
    REQUIRE(p->__str__() == "1");
    p = univariate_polynomial(x, {{1, 1}});
    REQUIRE(p->__str__() == "x");
    p = univariate_polynomial(x, {{0, 1}, {1, 2}});
    REQUIRE(p->__str__() == "2*x + 1");
    p = univariate_polynomial(x, {{0, -1}, {1, 2}});
    REQUIRE(p->__str__() == "2*x - 1");
    p = univariate_polynomial(x, {{0, -1}});
    REQUIRE(p->__str__() == "-1");
    p = univariate_polynomial(x, {{1, -1}});
    REQUIRE(p->__str__() == "-x");
    p = univariate_polynomial(x, {{0, -1}, {1, 1}});
    REQUIRE(p->__str__() == "x - 1");
    p = univariate_polynomial(x, {{0, 1}, {1, 1}, {2, 1}});
    REQUIRE(p->__str__() == "x**2 + x + 1");
    p = univariate_polynomial(x, {{0, 1}, {1, -1}, {2, 1}});
    REQUIRE(p->__str__() == "x**2 - x + 1");
    p = univariate_polynomial(x, {{0, 1}, {1, 2}, {2, 1}});
    REQUIRE(p->__str__() == "x**2 + 2*x + 1");
    p = univariate_polynomial(x, {{1, 2}, {2, 1}});
    REQUIRE(p->__str__() == "x**2 + 2*x");
    p = univariate_polynomial(x, {{0, -1}, {1, -2}, {2, -1}});
    REQUIRE(p->__str__() == "-x**2 - 2*x - 1");
    p = univariate_polynomial(x, {{-1, d}});

    REQUIRE(p->__str__() == "d*x**(-1)");
    REQUIRE(not(p->__str__() == "d*x**-1"));

    p = univariate_polynomial(x, {{-2, d}, {-1, c}, {0, b}, {1, a}});
    REQUIRE(p->__str__() == "a*x + b + c*x**(-1) + d*x**(-2)");
}

TEST_CASE("test_floats(): printing", "[printing]")
{
    RCP<const Basic> p;
    ;
    RCP<const Basic> x = symbol("x");

    p = real_double(11111.11);
    p = pow(p, x);
    REQUIRE(p->__str__() == "11111.11**x");

    p = real_double(0.00000011);
    p = mul(p, x);
    bool pr = (p->__str__() == "1.1e-07*x") or (p->__str__() == "1.1e-007*x");
    REQUIRE(pr == true);

    p = complex_double(std::complex<double>(0.1, 0.2));
    p = mul(p, x);
    REQUIRE(p->__str__() == "(0.1 + 0.2*I)*x");

    p = real_double(123);
    p = sub(p, x);
    REQUIRE(p->__str__() == "123.0 - x");

    p = complex_double(std::complex<double>(1, 2));
    p = add(p, x);
    REQUIRE(p->__str__() == "1.0 + 2.0*I + x");

#ifdef HAVE_SYMENGINE_MPFR
    SymEngine::mpfr_class m1(75);
    mpfr_set_ui(m1.get_mpfr_t(), 123, MPFR_RNDN);
    p = SymEngine::real_mpfr(m1);
    p = add(p, x);
    REQUIRE(p->__str__() == "123.000000000000000000000 + x");
#ifdef HAVE_SYMENGINE_MPC
    SymEngine::mpc_class m2(75);
    mpc_set_si_si(m2.get_mpc_t(), -10, 10, MPC_RNDNN);
    p = SymEngine::complex_mpc(m2);
    p = div(p, x);
    REQUIRE(p->__str__()
            == "(-10.0000000000000000000000 + 10.0000000000000000000000*I)/x");
#endif
#endif
}

TEST_CASE("test_functions(): printing", "[printing]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> p = loggamma(x);
    REQUIRE(p->__str__() == "loggamma(x)");

    p = erf(x);
    REQUIRE(p->__str__() == "erf(x)");

    p = erf(add(x, y));
    REQUIRE(p->__str__() == "erf(x + y)");
}

TEST_CASE("test custom printing", "[printing]")
{
    SymEngine::MyStrPrinter printer;
    RCP<const Basic> p;
    RCP<const Symbol> x = symbol("x");
    p = sin(x);
    CHECK(printer.apply(p) == "MySin(x)");
    p = cos(sin(x));
    CHECK(printer.apply(p) == "cos(MySin(x))");
}

TEST_CASE("Ascii Art", "[basic]")
{
    std::cout << SymEngine::ascii_art() << std::endl;
}
