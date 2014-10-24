#include <cmath>

#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "visitor.h"
#include "eval_double.h"

namespace CSymPy {

void EvalDoubleVisitor::visit(const Integer &x)
{
    double tmp = x.i.get_d();
    result_ = tmp;
}

void EvalDoubleVisitor::visit(const Rational &x)
{
    double tmp = x.i.get_d();
    result_ = tmp;
}

void EvalDoubleVisitor::visit(const Add &x)
{
    double tmp = 0;
    for (auto &p: x.get_args()) tmp = tmp + eval_double(*p);
    result_ = tmp;
}

void EvalDoubleVisitor::visit(const Mul &x)
{
    double tmp = 1;
    for (auto &p: x.get_args()) tmp = tmp * eval_double(*p);
    result_ = tmp;
}

void EvalDoubleVisitor::visit(const Pow &x)
{
    double a = eval_double(*(x.base_));
    double b = eval_double(*(x.exp_));
    result_ = ::pow(a, b);
}

void EvalDoubleVisitor::visit(const Sin &x)
{
    double tmp = eval_double(*(x.get_arg()));
    result_ = ::sin(tmp);
}

void EvalDoubleVisitor::visit(const Cos &x)
{
    double tmp = eval_double(*(x.get_arg()));
    result_ = ::cos(tmp);
}

void EvalDoubleVisitor::visit(const Tan &x)
{
    double tmp = eval_double(*(x.get_arg()));
    result_ = ::tan(tmp);
}

double eval_double(const Basic &b)
{
    EvalDoubleVisitor v;
    return v.apply(b);
}

} // CSymPy
