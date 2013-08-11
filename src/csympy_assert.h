#ifndef CSYMPY_ASSERT_H
#define CSYMPY_ASSERT_H


#if !defined(CSYMPY_ASSERT)
#if defined(WITH_CSYMPY_ASSERT)
#include <cassert>
#define CSYMPY_ASSERT(cond) assert(cond);
#else
#define CSYMPY_ASSERT(cond)
#endif
#endif


#endif
