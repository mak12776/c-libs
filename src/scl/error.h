
#define NO_ERROR 0
#define ERROR_NO_MEMORY 1
#define ERROR_FOPEN 2
#define ERROR_FSEEK 3
#define ERROR_FTELL 4
#define ERROR_IO_FREAD 5
#define ERROR_IO_FWRITE 6
#define ERROR_ZERO_FILE_SIZE 7
#define ERROR_INT_OVERFLOW 8
#define ERROR_FLOAT_OVERFLOW 9
#define ERROR_BAD_ARGUMENTS 10
#define ERROR_UNDEFINED_BEHAVIOR 0xFF

#define ERROR_MAX 0xFF

int error = NO_ERROR;

static inline
void clear_error()
{
	error = NO_ERROR;
}

static inline
char *str_error(int error)
{
#define ERROR_CASE(NAME) case NAME: return #NAME;
	switch (error)
	{
		ERROR_CASE(NO_ERROR)
		ERROR_CASE(ERROR_NO_MEMORY)
		ERROR_CASE(ERROR_FOPEN)
		ERROR_CASE(ERROR_FSEEK)
		ERROR_CASE(ERROR_FTELL)
		ERROR_CASE(ERROR_IO_FREAD)
		ERROR_CASE(ERROR_IO_FWRITE)
		ERROR_CASE(ERROR_ZERO_FILE_SIZE)
		ERROR_CASE(ERROR_INT_OVERFLOW)
		ERROR_CASE(ERROR_FLOAT_OVERFLOW)
		ERROR_CASE(ERROR_BAD_ARGUMENTS)
		ERROR_CASE(ERROR_UNDEFINED_BEHAVIOR)
	default:
		return "unknown error";
	}
#undef ERROR_CASE
}

static inline
void check_error()
{
	if (error == NO_ERROR)
		return;

	printf_error("%s", str_error(error));
	exit(EXIT_ERROR);
}

struct
{
	unsigned int log_flags;

	// usage:   ERROR_FSEEK, ERROR_FTELL, 
	//          ERROR_ZERO_FILE_SIZE, 
	//          ERROR_IO_FREAD, ERROR_IO_FWRITE,
	FILE *file;

	// usage: 	ERROR_FOPEN,
	const char *file_name;
	const char *file_mode;

	// usage: 	ERROR_NO_MEMORY,
	//          ERROR_IO_FREAD, ERROR_IO_FWRITE,
	size_t size;

	// usage: 	ERROR_BAD_ARGUMENTS
	const char *function_name;
	const char *argument_name;

} error_info;


static inline
void check_error_info()
{
	if (error == NO_ERROR)
		return;

	switch (error)
	{
	case ERROR_FOPEN:
		printf_error("can't open '%s': %s", error_info.file_name,
			strerror(errno));
		exit(EXIT_ERROR);
		break;

	default:
		printf_error("%s", str_error(error));
		exit(EXIT_ERROR);
		break;
	}
}