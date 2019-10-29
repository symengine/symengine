#include "catch.hpp"
#include <chrono>

#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/polys/cancel.cpp>
#include <symengine/polys/uintpoly_flint.h>

using SymEngine::cancel;
using SymEngine::mul;
using SymEngine::integer;
using SymEngine::UIntPolyFlint;
using SymEngine::RCP;

using namespace SymEngine::literals;

TEST_CASE("cancel", "[Basic]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const UIntPolyFlint> result_self, result_other;

    cancel(mul(x, integer(2)), x, outArg(result_self), outArg(result_other));
    REQUIRE(result_self->__str__() == "2");
    REQUIRE(result_other->__str__() == "1");
}
