
static inline
void *safe_malloc(size_t size)
{
	void *pntr = malloc(size);
	if (pntr == NULL)
	{
		error = ERROR_NO_MEMORY;
		error_info.size = size;
	}
}
