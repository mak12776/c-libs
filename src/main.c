
#include <scl.h>

int main(int argc, char **argv)
{
	const char *program_name = (argc != 0) ? argv[0] : "machine";

	if (argc != 2)
	{
		printf_line("usage: %s [file]", program_name);
		return EXIT_NORMAL;
	}

	char *buffer;
	size_t size;

	read_file_name(argv[1], &buffer, &size);
	check_error();
}
