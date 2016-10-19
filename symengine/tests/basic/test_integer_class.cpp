#include "catch.hpp"

#include <symengine/mp_class.h>
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/complex.h>
#include <symengine/complex_double.h>
#include <iostream>

using std::cout;
using std::endl;

using SymEngine::integer_class;
using SymEngine::mp_and;
using SymEngine::mp_fdiv_r;
using SymEngine::Integer;
using SymEngine::Rational;
using SymEngine::make_rcp;
using SymEngine::RCP;
using SymEngine::is_a;

namespace SymEngine {

void print_canonical_pow_checks(const Basic &base, const Basic &exp)
{
    /*
    // e.g. 0**x
    if (is_a<Integer>(base) and static_cast<const Integer &>(base).is_zero())
    {cout << "false" << endl;} else {cout << "true" << endl;}
    // e.g. 1**x
    if (is_a<Integer>(base) and static_cast<const Integer &>(base).is_one())
        {cout << "false" << endl;} else {cout << "true" << endl;}
    // e.g. x**0.0
    if (is_a_Number(exp) and static_cast<const Number &>(exp).is_zero())
        {cout << "false" << endl;} else {cout << "true" << endl;}
    // e.g. x**1
    if (is_a<Integer>(exp) and static_cast<const Integer &>(exp).is_one())
        {cout << "false" << endl;} else {cout << "true" << endl;}
    // e.g. 2**3, (2/3)**4
    if ((is_a<Integer>(base) or is_a<Rational>(base)) and is_a<Integer>(exp))
        {cout << "false" << endl;} else {cout << "true" << endl;}
    // e.g. (x*y)**2, should rather be x**2*y**2
    if (is_a<Mul>(base) and is_a<Integer>(exp))
        {cout << "false" << endl;} else {cout << "true" << endl;}
    // e.g. (x**y)**2, should rather be x**(2*y)
    //if (is_a<Pow>(base) and is_a<Integer>(exp))
        //{cout << "false" << endl;} else {cout << "true" << endl;}
    // If exp is a rational, it should be between 0  and 1, i.e. we don't
    // allow things like 2**(-1/2) or 2**(3/2)
    if ((is_a<Rational>(base) or is_a<Integer>(base)) and is_a<Rational>(exp)
        and (static_cast<const Rational &>(exp).i < 0
             or static_cast<const Rational &>(exp).i > 1))
        {cout << "false" << endl;} else {cout << "true" << endl;}
    // Purely Imaginary complex numbers with integral powers are expanded
    // e.g (2I)**3
    if (is_a<Complex>(base) and static_cast<const Complex &>(base).is_re_zero()
        and is_a<Integer>(exp))
        {cout << "false" << endl;} else {cout << "true" << endl;}
    // e.g. 0.5^2.0 should be represented as 0.25
    if (is_a_Number(base) and not static_cast<const Number &>(base).is_exact()
        and is_a_Number(exp)
        and not static_cast<const Number &>(exp).is_exact())
        {cout << "false" << endl;} else {cout << "true" << endl;}
    */
}

using namespace SymEngine::literals;

TEST_CASE("integer_class: helper functions", "[mp_class]")
{
	integer_class i = 9, j = 12; // i == 1001, j==1100
	integer_class k;
	mp_and(k,i,j);
	REQUIRE(k == 8); //mp_and is bitwise and

	j = -12;
	mp_fdiv_r(k, j, i);
	REQUIRE(k == 6);

	const Integer rcp_base(2_z);
	//const Integer rcp_exp(3);

	//print_canonical_pow_checks(rcp_base, rcp_exp);
   
}
}//SymEngine