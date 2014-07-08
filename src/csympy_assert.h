#ifndef CSYMPY_ASSERT_H
#define CSYMPY_ASSERT_H


#if !defined(CSYMPY_ASSERT)
#if defined(WITH_CSYMPY_ASSERT)
#define CSYMPY_ASSERT(cond) assert(cond);
#else
#define CSYMPY_ASSERT(cond)
#endif
#endif

#define CSYMPY_ERROR(description) std::cerr << description; std::cerr << "\n"; abort();
#define CSYMPY_CHECK_THROW(expression, exception) \
try \
{ \
expression; \
CSYMPY_ERROR("expected exception not thrown");\
} \
catch(exception &) \
{ \
}

#endif
