
#include <scl.h>
#include <buffer.h>

int main(int argc, char **argv)
{
	const char *program_name = (argc != 0) ? argv[0] : "machine";

	if (argc != 2)
	{
		printf_line("usage: %s [file]", program_name);
		return EXIT_NORMAL;
	}

	
}
