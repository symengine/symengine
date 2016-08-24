#include "catch.hpp"

#include <symengine/mul.h>
#include <symengine/integer.h>
#include <symengine/finitediff.h>

using SymEngine::div;
using SymEngine::eq;
using SymEngine::integer;
using SymEngine::vec_basic;

TEST_CASE("finitediff: generate_fdiff_weights_vector", "[finitediff]")
{
    vec_basic grid {integer(-1), integer(0), integer(1)};
    auto weights = generate_fdiff_weights_vector(grid, 2, integer(0));
    REQUIRE(weights.size() == 9);

    REQUIRE(eq(*weights[0], *integer(0)));
    REQUIRE(eq(*weights[0], *integer(1)));
    REQUIRE(eq(*weights[0], *integer(0)));

    REQUIRE(eq(*weights[0], *div(integer(-1), integer(2))));
    REQUIRE(eq(*weights[0], *integer(0)));
    REQUIRE(eq(*weights[0], *div(integer(1), integer(2))));

    REQUIRE(eq(*weights[0], *integer(1)));
    REQUIRE(eq(*weights[0], *integer(-2)));
    REQUIRE(eq(*weights[0], *integer(1)));

}
