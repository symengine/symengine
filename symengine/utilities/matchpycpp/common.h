#ifndef COMMON_H
#define COMMON_H

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

/*
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
*/

typedef deque<RCP<const Basic>> Deque;


Deque get_deque(RCP<const Basic> expr)
{
    Deque d;
    for (RCP<const Basic> i : expr->get_args()) {
        d.push_back(i);
    }
    return d;
}


/*
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
*/

RCP<const Basic> x = symbol("x");
RCP<const Basic> y = symbol("y");
RCP<const Basic> z = symbol("z");
// RCP<const Basic> w = symbol("w");

#endif
