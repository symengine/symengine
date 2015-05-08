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

namespace SymEngine {

class EvalDoubleVisitor : public BaseVisitor<EvalDoubleVisitor> {
private:
    /*
       The 'result_' variable is assigned into at the very end of each visit()
       methods below. The only place where these methods are called from is the
       line 'b.accept(*this)' in apply() and the 'result_' is immediately
       returned. Thus no corruption can happen and apply() can be safely called
       recursively.
    */
    double result_;
public:
    EvalDoubleVisitor() : BaseVisitor(this) { };
    double apply(const Basic &b) {
        b.accept(*this);
        return result_;
    }

    void bvisit(const Integer &x) {
        double tmp = x.i.get_d();
        result_ = tmp;
    }

    void bvisit(const Rational &x) {
        double tmp = x.i.get_d();
        result_ = tmp;
    }

    void bvisit(const Add &x) {
        double tmp = 0;
        for (auto &p: x.get_args()) tmp = tmp + apply(*p);
        result_ = tmp;
    }

    void bvisit(const Mul &x) {
        double tmp = 1;
        for (auto &p: x.get_args()) tmp = tmp * apply(*p);
        result_ = tmp;
    }

    void bvisit(const Pow &x) {
        double exp_ = apply(*(x.get_exp()));
        if (eq(x.get_base(), E)) {
            result_ = ::exp(exp_);
        } else {
            double base_ = apply(*(x.get_base()));
            result_ = ::pow(base_, exp_);
        }
    }

    void bvisit(const Sin &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::sin(tmp);
    }

    void bvisit(const Cos &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::cos(tmp);
    }

    void bvisit(const Tan &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::tan(tmp);
    }

    void bvisit(const Symbol &) {
        throw std::runtime_error("Symbol cannot be evaluated as a double.");
    };

    void bvisit(const Log &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::log(tmp);
    };

    void bvisit(const Cot &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = 1/::tan(tmp);
    };

    void bvisit(const Csc &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = 1/::cos(tmp);
    };

    void bvisit(const Sec &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = 1/::sin(tmp);
    };

    void bvisit(const ASin &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::asin(tmp);
    };

    void bvisit(const ACos &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::acos(tmp);
    };

    void bvisit(const ASec &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::asin(1/tmp);
    };

    void bvisit(const ACsc &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::acos(1/tmp);
    };

    void bvisit(const ATan &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::atan(tmp);
    };

    void bvisit(const ACot &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::atan(1/tmp);
    };

    void bvisit(const ATan2 &x) {
        double num = apply(*(x.get_num()));
        double den = apply(*(x.get_den()));
        result_ = ::atan2(num, den);
    };

    void bvisit(const Sinh &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::sinh(tmp);
    };

    void bvisit(const Cosh &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::cosh(tmp);
    };

    void bvisit(const Tanh &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::tanh(tmp);
    };

    void bvisit(const Coth &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = 1/::tanh(tmp);
    };

    void bvisit(const ASinh &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::asinh(tmp);
    };

    void bvisit(const ACosh &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::acosh(tmp);
    };

    void bvisit(const ATanh &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::atanh(tmp);
    };

    void bvisit(const ACoth &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::atanh(1/tmp);
    };

    void bvisit(const ASech &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::acosh(1/tmp);
    };

    void bvisit(const Gamma &x) {
        double tmp = apply(*(x.get_args()[0]));
        result_ = ::tgamma(tmp);
    };

    void bvisit(const Constant &x) {
        if (x.__eq__(*pi)) {
            result_ = ::atan2(0, -1);
        } else if (x.__eq__(*E)) {
            result_ = ::exp(1);
        } else {
            throw std::runtime_error("Constant " + x.get_name() + " is not implemented.");
        }
    };

    void bvisit(const Abs &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = std::abs(tmp);
    };

    // Classes not implemented are
    // Subs, UpperGamma, LowerGamma, Dirichlet_eta, Zeta
    // LeviCivita, KroneckerDelta, FunctionSymbol, LambertW
    // Derivative, Complex
    void bvisit(const Basic &) {
        throw std::runtime_error("Not implemented.");
    };
};

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
    table[LOG] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Log &>(x)).get_arg());
        return ::log(tmp);
    };
    table[COT] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Cot &>(x)).get_arg());
        return 1/::tan(tmp);
    };
    table[CSC] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Csc &>(x)).get_arg());
        return 1/::cos(tmp);
    };
    table[SEC] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Sec &>(x)).get_arg());
        return 1/::sin(tmp);
    };
    table[ASIN] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ASin &>(x)).get_arg());
        return ::asin(tmp);
    };
    table[ACOS] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ACos &>(x)).get_arg());
        return ::acos(tmp);
    };
    table[ASEC]= [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ASec &>(x)).get_arg());
        return ::asin(1/tmp);
    };
    table[ACSC] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ACsc &>(x)).get_arg());
        return ::acos(1/tmp);
    };
    table[ATAN] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ATan &>(x)).get_arg());
        return ::atan(tmp);
    };
    table[ACOT] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ACot &>(x)).get_arg());
        return ::atan(1/tmp);
    };
    table[ATAN2] = [](const Basic &x) {
        double num = eval_double(*(static_cast<const ATan2 &>(x)).get_num());
        double den = eval_double(*(static_cast<const ATan2 &>(x)).get_num());
        return ::atan2(num, den);
    };
    table[ACOT] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ACot &>(x)).get_arg());
        return ::atan(1/tmp);
    };
    table[SINH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Sinh &>(x)).get_arg());
        return ::sinh(tmp);
    };
    table[COSH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Cosh &>(x)).get_arg());
        return ::cosh(tmp);
    };
    table[TANH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Tanh &>(x)).get_arg());
        return ::tanh(tmp);
    };
    table[COTH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Coth &>(x)).get_arg());
        return 1/::tanh(tmp);
    };
    table[ASINH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ASinh &>(x)).get_arg());
        return ::asinh(tmp);
    };
    table[ACOSH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ACosh &>(x)).get_arg());
        return ::acosh(tmp);
    };
    table[ATANH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ATanh &>(x)).get_arg());
        return ::atanh(tmp);
    };
    table[ACOTH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ACoth &>(x)).get_arg());
        return ::atanh(1/tmp);
    };
    table[ASECH] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const ASech &>(x)).get_arg());
        return ::acosh(1/tmp);
    };
    table[GAMMA] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Gamma &>(x)).get_args()[0]);
        return ::tgamma(tmp);
    };
    table[CONSTANT] = [](const Basic &x) {
        if (x.__eq__(*pi)) {
            return ::atan2(0, -1);
        } else if (x.__eq__(*E)) {
            return ::exp(1);
        } else {
            throw std::runtime_error("Constant " + static_cast<const Constant &>(x).get_name() + " is not implemented.");
        }
    };
    table[ABS] = [](const Basic &x) {
        double tmp = eval_double(*(static_cast<const Abs &>(x)).get_arg());
        return std::abs(tmp);
    };
    return table;
}

const static std::vector<fn> table_eval_double = init_eval_double();

double eval_double(const Basic &b) {
    EvalDoubleVisitor v;
    return v.apply(b);
}

double eval_double_single_dispatch(const Basic &b) {
    return table_eval_double[b.get_type_code()](b);
}

} // SymEngine
