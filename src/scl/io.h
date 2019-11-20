
/*  errors:
*       ERROR_FSEEK, ERROR_FTELL
*/

static inline
long get_file_size(FILE *file)
{
    long file_size;
    if (fseek(file, 0, SEEK_END) == -1)
    {
        error = ERROR_FSEEK;
        error_info.file = file;
        return 0;
    }

    file_size = ftell(file);
    if (file_size == -1)
    {
        error = ERROR_FTELL;
        error_info.file = file;
        return 0;
    }

    if (fseek(file, 0, SEEK_SET) == -1)
    {
        error = ERROR_FSEEK;
        error_info.file = file;
        return 0;
    }

    return file_size;
}

/*  errors:
*       ERROR_ZERO_FILE_SIZE,
*/

static inline
void read_file(FILE *file, CHAR **buffer, size_t *size)
{
    long file_size;
    size_t read_number;

    file_size = get_file_size(file);
    if (error != NO_ERROR)
        return;

    if (file_size == 0)
    {
        error = ERROR_ZERO_FILE_SIZE;
        error_info.file = file;

        return;
    }

    #if LONG_MAX > SIZE_MAX
        #error LONG_MAX is greater than SIZE_MAX.
    #endif

    *buffer = (CHAR *)malloc(file_size);
    if (!(*buffer))
    {
        error = ERROR_NO_MEMORY;
        error_info.file = file
        return;
    }
    *size = file_size;

    read_number = fread(*buffer, 1, file_size, file);
    if (read_number != (size_t)file_size)
    {
        error = ERROR_IO;
        error_info.file = file;

        free(*buffer);
        return;
    }
}

static inline
void read_file_name(char *name, CHAR **buffer, size_t *size)
{
    FILE *file;

    file = fopen(name, "rb");
    if (file == NULL)
    {
        error = ERROR_FOPEN;
        error_info.file_name = name;
        return;
    }

    read_file(file, buffer, size);
    fclose(file);
}
