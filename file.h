#ifndef __FILE_H_INCUDED_
#define __FILE_H_INCUDED_

#include <stdio.h>

struct file {
	char *path;
	int id;

	int fh;
	int fd;
	int is_open:1;
	FILE *FILE;
};

struct file *file_create(const char *path, int id);
void file_delete(struct file *file);

#endif