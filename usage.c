#include <stdio.h>

void usage() {
	const char *usage = "Commands:\n\
help                   Show help\n\
info                   Show info about the files\n\
open ID MODE           Open the file. MODE is passed directly to fopen() call, so all modes are supported\n\
lock ID OPERATION      Lock the file. Valid operations are sh, ex, un. nb can be added to any operation\n\
close ID               Close the file\n\
seek ID OFFSET WHENCE  Set the file position indicator\n\
tell ID                Show the file position indicator\n\
read ID [x] LENGTH     Read LENGTH bytes from the file. If x is set, output will be HEX-formatted like hexdump -c does\n\
truncate ID LENGTH     Set file size to LENGTH bytes\n";
	printf("%s", usage);
}
