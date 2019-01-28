#include "catch.hpp"
#include <chrono>
#include <string>
#include <iostream>
#include <symengine/utilities/matchpycpp/hopcroft_karp.h>

using namespace std;


TEST_CASE("Hopcroft Karp algorithm", "Testing the implementation of the Hopcroft Karp algorithm.")
{
	SECTION("Test 1") {
		map<int, set<string>> graph = {{0, {"v0", "v1"}}, {1, {"v0", "v4"}}, {2, {"v2", "v3"}}, {3, {"v0", "v4"}}, {4, {"v0", "v3"}}};
		map<int, string> expected = {{0, "v1"}, {1, "v4"}, {2, "v2"}, {3, "v0"}, {4, "v3"}};
		HopcroftKarp<int, string> hk(graph);
		int matchings = hk.hopcroft_karp();
		REQUIRE(hk.pair_left == expected);
		REQUIRE(matchings == 5);
	}
}
