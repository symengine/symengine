#include "catch.hpp"

#include <symengine/ntheory_funcs.h>
#include <symengine/basic.h>
#include <symengine/rational.h>
#include <symengine/real_double.h>

using SymEngine::Basic;
using SymEngine::Complex;
using SymEngine::down_cast;
using SymEngine::Inf;
using SymEngine::Infty;
using SymEngine::integer;
using SymEngine::is_a;
using SymEngine::make_rcp;
using SymEngine::Nan;
using SymEngine::NaN;
using SymEngine::pi;
using SymEngine::PrimePi;
using SymEngine::Primorial;
using SymEngine::Rational;
using SymEngine::RCP;
using SymEngine::real_double;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::SymEngineException;

TEST_CASE("test_primepi(): ntheory_funcs", "[ntheory_funcs]")
{
    REQUIRE(eq(*primepi(integer(2)), *integer(1)));
    REQUIRE(eq(*primepi(pi), *integer(2)));
    REQUIRE(eq(*primepi(integer(5)), *integer(3)));
    REQUIRE(eq(*primepi(integer(28)), *integer(9)));
    REQUIRE(eq(*primepi(integer(19876)), *integer(2248)));
    REQUIRE(eq(*primepi(integer(1000000)), *integer(78498)));
    REQUIRE(eq(*primepi(integer(-1)), *integer(0)));
    REQUIRE(eq(*primepi(integer(-1000)), *integer(0)));
    REQUIRE(eq(*primepi(Rational::from_two_ints(1, 2)), *integer(0)));
    REQUIRE(eq(*primepi(Rational::from_two_ints(-1, 2)), *integer(0)));
    REQUIRE(eq(*primepi(Rational::from_two_ints(900000, 8999)), *integer(25)));
    REQUIRE(eq(*primepi(real_double(-23.5)), *integer(0)));
    REQUIRE(eq(*primepi(real_double(23.5)), *integer(9)));
    REQUIRE(eq(*primepi(real_double(16545.234)), *integer(1914)));
    REQUIRE(is_a<NaN>(*primepi(Nan)));
    REQUIRE(is_a<Infty>(*primepi(Inf)));
    REQUIRE(eq(*primepi(mul(integer(-1), Inf)), *integer(0)));
    CHECK_THROWS_AS(*primepi(Complex::from_two_nums(*integer(1), *integer(1))),
                    SymEngineException &);
    REQUIRE(eq(*primepi(symbol("x")), *make_rcp<PrimePi>(symbol("x"))));
    REQUIRE(primepi(symbol("x"))->__str__() == "primepi(x)");
    REQUIRE(eq(*PrimePi(symbol("x")).create(integer(2)), *integer(1)));
    REQUIRE(not(PrimePi(symbol("x")).is_canonical(integer(1))));
}

TEST_CASE("test_primorial(): ntheory_funcs", "[ntheory_funcs]")
{
    CHECK_THROWS_AS(*primorial(integer(0)), SymEngineException &);
    REQUIRE(eq(*primorial(integer(1)), *integer(1)));
    REQUIRE(eq(*primorial(integer(2)), *integer(2)));
    REQUIRE(eq(*primorial(integer(3)), *integer(6)));
    REQUIRE(eq(*primorial(integer(9)), *integer(210)));
    REQUIRE(eq(*primorial(real_double(23.9)), *integer(223092870)));
    REQUIRE(eq(*primorial(pi), *integer(6)));
    REQUIRE(is_a<NaN>(*primorial(Nan)));
    REQUIRE(is_a<Infty>(*primorial(Inf)));
    REQUIRE(eq(*primorial(symbol("x")), *make_rcp<Primorial>(symbol("x"))));
    REQUIRE(primorial(symbol("x"))->__str__() == "primorial(x)");
    REQUIRE(eq(*Primorial(symbol("x")).create(integer(1)), *integer(1)));
    REQUIRE(not(Primorial(symbol("x")).is_canonical(integer(1))));
}
