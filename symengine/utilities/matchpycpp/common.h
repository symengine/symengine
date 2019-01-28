#ifndef SYMENGINE_UTILITIES_MATCHPYCPP_COMMON_H_
#define SYMENGINE_UTILITIES_MATCHPYCPP_COMMON_H_

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/mul.h>
#include <map>
#include <string>
#include <queue>

#include <setjmp.h>

#include "substitution.h"

using namespace std;
using namespace SymEngine;

template <typename T>
using generator = vector<T>;

// Assuming TLeft = TRight, otherwise
// Node should be tuple<int, variant<TLeft, TRight>>
#define TYPES_DERIVED_FROM_TLEFT_TRIGHT                                        \
    typedef tuple<int, TLeft> Node;                                            \
    typedef vector<Node> NodeList;                                             \
    typedef set<Node> NodeSet;                                                 \
    typedef tuple<TLeft, TRight> Edge;

const int LEFT = 0;
const int RIGHT = 1;

typedef vector<Substitution> TEdgeValue;


typedef deque<RCP<const Basic>> Deque;

Deque get_deque(RCP<const Basic> expr)
{
    Deque d;
    for (RCP<const Basic> i : expr->get_args()) {
        d.push_back(i);
    }
    return d;
}

RCP<const Basic> x = symbol("x");
RCP<const Basic> y = symbol("y");
RCP<const Basic> z = symbol("z");
// RCP<const Basic> w = symbol("w");

#endif /* SYMENGINE_UTILITIES_MATCHPYCPP_COMMON_H_ */
