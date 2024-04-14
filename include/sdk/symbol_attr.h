#pragma once

#include "sdkconfig.h"

#if CONFIG_TEST_ENABLE
#define MEMU_STATIC
#else
#define MEMU_STATIC static
#endif

#define EXC_FLOW_NEVER_REACH do { assert(0); } while(0)

#ifdef __GNUC__
# define   likely(x) __builtin_expect(x, 1)
# define unlikely(x) __builtin_expect(x, 0)
# define NOINLINE __attribute__ ((noinline))
# define NORETURN __attribute__ ((noreturn))
# define ALWAYS_INLINE __attribute__ ((always_inline))
# define UNUSED __attribute__ ((unused))
# define NOAPI __attribute__((visibility("hidden")))
#else
# define   likely(x) (x)
# define unlikely(x) (x)
# define NOINLINE
# define NORETURN
# define ALWAYS_INLINE
# define UNUSED
#endif

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif
