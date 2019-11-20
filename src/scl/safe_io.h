
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

