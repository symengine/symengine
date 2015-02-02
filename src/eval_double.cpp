#include <cmath>
#include <functional>


#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "constants.h"
#include "visitor.h"
#include "eval_double.h"

namespace CSymPy {

/*
 * These two seem to be equivalent and about the same fast.
*/
//typedef double (*fn)(const Basic &);
typedef std::function<double(const Basic &)> fn;

std::vector<fn> init_eval_double()
{
    std::vector<fn> table;
    table.assign(TypeID_Count, [](const Basic &x) -> double {
        throw std::runtime_error("Not implemented.");
    });
    table[INTEGER] = [](const Basic &x) {
        double tmp = (static_cast<const Integer &>(x)).i.get_d();
        return tmp;
    };
    table[RATIONAL] = [](const Basic &x) {
        double tmp = (static_cast<const Rational &>(x)).i.get_d();
        return tmp;
    };
    table[ADD] = [](const Basic &x) {
        double tmp = 0;
        for (auto &p: x.get_args()) tmp = tmp + eval_double(*p);
        return tmp;
    };
    table[MUL] = [](const Basic &x) {
        double tmp = 1;
        for (auto &p: x.get_args()) tmp = tmp * eval_double(*p);
        return tmp;
    };
    table[POW] = [](const Basic &x) {
        double a = eval_double(*(static_cast<const Pow &>(x)).base_);
        double b = eval_double(*(static_cast<const Pow &>(x)).exp_);
        return ::pow(a, b);
    };
    table[SIN] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Sin &>(x)).get_arg());
        return ::sin(tmp);
    };
    table[COS] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Cos &>(x)).get_arg());
        return ::cos(tmp);
    };
    table[TAN] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Tan &>(x)).get_arg());
        return ::tan(tmp);
    };
    return table;
}

const static std::vector<fn> table_eval_double = init_eval_double();

double eval_double(const Basic &b)
{
    return table_eval_double[b.get_type_code()](b);
}

} // CSymPy
