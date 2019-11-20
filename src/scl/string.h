
#define LOWERCASE "abcdefghijklmnopqrstuvwxyz"
#define UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LETTERS (LOWERCASE UPPERCASE)
#define DIGITS "0123456789"

#define STRLEN(string) (sizeof(string) - 1)

#define string_cmp(string, pntr) memcmp(pntr, string, STRLEN(string))

#define STRING_BUFFER_SIZE 1024
char string_buffer[STRING_BUFFER_SIZE];

static inline
const char *format_human_size(size_t bytes)
{
	char *suffix[] = {"B", "KB", "MB", "GB", "TB"};
	char length = sizeof(suffix) / sizeof(suffix[0]);

	int i = 0;
	double double_bytes = bytes;

    while ((bytes / 1024) > 0 && i < length - 1)
    {
        i += 1;
        bytes /= 1024;
        double_bytes /= 1024.0;
    }

	snprintf(string_buffer, STRING_BUFFER_SIZE,
        "%.02lf %s", double_bytes, suffix[i]);
	return string_buffer;
}

static inline
int chr_in(char ch, const char *string)
{
    while (*string)
    {
        if (ch == (*string))
        {
            return 1;
        }
        string++;
    }
    return 0;
}

// find index

static inline
size_t find_test(char *pntr, size_t start, size_t end, int (*test) (int ch))
{
    size_t index = start;
    while (index < end)
    {
        if (test(pntr[index]))
        {
            return index;
        }
        index += 1;
    }
    return end;
}

static inline
size_t rfind_test(char *pntr, size_t start, size_t end, int (*test) (int ch))
{
    size_t index = end;
    while (index > start)
    {
        index -= 1;
        if (test(pntr[index]))
        {
            return index;
        }
    }
    return end;
}
