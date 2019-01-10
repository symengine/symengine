#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <deque>

#include <symengine/basic.h>
#include <symengine/pow.h>

#include "coroutine.h"

using namespace std;
using namespace SymEngine;

typedef map<string, RCP<const Basic>> Substitution2;

int try_add_variable(Substitution2 &subst, string variable_name,
                     RCP<const Basic> &replacement)
{
    if (subst.find(variable_name) == subst.end()) {
        subst[variable_name] = replacement;
    } else {
    }
    return 0;
}

typedef deque<RCP<const Basic>> Deque;

Deque get_deque(RCP<const Basic> expr)
{
    Deque d;
    for (RCP<const Basic> i : expr->get_args()) {
        d.push_back(i);
    }
    return d;
}

coroutine::Channel<tuple<int, Substitution2>> channel;


void yield(tuple<int, Substitution2> val)
{
	channel.push(val);
	coroutine::yield();
}

void match_root(RCP<const Basic> subject)
{
    Deque subjects;
    subjects.push_back(subject);
    Substitution2 subst0;
    // state 2200;
    if (subjects.size() >= 1 && is_a<Pow>(*subjects[0])) {
        RCP<const Basic> tmp1 = subjects.front();
        subjects.pop_front();
        Deque subjects2 = get_deque(tmp1);
        // state 2201;
        if (subjects2.size() >= 1 && subjects2[0]->__eq__(*symbol("x"))) {
            RCP<const Basic> tmp3 = subjects2.front();
            subjects2.pop_front();
            // state 2202;
            if (subjects2.size() >= 1) {
                auto tmp4 = subjects2.front();
                subjects2.pop_front();
                Substitution2 subst1(subst0);
                if (!try_add_variable(subst1, "i2", tmp4)) {
                    // state 2203;
                    if (subjects2.size() == 0) {
                        // state 2204;
                        if (subjects.size() == 0) {
                            Substitution2 tmp_subst;
                            tmp_subst["w"] = subst1["i2"];
                            // 0: x**w;
                            yield(make_tuple(0, tmp_subst));
                        }
                    }
                }
                subjects2.push_front(tmp4);
            }
            subjects2.push_front(tmp3);
        }
        subjects.push_front(tmp1);
    }
    if (subjects.size() >= 1 && subjects[0]->__eq__(*symbol("y"))) {
        auto &tmp6 = subjects.front();
        subjects.pop_front();
        // state 2205;
        if (subjects.size() == 0) {
            // 1: y;
            yield(make_tuple(1, subst0));
        }
        subjects.push_front(tmp6);
    }
    yield(make_tuple(-1, subst0));
};

int main(int argc, char *argv[])
{
    // Allowed patterns: x^w, y. w is a wildcard.
    RCP<const Basic> expr;
    RCP<const Basic> x = symbol("x");

    expr = pow(x, integer(3));
    coroutine::routine_t mr1 = coroutine::create(std::bind(match_root, expr));
    coroutine::resume(mr1);
    auto result = channel.pop();
    coroutine::destroy(mr1);

    RCP<const Basic> y = symbol("y");
    std::cout << "Result for " << expr->__str__() << " is " << get<0>(result)
              << std::endl;
    mr1 = coroutine::create(std::bind(match_root, y));
    coroutine::resume(mr1);
    result = channel.pop();
    coroutine::destroy(mr1);

    std::cout << "Result for " << y->__str__() << " is " << get<0>(result)
              << std::endl;
    mr1 = coroutine::create(std::bind(match_root, x));
    coroutine::resume(mr1);
    result = channel.pop();
    coroutine::destroy(mr1);
    std::cout << "Result for " << x->__str__() << " is " << get<0>(result)
              << std::endl;
}
