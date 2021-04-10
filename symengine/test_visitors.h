#ifndef SYMENGINE_TEST_VISITORS_H
#define SYMENGINE_TEST_VISITORS_H

#include <symengine/visitor.h>

namespace SymEngine
{

class ZeroVisitor : public BaseVisitor<ZeroVisitor>
{
private:
    tribool is_zero_;

public:
    void bvisit(const Basic &x)
    {
        is_zero_ = tribool::indeterminate;
    };
    void bvisit(const Symbol &x)
    {
        is_zero_ = tribool::indeterminate;
    };
    void bvisit(const Number &x);
    void bvisit(const Set &x)
    {
        is_zero_ = tribool::trifalse;
    };
    void bvisit(const Relational &x)
    {
        is_zero_ = tribool::trifalse;
    };
    void bvisit(const Boolean &x)
    {
        is_zero_ = tribool::trifalse;
    };
    void bvisit(const Constant &x)
    {
        is_zero_ = tribool::trifalse;
    };

    tribool apply(const Basic &b);
};

class PositiveVisitor : public BaseVisitor<PositiveVisitor>
{
private:
    tribool is_positive_;

public:
    void bvisit(const Symbol &x)
    {
        is_positive_ = tribool::indeterminate;
    };
    void bvisit(const Number &x);
    void bvisit(const Constant &x)
    {
        is_positive_ = tribool::tritrue;
    };
    void bvisit(const Basic &x)
    {
        is_positive_ = tribool::indeterminate;
    };
    void bvisit(const Set &x)
    {
        is_positive_ = tribool::trifalse;
    };
    void bvisit(const Relational &x)
    {
        is_positive_ = tribool::trifalse;
    };
    void bvisit(const Boolean &x)
    {
        is_positive_ = tribool::trifalse;
    };

    tribool apply(const Basic &b);
};

class NonPositiveVisitor : public BaseVisitor<NonPositiveVisitor>
{
private:
    tribool is_nonpositive_;

public:
    void bvisit(const Symbol &x)
    {
        is_nonpositive_ = tribool::indeterminate;
    };
    void bvisit(const Number &x);
    void bvisit(const Constant &x)
    {
        is_nonpositive_ = tribool::trifalse;
    };
    void bvisit(const Basic &x)
    {
        is_nonpositive_ = tribool::indeterminate;
    };
    void bvisit(const Set &x)
    {
        is_nonpositive_ = tribool::trifalse;
    };
    void bvisit(const Relational &x)
    {
        is_nonpositive_ = tribool::trifalse;
    };
    void bvisit(const Boolean &x)
    {
        is_nonpositive_ = tribool::trifalse;
    };

    tribool apply(const Basic &b);
};

class NegativeVisitor : public BaseVisitor<NegativeVisitor>
{
private:
    tribool is_negative_;

public:
    void bvisit(const Basic &x)
    {
        is_negative_ = tribool::indeterminate;
    };
    void bvisit(const Symbol &x)
    {
        is_negative_ = tribool::indeterminate;
    };
    void bvisit(const Number &x);
    void bvisit(const Set &x)
    {
        is_negative_ = tribool::trifalse;
    };
    void bvisit(const Relational &x)
    {
        is_negative_ = tribool::trifalse;
    };
    void bvisit(const Boolean &x)
    {
        is_negative_ = tribool::trifalse;
    };
    void bvisit(const Constant &x)
    {
        is_negative_ = tribool::trifalse;
    };

    tribool apply(const Basic &b);
};

class NonNegativeVisitor : public BaseVisitor<NonNegativeVisitor>
{
private:
    tribool is_nonnegative_;

public:
    void bvisit(const Basic &x)
    {
        is_nonnegative_ = tribool::indeterminate;
    };
    void bvisit(const Symbol &x)
    {
        is_nonnegative_ = tribool::indeterminate;
    };
    void bvisit(const Number &x);
    void bvisit(const Set &x)
    {
        is_nonnegative_ = tribool::trifalse;
    };
    void bvisit(const Relational &x)
    {
        is_nonnegative_ = tribool::trifalse;
    };
    void bvisit(const Boolean &x)
    {
        is_nonnegative_ = tribool::trifalse;
    };
    void bvisit(const Constant &x)
    {
        is_nonnegative_ = tribool::tritrue;
    };

    tribool apply(const Basic &b);
};

class RealVisitor : public BaseVisitor<RealVisitor>
{
private:
    tribool is_real_;
    const Assumptions *assumptions_;

public:
    RealVisitor(const Assumptions *assumptions) : assumptions_(assumptions){};
    void bvisit(const Basic &x)
    {
        is_real_ = tribool::indeterminate;
    };
    void bvisit(const Symbol &x);
    void bvisit(const Number &x);
    void bvisit(const Set &x)
    {
        is_real_ = tribool::trifalse;
    };
    void bvisit(const Relational &x)
    {
        is_real_ = tribool::trifalse;
    };
    void bvisit(const Boolean &x)
    {
        is_real_ = tribool::trifalse;
    };
    void bvisit(const Constant &x);
    void bvisit(const Add &x);

    tribool apply(const Basic &b);
};

class PolynomialVisitor : public BaseVisitor<PolynomialVisitor>
{
private:
    bool is_polynomial_ = true;
    bool variables_allowed_ = true;
    const set_basic &variables_;

    void check_power(const Basic &base, const Basic &exp);

public:
    PolynomialVisitor(const set_basic &variables) : variables_(variables)
    {
    }
    void bvisit(const Basic &x);
    void bvisit(const Number &x){};
    void bvisit(const Constant &x){};
    void bvisit(const Symbol &x);
    void bvisit(const Add &x);
    void bvisit(const Mul &x);
    void bvisit(const Pow &x);
    void bvisit(const Set &x)
    {
        is_polynomial_ = false;
    };
    void bvisit(const Relational &x)
    {
        is_polynomial_ = false;
    };

    bool apply(const Basic &b);
};

/**
 * @brief Check if expression is a polynomial
 * @param b Basic
 * @param variables Set of symbols for variables in polynomial
 * @returns True if b is a polynomial in variables and false otherwise
 *
 * Check if b is a polynomial in variables. If no variables are specified
 * all free symbols in b are considered to be variables. All symbols that
 * are not variables will be considered to be constants.
 */
bool is_polynomial(const Basic &b, const set_basic &variables = {});
}

#endif // SYMENGINE_TEST_VISITORS_H
