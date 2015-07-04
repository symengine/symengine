#ifndef SYMENGINE_ASSERT_H
#define SYMENGINE_ASSERT_H

#define SYMENGINE_ASSERT_DO(cond) \
{ \
if (0 == (cond)) { \
std::cerr << "SYMENGINE_ASSERT failed: " << __FILE__ << \
	"\nfunction " << __func__ <<  "(), line number " << __LINE__ << \
	" at \n" << #cond << "\n"; \
abort(); \
} \
}

// SYMENGINE_ASSERT uses internal functions to perform as assert
// so that there is no effect with NDEBUG
#if !defined(SYMENGINE_ASSERT)
#if defined(WITH_SYMENGINE_ASSERT)
#define SYMENGINE_ASSERT(cond) SYMENGINE_ASSERT_DO(cond)
#else
#define SYMENGINE_ASSERT(cond)
#endif
#endif

#define SYMENGINE_ERROR(description) std::cerr << description; std::cerr << "\n"; abort();
#define SYMENGINE_CHECK_THROW(expression, exception) \
try \
{ \
expression; \
SYMENGINE_ERROR("expected exception not thrown");\
} \
catch(exception &) \
{ \
}

#endif
