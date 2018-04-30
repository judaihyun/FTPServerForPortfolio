#define LOG_NONE 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_LEVEL LOG_WARN


#include <WinSock2.h>
#include <iostream>
void err_quit(const char * msg);
void err_display(const char* msg);

int ftpLog(const char*, int level);