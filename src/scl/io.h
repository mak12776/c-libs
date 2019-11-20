
/*  errors:
*       ERROR_FSEEK, ERROR_FTELL
*/

static inline
FILE *safe_fopen(const char *name, const char *mode)
{
	FILE *file = fopen(name, mode);
	if (file == NULL)
	{
		error = ERROR_FOPEN;
		error_info.file_name = name;
		error_info.open_mode = mode;
	}
	return file;
}

static inline
size_t safe_fread(void *pntr, size_t size, FILE *file)
{
	size_t read_number;

	read_number = fread(pntr, 1, size, file);
	if (read_number != size)
	{
		error = ERROR_IO_FREAD;
		error_info.file = file;
	}
	return read_number;
}

static inline
size_t safe_fwrite(void *pntr, size_t size, FILE *file)
{
	size_t write_number;

	write_number = fwrite(pntr, 1, size, file);
	if (write_number != size)
	{
		error = ERROR_IO_FWRITE;
		error_info.file = file;
	}
	return write_number;
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
long get_file_size(FILE *file)
{
	long file_size;

	safe_fseek(file, 0, SEEK_END);
	if (error) 
		return 0;

	safe_ftell(file);
	if (error)
		return 0;

	safe_fseek(file, 0, SEEK_SET);
	if (error)
		return 0;

	return file_size;
}

/*  errors:
*       ERROR_ZERO_FILE_SIZE
*/

static inline
void read_file(FILE *file, void **buffer, size_t *size)
{
	long file_size;
	size_t read_number;

	file_size = get_file_size(file);
	if (error)
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

	*buffer = safe_malloc(file_size);
	if (error)
		return;

	*size = file_size;

	safe_fread(*buffer, *size, file);
	if (error)
		return;
}

static inline
void read_file_name(char *name, CHAR **buffer, size_t *size)
{
	FILE *file;

	file = safe_fopen(name, "rb");
	if (error)
		return;

	read_file(file, buffer, size);
	fclose(file);
}
