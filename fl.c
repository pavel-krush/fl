#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/file.h>

#include "usage.h"
#include "fl.h"
#include "file.h"
#include "commands.h"

int files_count;
struct file **files;

void parse_options(int argc, char **argv) {
	int i;
	if (argc < 2) {
		usage();
		exit(1);
	}

	files_count = argc - 1;

	files = calloc(sizeof(struct file *), files_count + 1);

	for (i = 0; i < files_count; i++) {
		if (!strcmp(argv[i + 1], "--help")) {
			usage();
			exit(0);
		}
		files[i] = file_create(argv[i + 1], i);
	}

	files[files_count] = NULL;
}

void skip_spaces(const char **src) {
	while (**src && isspace(**src)) {
		(*src)++;
	}
}

int *parse_int(const char **src) {
	int *ret;
	if (!isdigit(**src))
		return NULL;

	ret = malloc(sizeof(int));
	*ret = 0;

	while (**src && isdigit(**src)) {
		*ret *= 10;
		*ret += (**src) - '0';
		(*src)++;
	}

	return ret;
}

char *parse_word(const char **src) {
	const char *start, *end;
	char *ret;

	if (!isalpha(**src))
		return NULL;

	start = *src;
	while (**src && isalpha(**src)) {
		(*src)++;
	}
	end = *src;

	ret = strndup(start, end - start);

	return ret;
}

char *parse_until_space(const char **src) {
	const char *start, *end;
	char *ret;

	if (isspace(**src))
		return NULL;

	start = *src;
	while (**src && !isspace(**src)) {
		(*src)++;
	}
	end = *src;

	ret = strndup(start, end - start);

	return ret;
}

void do_command(const char *command_line) {
	skip_spaces(&command_line);

	char *command = parse_word(&command_line);
	if (command == NULL)
		return;

	skip_spaces(&command_line);

	int command_length = strlen(command);

	#define CMD_COND1(str) ( ( command_length == sizeof(str) - 1 && !strcmp(command, str) ) )
	#define CMD_COND2(str) ( ( command_length == 1 && command[0] == str[0] ) || ( ( command_length == sizeof(str) - 1 ) && !strcmp(command, str) ) )

	#define  PARSE_ID(fail_label) int *id; id = parse_int(&command_line); \
		if (!id) { printf("ID required\n"); goto fail_label; } \
		if (*id < 0 || *id >= files_count) { printf("Invalid ID. Valid values are from %d to %d\n", 0, files_count); goto fail_label; }

	if (CMD_COND2("help")) {
		exec_help();
	}
	else if (CMD_COND2("info")) {
		exec_info();
	}
	// open ID mode
	else if (CMD_COND2("open")) {
		char *mode = NULL;
		PARSE_ID(open_cleanup)

		skip_spaces(&command_line);

		mode = parse_until_space(&command_line);
		if (!mode) {
			printf("MODE required\n");
			goto open_cleanup;
		}

		exec_open(*id, mode);

		open_cleanup:
			if (id) free(id);
			if (mode) free(mode);
			goto cleanup;
	}
	else if (CMD_COND2("lock")) {
		char *operation = NULL;

		PARSE_ID(lock_cleanup)

		skip_spaces(&command_line);

		operation = parse_until_space(&command_line);
		if (!operation) {
			printf("Operation required\n");
			goto lock_cleanup;
		}

		int op;

		if (!strcmp(operation, "shnb"))
			op = LOCK_SH | LOCK_NB;
		else if (!strcmp(operation, "sh"))
			op = LOCK_SH;
		else if (!strcmp(operation ,"exnb"))
			op = LOCK_EX | LOCK_NB;
		else if (!strcmp(operation, "ex"))
			op = LOCK_EX;
		else if (!strcmp(operation, "unnb"))
			op = LOCK_UN | LOCK_NB;
		else if (!strcmp(operation, "un"))
			op = LOCK_UN;
		else {
			printf("Invalid <operation>\n");
			goto lock_cleanup;
		}

		exec_lock(*id, op);

		lock_cleanup:
			if (id) free(id);
			if (operation) free(operation);
			goto cleanup;
	}
	else if (CMD_COND2("close")) {
		PARSE_ID(close_cleaup)

		exec_close(*id);

		close_cleaup:
			if (id) free(id);
			goto cleanup;
	}
	else if (CMD_COND2("seek")) {
		int *offset = NULL;
		char *whence_str = NULL;

		PARSE_ID(seek_cleanup)

		skip_spaces(&command_line);

		int sign = 1;
		if (command_line[0] == '-') {
			sign = -1;
			command_line++;
		}

		offset = parse_int(&command_line);
		if (!offset) {
			printf("Invalid <offset>\n");
			goto seek_cleanup;
		}
		*offset = *offset * sign;

		skip_spaces(&command_line);

		whence_str = parse_until_space(&command_line);
		if (!whence_str) {
			printf("Invalid <whence>. Valid values are: set, end, cur\n");
			goto seek_cleanup;
		}

		int whence = 0;

		if (!strcmp(whence_str, "set")) {
			whence = SEEK_SET;
		}
		else if (!strcmp(whence_str, "end")) {
			whence = SEEK_END;
		}
		else if (!strcmp(whence_str, "cur")) {
			whence = SEEK_CUR;
		}

		exec_seek(*id, *offset, whence);

		seek_cleanup:
			if (id) free(id);
			if (offset) free(offset);
			if (whence_str) free(whence_str);
			goto cleanup;
	}
	else if (CMD_COND2("tell")) {
		PARSE_ID(tell_cleanup)

		skip_spaces(&command_line);

		exec_tell(*id);

		tell_cleanup:
			if (id) free(id);
			goto cleanup;
	}
	else if (CMD_COND2("write")) {
		printf("not implemented\n");
		goto write_cleanup;
		write_cleanup:
			goto cleanup;
	}
	else if (CMD_COND2("read")) {
		int *length = NULL;
		PARSE_ID(read_cleanup);

		skip_spaces(&command_line);

		int hex = 0;

		if (command_line[0] == 'x') {
			hex = 1;
			command_line++;
			skip_spaces(&command_line);
		}

		length = parse_int(&command_line);
		if (!length) {
			printf("Invalid <length>\n");
			goto read_cleanup;
		}

		exec_read(*id, *length, hex);

		read_cleanup:
			if (id) free(id);
			if (length) free(length);
			goto cleanup;
	}
	else if (CMD_COND1("truncate")) {
		int *length = NULL;
		PARSE_ID(truncate_cleanup)

		skip_spaces(&command_line);

		length = parse_int(&command_line);
		if (!length) {
			printf("Invalid <length>\n");
			goto truncate_cleanup;
		}

		exec_truncate(*id, *length);

		truncate_cleanup:
			if (id) free(id);
			if (length) free(length);
			goto cleanup;
	}
	else {
		printf("Unknown command\n");
	}

cleanup:
	free(command);
}

int main(int argc, char **argv) {
	parse_options(argc, argv);
	exec_info();

	while (1) {
		printf("> ");
		char command_line[1024];
		memset(command_line, 0, 1024);
		fgets(command_line, 1024, stdin);

		if (strlen(command_line) == 0 && feof(stdin)) {
			printf("\n");
			break;
		}

		//printf("command: %s\n", command_line);

		do_command(command_line);
	}

	int i;
	for (i = 0; i < files_count; i++) {
		file_delete(files[i]);
	}
	free(files);
	return 0;
}
