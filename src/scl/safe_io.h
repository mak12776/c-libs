
/*
*       Safe IO
*/

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
