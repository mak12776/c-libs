
#if SIZE_MAX == ULLONG_MAX
#define SIZE_C "%I64u"
#elif SIZE_MAX == ULONG_MAX
#define SIZE_C "%I32u"
#else
#error unknown SIZE_MAX
#endif

#if SIZE_MAX == 0xFF
#define SIZE_WIDTH 1
#elif SIZE_MAX == 0xFFFF
#define SIZE_WIDTH 2
#elif SIZE_MAX == 0xFFFFFFFF
#define SIZE_WIDTH 4
#elif SIZE_MAX == 0xFFFFFFFFFFFFFFFF
#define SIZE_WIDTH 8
#else
#error unknown SIZE_MAX
#endif
