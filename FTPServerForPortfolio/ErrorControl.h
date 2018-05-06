#define LOG_FETAL 1
#define LOG_ERROR 2
#define LOG_WARN 3
#define LOG_INFO 4
#define LOG_DEBUG 5

#define LOG_LEVEL 4


#include <WinSock2.h>
#include <iostream>
#include <string>
using namespace std;


void err_quit(const char * msg);
void err_display(const char* msg);

void ftpLog(int, const char*, ...);