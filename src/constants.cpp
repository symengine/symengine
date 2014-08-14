#include "constants.h"
#include "complex.h"
#include "functions.h"
#include "mul.h"
#include "add.h"
#include "pow.h"

namespace CSymPy {

RCP<const Integer> zero = rcp(new Integer(0));
RCP<const Integer> one = rcp(new Integer(1));
RCP<const Integer> minus_one = rcp(new Integer(-1));
RCP<const Number> I = Complex::from_two_nums(*zero, *one);

RCP<const Symbol> pi = rcp(new Symbol("pi"));
RCP<const Symbol> E = rcp(new Symbol("E"));

// Global variables declared in functions.cpp
// Look over https://github.com/sympy/csympy/issues/272
// for further details
RCP<const Basic> i2 = rcp(new Integer(2));

RCP<const Basic> sqrt_(const RCP<const Basic>& arg)
{
    return pow(arg, div(one, i2));
}


RCP<const Basic> i3 = rcp(new Integer(3));
RCP<const Basic> i5 = rcp(new Integer(5));
RCP<const Basic> im2 = rcp(new Integer(-2));
RCP<const Basic> im3 = rcp(new Integer(-3));
RCP<const Basic> im5 = rcp(new Integer(-5));

RCP<const Basic> sq3 = sqrt_(i3);
RCP<const Basic> sq2 = sqrt_(i2);
RCP<const Basic> sq5 = sqrt_(i5);

RCP<const Basic> C0 = div(sub(sq3, one), mul(i2, sq2));
RCP<const Basic> C1 = div(one, i2);
RCP<const Basic> C2 = div(sq2, i2);
RCP<const Basic> C3 = div(sq3, i2);
RCP<const Basic> C4 = div(add(sq3, one), mul(i2, sq2));
RCP<const Basic> C5 = div(sqrt_(sub(i5, sqrt_(i5))), integer(8));
RCP<const Basic> C6 = div(sub(sqrt_(i5), one), integer(4));

RCP<const Basic> mC0 = mul(minus_one, C0);
RCP<const Basic> mC1 = mul(minus_one, C1);
RCP<const Basic> mC2 = mul(minus_one, C2);
RCP<const Basic> mC3 = mul(minus_one, C3);
RCP<const Basic> mC4 = mul(minus_one, C4);
RCP<const Basic> mC5 = mul(minus_one, C5);
RCP<const Basic> mC6 = mul(minus_one, C6);

}
