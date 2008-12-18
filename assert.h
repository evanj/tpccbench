#ifndef ASSERT_H__
#define ASSERT_H__

#include <cassert>

// Wraps the standard assert macro to avoids "unused variable" warnings when compiled away.
// Inspired by: http://powerof2games.com/node/10
// This is not the "default" because it does not conform to the requirements of the C standard,
// which requires that the NDEBUG version be ((void) 0).
#ifdef NDEBUG
#define ASSERT(x) do { (void)sizeof(x); } while(0)
#else
#define ASSERT(x) assert(x)
#endif

#endif
