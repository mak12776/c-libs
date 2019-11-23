
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

static inline
void buffer_split_lines(buffer_t *buffer, view_t *lines);

static inline
size_t buffer_write_view(buffer_t *buffer, buffer_view_t *view, FILE *file);

static inline
size_t buffer_write_lines(buffer_t *buffer, buffer_views_t *lines, FILE *file);

/* function definitions */

static inline
void buffer_read_file(buffer_t *buffer, FILE *file)
{
	read_file(file, &(buffer->pntr), &(buffer->size));
}

static inline
void buffer_read_file_name(buffer_t *buffer, char *name)
{
	read_file_name(name, &(buffer->pntr), &(buffer->size));
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
		total = 0;

		while (1)
		{
			if (CH == '\r')
			{
				total += 1;

				index += 1;
				if (END) break;

				if (CH == '\n')
				{
					index += 1;
					if (END) break;
				}
			}
			else if (CH == '\n')
			{
				total += 1;

				index += 1;
				if (END) break;
			}
			else
			{
				index += 1;
				if (END)
				{
					total += 1;
					break;
				}
			}
		}

#undef CH
#undef END

	return total;
}

static inline
void buffer_split_lines(buffer_t *buffer, buffer_views_t *lines)
{
	size_t index;
	size_t lnum;
	size_t total;

	total = buffer_count_lines(buffer);
	if (error) return;

	lines_initialize_total(lines, total);
	if (error) return;

#define CH (buffer->pntr[index])
#define END (index == buffer->size)
#define LINE (lines->pntr[lnum])

	index = 0;
	lnum = 0;

loop:
	LINE.start = index;

	while (1)
	{
		if (CH == '\n')
		{
			index += 1;
			LINE.end = index;

			if (END) break;

			lnum += 1;
			goto loop;
		}
		else if (CH == '\r')
		{
			CH = '\n';

			index += 1;
			LINE.end = index;

			if (END) break;

			if (CH == '\n')
			{
				index += 1;
				if (END) break;
			}

			lnum += 1;
			goto loop;
		}
		else
		{
			index += 1;
			if (END)
			{
				LINE.end = index;
				break;
			}
		}
	}

#undef CH
#undef END
#undef LINE

}

// buffer view

static inline
size_t view_find_test(view_t view, char *pntr, int (*test) (int ch))
{
	size_t index = view.start;
	while (index < view.end)
	{
		if (test(pntr[index]))
		{
			return index;
		}
		index += 1;
	}
	return view.end;
}

static inline
size_t view_rfind_test(view_t view, char *pntr, int (*test) (int ch))
{
	size_t index = view.end;
	while (index > view.start)
	{
		index -= 1;
		if (test(pntr[index]))
		{
			return index;
		}
	}
	return view.end;
}

static inline
size_t view_find_chr(view_t view, char *pntr, const char *string)
{
	int find_chr(int ch) { return chr_in(ch, string); }
	return buffer_view_find_test(view, pntr, &find_chr);
}

static inline
size_t view_rfind_chr(view_t view, char *pntr, const char *string)
{
	int find_chr(int ch) { return chr_in(ch, string); }
	return buffer_view_rfind_test(view, pntr, &find_chr);
}

#endif // BUFFER_H