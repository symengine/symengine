// This tells Catch to provide a main() - only do this in one cpp file:
// User defined macros can be added in this file

#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_SFINAE
#include "catch.hpp"

#include <symengine/symengine_config.h>

#if defined(HAVE_SYMENGINE_MPFR)
#include <mpfr.h>
#endif // HAVE_SYMENGINE_MPFR

#if defined(HAVE_SYMENGINE_ARB)
#include <arb.h>
#endif // HAVE_SYMENGINE_ARB

int main(int argc, char* const argv[])
{
	int result = Catch::Session().run( argc, argv );

	#if defined(HAVE_SYMENGINE_MPFR)
	   mpfr_free_cache();
	#endif // HAVE_SYMENGINE_MPFR

	#if defined(HAVE_SYMENGINE_ARB)
	   flint_cleanup();
	#endif // HAVE_SYMENGINE_ARB

	return result;
}
