#include "file.h"
#include "fl.h"
#include "commands.h"
#include "usage.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

void exec_help() {
	usage();
}

void exec_info() {
	int i;
	printf("Total files: %d\n", files_count);

	printf("%3s  %-20s %4s  %3s\n", "id", "path", "open", "eof");

	for (i = 0; i < files_count; i++) {
		char *open = files[i]->is_open ? "t" : "f";
		char *eof = "f";
		if (files[i]->is_open && feof(files[i]->FILE))
			eof = "t";
		printf("% 3d: %-20s %-4s  %-3s\n", i, files[i]->path, open, eof);
	}
}

void exec_open(int id, const char *mode) {
	struct file *file = files[id];

	if (!file) {
		printf("fd %d not found\n", id);
		return;
	}

	if (file->is_open) {
		printf("Already open\n");
		return;
	}

	file->FILE = fopen(file->path, mode);

	if (file->FILE) {
		file->is_open = 1;
		file->fd = fileno(file->FILE);
		printf("Success\n");
	}
	else {
		perror("fopen");
	}
}

void exec_lock(int id, const int operation) {
	struct file *file = files[id];

	if (!file) {
		printf("fd %d not found\n", id);
		return;
	}

	if (!file->is_open) {
		printf("File closed\n");
		return;
	}

	int ret = flock(file->fd, operation);

	if (ret == 0) {
		printf("Success\n");
	}
	else {
		perror("flock");
	}
}

void exec_close(int id) {
	struct file *file = files[id];

	if (!file) {
		printf("file %d not found", id);
		return;
	}

	if (!file->is_open) {
		printf("File closed\n");
		return;
	}


	int ret = fclose(file->FILE);

	if (ret == 0) {
		file->is_open = 0;
		printf("Success\n");
	}
	else {
		perror("fclose");
	}
}

void exec_seek(int id, int offset, int whence) {
	struct file *file = files[id];

	if (!file) {
		printf("file %d not found", id);
		return;
	}

	if (!file->is_open) {
		printf("File closed\n");
		return;
	}

	int ret = fseek(file->FILE, offset, whence);

	if (ret != -1) {
		printf("Success\n");
	}
	else {
		perror("fseek");
	}
}

void exec_tell(int id) {
	struct file *file = files[id];

	if (!file) {
		printf("file %d not found", id);
		return;
	}

	if (!file->is_open) {
		printf("File closed\n");
		return;
	}

	long ret = ftell(file->FILE);

	if (ret != -1) {
		printf("%ld\n", ret);
	}
	else {
		perror("ftell");
	}
}

void exec_truncate(int id, int length) {
	struct file *file = files[id];

	if (!file) {
		printf("file %d not found", id);
		return;
	}

	if (!file->is_open) {
		printf("File closed\n");
		return;
	}

	int ret = ftruncate(file->fd, length);

	if (ret == 0) {
		printf("Success\n");
	}
	else {
		perror("ftruncate");
	}
}

void exec_read(int id, int length, int hex) {
	struct file *file = files[id];

	if (!file) {
		printf("file %d not found", id);
		return;
	}

	if (!file->is_open) {
		printf("File closed\n");
		return;
	}

	unsigned char *buffer = malloc(length + 1);
	if (!buffer) {
		printf("Failed to allocate memory for reading\n");
		return;
	}

	int start = ftell(file->FILE);
	int ret = fread(buffer, 1, length, file->FILE);
	buffer[ret] = 0;

	if (ret > 0) {
		printf("Success %d:\n", ret);
		if (!hex) {
			write(fileno(stdout), buffer, ret);
		}
		else {
			int i;
			int line_pos = start - (start / 16) * 16;
			int buf_pos = 0;

			int line_no = start / 16;

			if (line_pos != 0) {
				printf("%08X ", line_no++);
				for (i = 0; i < line_pos; i++) {
					printf("   ");
				}
			}

			while (buf_pos < ret) {
				if (line_pos == 0) {
					printf("%08X ", line_no++);
				}
				printf("%02X ", buffer[buf_pos]);
				line_pos++;

				if (line_pos == 16) {
					printf(" |");
					for (i = 15; i >= 0; i--) {
						if (buf_pos - i < 0) {
							printf(" ");
							continue;
						}

						unsigned char c = buffer[buf_pos - i];
						if (isprint(c))
							printf("%c", c);
						else
							printf(".");
					}

					printf("|");

					printf("\n");
					line_pos = 0;
				}
				else if (line_pos == 8) {
					printf(" ");
				}

				buf_pos++;
			}

			for (i = line_pos; i < 16; i++) {
				printf("   ");
			}
			printf(" |");
			while (line_pos > 0) {
				unsigned char c = buffer[ret - line_pos];
				if (isprint(c))
					printf("%c", c);
				else
					printf(".");
				line_pos--;
			}

			printf("\n");
		}
	}
	else if (ret == 0) {
		if (feof(file->FILE))
			printf("read: EOF\n");
		else {
			printf("read: ERROR\n");
		}
	}
	else {
		perror("read");
	}

	free(buffer);
}
