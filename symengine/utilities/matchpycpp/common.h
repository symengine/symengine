#ifndef COMMON_H
#define COMMON_H

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/mul.h>
#include <map>
#include <string>
#include <queue>

using namespace std;
using namespace SymEngine;

typedef map<string, RCP<const Basic>> Substitution;
typedef deque<RCP<const Basic>> Deque;

int try_add_variable(Substitution &subst, string variable_name,
                     RCP<const Basic> &replacement)
{
    if (subst.find(variable_name) == subst.end()) {
        subst[variable_name] = replacement;
    } else {
    }
    return 0;
}

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
//RCP<const Basic> w = symbol("w");

#endif
