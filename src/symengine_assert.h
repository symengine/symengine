#ifndef SYMENGINE_ASSERT_H
#define SYMENGINE_ASSERT_H


#if !defined(SYMENGINE_ASSERT)
#if defined(WITH_SYMENGINE_ASSERT)
#define SYMENGINE_ASSERT(cond) assert(cond);
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
