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
    vec_basic grid{integer(-1), integer(0), integer(1)};
    auto weights = generate_fdiff_weights_vector(grid, 2, integer(0));
    CHECK(weights.size() == 9);

    CHECK(eq(*weights[0], *integer(0)));
    CHECK(eq(*weights[1], *integer(1)));
    CHECK(eq(*weights[2], *integer(0)));

    CHECK(eq(*weights[3], *div(integer(-1), integer(2))));
    CHECK(eq(*weights[4], *integer(0)));
    CHECK(eq(*weights[5], *div(integer(1), integer(2))));

    CHECK(eq(*weights[6], *integer(1)));
    CHECK(eq(*weights[7], *integer(-2)));
    CHECK(eq(*weights[8], *integer(1)));

    vec_basic grid7{integer(0), integer(1), integer(2), integer(3),
                    integer(4), integer(5), integer(6)};
    auto weights7 = generate_fdiff_weights_vector(grid7, 2, integer(3));
    CHECK(weights7.size() == 3 * 7);

    CHECK(eq(*weights7[7], *div(integer(-1), integer(60))));
    CHECK(eq(*weights7[8], *div(integer(3), integer(20))));
    CHECK(eq(*weights7[9], *div(integer(-3), integer(4))));
    CHECK(eq(*weights7[10], *integer(0)));
    CHECK(eq(*weights7[11], *div(integer(3), integer(4))));
    CHECK(eq(*weights7[12], *div(integer(-3), integer(20))));
    CHECK(eq(*weights7[13], *div(integer(1), integer(60))));

    CHECK(eq(*weights7[14], *div(integer(1), integer(90))));
    CHECK(eq(*weights7[15], *div(integer(-3), integer(20))));
    CHECK(eq(*weights7[16], *div(integer(3), integer(2))));
    CHECK(eq(*weights7[17], *div(integer(-49), integer(18))));
    CHECK(eq(*weights7[18], *div(integer(3), integer(2))));
    CHECK(eq(*weights7[19], *div(integer(-3), integer(20))));
    CHECK(eq(*weights7[20], *div(integer(1), integer(90))));
}
