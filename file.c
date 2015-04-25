#include "file.h"

#include <stdlib.h>
#include <string.h>

struct file *file_create(const char *path, int id) {
	struct file *ret = malloc(sizeof(struct file));
	char *p;

	if (ret == NULL)
		return NULL;

	p = strdup(path);

	if (p == NULL) {
		free(ret);
		return NULL;
	}

	ret->path = p;
	ret->id = id;

	ret->fh = 0;
	ret->fd = 0;
	ret->is_open = 0;
	ret->FILE = NULL;

	return ret;
}

void file_delete(struct file *file) {
	if (!file)
		return;

	if (file->is_open && file->FILE)
		fclose(file->FILE);

	if (file->path)
		free(file->path);

	free(file);
}