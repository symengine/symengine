//#include "catch.hpp"

#include <symengine/mp_class.h>
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/complex.h>
#include <symengine/complex_double.h>
#include <symengine/constants.h>
#include <iostream>

using std::cout;
using std::endl;

using SymEngine::integer_class;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Rational;
using SymEngine::rational;
using SymEngine::make_rcp;
using SymEngine::RCP;
using SymEngine::pi;

namespace SymEngine {

using namespace SymEngine::literals;

int main() {
    RCP<const Integer> i3 = integer(3);
    mul(i3, pi);
}
}