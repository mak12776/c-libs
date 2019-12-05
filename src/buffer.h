
#ifndef BUFFER_H
#define BUFFER_H

typedef struct
{
	char *pntr;
	size_t size;
} buffer_t;

typedef struct
{
	size_t start;
	size_t end;
} view_t;

typedef struct
{
	buffer_t buffer;
	view_t *views;
	size_t total;
} buffer_views_t;

/* function declarations */

static inline
void buffer_read_file(buffer_t *buffer, FILE *file);

static inline
void buffer_read_file_name(buffer_t *buffer, char *name);

static inline
size_t buffer_count_lines(buffer_t *buffer);

/* function definitions */

static inline
void buffer_read_file(buffer_t *buffer, FILE *file)
{
	read_file(file, (void *) &(buffer->pntr), &(buffer->size));
}

static inline
void buffer_read_file_name(buffer_t *buffer, char *name)
{
	read_file_name(name, (void *) &(buffer->pntr), &(buffer->size));
}

static inline
size_t buffer_count_lines(buffer_t *buffer)
{
	size_t index;
	size_t total;

	if (buffer->size == 0)
	{
		error = ERROR_BAD_ARGUMENTS;
		return 0;
	}

#define CH (buffer->pntr[index])
#define END (index == (buffer->size))

	index = 0;
	total = 1;

loop:
	if (CH == '\r')
	{
		total += 1;

		index += 1;
		if (END) goto end;

		if (CH != '\n') goto loop;
	}
	else if (CH == '\n')
	{
		total += 1;
	}

	index += 1;
	if (!END) goto loop;

#undef CH
#undef END

end:
	return total;
}

// buffer view

static inline
size_t view_find_test(view_t view, char *pntr, int (*test) (int ch))
{
	return find_test(pntr, view.start, view.end, test);
}

static inline
size_t view_rfind_test(view_t view, char *pntr, int (*test) (int ch))
{
	return rfind_test(pntr, view.start, view.end, test);
}

static inline
size_t view_find_chr(view_t view, char *pntr, const char *string)
{
	int find_chr(int ch) { return chr_in(ch, string); }
	return view_find_test(view, pntr, &find_chr);
}

static inline
size_t view_rfind_chr(view_t view, char *pntr, const char *string)
{
	int find_chr(int ch) { return chr_in(ch, string); }
	return view_rfind_test(view, pntr, &find_chr);
}

#endif // BUFFER_H