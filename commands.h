#ifndef __COMMANDS_H_INCLUDED_
#define __COMMANDS_H_INCLUDED_

void exec_help();
void exec_info();
void exec_open(int id, const char *mode);
void exec_lock(int id, const int operation);
void exec_close(int id);
void exec_seek(int id, int offset, int whence);
void exec_tell(int id);
void exec_truncate(int id, int length);
void exec_read(int id, int length, int hex);

#endif