
/*
*       Safe IO
*/

static inline
FILE *safe_fopen(const char *name, const char *mode)
{
    FILE *file = fopen(name, mode);
    if (file == NULL)
    {
        error = ERROR_FOPEN;
        error_info.file_name = name;
    }
    return file;
}

static inline
void safe_fseek(FILE *file, long offset, int whence)
{
    if (fseek(file, offset, whence))
    {
        error = ERROR_FSEEK;
        error_info.file = file;
    }
}

static inline
long safe_ftell(FILE *file)
{
    long file_size;

    if ((file_size = ftell(file)) == -1)
    {
        error = ERROR_FTELL;
        error_info.file = file;
    }
    return file_size;
}

static inline
size_t safe_get_file_size(FILE *file)
{
    size_t file_size;

    file_size = get_file_size(file, &error);
    if (error != NO_ERROR)
    {
        error_info.file = file;
        return 0;
    }

    if (file_size == 0)
    {
        error = ERROR_ZERO_FILE_SIZE;
        error_info.file = file;
        return 0;
    }

    return file_size;
}

static inline
size_t safe_fwrite(void *pntr, size_t size, FILE *file)
{
    size_t write_number;

    write_number = fwrite(pntr, 1, size, file);
    if (write_number != size)
    {
        error = ERROR_IO;
        error_info.file = file;
    }
    return write_number;
}

static inline
size_t safe_fread(void *pntr, size_t size, FILE *file)
{
    size_t read_number;

    read_number = fread(pntr, 1, size, file);
    if (read_number != size)
    {
        error = ERROR_IO;
        error_info.file = file;
    }
    return read_number;
}
