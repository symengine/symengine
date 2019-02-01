/*
 * This file was automatically generated: DO NOT EDIT.
 *
 * Decision tree matching expressions ['x']
 */
#include "catch.hpp"
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <symengine/basic.h>
#include <symengine/pow.h>
#include <symengine/utilities/matchpycpp/common.h>
#include <symengine/utilities/matchpycpp/substitution.h>
#include <tuple>

generator<tuple<int, Substitution>> match_root(RCP<const Basic> subject)
{
    generator<tuple<int, Substitution>> result;
    Deque subjects;
    subjects.push_front(subject);
    Substitution subst0;
    // State 2194
    if (subjects.size() >= 1 && subjects[0]->__eq__(*x)) {
        RCP<const Basic> tmp1 = subjects.front();
        subjects.pop_front();
        // State 2195
        if (subjects.size() == 0) {
            // 0: x
            result.push_back(make_tuple(0, subst0));
        }
        subjects.push_front(tmp1);
    }
    return result;
}

TEST_CASE("GeneratedMatchPyTest0", "")
{
    generator<tuple<int, Substitution>> ret;
    Substitution substitution;

    // Pattern x matching x with substitution {}:
    ret = match_root(x);
    substitution = get<1>(ret[0]);
    REQUIRE(ret.size() > 0);
    REQUIRE(get<0>(ret[0]) == 0);

    // Pattern y not matching:
    ret = match_root(y);
    REQUIRE(ret.size() == 0);
}
