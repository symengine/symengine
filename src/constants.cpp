#include "constants.h"
#include "complex.h"

namespace CSymPy {

RCP<const Integer> zero = rcp(new Integer(0));
RCP<const Integer> one = rcp(new Integer(1));
RCP<const Integer> minus_one = rcp(new Integer(-1));
RCP<const Number> I = Complex::from_two_nums(*zero, *one);

RCP<const Symbol> pi = rcp(new Symbol("pi"));
RCP<const Symbol> E = rcp(new Symbol("E"));

}