#include "catch.hpp"
#include <iostream>

#include <symengine/basic.h>
#include <symengine/fields.h>
#include <symengine/symengine_rcp.h>

using SymEngine::RCP;
using SymEngine::GaloisField;
using SymEngine::gf;

TEST_CASE("Interval : Basic", "[basic]")
{
    RCP<const GaloisField> r1, r2, r3;
    std::vector<int> a = {2, 3, 4};
    std::vector<int> b = {3, 3, 6, 6};
    r1 = gf(a, 5);
    r2 = gf(b, 5);
    unsigned int k = 0;
    std::cout << *r1 << "\n";
    std::map<unsigned, int> mp = r1->dict_;
    REQUIRE(mp[0] == 2);
    REQUIRE(mp[1] == 3);
    REQUIRE(mp[2] == 4);
    std::cout << *r2 << "\n";
    mp = r2->dict_;
    REQUIRE(mp[0] == 3);
    REQUIRE(mp[1] == 3);
    REQUIRE(mp[2] == 1);
    REQUIRE(mp[3] == 1);
    r3 = r1->gf_add(r2);
    mp = r3->dict_;
    REQUIRE(mp[1] == 1);
    REQUIRE(mp[3] == 1);
    r3 = r1->gf_sub(r2);
    mp = r3->dict_;
    REQUIRE(mp[0] == 4);
    REQUIRE(mp[2] == 3);
    REQUIRE(mp[3] == 4);
    r3 = r1->gf_mul(r2);
    mp = r3->dict_;
    REQUIRE(mp[0] == 1);
    REQUIRE(mp[2] == 3);
    REQUIRE(mp[3] == 2);
    REQUIRE(mp[4] == 2);
    REQUIRE(mp[5] == 4);
}
