#include "catch.hpp"
#include <symengine/utilities/matchpycpp/bipartite.h>
#include <map>

using namespace std;

TEST_CASE("BipartiteGraph", "")
{
    SECTION("Test 1")
    {
        map<tuple<int, int>, bool> m = {{{0, 1}, true},
                                        {{1, 0}, true},
                                        {{1, 1}, true},
                                        {{2, 0}, true},
                                        {{2, 1}, true}};
        BipartiteGraph<int, int, bool> bg(m);

        vector<map<int, int>> expected = {{{1, 0}, {0, 1}},
                                          {{1, 0}, {2, 1}},
                                          {{0, 1}, {2, 0}},
                                          {{2, 0}, {1, 1}}};
        generator<map<int, int>> result = enum_maximum_matchings_iter(bg);
        REQUIRE(result.size() == expected.size());
    }

    SECTION("Test 2")
    {
        map<tuple<int, int>, bool> m
            = {{{0, 0}, true}, {{1, 1}, true}, {{2, 0}, true}};
        BipartiteGraph<int, int, bool> bg(m);

        vector<map<int, int>> expected = {{{0, 0}, {1, 1}}, {{1, 1}, {2, 0}}};
        generator<map<int, int>> result = enum_maximum_matchings_iter(bg);
        REQUIRE(result.size() == expected.size());
    }

    SECTION("Test 3")
    {
        map<tuple<int, int>, bool> m
            = {{{0, 0}, true}, {{1, 1}, true}, {{2, 0}, true}, {{2, 1}, true}};
        BipartiteGraph<int, int, bool> bg(m);

        vector<map<int, int>> expected
            = {{{0, 0}, {1, 1}}, {{1, 1}, {2, 0}}, {{0, 0}, {2, 1}}};
        generator<map<int, int>> result = enum_maximum_matchings_iter(bg);
        REQUIRE(result.size() == expected.size());
    }

    SECTION("Test 4")
    {
        map<tuple<int, int>, bool> m = {};
        BipartiteGraph<int, int, bool> bg(m);

        generator<map<int, int>> result = enum_maximum_matchings_iter(bg);
        REQUIRE(result.empty());
    }
}
