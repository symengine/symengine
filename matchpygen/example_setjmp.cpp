#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <deque>
#include <functional>
#include <setjmp.h>

#include <symengine/basic.h>
#include <symengine/pow.h>

using namespace std;
using namespace SymEngine;

typedef map<string, const Basic *> Substitution2;

int try_add_variable(Substitution2 &subst, string variable_name,
                     const Basic *replacement)
{
    if (subst.find(variable_name) == subst.end()) {
        subst[variable_name] = replacement;
    } else {
    }
    return 0;
}

class Deque
{
public:
    Deque()
    {
        N = 0;
    }
    const Basic *front()
    {
        return _deque[N - 1];
    }
    void pop_front()
    {
        N--;
    }
    void push_front(const Basic *obj)
    {
        N++;
        _deque[N - 1] = obj;
    }
    void push_back(const Basic *obj)
    {
        for (int i = N; i > 0; i--) {
            _deque[N] = _deque[N - 1];
        }
        _deque[0] = obj;
        N++;
    }
    int size()
    {
        return N;
    }
    const Basic *operator[](int i)
    {
        return _deque[N - i - 1];
    }

private:
    const Basic *_deque[100];
    int N;
};

Deque get_deque(const Basic *expr)
{
    Deque d;
    for (RCP<const Basic> i : expr->get_args()) {
        d.push_back(i.access_private_ptr());
    }
    return d;
}

#define YIELDABLE                                                              \
    if (yielded) {                                                             \
        longjmp(childTask, 1);                                                 \
    } else {                                                                   \
        yielded = true;                                                        \
    }

#define yield(val)                                                             \
    if (!setjmp(childTask)) {                                                  \
        value = (val);                                                         \
        return;                                                                \
    }

template <typename T>
class Yielder
{
public:
    Yielder() : finished(false), yielded(false)
    {
    }

    virtual ~Yielder()
    {
    }

    T _next()
    {
        run();
        return value;
    }

    T next()
    {
        T v = _next();
        if (finished) {
            throw(-1);
        }
        return v;
    }

    std::vector<T> tovector()
    {
        if (yielded)
            throw("iterator already started");
        std::vector<T> vec;
        while (true) {
            T v = _next();
            if (finished) {
                break;
            }
            vec.push_back(v);
        }
        return vec;
    }

    void stop()
    {
        finished = true;
    }

    virtual void run() = 0;

protected:
    jmp_buf childTask;
    bool finished;
    bool yielded;
    T value;
};

class match_root : public Yielder<tuple<int, Substitution2>>
{
public:
    match_root(Basic *subject) : subject(subject)
    {
    }
    match_root(RCP<const Basic> subject)
    {
        this->subject = subject.access_private_ptr();
    }
    void run()
    {

        YIELDABLE

        subjects.push_back(subject);
        // state 2200;
        if (subjects.size() >= 1 && is_a<Pow>(*subjects[0])) {
            tmp1 = subjects.front();
            subjects.pop_front();
            subjects2 = get_deque(tmp1);
            // state 2201;
            if (subjects2.size() >= 1 && subjects2[0]->__eq__(*symbol("x"))) {
                tmp3 = subjects2.front();
                subjects2.pop_front();
                // state 2202;
                if (subjects2.size() >= 1) {
                    tmp4 = subjects2.front();
                    subjects2.pop_front();
                    Substitution2 subst1(subst0);
                    if (!try_add_variable(subst1, "i2", tmp4)) {
                        // state 2203;
                        if (subjects2.size() == 0) {
                            // state 2204;
                            if (subjects.size() == 0) {
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
            tmp6 = subjects.front();
            subjects.pop_front();
            // state 2205;
            if (subjects.size() == 0) {
                // 1: y;
                yield(make_tuple(1, subst0));
            }
            subjects.push_front(tmp6);
        }
        yield(make_tuple(-1, subst0));
        stop();
    }

private:
    const Basic *subject;

    Deque subjects;
    Deque subjects2;
    Substitution2 subst0;
    Substitution2 tmp_subst;
    const Basic *tmp1;
    const Basic *tmp2;
    const Basic *tmp3;
    const Basic *tmp4;
    const Basic *tmp5;
    const Basic *tmp6;
};

class generator : public Yielder<int>
{
public:
    void run()
    {
        YIELDABLE
        for (i = 0; i < 10; ++i) {
            yield(i);
        }
        stop();
    }
    int i;
};

int main(int argc, char *argv[])
{
    generator g;
    cout << "Size " << g.tovector().size() << endl;

    // Allowed patterns: x^w, y. w is a wildcard.
    RCP<const Basic> expr;
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> i3 = integer(3);
    expr = pow(x, i3);
    match_root mr1(expr);
    auto result = mr1.next();

    RCP<const Basic> y = symbol("y");
    std::cout << "Result for " << expr->__str__() << " is " << get<0>(result)
              << std::endl;
    result = match_root(y).next();
    std::cout << "Result for " << y->__str__() << " is " << get<0>(result)
              << std::endl;
    result = match_root(x).next();
    std::cout << "Result for " << x->__str__() << " is " << get<0>(result)
              << std::endl;

    // Test `.tovector()`:
    vector<tuple<int, Substitution2>> vec;
    vec = match_root(expr).tovector();
    std::cout << "Vector:\n";
    for (const tuple<int, Substitution2> &p : vec) {
        std::cout << "pair::first\t" << get<0>(p) << std::endl;
    }
}
