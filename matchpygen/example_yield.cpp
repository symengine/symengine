#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <deque>
#include <functional>

#include <symengine/basic.h>
#include <symengine/pow.h>

using namespace std;
using namespace SymEngine;

typedef map<string, RCP<const Basic>> Substitution2;

int try_add_variable(Substitution2 &subst, string variable_name,
                     RCP<const Basic> &replacement)
{
    if (subst.find(variable_name) == subst.end()) {
        subst[variable_name] = replacement;
        // this[variable_name] = replacement; //.copy() if
        // isinstance(replacement, Multiset) else replacement
    } else {
        // Node &existing_value = *(this->find(variable_name));
        /*
         if (isinstance(existing_value, tuple)) {
         if (isinstance(replacement, Multiset)) {
         if (Multiset(existing_value) != replacement){
         return -1;
         }
         } else if (replacement != existing_value) {
         return -1;
         }
         else if (isinstance(existing_value, Multiset)) {
         if (!isinstance(replacement, (tuple, list, Multiset))) {
         return -1;
         }
         auto &compare_value = Multiset(replacement);
         if (existing_value == compare_value){
         if (! isinstance(replacement, Multiset)) {
         this->insert(make_pair(variable_name, replacement));
         } else {
         return -1;
         }
         } else if (replacement != existing_value){
         return -1;
         }
         }
         }
         */
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

class match_root
{
public:
    typedef tuple<int, Substitution2> YieldType;
    void (match_root::*current)();
    Deque subjects;
    Deque subjects2;
    Substitution2 subst0;
    Substitution2 subst1;
    RCP<const Basic> tmp1;
    RCP<const Basic> tmp3;
    RCP<const Basic> tmp4;
    RCP<const Basic> tmp6;
    Substitution2 tmp_subst;

    deque<tuple<int, Substitution2>> yield_queue;

    void yield(int a, Substitution2 b)
    {
        yield_queue.push_back(make_tuple(a, b));
    }

    shared_ptr<YieldType> next()
    {
        while (true) {
            if (current == NULL) {
                break;
            }
            (*this.*current)();
            if (yield_queue.size() > 0) {
                shared_ptr<YieldType> front
                    = make_shared<YieldType>(yield_queue.front());
                yield_queue.pop_front();
                return front;
            }
        }
        return NULL;
    }

    match_root(RCP<const Basic> &subject)
    {
        subjects.push_back(subject);
        current = &match_root::state2200;
    }

    void state2200()
    {
        // state 2200;
        cout << "State 2200" << endl;
        if (subjects.size() >= 1 && is_a<Pow>(*subjects[0])) {
            tmp1 = subjects.front();
            subjects.pop_front();
            subjects2 = get_deque(tmp1);
            current = &match_root::state2201;
        } else {
            current = &match_root::state2200part003;
        }
    }

    void state2201()
    {
        // state 2201;
        cout << "State 2201" << endl;
        if (subjects2.size() >= 1 && subjects2[0]->__eq__(*symbol("x"))) {
            tmp3 = subjects2.front();
            subjects2.pop_front();
            current = &match_root::state2202;
        } else {
            current = &match_root::state2201part002;
        }
    }

    void state2201part002()
    {
        subjects2.push_front(tmp3);
        current = &match_root::state2200part002;
    }

    void state2202()
    {
        // state 2202;
        cout << "State 2202" << endl;
        if (subjects2.size() >= 1) {
            tmp4 = subjects2.front();
            subjects2.pop_front();
            subst1 = subst0;
            if (!try_add_variable(subst1, "i2", tmp4)) {
                current = &match_root::state2203;
            }
        } else {
            current = &match_root::state2202part002;
        }
    }

    void state2202part002()
    {
        subjects2.push_front(tmp4);
        current = &match_root::state2201part002;
    }

    void state2203()
    {
        // state 2203;
        cout << "State 2203" << endl;
        if (subjects2.size() == 0) {
            current = &match_root::state2204;
        } else {
            current = &match_root::state2202part002;
        }
    }

    void state2204()
    {
        // state 2204;
        cout << "State 2204" << endl;
        if (subjects.size() == 0) {
            tmp_subst["w"] = subst1["i2"];
            // 0: x**w;
            yield(0, tmp_subst);
        }
        current = &match_root::state2202part002;
    }

    void state2200part002()
    {
        subjects.push_front(tmp1);
        current = &match_root::state2200part003;
    }

    void state2200part003()
    {
        if (subjects.size() >= 1 && subjects[0]->__eq__(*symbol("y"))) {
            tmp6 = subjects.front();
            subjects.pop_front();
            current = &match_root::state2205;
        } else {
            current = &match_root::state2200part004;
        }
    }

    void state2205()
    {
        // state 2205;
        cout << "State 2205" << endl;
        if (subjects.size() == 0) {
            // 1: y;
            yield(1, subst0);
        }
        current = &match_root::state2205part002;
    }

    void state2205part002()
    {
        subjects.push_front(tmp6);
        current = &match_root::state2200part002;
    }

    void state2200part004()
    {
        current = NULL;
    }
};

int main(int argc, char *argv[])
{
    // Allowed patterns: x^w, y. w is a wildcard.
    RCP<const Basic> expr;
    RCP<const Basic> x = symbol("x");
    expr = pow(x, integer(3));
    RCP<const Basic> y = symbol("y");

    match_root matcher(expr);

    shared_ptr<tuple<int, Substitution2>> result = matcher.next();
    std::cout << "Result for " << expr->__str__() << " is " << get<0>(*result)
              << std::endl;

    matcher = match_root(y);
    result = matcher.next();
    std::cout << "Result for " << y->__str__() << " is " << get<0>(*result)
              << std::endl;

    matcher = match_root(x);
    result = matcher.next();
    if (result == NULL) {
        std::cout << "Result for " << x->__str__() << " is NULL" << std::endl;
    }
}
