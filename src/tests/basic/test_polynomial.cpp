#include <iostream>
#include <chrono>

#include "polynomial.h"

using SymEngine::Polynomial;
using SymEngine::polynomial;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::Basic;

void polynomial_constructor()
{   
    std::string x = "x";
    RCP<const Polynomial> P = polynomial(x, {{0, 1}, {1, 2}, {2, 1}});
    assert(P->degree == 2);
}

void test_add_poly()
{
	std::string x = "x";
	map_uint_mpz adict_ = {{0, 1}, {1, 2}, {2, 1}};
	map_uint_mpz bdict_ = {{0, 2}, {1, 3}, {2, 4}};
	const Polynomial a(x, std::move(adict_));
	const Polynomial b(x, std::move(bdict_));

	RCP<const Basic> c = add_poly(a, b);
	//std::cout<<c->__str__();
	assert(c->__str__() == "5*x**2 + 5*x + 3");
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    polynomial_constructor();

    test_add_poly();
    return 0;
}
