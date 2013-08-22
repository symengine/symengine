#ifndef CSYMPY_ASSERT_H
#define CSYMPY_ASSERT_H


#if !defined(CSYMPY_ASSERT)
#if defined(WITH_CSYMPY_ASSERT)
#define CSYMPY_ASSERT(cond) assert(cond);
#else
#define CSYMPY_ASSERT(cond)
#endif
#endif


#endif
