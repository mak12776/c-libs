
#define printf_error(fmt, ...) \
    fprintf(stderr, "error: " fmt "\n" __VA_OPT__(,) __VA_ARGS__)

#define printf_line(fmt, ...) \
    fprintf(stdout, fmt "\n" __VA_OPT__(,) __VA_ARGS__)

#define fprintf_line(file, fmt, ...) \
    fprintf(file, fmt "\n" __VA_OPT__(,) __VA_ARGS__)
