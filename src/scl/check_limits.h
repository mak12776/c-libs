
#ifdef SCL_CHECK_LIMITS
#include <limits.h>

#if UCHAR_MAX != (0xFF)
#error unsigned char is not 1 byte.
#endif

#if USHRT_MAX != (0xFFFF)
#error unsigned short is not 2 byte.
#endif

#if UINT_MAX != (0xFFFFFFFF)
#error unsigned int is not 4 byte.
#endif

#if ULLONG_MAX != (0xFFFFFFFFFFFFFFFF)
#error unsigned long long is not 8 byte.
#endif

#endif // SCL_CHECK_LIMITS
