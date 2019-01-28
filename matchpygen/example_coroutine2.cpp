#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <deque>
#include <functional>

#include <symengine/basic.h>
#include <symengine/pow.h>

#include "coroutine.h"

using namespace SymEngine;

typedef std::map<std::string, RCP<const Basic>> Substitution2;

int try_add_variable(Substitution2 &subst, string variable_name,
                     RCP<const Basic> &replacement)
{
    if (subst.find(variable_name) == subst.end()) {
        subst[variable_name] = replacement;
    } else {
    }
    return 0;
}

typedef std::deque<RCP<const Basic>> Deque;

Deque get_deque(RCP<const Basic>& expr)
{
    Deque d;
    for (RCP<const Basic> i : expr->get_args()) {
        d.push_back(i);
    }
    return d;
}

template <typename T>
class Yielder
{
public:
    Yielder()
    {
    }
    virtual ~Yielder()
    {
        std::cout << "print destructor" << std::endl;
        coroutine::destroy(routine);
    }

    T next()
    {
        coroutine::resume(routine);
        return std::make_tuple(value_int, value_subst);
    }

protected:
    void yield(int value_int_, Substitution2& value_subst_)
    {
        value_int = value_int_;
        value_subst = value_subst_;
        coroutine::yield();
    }
    coroutine::routine_t routine;
    int value_int;
    Substitution2 value_subst;
};

class match_root : public Yielder<std::tuple<int, Substitution2>>
{
public:
    RCP<const Basic> subject;
    Deque subjects, subjects2;
    RCP<const Basic> tmp1, tmp3, tmp4, tmp6;
    Substitution2 subst0, subst1, tmp_subst;

    match_root(RCP<const Basic>& subject_) : subject(subject_)
    {
        routine = coroutine::create(std::bind(&match_root::run, this));
    }

    void run()
    {
        subjects.push_back(subject);
        // state 2200;
        if (subjects.size() >= 1 && is_a<Pow>(*subjects[0])) {
            tmp1 = subjects.front();
            subjects.pop_front();
            subjects2 = get_deque(tmp1);
            // state 2201;
            if (subjects2.size() >= 1 && eq(*subjects2[0], *symbol("x"))) {
                tmp3 = subjects2.front();
                subjects2.pop_front();
                // state 2202;
                if (subjects2.size() >= 1) {
                    tmp4 = subjects2.front();
                    subjects2.pop_front();
                    subst1 = subst0;
                    if (!try_add_variable(subst1, "i2", tmp4)) {
                        // state 2203;
                        if (subjects2.size() == 0) {
                            // state 2204;
                            if (subjects.size() == 0) {
                                tmp_subst["w"] = symbol("x");
                                tmp_subst["w2"] = symbol("y");
                                // 0: x**w;
                                yield(0, tmp_subst);
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
            tmp6 = subjects.front();
            subjects.pop_front();
            // state 2205;
            if (subjects.size() == 0) {
                // 1: y;
                yield(1, subst0);
            }
            subjects.push_front(tmp6);
        }
        yield(-1, subst0);
    }
};

int main(int argc, char *argv[])
{
    // Allowed patterns: x^w, y. w is a wildcard.
    RCP<const Basic> expr;
    RCP<const Basic> x = symbol("x");
    expr = pow(x, integer(3));
    match_root mr1(expr);
    auto result = mr1.next();

    RCP<const Basic> y = symbol("y");
    std::cout << "Result for " << expr->__str__() << " is " << std::get<0>(result)
              << std::endl;
    result = match_root(y).next();
    std::cout << "Result for " << y->__str__() << " is " << std::get<0>(result)
              << std::endl;
    result = match_root(x).next();
    std::cout << "Result for " << x->__str__() << " is " << std::get<0>(result)
              << std::endl;
}
